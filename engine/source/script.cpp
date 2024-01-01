/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

#include "script.hpp"
#include "binary.hpp"
#include <filesystem>
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "boost/stacktrace.hpp"
#include "fmtengine.hpp"

void AWEColourTypeCopyConstructor(const sf::Color& origin, void* memory) {
    new(memory) sf::Color(origin);
}

void AWEColourTypeConstructor(const int r, const int g, const int b,
    const int a, void* memory) {
    new(memory) sf::Color((sf::Uint8)r, (sf::Uint8)g, (sf::Uint8)b, (sf::Uint8)a);
}

static sf::Uint8 gradient(const sf::Uint8 from, const sf::Uint8 to,
    const double percent) {
    return (from < to) ?
        (from + static_cast<sf::Uint8>((to - from) * (percent / 100.0))) :
        (from - static_cast<sf::Uint8>((from - to) * (percent / 100.0)));
}

sf::Color AWEColourGradientTo(void* memory, const sf::Color& colourTo,
    double percent, const bool includeAlpha = false) {
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    auto colourFrom = (const sf::Color*)memory;
    return sf::Color(
        gradient(colourFrom->r, colourTo.r, percent),
        gradient(colourFrom->g, colourTo.g, percent),
        gradient(colourFrom->b, colourTo.b, percent),
        includeAlpha ? gradient(colourFrom->a, colourTo.a, percent) : colourFrom->a
    );
}

// Common colours.
// AngelScript doesn't like const pointers...
static sf::Color Transparent = sf::Color::Transparent;
static sf::Color Black = sf::Color::Black;
static sf::Color White = sf::Color::White;

void engine::RegisterColourType(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
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
            "void Colour(const Colour&in)",
            asFUNCTION(AWEColourTypeCopyConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour("Colour", asBEHAVE_CONSTRUCT,
            "void Colour(const int, const int, const int, const int)",
            asFUNCTION(AWEColourTypeConstructor), asCALL_CDECL_OBJLAST);
        document->DocumentObjectType(r, "Represents a colour value.");
        r = engine->RegisterObjectMethod("Colour", "Colour gradientTo("
            "const Colour&in, double, const bool = false) const",
            asFUNCTION(AWEColourGradientTo), asCALL_CDECL_OBJFIRST);
        document->DocumentObjectMethod(r, "Calculates the colour that is "
            "<tt>double</tt>% from the current colour, to the given colour. If "
            "the bool is <tt>TRUE</tt>, then the alpha channel will be included.");

        // Define the common colour constants.
        r = engine->RegisterGlobalProperty("const Colour Transparent",
            &Transparent);
        r = engine->RegisterGlobalProperty("const Colour Black", &Black);
        r = engine->RegisterGlobalProperty("const Colour White", &White);
    }
}

void AWEVector2iTypeConstructor(const int x, const int y, void* memory) {
    new(memory) sf::Vector2i(x, y);
}

void AWEVector2fTypeConstructor(const float x, const float y, void* memory) {
    new(memory) sf::Vector2f(x, y);
}

void AWEVector2fTypeConstructorFromVector2i(const sf::Vector2i& v, void* memory) {
    new(memory) sf::Vector2f(v);
}

void AWEVector2TypeConstructor(const unsigned int x, const unsigned int y,
    void* memory) {
    new(memory) sf::Vector2u(x, y);
}

void AWEVector2TypeConstructFromString(const std::string& s, void* memory) {
    const unsigned int x =
        (unsigned int)std::stoll(s.substr(1, s.find(',')));
    const unsigned int y =
        (unsigned int)std::stoll(s.substr(s.find(',') + 1, s.find(')')));
    AWEVector2TypeConstructor(x, y, memory);
}

std::string AWEVector2TypeToString(void* memory) {
    if (memory) {
        sf::Vector2u* v = (sf::Vector2u*)memory;
        return "(" + std::to_string(v->x) + ", " + std::to_string(v->y) + ")";
    }
    return "";
}

std::string AWEVector2iTypeToString(void* memory) {
    if (memory) {
        sf::Vector2i* v = (sf::Vector2i*)memory;
        return "(" + std::to_string(v->x) + ", " + std::to_string(v->y) + ")";
    }
    return "";
}

std::string AWEVector2fTypeToString(void* memory) {
    if (memory) {
        sf::Vector2f* v = (sf::Vector2f*)memory;
        return "(" + std::to_string(v->x) + ", " + std::to_string(v->y) + ")";
    }
    return "";
}

// Wrapper for sf::Vector2<> operator==s.

