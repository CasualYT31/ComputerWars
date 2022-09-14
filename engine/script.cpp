/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "script.h"
#include <filesystem>
#include "SFML/Graphics/Color.hpp"

void AWEColourTypeConstructor(const int r, const int g, const int b,
    const int a, void* memory) {
    new(memory) sf::Color((sf::Uint8)r, (sf::Uint8)g, (sf::Uint8)b, (sf::Uint8)a);
}

void engine::RegisterColourType(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) noexcept {
    if (!engine->GetTypeInfoByName("Colour")) {
        auto r = engine->RegisterObjectType("Colour", sizeof(sf::Color),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Color>());
        engine->RegisterObjectProperty("Colour", "uint8 r",
            asOFFSET(sf::Color, r));
        engine->RegisterObjectProperty("Colour", "uint8 g",
            asOFFSET(sf::Color, g));
        engine->RegisterObjectProperty("Colour", "uint8 b",
            asOFFSET(sf::Color, b));
        engine->RegisterObjectProperty("Colour", "uint8 a",
            asOFFSET(sf::Color, a));
        engine->RegisterObjectBehaviour("Colour", asBEHAVE_CONSTRUCT,
            "void Colour(const int, const int, const int, const int)",
            asFUNCTION(AWEColourTypeConstructor), asCALL_CDECL_OBJLAST);
        document->DocumentObjectType(r, "Represents a colour value.");
    }
}

engine::scripts::scripts(const std::string& name) noexcept : _logger(name) {
    _engine = asCreateScriptEngine();
    if (_engine) {
        // Allocate the documentation generator.
        ScriptDocumentationOptions options;
        options.htmlSafe = false;
        options.projectName = "Computer Wars";
        std::string filename = name + " Script Interface Documentation.html";
        options.outputFile = filename;
        _document = std::make_shared<DocumentationGenerator>(_engine, options);
        // Allocate the script engine.
        int r = _engine->SetMessageCallback(asMETHOD(engine::scripts,
            scriptMessageCallback), this, asCALL_THISCALL);
        if (r < 0) {
            _logger.error("Fatal error: failed to assign the message callback "
                "routine - this is likely a faulty engine build. Code {}.", r);
        }
        r = _engine->SetTranslateAppExceptionCallback(asMETHOD(engine::scripts,
            translateExceptionCallback), this, asCALL_THISCALL);
        if (r < 0) {
            _logger.error("Fatal error: failed to assign the translate exception "
                "callback - this is likely a faulty engine build. Code {}.", r);
        }
        RegisterStdString(_engine);
        RegisterScriptArray(_engine, false);
        RegisterScriptDictionary(_engine);
        RegisterScriptDateTime(_engine);
        RegisterScriptFileSystem(_engine);
        RegisterExceptionRoutines(_engine);
    } else {
        _logger.error("Fatal error: script engine failed to load. Ensure that "
            "version \"{}\" of AngelScript is being loaded (DLL).",
            ANGELSCRIPT_VERSION_STRING);
    }
}

engine::scripts::~scripts() noexcept {
    for (auto context : _context) context->Release();
    if (_engine) _engine->ShutDownAndRelease();
}

void engine::scripts::addRegistrant(engine::script_registrant* const r) noexcept {
    if (r) {
        _registrants.push_back(r);
    } else {
        _logger.warning("Attempted to add a nullptr script registrant!");
    }
}

void engine::scripts::scriptMessageCallback(const asSMessageInfo* msg, void* param)
    noexcept {
    if (msg->type == asMSGTYPE_INFORMATION) {
        _logger.write("INFO: (@{}:{},{}): {}.",
            msg->section, msg->row, msg->col, msg->message);
    } else if (msg->type == asMSGTYPE_WARNING) {
        _logger.warning("WARNING: (@{}:{},{}): {}.",
            msg->section, msg->row, msg->col, msg->message);
    } else {
        _logger.error("ERROR: (@{}:{},{}): {}.",
            msg->section, msg->row, msg->col, msg->message);
    }
}

void engine::scripts::contextExceptionCallback(asIScriptContext* context)
    noexcept {
    if (!context) return;
    _logger.error("RUNTIME ERROR: (@{}:{}:{}): {}.",
        context->GetExceptionFunction()->GetScriptSectionName(),
        context->GetExceptionFunction()->GetDeclaration(),
        context->GetExceptionLineNumber(), context->GetExceptionString());
}

void engine::scripts::translateExceptionCallback(asIScriptContext* context, void*)
    noexcept {
    if (!context) return;
    //https://www.angelcode.com/angelscript/sdk/docs/manual/doc_cpp_exceptions.html
    try {
        throw;
    } catch (std::exception& e) {
        context->SetException(e.what());
    } catch (...) {}
}

