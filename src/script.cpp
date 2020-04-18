/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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
#include <iostream>

void print(std::string& in) {
    std::cout << in;
}

void printNumber(int in) {
    std::cout << in << std::endl;
}

void printFloat(float in) {
    std::cout << in << std::endl;
}

awe::scripts::scripts(const std::string& folder, const std::string& name) noexcept : _logger(name) {
    _engine = asCreateScriptEngine();
    if (_engine) {
        int r = _engine->SetMessageCallback(asMETHOD(awe::scripts, scriptMessageCallback), this, asCALL_THISCALL);
        if (r < 0) {
            _logger.error("Fatal error: failed to assign the message callback routine - this is likely a faulty engine build. Code {}.", r);
        }
        RegisterStdString(_engine);
        _registerInterface();
        if (folder != "") reloadScripts(folder);
        _context = _engine->CreateContext();
        if (_context) {
            if ((r = _context->SetExceptionCallback(asMETHOD(awe::scripts, contextExceptionCallback), this, asCALL_THISCALL)) < 0) {
                _logger.error("Fatal error: failed to assign the exception callback routine - this is likely a faulty engine build. Code {}.", r);
            }
        } else {
            _logger.error("Fatal error: failed to allocate the context for this script engine.");
        }
    } else {
        _logger.error("Fatal error: script engine failed to load. Ensure that version \"{}\" of AngelScript is being loaded (DLL).", ANGELSCRIPT_VERSION_STRING);
    }
}

awe::scripts::~scripts() noexcept {
    if (_context) _context->Release();
    if (_engine) _engine->ShutDownAndRelease();
}

void awe::scripts::scriptMessageCallback(const asSMessageInfo* msg, void* param) noexcept {
    if (msg->type == asMSGTYPE_INFORMATION) {
        _logger.write("INFO: (@{}:{},{}): {}.", msg->section, msg->row, msg->col, msg->message);
    } else {
        _logger.error("{}: (@{}:{},{}): {}.", (msg->type==asMSGTYPE_WARNING?"WARN":"ERR "), msg->section, msg->row, msg->col, msg->message);
    }
}

void awe::scripts::contextExceptionCallback(asIScriptContext* context) noexcept {
    if (!context) return;
    _logger.error("RUNTIME ERROR: (@{}:{}:{}): {}.", context->GetExceptionFunction()->GetScriptSectionName(), context->GetExceptionFunction()->GetDeclaration(), context->GetExceptionLineNumber(), context->GetExceptionString());
}

bool awe::scripts::reloadScripts(std::string folder) noexcept {
    _logger.write("Loading scripts from \"{}\"...", folder);
    if (folder == "") folder = getScriptsFolder();
    if (folder == "" || !_engine) return false;
    CScriptBuilder builder;
    int r = builder.StartNewModule(_engine, "ComputerWars");
    if (r < 0) {
        _logger.error("Failure to start a new module while loading scripts: code {}.", r);
        return false;
    }
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        try {
            if (entry.is_regular_file()) {
                if ((r = builder.AddSectionFromFile(entry.path().string().c_str())) < 0) {
                    _logger.error("Failed to add script \"{}\" to the module: code {}.", entry.path().string().c_str(), r);
                    return false;
                }
            }
        } catch (std::exception& e) {
            _logger.error("Failed to interact with directory entry: {}.", e.what());
        }
    }
    if ((r = builder.BuildModule()) < 0) {
        _logger.error("Failed to build the module: code {}.", r);
        return false;
    }
    _scriptsFolder = folder;
    _logger.write("Finished loading scripts.");
    return true;
}

const std::string& awe::scripts::getScriptsFolder() const noexcept {
    return _scriptsFolder;
}

bool awe::scripts::callFunction(const std::string& name) noexcept {
    if (!_callFunction_TemplateCall) {
        //if this method is being called directly and not from the template version
        //then we must set up the context
        if (!_setupContext(name)) return false;
    }
    _callFunction_TemplateCall = false;
    _argumentID = 0;
    //execute the function and return if it worked or not
    int r = _context->Execute();
    if (r != asEXECUTION_FINISHED) {
        _logger.error("Failed to execute function \"{}\": code {}.", name, r);
        return false;
    }
    return true;
}

bool awe::scripts::_setupContext(const std::string& name) noexcept {
    if (!_engine) return false;
    asIScriptFunction* func = _engine->GetModule("ComputerWars")->GetFunctionByName(name.c_str());
    if (!func) {
        _logger.error("Failed to access function \"{}\": either it was not defined in any of the scripts or it was defined more than once.", name);
        return false;
    }
    int r = _context->Prepare(func);
    if (r < 0) {
        _logger.error("Failed to prepare context for function \"{}\": code {}.", name, r);
        return false;
    }
    return true;
}

//INCOMPLETE: functionality will be added as and when necessary, print methods will be removed at some point
void awe::scripts::_registerInterface() noexcept {
    _logger.write("Registering script interface...");
    _engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    _engine->RegisterGlobalFunction("void printno(const int)", asFUNCTION(printNumber), asCALL_CDECL);
    _engine->RegisterGlobalFunction("void printfloat(const float)", asFUNCTION(printFloat), asCALL_CDECL);
    _logger.write("Finished registering script interface.");
}