bool iEqI(void* pLhs, const sf::Vector2i& rhs) {
    auto lhs = (const sf::Vector2i*)pLhs;
    return lhs->x == rhs.x && lhs->y == rhs.y;
}

bool iEqU(void* pLhs, const sf::Vector2u& rhs) {
    auto lhs = (const sf::Vector2i*)pLhs;
    return lhs->x == rhs.x && lhs->y == rhs.y;
}

bool uEqI(void* pLhs, const sf::Vector2i& rhs) {
    auto lhs = (const sf::Vector2u*)pLhs;
    return lhs->x == rhs.x && lhs->y == rhs.y;
}

bool uEqU(void* pLhs, const sf::Vector2u& rhs) {
    auto lhs = (const sf::Vector2u*)pLhs;
    return lhs->x == rhs.x && lhs->y == rhs.y;
}

// Wrapper for sf::Vector2<> operator+s.

// Don't bother handling integer overflow.
sf::Vector2u uAddI(void* pLhs, const sf::Vector2i& rhs) {
    auto lhs = (const sf::Vector2u*)pLhs;
    return sf::Vector2u(lhs->x + rhs.x, lhs->y + rhs.y);
}

// DON'T FORGET TO KEEP sfx::INVALID_MOUSE UP TO DATE!
sf::Vector2i INVALID_MOUSE_SCRIPT = sf::Vector2i(INT_MIN, INT_MIN);