bool engine::scripts::loadScripts(std::string folder) noexcept {
    // First check if the interface has been registered, and if not, register it.
    if (_registrants.size() > 0) {
        _logger.write("Registering the script interface...");
        for (auto& reg : _registrants) reg->registerInterface(_engine, _document);
        _logger.write("Finished registering the script interface.");
        _registrants.clear();
    }
    // Now load the scripts.
    if (folder == "") folder = getScriptsFolder();
    _logger.write("Loading scripts from \"{}\"...", folder);
    if (folder == "" || !_engine) return false;
    CScriptBuilder builder;
    // Before starting a new module, if it already exists, the ComputerWars module
    // should be discarded before being replaced. If it doesn't yet exist, then the
    // negative value returned is ignored.
    _engine->DiscardModule("ComputerWars");
    int r = builder.StartNewModule(_engine, "ComputerWars");
    if (r < 0) {
        _logger.error("Failure to start a new module while loading scripts: code "
            "{}.", r);
        return false;
    }
    try {
        std::function<bool(const std::string&)> directoryIterator =
            [&](const std::string& dir) -> bool {
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                if (entry.is_regular_file()) {
                    if ((r =
                        builder.AddSectionFromFile(entry.path().string().c_str()))
                        < 0) {
                        _logger.error("Failed to add script \"{}\" to the module: "
                            "code {}.", entry.path().string().c_str(), r);
                        return false;
                    }
                } else if (entry.is_directory()) {
                    if (!directoryIterator(entry.path().string())) return false;
                }
            }
            return true;
        };
        if (!directoryIterator(folder)) return false;
    } catch (std::exception& e) {
        _logger.error("Failed to interact with directory entry: {}.",
            e.what());
    }
    if ((r = builder.BuildModule()) < 0) {
        _logger.error("Failed to build the module: code {}.", r);
        return false;
    }
    _scriptsFolder = folder;
    _logger.write("Finished loading scripts.");
    return true;
}

int engine::scripts::generateDocumentation() noexcept {
#if AS_GENERATE_DOCUMENTATION == 1
    if (_document) {
        _logger.write("Generating the script interface documentation...");
        auto ret = _document->Generate();
        _logger.write("Finished generating the script interface documentation.");
        return ret;
    } else {
        _logger.error("Couldn't generate script interface documentation; the "
            "DocumentationGenerator object was uninitialised!");
    }
#endif
    return INT_MIN + 1;
}

const std::string& engine::scripts::getScriptsFolder() const noexcept {
    return _scriptsFolder;
}

bool engine::scripts::functionExists(const std::string& name) const noexcept {
    return _engine->GetModule("ComputerWars")->GetFunctionByName(name.c_str());
}

bool engine::scripts::callFunction(const std::string& name) noexcept {
    if (!_callFunction_TemplateCall) {
        // If this method is being called directly and not from the template
        // version then we must set up the context.
        if (!_setupContext(name)) return false;
    }
    // first check that all parameters have been accounted for
    // passing too few arguments is dangerous when object pointer paramters haven't
    // been given as this will cause the program to crash
    // _setupContext() ensures that the function exists
    auto expected = _engine->GetModule("ComputerWars")->
        GetFunctionByName(name.c_str())->GetParamCount();
    if (expected != _argumentID) {
        // Passing in too many arguments would have caused an error earlier.
        _logger.error("Too few arguments have been given to function call \"{}\": "
            "{} {} been given, but {} {} expected: function call aborted.", name,
            _argumentID, ((_argumentID == 1) ? ("has") : ("have")), expected,
            ((expected == 1) ? ("was") : ("were")));
        _resetCallFunctionVariables();
        return false;
    }
    _resetCallFunctionVariables();
    // Increment the context ID now so that future calls will recognise that this
    // context is in use now.
    _contextId++;
    // Execute the function and return if it worked or not.
    int r = _context[_contextId - 1]->Execute();
    // This context is free now.
    _contextId--;
    if (r != asEXECUTION_FINISHED) {
        _logger.error("Failed to execute function \"{}\": code {}.", name, r);
        return false;
    }
    return true;
}

CScriptDictionary* engine::scripts::createDictionary() noexcept {
    return CScriptDictionary::Create(_engine);
}

int engine::scripts::_allocateContext() noexcept {
    asIScriptContext* context = _engine->CreateContext();
    if (context) {
        int r = context->SetExceptionCallback(asMETHOD(engine::scripts,
            contextExceptionCallback), this, asCALL_THISCALL);
        if (r < 0) {
            _logger.error("Fatal error: failed to assign the exception "
                "callback routine for context {} - this is likely a faulty engine "
                "build. Code {}.", _context.size(), r);
            return r;
        }
        _context.emplace_back(context);
        return 0;
    } else {
        _logger.error("Fatal error: failed to allocate context {} for this "
            "script engine.", _context.size());
        return INT_MIN;
    }
}

bool engine::scripts::_setupContext(const std::string& name) noexcept {
    if (!_engine) return false;
    asIScriptFunction* func =
        _engine->GetModule("ComputerWars")->GetFunctionByName(name.c_str());
    if (!func) {
        _logger.error("Failed to access function \"{}\": either it was not "
            "defined in any of the scripts or it was defined more than once.",
            name);
        return false;
    }

    static const std::string errorMsg = "Failed to prepare context for function "
        "\"{}\": code {}.";
    int r = 0;
    if (_contextId >= _context.size()) {
        // All the existing contexts are in use, so allocate a new one.
        r = _allocateContext();
        if (r < 0) {
            _logger.error(errorMsg, name, r);
            return false;
        }
    }
    r = _context[_contextId]->Prepare(func);
    if (r < 0) {
        _logger.error(errorMsg, name, r);
        return false;
    }
    return true;
}

void engine::scripts::_resetCallFunctionVariables() noexcept {
    _callFunction_TemplateCall = false;
    _argumentID = 0;
}