void engine::RegisterVectorTypes(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    if (!engine->GetTypeInfoByName("Vector2")) {
        auto r = engine->RegisterObjectType("Vector2", sizeof(sf::Vector2u),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2u>());
        engine->RegisterObjectProperty("Vector2", "uint x",
            asOFFSET(sf::Vector2u, x));
        engine->RegisterObjectProperty("Vector2", "uint y",
            asOFFSET(sf::Vector2u, y));
        engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,
            "void Vector2(const uint, const uint)",
            asFUNCTION(AWEVector2TypeConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,
            "void Vector2(const string&in)",
            asFUNCTION(AWEVector2TypeConstructFromString), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod("Vector2", "string toString() const",
            asFUNCTION(AWEVector2TypeToString), asCALL_CDECL_OBJLAST);
        document->DocumentObjectType(r, "Represents a 2D vector.");

        r = engine->RegisterObjectType("MousePosition", sizeof(sf::Vector2i),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2i>());
        document->DocumentObjectType(r, "Represents a mouse position.");
        r = engine->RegisterGlobalProperty("const MousePosition INVALID_MOUSE",
            &INVALID_MOUSE_SCRIPT);
        engine->RegisterObjectProperty("MousePosition", "int x",
            asOFFSET(sf::Vector2i, x));
        engine->RegisterObjectProperty("MousePosition", "int y",
            asOFFSET(sf::Vector2i, y));
        engine->RegisterObjectBehaviour("MousePosition", asBEHAVE_CONSTRUCT,
            "void MousePosition(const int, const int)",
            asFUNCTION(AWEVector2iTypeConstructor), asCALL_CDECL_OBJLAST);
        r = engine->RegisterObjectMethod("MousePosition",
            "bool opEquals(const MousePosition&in) const",
            asFUNCTION(iEqI), asCALL_CDECL_OBJFIRST);
        r = engine->RegisterObjectMethod("MousePosition",
            "bool opEquals(const Vector2&in) const",
            asFUNCTION(iEqU), asCALL_CDECL_OBJFIRST);
        engine->RegisterObjectMethod("MousePosition", "string toString() const",
            asFUNCTION(AWEVector2iTypeToString), asCALL_CDECL_OBJLAST);

        // Vector2 opEquals
        r = engine->RegisterObjectMethod("Vector2",
            "bool opEquals(const Vector2&in) const",
            asFUNCTION(uEqU), asCALL_CDECL_OBJFIRST);
        r = engine->RegisterObjectMethod("Vector2",
            "bool opEquals(const MousePosition&in) const",
            asFUNCTION(uEqI), asCALL_CDECL_OBJFIRST);

        // Vector2 opAdd
        r = engine->RegisterObjectMethod("Vector2",
            "Vector2 opAdd(const MousePosition&in) const",
            asFUNCTION(uAddI), asCALL_CDECL_OBJFIRST);

        r = engine->RegisterObjectType("Vector2f", sizeof(sf::Vector2f),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2f>());
        engine->RegisterObjectProperty("Vector2f", "float x",
            asOFFSET(sf::Vector2f, x));
        engine->RegisterObjectProperty("Vector2f", "float y",
            asOFFSET(sf::Vector2f, y));
        engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT,
            "void Vector2f(const float, const float)",
            asFUNCTION(AWEVector2fTypeConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT,
            "void Vector2f(const MousePosition&in)",
            asFUNCTION(AWEVector2fTypeConstructorFromVector2i),
            asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod("Vector2f", "string toString() const",
            asFUNCTION(AWEVector2fTypeToString), asCALL_CDECL_OBJLAST);
    }
}

void AWEIntRectTypeConstructor(const int left, const int top, const int width,
    const int height, void* memory) {
    new(memory) sf::IntRect(left, top, width, height);
}

std::string AWEIntRectTypeToString(void* memory) {
    if (memory) {
        sf::IntRect* r = (sf::IntRect*)memory;
        return "RECT (" + std::to_string(r->left) + ", " + std::to_string(r->top) +
            ") [" + std::to_string(r->width) + " x " + std::to_string(r->height) +
            "]";
    }
    return "";
}

void engine::RegisterRectTypes(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    engine::RegisterVectorTypes(engine, document);
    if (!engine->GetTypeInfoByName("IntRect")) {
        auto r = engine->RegisterObjectType("IntRect", sizeof(sf::IntRect),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::IntRect>());
        engine->RegisterObjectProperty("IntRect", "int left",
            asOFFSET(sf::IntRect, left));
        engine->RegisterObjectProperty("IntRect", "int top",
            asOFFSET(sf::IntRect, top));
        engine->RegisterObjectProperty("IntRect", "int width",
            asOFFSET(sf::IntRect, width));
        engine->RegisterObjectProperty("IntRect", "int height",
            asOFFSET(sf::IntRect, height));
        engine->RegisterObjectBehaviour("IntRect", asBEHAVE_CONSTRUCT,
            "void IntRect(const int, const int, const int, const int)",
            asFUNCTION(AWEIntRectTypeConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod("IntRect", "string toString() const",
            asFUNCTION(AWEIntRectTypeToString), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod("IntRect",
            "bool contains(const MousePosition&in) const",
            asMETHODPR(sf::IntRect, contains, (const sf::Vector2i&) const, bool),
            asCALL_THISCALL);
        document->DocumentObjectType(r, "Represents a rectangle.");
    }
}

void engine::RegisterTimeTypes(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    if (!engine->GetTypeInfoByName("Time")) {
        // Time class.
        auto r = engine->RegisterObjectType("Time", sizeof(sf::Time),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Time>());
        document->DocumentObjectType(r, "Represents a time value.");
        r = engine->RegisterObjectMethod("Time", "float asSeconds()",
            asMETHOD(sf::Time, asSeconds), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Return the time value as a number of "
            "seconds.");
        r = engine->RegisterObjectMethod("Time", "int32 asMilliseconds()",
            asMETHOD(sf::Time, asMilliseconds), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Return the time value as a number of "
            "milliseconds.");
        r = engine->RegisterObjectMethod("Time", "int64 asMicroseconds()",
            asMETHOD(sf::Time, asMicroseconds), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Return the time value as a number of "
            "microseconds.");
        // Time class factory functions.
        r = engine->RegisterGlobalFunction("Time seconds(const float)",
            asFUNCTION(sf::seconds), asCALL_CDECL);
        document->DocumentGlobalFunction(r, "Constructs a Time object using "
            "seconds.");
        r = engine->RegisterGlobalFunction("Time milliseconds(const int32)",
            asFUNCTION(sf::milliseconds), asCALL_CDECL);
        document->DocumentGlobalFunction(r, "Constructs a Time object using "
            "milliseconds.");
        r = engine->RegisterGlobalFunction("Time microseconds(const int64)",
            asFUNCTION(sf::microseconds), asCALL_CDECL);
        document->DocumentGlobalFunction(r, "Constructs a Time object using "
            "microseconds.");

        // Clock class.
        r = engine->RegisterObjectType("Clock", sizeof(sf::Clock),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Clock>());
        document->DocumentObjectType(r, "Used to calculate elapsed time.");
        r = engine->RegisterObjectMethod("Clock", "Time getElapsedTime()",
            asMETHOD(sf::Clock, getElapsedTime), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Calculates the elapsed time since the "
            "clock was constructed or since <tt>restart()</tt> was called.");
        r = engine->RegisterObjectMethod("Clock", "Time restart()",
            asMETHOD(sf::Clock, restart), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Restarts the clock. Returns the time "
            "elapsed.");
    }
}

void engine::RegisterStreamTypes(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    if (!engine->GetTypeInfoByName("BinaryIStream")) {
        auto r = engine->RegisterObjectType("BinaryIStream", 0,
            asOBJ_REF | asOBJ_NOCOUNT);
        document->DocumentObjectType(r, "Represents an input stream of binary "
            "data.");
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(int8&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int8&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(int16&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int16&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(int32&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int32&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(int64&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int64&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(uint8&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint8&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(uint16&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint16&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(uint32&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint32&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(uint64&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint64&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(bool&out)",
            asMETHODPR(engine::binary_istream, readBool, (bool&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryIStream", "void read(string&out)",
            asMETHODPR(engine::binary_istream, readString, (std::string&), void),
            asCALL_THISCALL);

        r = engine->RegisterObjectType("BinaryOStream", 0,
            asOBJ_REF | asOBJ_NOCOUNT);
        document->DocumentObjectType(r, "Represents an output stream of binary "
            "data.");
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const int8)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int8>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const int16)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int16>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const int32)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int32>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const int64)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int64>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const uint8)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint8>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const uint16)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint16>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const uint32)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint32>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const uint64)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint64>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const bool)",
            asMETHOD(engine::binary_ostream, writeBool),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod("BinaryOStream",
            "void write(const string&in)",
            asMETHOD(engine::binary_ostream, writeString), asCALL_THISCALL);
    }
}

engine::scripts::scripts(const engine::logger::data& data) : _logger(data) {
    _engine = asCreateScriptEngine();
    if (_engine) {
        // Allocate the documentation generator.
        ScriptDocumentationOptions options;
        options.htmlSafe = false;
        options.projectName = "Computer Wars";
        std::string filename = data.name + " Script Interface Documentation.html";
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
        RegisterScriptAny(_engine);
        RegisterScriptArray(_engine, false);
        RegisterScriptDictionary(_engine);
        RegisterScriptDateTime(_engine);
        RegisterScriptFileSystem(_engine);
        RegisterScriptMath(_engine);
        RegisterScriptHandle(_engine);
        RegisterScriptWeakRef(_engine);
        RegisterExceptionRoutines(_engine);
    } else {
        _logger.error("Fatal error: script engine failed to load. Ensure that "
            "version \"{}\" of AngelScript is being loaded (DLL).",
            ANGELSCRIPT_VERSION_STRING);
    }
}

engine::scripts::~scripts() noexcept {
    if (_executeCodeContext) _executeCodeContext->Release();
    for (auto context : _context) context->Release();
    if (_engine) _engine->ShutDownAndRelease();
}

void engine::scripts::addRegistrant(engine::script_registrant* const r) {
    if (r) {
        _registrants.push_back(r);
    } else {
        _logger.warning("Attempted to add a nullptr script registrant!");
    }
}

void engine::scripts::scriptMessageCallback(const asSMessageInfo* msg,
    void* param) {
    if (_fillCachedMsg) {
        _cachedMsg += msg->message;
        _cachedMsg += " (Section \"" + std::string(msg->section) + "\", Row " +
            std::to_string(msg->row) + ", Col " + std::to_string(msg->col) + "), ";
    } else {
        _cachedMsg = "";
    }
    if (msg->type == asMSGTYPE_INFORMATION) {
        _logger.write("INFO: {}.", *msg);
    } else if (msg->type == asMSGTYPE_WARNING) {
        _logger.warning("WARNING: {}.", *msg);
    } else {
        _logger.error("ERROR: {}.", *msg);
    }
}

void engine::scripts::contextExceptionCallback(asIScriptContext* c) {
    if (!c) return;
    _logger.error("RUNTIME ERROR: (@{}:{}:{}): {}.",
        c->GetExceptionFunction()->GetScriptSectionName(),
        c->GetExceptionFunction()->GetDeclaration(),
        c->GetExceptionLineNumber(), c->GetExceptionString());
}

void engine::scripts::translateExceptionCallback(asIScriptContext* context, void*)
    noexcept {
    if (!context) return;
    //https://www.angelcode.com/angelscript/sdk/docs/manual/doc_cpp_exceptions.html
    try {
        throw;
    } catch (const std::exception& e) {
        context->SetException(e.what());
    } catch (...) {}
}

bool engine::scripts::loadScripts(std::string folder) {
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
    // Before starting a new module, if it already exists, the MAIN_MODULE should
    // be discarded before being replaced. If it doesn't yet exist, then the
    // negative value returned is ignored.
    _engine->DiscardModule(MAIN_MODULE);
    int r = _builder.StartNewModule(_engine, MAIN_MODULE);
    if (r < 0) {
        _logger.error("Failed to start the main module while loading scripts: "
            "code {}.", r);
        return false;
    }
    try {
        std::function<bool(const std::string&)> directoryIterator =
            [&](const std::string& dir) -> bool {
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                if (entry.is_regular_file()) {
                    if ((r =
                        _builder.AddSectionFromFile(entry.path().string().c_str()))
                        < 0) {
                        _logger.error("Failed to add script \"{}\" to the main "
                            "module: code {}.", entry.path().string().c_str(), r);
                        return false;
                    }
                } else if (entry.is_directory()) {
                    if (!directoryIterator(entry.path().string())) return false;
                }
            }
            return true;
        };
        if (!directoryIterator(folder)) return false;
    } catch (const std::exception& e) {
        _logger.error("Failed to interact with directory entry: {}.", e);
    }
    if ((r = _builder.BuildModule()) < 0) {
        _logger.error("Failed to build the main module: code {}.", r);
        return false;
    }
    _scriptsFolder = folder;
    _logger.write("Finished loading scripts.");
    return true;
}

int engine::scripts::generateDocumentation() {
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

bool engine::scripts::functionExists(const std::string& module,
    const std::string& name) const {
    const auto* const m = _engine->GetModule(module.c_str());
    return m && m->GetFunctionByName(name.c_str());
}

bool engine::scripts::functionDeclExists(const std::string& module,
    const std::string& decl) const {
    const auto* const m = _engine->GetModule(module.c_str());
    return m && m->GetFunctionByDecl(decl.c_str());
}

void engine::scripts::writeToLog(const std::string& message) const {
    _logger.write(_constructMessage(message));
}

void engine::scripts::warningToLog(const std::string& message) const {
    _logger.warning(_constructMessage(message));
}

void engine::scripts::errorToLog(const std::string& message) const {
    _logger.error(_constructMessage(message));
}

void engine::scripts::criticalToLog(const std::string& message) const {
    _logger.critical(_constructMessage(message));
}

/**
 * Formats a given AngelScript context as a stacktrace.
 * This function follows the same format as employed by the backend used to
 * retrieve the C++ stacktrace:
 * <tt>Index# FunctionName at SourceFilePath:LineNumber</tt>.
 * @param  context         The context to retrieve the stacktrace of.
 * @param  stackIndexWidth The width of the \c Index field, in characters.
 * @return The stacktrace of the given AngelScript context.
 * @safety No guarantee.
 */
static std::string asStacktrace(asIScriptContext* const context,
    const std::streamsize stackIndexWidth) {
    std::stringstream asStacktrace;
    asStacktrace << "\n----------\n";
    if (!context) {
        asStacktrace << "AngelScript Context Does Not Exist!";
    } else {
        for (asUINT i = 0, stackSize = context->GetCallstackSize(); i < stackSize;
            ++i) {
            const auto asFunc = context->GetFunction(i);
            asStacktrace << std::setfill(' ') << std::setw(stackIndexWidth) << i;
            asStacktrace << "# " << asFunc->GetName() << " at " <<
                asFunc->GetScriptSectionName() << ':' << context->GetLineNumber(i)
                << std::endl;
        }
    }
    asStacktrace << "----------";
    return asStacktrace.str();
}

void engine::scripts::stacktraceToLog() const {
    const auto boostStacktrace = boost::stacktrace::stacktrace();
    std::stringstream strstream;
    strstream << "Stacktrace\n" << boostStacktrace;
    const auto stackIndexWidth = std::to_string(boostStacktrace.size()).size();
    auto trace = strstream.str();
    // Go through the C++ stacktrace and insert AngelScript stacktraces.
    auto asStack = _context.rbegin(), asStackEnd = _context.rend();
    const std::string SEARCH_FOR = " in angelscriptd";
    std::size_t pointer = 0;
    std::size_t asLocation = trace.find(SEARCH_FOR, pointer);
    while (asLocation != std::string::npos) {
        pointer = asLocation + SEARCH_FOR.size();
        const auto asStacktraceStr = asStacktrace(*asStack, stackIndexWidth);
        trace.insert(pointer, asStacktraceStr);
        pointer += asStacktraceStr.size();
        if (asStack != asStackEnd) ++asStack;
        asLocation = trace.find(SEARCH_FOR, pointer);
    }
    if (asStack == asStackEnd) {
        trace.append("\nAll AngelScript Contexts Were Output");
    } else {
        trace.append("\nSome AngelScript Contexts Were Not Output!");
        for (; asStack != asStackEnd; ++asStack)
            trace.append(asStacktrace(*asStack, stackIndexWidth));
    }
    _logger.write(trace);
}

bool engine::scripts::callFunction(asIScriptFunction* const func) {
    if (!_callFunction_TemplateCall) {
        // If this method is being called directly and not from the template
        // version then we must set up the context.
        if (!_setupContext(func)) return false;
    }
    // Check that all parameters have been accounted for. Passing too few arguments
    // is dangerous when object pointer parameters haven't been given as this will
    // cause the program to crash. _setupContext() ensures that the function
    // exists.
    auto expected = func->GetParamCount();
    if (expected != _argumentID) {
        // Passing in too many arguments would have caused an error earlier.
        _logger.error("Too few arguments have been given to function call \"{}\": "
            "{} {} been given, but {} {} expected: function call aborted.",
            func->GetName(),  _argumentID,
            ((_argumentID == 1) ? ("has") : ("have")), expected,
            ((expected == 1) ? ("was") : ("were")));
        _resetCallFunctionVariables();
        return false;
    }
    // If this is a method call, set the context's object now.
    if (_functionObject) {
        int r = _context[_contextId]->SetObject(_functionObject);
        if (r < 0) {
            _logger.error("Could not set object of type \"{}\" to context when "
                "invoking method \"{}\": code {}.",
                _functionObject->GetObjectType()->GetName(), func->GetName(), r);
            _resetCallFunctionVariables();
            return false;
        }
    }
    _resetCallFunctionVariables();
    // Increment the context ID now so that future calls will recognise that this
    // context is in use now.
    ++_contextId;
    // Execute the function and return if it worked or not.
    int r = _context[_contextId - 1]->Execute();
    // This context is free now.
    --_contextId;
    if (r != asEXECUTION_FINISHED) {
        _logger.error("Failed to execute function \"{}\": code {}.",
            func->GetName(), r);
        return false;
    }
    return true;
}

std::string engine::scripts::executeCode(std::string code) {
    // If only I had known about ExecuteString() before writing all of this...
    // Bruh...
    static const auto Log = [&](const std::string& m) -> std::string {
        _logger.error(m); return m;
    };
    auto m = _engine->GetModule(MAIN_MODULE);
    if (m) {
        static std::size_t counter = 0;
        asIScriptFunction* func = nullptr;
        code = "void EXECUTE_CODE_" + std::to_string(counter++) + "() {" + code +
            "}";
        _fillCachedMsg = true;
        _cachedMsg = "";
        auto r = m->CompileFunction("EXECUTE_CODE_SECTION", code.c_str(),
            0, 0, &func);
        _fillCachedMsg = false;
        if (r >= 0 && func) {
            if (!_executeCodeContext) {
                _executeCodeContext = _engine->CreateContext();
                if (!_executeCodeContext) {
                    return Log("Could not create context for executeCode()!");
                }
            }
            _executeCodeContext->Prepare(func);
            r = _executeCodeContext->Execute();
            func->Release();
            std::string ret = "asEXECUTION_EXCEPTION: ";
            switch (r) {
            case asCONTEXT_NOT_PREPARED:
                return Log("asCONTEXT_NOT_PREPARED: The context is not prepared "
                    "or it is not in suspended state.");
                break;
            case asEXECUTION_ABORTED:
                return Log("asEXECUTON_ABORTED: The execution was aborted with a "
                    "call to Abort.");
                break;
            case asEXECUTION_SUSPENDED:
                return Log("asEXECUTION_SUSPENDED: The execution was suspended "
                    "with a call to Suspend.");
                break;
            case asEXECUTION_FINISHED:
                return "";
                break;
            case asEXECUTION_EXCEPTION:
                ret += _executeCodeContext->GetExceptionString();
                return Log(ret);
                break;
            default:
                return Log("An unknown error occurred during runtime: code " +
                    std::to_string(r) + ".");
            }
        } else {
            return Log(_constructBuildErrorMessage(r));
        }
    } else {
        return Log("The main module does not exist!");
    }
}

CScriptDictionary* engine::scripts::createDictionary() {
    return CScriptDictionary::Create(_engine);
}

CScriptArray* engine::scripts::createArray(const std::string& type) const {
    std::string decl = "array<" + type + ">";
    auto typeInfo = _engine->GetTypeInfoByDecl(decl.c_str());
    if (typeInfo) return CScriptArray::Create(typeInfo);
    return nullptr;
}

CScriptAny* engine::scripts::createAny() const {
    return new CScriptAny(_engine);
}

asIScriptObject* engine::scripts::createObject(const std::string& type) {
    auto m = _engine->GetModule(MAIN_MODULE);
    if (!m) {
        _logger.error("Could not create object of type \"{}\" as the module "
            "\"{}\" does not exist.", type, MAIN_MODULE);
        return nullptr;
    }
    const auto typeInfo = m->GetTypeInfoByDecl(type.c_str());
    if (!typeInfo) {
        _logger.error("Could not create object of non-existent type \"{}\".",
            type);
        return nullptr;
    }
    const auto defaultFactoryFuncSig = type + " @" + type + "()";
    const auto defaultFactoryFunc = typeInfo->GetFactoryByDecl(
        defaultFactoryFuncSig.c_str());
    if (!defaultFactoryFunc) {
        _logger.error("Could not create object of type \"{}\" as it does not have "
            "a factory function of signature \"{}\".", type,
            defaultFactoryFuncSig);
        return nullptr;
    }
    auto ctx = _engine->CreateContext();
    if (!ctx) {
        _logger.error("Could not create object of type \"{}\" as the factory "
            "function context could not be initialised.", type);
        return nullptr;
    }
    auto r = ctx->SetExceptionCallback(asMETHOD(engine::scripts,
        contextExceptionCallback), this, asCALL_THISCALL);
    if (r < 0) {
        _logger.error("Failed to assign the exception callback routine for the "
            "factory function context - this is likely a faulty engine build. "
            "Code {}.", r);
    }
    r = ctx->Prepare(defaultFactoryFunc);
    if (r < 0) {
        _logger.error("Could not create object of type \"{}\" as the factory "
            "function context could not be prepared. Error code {}.", type, r);
        ctx->Release();
        return nullptr;
    }
    r = ctx->Execute();
    if (r != asEXECUTION_FINISHED) {
        _logger.error("Could not create object of type \"{}\" as the factory "
            "function context could not be executed. Error code {}.", type, r);
        ctx->Release();
        return nullptr;
    }
    auto obj = *static_cast<asIScriptObject**>(ctx->GetAddressOfReturnValue());
    // Must add ref before releasing the context!
    obj->AddRef();
    ctx->Release();
    return obj;
}

int engine::scripts::getTypeID(const std::string& type) const {
    // Handle primitive types separately as they don't seem to work with TypeInfo.
    if (type == "void") {
        return 0;
    } else if (type == "bool") {
        return 1;
    } else if (type == "int8") {
        return 2;
    } else if (type == "int16") {
        return 3;
    } else if (type == "int" || type == "int32") {
        return 4;
    } else if (type == "int64") {
        return 5;
    } else if (type == "uint8") {
        return 6;
    } else if (type == "uint16") {
        return 7;
    } else if (type == "uint" || type == "uint32") {
        return 8;
    } else if (type == "uint64") {
        return 9;
    } else if (type == "float") {
        return 10;
    } else if (type == "double") {
        return 11;
    }
    // Handle other types.
    auto typeInfo = _engine->GetTypeInfoByDecl(type.c_str());
    if (typeInfo)
        return typeInfo->GetTypeId();
    else
        return -1;
}

std::string engine::scripts::getTypeName(const int id) const {
    // Handle primitive types separately as they don't seem to work with TypeInfo.
    if (id == 0) {
        return "void";
    } else if (id == 1) {
        return "bool";
    } else if (id == 2) {
        return "int8";
    } else if (id == 3) {
        return "int16";
    } else if (id == 4) {
        return "int";
    } else if (id == 5) {
        return "int64";
    } else if (id == 6) {
        return "uint8";
    } else if (id == 7) {
        return "uint16";
    } else if (id == 8) {
        return "uint";
    } else if (id == 9) {
        return "uint64";
    } else if (id == 10) {
        return "float";
    } else if (id == 11) {
        return "double";
    }
    // Handle other types.
    auto typeInfo = _engine->GetTypeInfoById(id);
    if (typeInfo) return typeInfo->GetName();
    else return "";
}

std::vector<std::string> engine::scripts::getConcreteClassNames(
    const std::string& interfaceName) const {
    auto m = _engine->GetModule(MAIN_MODULE);
    if (!m) {
        _logger.error("Cannot get concrete classes that implement interface "
            "\"{}\" as the \"{}\" module does not exist.", interfaceName,
            MAIN_MODULE);
        return {};
    }
    const auto interfaceTypeInfo = m->GetTypeInfoByDecl(interfaceName.c_str());
    if (!interfaceTypeInfo) {
        _logger.error("Cannot get concrete classes that implement interface "
            "\"{}\" as it does not exist.", interfaceName);
        return {};
    }
    std::vector<std::string> list;
    for (asUINT i = 0, count = m->GetObjectTypeCount(); i < count; ++i) {
        const auto typeInfo = m->GetObjectTypeByIndex(i);
        if (typeInfo->Implements(interfaceTypeInfo))
            list.emplace_back(typeInfo->GetName());
    }
    return list;
}

bool engine::scripts::createModule(const std::string& name,
    const engine::scripts::files& code, std::string& errorString) {
    if (name == MAIN_MODULE) {
        _logger.error("Attempted to create new module called \"{}\", which is not "
            "allowed!", name);
        return false;
    }
    if (name.find('~') != std::string::npos) {
        _logger.error("Attempted to create a new module called \"{}\" that "
            "contains illegal character '~'!", name);
        return false;
    }
    _logger.write("Creating module \"{}\"...", name);
    const std::string tempName = name + "~temp";
    auto r = _builder.StartNewModule(_engine, tempName.c_str());
    if (r < 0) {
        _logger.error("Failed to start a new module \"{}\": code {}.", tempName,
            r);
        return false;
    }
    for (const auto& file : code) {
        r = _builder.AddSectionFromMemory(file.first.c_str(), file.second.c_str(),
            static_cast<int>(file.second.size()));
        if (r < 0) {
            _logger.error("Failed to add code file \"{}\" to new module \"{}\": "
                "code {}.", file.first, tempName, r);
            return false;
        }
    }
    _fillCachedMsg = true;
    _cachedMsg = "";
    r = _builder.BuildModule();
    _fillCachedMsg = false;
    if (r < 0) {
        _logger.error("Failed to build the new module \"{}\": code {}.", tempName,
            r);
        errorString = _constructBuildErrorMessage(r);
        return false;
    }
    auto const newModule = _engine->GetModule(tempName.c_str());
    if (!newModule) {
        _logger.error("Could not get new module \"{}\" to rename it.", tempName);
        return false;
    }
    // Discard the old module if it exists.
    _engine->DiscardModule(name.c_str());
    newModule->SetName(name.c_str());
    _logger.write("Successfully created module \"{}\".", name);
    return true;
}

bool engine::scripts::deleteModule(const std::string& name) {
    if (name == MAIN_MODULE) {
        _logger.error("Cannot discard module \"{}\"!", name);
        return false;
    }
    const auto r = _engine->DiscardModule(name.c_str());
    if (r < 0) _logger.error("Could not discard module \"{}\": code {}.", name, r);
    else _logger.write("Successfully discarded module \"{}\".", name);
    return r == 0;
}

bool engine::scripts::doesModuleExist(const std::string& name) const {
    return _engine->GetModule(name.c_str());
}

int engine::scripts::_allocateContext() {
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

bool engine::scripts::_setupContext(asIScriptFunction* const func) {
    if (!_engine) return false;
    if (!func) {
        _logger.error("Attempted to call a NULL script function!");
        return false;
    }
    static const auto errorMsg = "Failed to prepare context for function \"{}\": "
        "code {}.";
    int r = 0;
    if (_contextId >= _context.size()) {
        // All the existing contexts are in use, so allocate a new one.
        r = _allocateContext();
        if (r < 0) {
            _logger.error(errorMsg, func->GetName(), r);
            return false;
        }
    }
    r = _context[_contextId]->Prepare(func);
    if (r < 0) {
        _logger.error(errorMsg, func->GetName(), r);
        return false;
    }
    return true;
}

void engine::scripts::_resetCallFunctionVariables() noexcept {
    _callFunction_TemplateCall = false;
    _argumentID = 0;
    _functionObject = nullptr;
}

std::string engine::scripts::_constructMessage(const std::string& msg) const {
    // There are rare cases where something may be written to the log from the
    // script code without the C++ engine having directly invoked an AngelScript
    // function. An example of this is within the constructor of a class defined
    // within the scripts but instantiated by the C++ engine using createObject().
    // In such cases, just write the message given.
    if (_context.empty()) return msg;
    asIScriptContext* context = _context.back();
    const char* sectionName = nullptr;
    asIScriptFunction* function = context->GetFunction(0);
    const int lineNumber = context->GetLineNumber(0, nullptr, &sectionName);
    return "In " + std::string(sectionName) + ", function " +
        std::string(function->GetDeclaration()) + ", at line " +
        std::to_string(lineNumber) + ": " + msg;
}

std::string engine::scripts::_constructBuildErrorMessage(const int code) const {
    switch (code) {
    case asINVALID_ARG:
        return "asINVALID_ARG: One or more arguments have invalid values.";
    case asINVALID_CONFIGURATION:
        return "asINVALID_CONFIGURATION: The engine configuration is invalid.";
    case asBUILD_IN_PROGRESS:
        return "asBUILD_IN_PROGRESS: Another build is in progress.";
    case asERROR:
        return "asERROR: " + _cachedMsg;
    case asNOT_SUPPORTED:
        return "asNOT_SUPPORTED: Compiler support is disabled in the engine.";
    default:
        return "An unknown error occurred during compilation: code " +
            std::to_string(code) + ".";
    }
}
