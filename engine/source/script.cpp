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
#include <filesystem>
#include <regex>
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
    if (!engine->GetTypeInfoByName(engine::script_type<sf::Color>().c_str())) {
        auto r = engine->RegisterObjectType(
            engine::script_type<sf::Color>().c_str(), sizeof(sf::Color),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Color>());
        engine->RegisterObjectProperty(engine::script_type<sf::Color>().c_str(), "uint8 r",
            asOFFSET(sf::Color, r));
        engine->RegisterObjectProperty(engine::script_type<sf::Color>().c_str(), "uint8 g",
            asOFFSET(sf::Color, g));
        engine->RegisterObjectProperty(engine::script_type<sf::Color>().c_str(), "uint8 b",
            asOFFSET(sf::Color, b));
        engine->RegisterObjectProperty(engine::script_type<sf::Color>().c_str(), "uint8 a",
            asOFFSET(sf::Color, a));
        engine->RegisterObjectBehaviour(engine::script_type<sf::Color>().c_str(),
            asBEHAVE_CONSTRUCT, "void Colour(const Colour&in)",
            asFUNCTION(AWEColourTypeCopyConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour(engine::script_type<sf::Color>().c_str(),
            asBEHAVE_CONSTRUCT,
            "void Colour(const int, const int, const int, const int)",
            asFUNCTION(AWEColourTypeConstructor), asCALL_CDECL_OBJLAST);
        document->DocumentObjectType(r, "Represents a colour value.");
        r = engine->RegisterObjectMethod(engine::script_type<sf::Color>().c_str(),
            "Colour gradientTo(const Colour&in, double, const bool = false) const",
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
    if (!engine->GetTypeInfoByName(engine::script_type<sf::Vector2u>().c_str())) {
        auto r = engine->RegisterObjectType(engine::script_type<sf::Vector2u>().c_str(),
            sizeof(sf::Vector2u),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2u>());
        engine->RegisterObjectProperty(engine::script_type<sf::Vector2u>().c_str(),
            "uint x", asOFFSET(sf::Vector2u, x));
        engine->RegisterObjectProperty(engine::script_type<sf::Vector2u>().c_str(),
            "uint y", asOFFSET(sf::Vector2u, y));
        engine->RegisterObjectBehaviour(engine::script_type<sf::Vector2u>().c_str(),
            asBEHAVE_CONSTRUCT, "void Vector2(const uint, const uint)",
            asFUNCTION(AWEVector2TypeConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour(engine::script_type<sf::Vector2u>().c_str(),
            asBEHAVE_CONSTRUCT, "void Vector2(const string&in)",
            asFUNCTION(AWEVector2TypeConstructFromString), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod(engine::script_type<sf::Vector2u>().c_str(),
            "string toString() const",
            asFUNCTION(AWEVector2TypeToString), asCALL_CDECL_OBJLAST);
        document->DocumentObjectType(r, "Represents a 2D vector.");

        r = engine->RegisterObjectType(engine::script_type<sf::Vector2i>().c_str(),
            sizeof(sf::Vector2i),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2i>());
        document->DocumentObjectType(r, "Represents a mouse position.");
        r = engine->RegisterGlobalProperty("const MousePosition INVALID_MOUSE",
            &INVALID_MOUSE_SCRIPT);
        engine->RegisterObjectProperty(engine::script_type<sf::Vector2i>().c_str(),
            "int x", asOFFSET(sf::Vector2i, x));
        engine->RegisterObjectProperty(engine::script_type<sf::Vector2i>().c_str(),
            "int y", asOFFSET(sf::Vector2i, y));
        engine->RegisterObjectBehaviour(engine::script_type<sf::Vector2i>().c_str(),
            asBEHAVE_CONSTRUCT, "void MousePosition(const int, const int)",
            asFUNCTION(AWEVector2iTypeConstructor), asCALL_CDECL_OBJLAST);
        r = engine->RegisterObjectMethod(engine::script_type<sf::Vector2i>().c_str(),
            "bool opEquals(const MousePosition&in) const",
            asFUNCTION(iEqI), asCALL_CDECL_OBJFIRST);
        r = engine->RegisterObjectMethod(engine::script_type<sf::Vector2i>().c_str(),
            "bool opEquals(const Vector2&in) const",
            asFUNCTION(iEqU), asCALL_CDECL_OBJFIRST);
        engine->RegisterObjectMethod(engine::script_type<sf::Vector2i>().c_str(),
            "string toString() const",
            asFUNCTION(AWEVector2iTypeToString), asCALL_CDECL_OBJLAST);

        // Vector2 opEquals
        r = engine->RegisterObjectMethod(engine::script_type<sf::Vector2u>().c_str(),
            "bool opEquals(const Vector2&in) const",
            asFUNCTION(uEqU), asCALL_CDECL_OBJFIRST);
        r = engine->RegisterObjectMethod(engine::script_type<sf::Vector2u>().c_str(),
            "bool opEquals(const MousePosition&in) const",
            asFUNCTION(uEqI), asCALL_CDECL_OBJFIRST);

        // Vector2 opAdd
        r = engine->RegisterObjectMethod(engine::script_type<sf::Vector2u>().c_str(),
            "Vector2 opAdd(const MousePosition&in) const",
            asFUNCTION(uAddI), asCALL_CDECL_OBJFIRST);

        r = engine->RegisterObjectType(engine::script_type<sf::Vector2f>().c_str(),
            sizeof(sf::Vector2f),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Vector2f>());
        engine->RegisterObjectProperty(engine::script_type<sf::Vector2f>().c_str(),
            "float x", asOFFSET(sf::Vector2f, x));
        engine->RegisterObjectProperty(engine::script_type<sf::Vector2f>().c_str(),
            "float y", asOFFSET(sf::Vector2f, y));
        engine->RegisterObjectBehaviour(engine::script_type<sf::Vector2f>().c_str(),
            asBEHAVE_CONSTRUCT, "void Vector2f(const float, const float)",
            asFUNCTION(AWEVector2fTypeConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour(engine::script_type<sf::Vector2f>().c_str(),
            asBEHAVE_CONSTRUCT, "void Vector2f(const MousePosition&in)",
            asFUNCTION(AWEVector2fTypeConstructorFromVector2i),
            asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod(engine::script_type<sf::Vector2f>().c_str(),
            "string toString() const",
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
    if (!engine->GetTypeInfoByName(engine::script_type<sf::IntRect>().c_str())) {
        auto r = engine->RegisterObjectType(engine::script_type<sf::IntRect>().c_str(),
            sizeof(sf::IntRect),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::IntRect>());
        engine->RegisterObjectProperty(engine::script_type<sf::IntRect>().c_str(),
            "int left", asOFFSET(sf::IntRect, left));
        engine->RegisterObjectProperty(engine::script_type<sf::IntRect>().c_str(),
            "int top", asOFFSET(sf::IntRect, top));
        engine->RegisterObjectProperty(engine::script_type<sf::IntRect>().c_str(),
            "int width", asOFFSET(sf::IntRect, width));
        engine->RegisterObjectProperty(engine::script_type<sf::IntRect>().c_str(),
            "int height", asOFFSET(sf::IntRect, height));
        engine->RegisterObjectBehaviour(engine::script_type<sf::IntRect>().c_str(),
            asBEHAVE_CONSTRUCT,
            "void IntRect(const int, const int, const int, const int)",
            asFUNCTION(AWEIntRectTypeConstructor), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod(engine::script_type<sf::IntRect>().c_str(),
            "string toString() const",
            asFUNCTION(AWEIntRectTypeToString), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectMethod(engine::script_type<sf::IntRect>().c_str(),
            "bool contains(const MousePosition&in) const",
            asMETHODPR(sf::IntRect, contains, (const sf::Vector2i&) const, bool),
            asCALL_THISCALL);
        document->DocumentObjectType(r, "Represents a rectangle.");
    }
}

void engine::RegisterTimeTypes(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    if (!engine->GetTypeInfoByName(engine::script_type<sf::Time>().c_str())) {
        // Time class.
        auto r = engine->RegisterObjectType(engine::script_type<sf::Time>().c_str(),
            sizeof(sf::Time),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Time>());
        document->DocumentObjectType(r, "Represents a time value.");
        r = engine->RegisterObjectMethod(engine::script_type<sf::Time>().c_str(),
            "float asSeconds()",
            asMETHOD(sf::Time, asSeconds), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Return the time value as a number of "
            "seconds.");
        r = engine->RegisterObjectMethod(engine::script_type<sf::Time>().c_str(),
            "int32 asMilliseconds()",
            asMETHOD(sf::Time, asMilliseconds), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Return the time value as a number of "
            "milliseconds.");
        r = engine->RegisterObjectMethod(engine::script_type<sf::Time>().c_str(),
            "int64 asMicroseconds()",
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
        r = engine->RegisterObjectType(engine::script_type<sf::Clock>().c_str(),
            sizeof(sf::Clock),
            asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Clock>());
        document->DocumentObjectType(r, "Used to calculate elapsed time.");
        r = engine->RegisterObjectMethod(engine::script_type<sf::Clock>().c_str(),
            "Time getElapsedTime()",
            asMETHOD(sf::Clock, getElapsedTime), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Calculates the elapsed time since the "
            "clock was constructed or since <tt>restart()</tt> was called.");
        r = engine->RegisterObjectMethod(engine::script_type<sf::Clock>().c_str(),
            "Time restart()",
            asMETHOD(sf::Clock, restart), asCALL_THISCALL);
        document->DocumentObjectMethod(r, "Restarts the clock. Returns the time "
            "elapsed.");
    }
}

void engine::RegisterStreamTypes(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    if (!engine->GetTypeInfoByName(
        engine::script_type<engine::binary_istream>().c_str())) {
        auto r = engine->RegisterObjectType(
            engine::script_type<engine::binary_istream>().c_str(), 0,
            asOBJ_REF | asOBJ_NOCOUNT);
        document->DocumentObjectType(r, "Represents an input stream of binary "
            "data.");
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(int8&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int8&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(int16&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int16&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(int32&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int32&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(int64&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Int64&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(uint8&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint8&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(uint16&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint16&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(uint32&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint32&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(uint64&out)",
            asMETHODPR(engine::binary_istream, readNumber, (sf::Uint64&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(bool&out)",
            asMETHODPR(engine::binary_istream, readBool, (bool&), void),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_istream>().c_str(), "void read(string&out)",
            asMETHODPR(engine::binary_istream, readString, (std::string&), void),
            asCALL_THISCALL);

        r = engine->RegisterObjectType(
            engine::script_type<engine::binary_ostream>().c_str(), 0,
            asOBJ_REF | asOBJ_NOCOUNT);
        document->DocumentObjectType(r, "Represents an output stream of binary "
            "data.");
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const int8)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int8>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const int16)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int16>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const int32)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int32>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const int64)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Int64>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const uint8)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint8>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const uint16)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint16>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const uint32)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint32>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const uint64)",
            asMETHOD(engine::binary_ostream, writeNumber<sf::Uint64>),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const bool)",
            asMETHOD(engine::binary_ostream, writeBool),
            asCALL_THISCALL);
        r = engine->RegisterObjectMethod(
            engine::script_type<engine::binary_ostream>().c_str(),
            "void write(const string&in)",
            asMETHOD(engine::binary_ostream, writeString), asCALL_THISCALL);
    }
}

const std::array<const std::string, 2> engine::scripts::modules = {
    "ComputerWars",
    "BankOverrides"
};

engine::scripts::scripts(const engine::logger::data& data) :
    json_script({ data.sink, "json_script" }), _logger(data) {
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

std::string engine::scripts::executeCode(std::string code,
    std::string moduleName) {
    static const auto Log = [&](const std::string& m) -> std::string {
        _logger.error(m); return m;
    };
    if (moduleName.empty()) moduleName = modules[MAIN];
    if (!doesModuleExist(moduleName))
        return Log(fmt::format("Cannot executeCode() in non-existent module "
            "\"{}\"!", moduleName));
    auto m = _engine->GetModule(moduleName.c_str());
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
        return Log(fmt::format("The module \"{}\" does not exist!", moduleName));
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
    auto m = _engine->GetModule(modules[MAIN].c_str());
    if (!m) {
        _logger.error("Could not create object of type \"{}\" as the module "
            "\"{}\" does not exist.", type, modules[MAIN]);
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
    const std::string& moduleName, const std::string& interfaceName) const {
    auto m = _engine->GetModule(moduleName.c_str());
    if (!m) {
        _logger.error("Cannot get concrete classes that implement interface "
            "\"{}\" as the \"{}\" module does not exist.", interfaceName,
            moduleName);
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
    if (IsCoreModule(name)) {
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
            static_cast<unsigned int>(file.second.size()));
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
    if (IsCoreModule(name)) {
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

engine::scripts::global_function_metadata
    engine::scripts::getGlobalFunctionMetadata(
        const std::string& moduleName) const {
    if (!_engine->GetModule(moduleName.c_str())) {
        _logger.error("Could not return function metadata of non-existent module "
            "\"{}\".", moduleName);
        return {};
    }
    return _functionMetadata.at(moduleName);
}

engine::scripts::global_variable_metadata
    engine::scripts::getGlobalVariableMetadata(
    const std::string& moduleName) const {
    if (!_engine->GetModule(moduleName.c_str())) {
        _logger.error("Could not return variable metadata of non-existent module "
            "\"{}\".", moduleName);
        return {};
    }
    return _variableMetadata.at(moduleName);
}

engine::scripts::global_functions_and_their_namespaces
    engine::scripts::getGlobalFunctionsAndTheirNamespaces(
    const std::string& moduleName) const {
    if (!_engine->GetModule(moduleName.c_str())) {
        _logger.error("Could not return the namespaces of functions within "
            "non-existent module \"{}\".", moduleName);
        return {};
    }
    return _functionNamespaces.at(moduleName);
}

engine::scripts::global_variables_and_their_namespaces
    engine::scripts::getGlobalVariablesAndTheirNamespaces(
    const std::string& moduleName) const {
    if (!_engine->GetModule(moduleName.c_str())) {
        _logger.error("Could not return the namespaces of variables within "
            "non-existent module \"{}\".", moduleName);
        return {};
    }
    return _variableNamespaces.at(moduleName);
}

bool engine::scripts::getGlobalVariable(const std::string& moduleName,
    const asUINT variable, std::string& name, int& typeID) const {
    const auto m = _engine->GetModule(moduleName.c_str());
    if (!m) {
        _logger.error("Could not return the properties of global variable {} "
            "within non-existent module \"{}\".", variable, moduleName);
        return false;
    }
    const char* tempName = nullptr;
    int tempTypeID = 0;
    if (auto r = m->GetGlobalVar(variable, &tempName, nullptr, &tempTypeID)) {
        _logger.error("Could not return the properties of global variable {} "
            "within module \"{}\", error code: {}.", variable, moduleName, r);
        return false;
    }
    if (!tempName) {
        _logger.error("Could not return the name of global variable {} within "
            "module \"{}\", that has type ID {}.", variable, moduleName,
            tempTypeID);
        return false;
    }
    name = tempName;
    typeID = tempTypeID;
    return true;
}

void* engine::scripts::getGlobalVariableAddress(const std::string& moduleName,
    const asUINT variable) const {
    const auto m = _engine->GetModule(moduleName.c_str());
    if (!m) {
        _logger.error("Could not return the address of global variable {} within "
            "non-existent module \"{}\".", variable, moduleName);
        return nullptr;
    }
    void* const v = m->GetAddressOfGlobalVar(variable);
    if (!v) {
        _logger.error("Could not return the address of non-existent global "
            "variable {} within module \"{}\".", variable, moduleName);
    }
    return v;
}

static const auto EVAL_ASSERTS = "__evaluate_assertions__";

bool engine::scripts::evaluateAssertions() {
    for (const auto& mName : engine::scripts::modules) {
        if (!doesModuleExist(mName)) {
            _logger.error("Couldn't evaluate module \"{}\"'s assertions because "
                "the module does not exist.", mName);
            continue;
        }
        _logger.write("Evaluating module \"{}\"'s assertions...", mName);
        bool res = false;
        std::string code, helpText;
        callFunction(mName, EVAL_ASSERTS, &res, &code, &helpText);
        if (!res) {
            _logger.error("Assertion {} failed! {}", code, helpText);
            return false;
        }
    }
    _logger.write("All assertions passed!");
    return true;
}

bool engine::scripts::_load(engine::json& j) {
    // First check if the interface has been registered, and if not, register it.
    if (!_registrants.empty()) {
        _logger.write("Registering the script interface...");
        for (auto& reg : _registrants) reg->registerInterface(_engine, _document);
        _logger.write("Finished registering the script interface.");
        _registrants.clear();
    }
    // Next, load the path of the folder containing all of the scripts to load for
    // each module.
    std::array<std::string, modules.size()> paths;
    for (std::size_t i = 0; i < paths.size(); ++i) {
        j.apply(paths[i], { modules[i] });
        if (!j.inGoodState()) {
            _logger.error("Will not discard old script modules; no folder was "
                "given for module \"{}\".", modules[i]);
            return false;
        }
    }
    // Clear the metadata and namespace containers, as we are now going to discard
    // the old modules.
    _clearState();
    // Now load each module, automatically discarding the previous version of each.
    for (std::size_t i = 0; i < paths.size(); ++i) {
        if (!_loadScripts(modules[i].c_str(), paths[i])) {
            _clearTemplatesAndAssertions();
            return false;
        }
        // Templates and assertions are not shared between modules.
        _clearTemplatesAndAssertions();
    }
    return true;
}

// ADAPTATION OF ANGELSCRIPT CODE TAKEN FROM scriptstdstring_utils.cpp
// This function takes an input string and splits it into parts by looking
// for a specified delimiter. Example:
//
// string str = "A|B||D";
// array<string>@ array = str.split("|");
//
// The resulting array has the following elements:
//
// {"A", "B", "", "D"}
//
// If str is empty, a vector with a single empty string should be returned.
// If delim is empty, the string should be split up character by character.
static std::vector<std::string> stringSplit(const std::string& str,
    const std::string& delim) {
    std::vector<std::string> vec;
    std::size_t pos = 0, prev = 0;
    while ((pos = str.find(delim, prev)) != std::string::npos) {
        vec.emplace_back(str.substr(prev, pos - prev));
        prev = pos + delim.length();
    }
    vec.emplace_back(str.substr(prev));
    return vec;
}

bool engine::scripts::_loadScripts(const char* const moduleName,
    const std::string& folder) {
    // Now load the scripts.
    _logger.write("Loading scripts from \"{}\" for module \"{}\"...", folder,
        moduleName);
    if (folder == "" || !_engine) {
        _logger.error("Will not build module \"{}\" as given folder was empty or "
            "the script engine was not initialised.", moduleName);
        return false;
    }
    // Before starting a new module, if it already exists, the old module should be
    // discarded before being replaced. If it doesn't yet exist, then the negative
    // value returned is ignored.
    _engine->DiscardModule(moduleName);
    int r = _builder.StartNewModule(_engine, moduleName);
    if (r < 0) {
        _logger.error("Failed to start the \"{}\" module while loading scripts: "
            "code {}.", moduleName, r);
        return false;
    }
    try {
        std::function<bool(const std::string&)> directoryIterator =
            [&](const std::string& dir) -> bool {
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                const auto path = entry.path().string();
                if (entry.is_regular_file()) {
                    std::string scriptFile;
                    {
                        std::ifstream file(path);
                        scriptFile = _parseDirectives(path, file);
                    }
                    if (scriptFile.empty()) continue;
                    if ((r = _builder.AddSectionFromMemory(path.c_str(),
                        scriptFile.c_str(),
                        static_cast<unsigned int>(scriptFile.size())) < 0)) {
                        _logger.error("Failed to add script \"{}\" to the \"{}\" "
                            "module: code {}.", path, moduleName, r);
                        return false;
                    }
                } else if (entry.is_directory()) {
                    if (!directoryIterator(path)) return false;
                }
            }
            return true;
        };
        if (!directoryIterator(folder)) return false;
    } catch (const std::exception& e) {
        _logger.error("Failed to interact with directory entry: {}.", e);
    }
    if (!_instantiateTemplatesInQueue()) return false;
    assert(_templateInstantiationQueue.empty());
    // ...Don't ask why I did assertions like this and not using ExecuteString or
    // executeCode()... I didn't just spend the past however many minutes trying to
    // check countries.length() instead of country.length() in an assertion and
    // then scratching my head, wondering why ExecuteString couldn't access the
    // engine's interface... Nope, I didn't just do that.
    _logger.write("Compiling {} assertion{}...", _assertionQueue.size(),
        (_assertionQueue.size() == 1 ? "" : "s"));
    std::string asserts = fmt::format("void {}(bool&out res, "
        "string&out code, string&out helpText) {{", EVAL_ASSERTS);
    for (const auto& assertion : _assertionQueue) {
        asserts += fmt::format("if (!({})) {{ code = \"{}\"; helpText = {}; "
            "return; }}", assertion.code, assertion.code, assertion.helpText);
    }
    asserts += "res = true;}";
    if ((r = _builder.AddSectionFromMemory(EVAL_ASSERTS, asserts.c_str(),
        static_cast<unsigned int>(asserts.size())) < 0)) {
        _logger.error("Failed to add assertion evaluation code to the \"{}\" "
            "module: code {}. The code follows:\n{}", moduleName, r, asserts);
        return false;
    }
    if ((r = _builder.BuildModule()) < 0) {
        _logger.error("Failed to build the \"{}\" module: code {}. Here is the "
            "assertion code:\n{}", moduleName, r, asserts);
        _engine->DiscardModule(moduleName);
        return false;
    }
    const auto m = _builder.GetModule();
    _logger.write("Loading metadata and namespaces for module \"{}\"...",
        moduleName);
    _functionNamespaces[moduleName] = {};
    for (asUINT i = 0, len = m->GetFunctionCount(); i < len; ++i) {
        const auto func = m->GetFunctionByIndex(i);
        _functionNamespaces[moduleName][func] = {};
        const auto namespaces = func->GetNamespace();
        if (namespaces) {
            _functionNamespaces[moduleName][func] = stringSplit(namespaces, "::");
            // If there's one empty string element in the vector, delete it, as
            // this indicates it's within the global namespace.
            if (_functionNamespaces[moduleName][func].size() == 1 &&
                _functionNamespaces[moduleName][func][0].empty())
                _functionNamespaces[moduleName][func].clear();
        }
        const auto data = _builder.GetMetadataForFunc(func);
        if (!data.empty()) {
            _functionMetadata[moduleName][func].declaration =
                func->GetDeclaration();
            _functionMetadata[moduleName][func].metadata = data;
        }
    }
    _variableNamespaces[moduleName] = {};
    const char* namespaces = nullptr;
    for (asUINT i = 0, len = m->GetGlobalVarCount(); i < len; ++i) {
        _variableNamespaces[moduleName][i] = {};
        m->GetGlobalVar(i, nullptr, &namespaces);
        if (namespaces) {
            _variableNamespaces[moduleName][i] = stringSplit(namespaces, "::");
            // If there's one empty string element in the vector, delete it, as
            // this indicates it's within the global namespace.
            if (_variableNamespaces[moduleName][i].size() == 1 &&
                _variableNamespaces[moduleName][i][0].empty())
                _variableNamespaces[moduleName][i].clear();
        }
        const auto data = _builder.GetMetadataForVar(i);
        if (!data.empty()) {
            _variableMetadata[moduleName][i].declaration =
                m->GetGlobalVarDeclaration(i);
            _variableMetadata[moduleName][i].metadata = data;
        }
    }
    _logger.write("Finished loading scripts for module \"{}\".", moduleName);
    return true;
}

static constexpr std::size_t SUBSECTIONS = 6;

static const auto PARAMETER = '$';

static const auto COALESCE = '\\';

static const auto LOOP = '`';

static const std::string NUMBERS = "0123456789";

static const std::regex LOOP_INDEX("\\$i|\\$I");

bool engine::scripts::_instantiateTemplatesInQueue() {
    // More requests may be added to the queue during the handling of previous
    // requests, so don't use a for loop.
    while (!_templateInstantiationQueue.empty()) {
        const std::string templateName =
            _templateInstantiationQueue.begin()->first;
        if (_templates.count(templateName) == 0) {
            _logger.error("Could not find template with the name \"{}\".",
                templateName);
            _templateInstantiationQueue.erase(templateName);
            continue;
        }
        const std::string templateScript = _templates[templateName];
        if (std::ranges::count(templateScript, LOOP) % SUBSECTIONS) {
            _logger.error("BUILD FAILED. There are an incorrect number of loop "
                "control characters (\"{}\") in the \"{}\" template, which means "
                "a loop construct within the template is incomplete.", LOOP,
                templateName);
            return false;
        }
        while (!_templateInstantiationQueue.begin()->second.empty()) {
            // Let's be safe and copy even the parameters, as well as the name and
            // template itself. There is a chance that adding elements to
            // _templateInstantiationQueue whilst working with it could invalidate
            // references [if it reallocates]? Not sure I want to take that chance.
            const auto params(
                *_templateInstantiationQueue.begin()->second.begin());
            if (!_instantiateTemplate(templateName, templateScript, params))
                return false;
            // We're finished with this particular instantiation now, so erase it.
            _templateInstantiationQueue.begin()->second.erase(params);
        }
        // We're now finished with this template. Both it and its previous
        // instantiations could come back, but I'm pretty sure they can never come
        // back recursively...
        _templateInstantiationQueue.erase(templateName);
    }
    return true;
}

bool engine::scripts::_instantiateTemplate(const std::string& templateName,
    const std::string& templateScript,
    const std::vector<std::string>& parameters) {
    const auto fullName = fmt::format("{} {}", templateName,
        engine::json::synthesiseKeySequence(parameters));
    _logger.write("Instantiating template: {}", fullName);
    std::string script, repeatedSection;
    sf::Int64 start = 0, stop = 0, step = 0;
    const auto sections = stringSplit(templateScript, std::string(1, LOOP));
    for (std::size_t i = 0, end = sections.size(); i < end; ++i) {
        const auto sectionType = i % SUBSECTIONS;
        switch (sectionType) {
        case 0:
            // [0], [6], [12], ... = Normal substitution.
            script += _normalSubstitution(sections[i], parameters);
            break;
        case 1:
            // [1], [7], [13], ... = Set the start index of the next loop.
            start = _readInt(sections[i], parameters, "start index");
            break;
        case 2:
            // [2], [8], [14], ... = Set the stop index of the next loop.
            stop = _readInt(sections[i], parameters, "stop index");
            break;
        case 3:
            // [3], [9], [15], ... = Set the step of the next loop.
            step = _readInt(sections[i], parameters, "step value");
            if (step == 0) {
                _logger.warning("A step value of 0 is invalid! Using 1...");
                step = 1;
            }
            if ((step > 0 && start > stop) ||
                (step < 0 && start < stop) ||
                (start == stop)) {
                _logger.warning("A start index of {}, a stop index of {}, and a "
                    "step value of {} will result in a loop that is not expanded.",
                    start, stop, step);
            }
            break;
        case 4:
            // [4], [10], [16], ... = Main body of the loop substitution.
            repeatedSection = sections[i];
            break;
        case 5:
            // [5], [11], [17], ... = End body of the loop substitution.
            script += _loopSubtitution(repeatedSection, sections[i], parameters,
                start, stop, step);
            break;
        }
    }
    // Add the final script to the builder.
    std::stringstream emulateFile;
    emulateFile << script;
    script = _parseDirectives(fullName, emulateFile);
    if (script.empty()) {
        // It's possible to create more templates using a template, but there's no
        // way for #expand to instantiate them, so it will just store it, or
        // replace itself, but that won't have any effect because the template is
        // copied when it's extracted from the _templates map.
        return true;
    }
    // Always dump templates to help the user debug any issues, even if the build
    // succeeds.
    _logger.write("Adding the template \"{}\" to the builder for this "
        "module...\n{}", fullName, script);
    const auto r = _builder.AddSectionFromMemory(fullName.c_str(),
        script.c_str(), static_cast<unsigned int>(script.size()));
    if (r == 0) {
        _logger.error("When instantiating template: section \"{}\" already "
            "existed!", fullName);
    } else if (r < 0) {
        _logger.error("Failed to add instantiated template \"{}\" to engine: code "
            "{}.", fullName, r);
        return false;
    }
    return true;
}

std::string engine::scripts::_normalSubstitution(const std::string& section,
    const std::vector<std::string>& parameters) {
    std::string result, number;
    bool readingParam = false;
    static const auto substituteParameter = [](const char* chr,
        bool& readingParam, std::string& number, std::string& result,
        const std::vector<std::string>& parameters,
        const engine::scripts* const scripts) -> void {
    };
    for (const auto chr : section) {
        if (!readingParam) {
            _normalSubstitution_parseNormalChar(chr, readingParam, number, result);
        } else {
            if (NUMBERS.find(chr) != std::string::npos) {
                // We're reading a parameter, and a digit has been given.
                // Append it to our running list of digits.
                number += chr;
            } else {
                _normalSubstitution_substituteParameter(&chr, readingParam, number,
                    result, parameters);
            }
        }
    }
    // If we were still reading a number once we reached the end of the string,
    // don't neglect to substitute a variable if possible.
    if (readingParam) _normalSubstitution_substituteParameter(nullptr,
        readingParam, number, result, parameters);
    return result;
}

void engine::scripts::_normalSubstitution_parseNormalChar(const char chr,
    bool& readingParam, std::string& number, std::string& result) {
    if (chr == PARAMETER) {
        // If we're not already reading a parameter, and we come across $,
        // start reading a parameter.
        readingParam = true;
        number.clear();
    } else {
        // Add the character as normal.
        result += chr;
    }
}

void engine::scripts::_normalSubstitution_substituteParameter(const char* chr,
    bool& readingParam, std::string& number, std::string& result,
    const std::vector<std::string>& parameters) {
    // We've come across a non-positive digit
    // (or the end of the string if chr == nullptr),
    // so stop reading our number now.
    // Attempt to subtitute a parameter in its place.
    readingParam = false;
    if (number.empty()) {
        // If there was just a $ on its own, warn the user, but leave it in.
        result += PARAMETER;
        // Don't forget to write the non-digit!
        if (chr) _normalSubstitution_parseNormalChar(*chr, readingParam,
            number, result);
        _logger.warning("Stray {} was found, inserting it into the final "
            "instantiation...", PARAMETER);
        return;
    }
    const std::size_t paramIndex = std::stoull(number);
    if (paramIndex == 0) {
        // $0 == the number of parameters.
        result += std::to_string(parameters.size());
    } else if (paramIndex <= parameters.size()) {
        // $n == nth parameter.
        result += parameters[paramIndex - 1];
    } else {
        // Substitution didn't work. Leave $n as is and log an error.
        result += PARAMETER;
        result += number;
        _logger.error("Attempted to substitute \"{}{}\", but the number of "
            "parameters given was {}.", PARAMETER, number, parameters.size());
    }
    // Don't forget to write the non-digit!
    if (chr) _normalSubstitution_parseNormalChar(*chr, readingParam,
        number, result);
}

sf::Int64 engine::scripts::_readInt(std::string from,
    const std::vector<std::string>& parameters, const char* const which) {
    from.erase(std::remove_if(from.begin(), from.end(), ::isspace), from.end());
    // First, see if '$' on its own was given.
    // If it was, return the number of parameters.
    if (from == std::string(1, PARAMETER)) {
        return static_cast<sf::Int64>(parameters.size() + 1);
    }
    // Assume it's an ordinary integer.
    try {
        return std::stoll(from);
    } catch (const std::invalid_argument&) {
        _logger.error("\"{}\" is an invalid {} for a loop construct! Using 0...",
            from, which);
    } catch (const std::out_of_range&) {
        _logger.error("\"{}\" is out-of-range as a {} for a loop construct! Using "
            "0...", from, which);
    }
    return 0;
}

std::string engine::scripts::_loopSubtitution(const std::string& section,
    const std::string& endSection, const std::vector<std::string>& parameters,
    sf::Int64 start, sf::Int64 stop, sf::Int64 step) {
    if (start == stop) return "";
    std::string result;
    for (sf::Int64 i = start;
        (step > 0 && i < stop) || (step < 0 && i > stop);
        i += step) {
        std::string sectionToEvaluate = section;
        // If this isn't the last iteration, include the endSection, as well.
        if ((step > 0 && (i + step) < stop) || (step < 0 && (i + step) > stop)) {
            sectionToEvaluate += endSection;
        }
        // First pass through: replace all instances of $i with i.
        const auto newSection = std::regex_replace(sectionToEvaluate, LOOP_INDEX,
            std::to_string(i), std::regex_constants::match_flag_type::match_any);
        // Second pass through: normal substitution.
        result += _normalSubstitution(newSection, parameters);
    }
    return result;
}

static const std::string WHITESPACE = " \t\n\r\f\v";

static const std::string TEMPLATE_DIRECTIVE = "#template";

static const std::string INSTANTIATE_DIRECTIVE = "#expand";

static const std::string ASSERT_DIRECTIVE = "#assert";

std::string engine::scripts::_parseDirectives(const std::string& filePath,
    std::istream& file) {
    _logger.write("Reading script file \"{}\" for directives...", filePath);
    if (!file) {
        _logger.error("The input stream for this file could not be opened!");
        return {};
    }
    std::string scriptFile;
    // If an exception occurs, don't add to the actual queues.
    // (A template will only ever be recorded if it could be read successfully,
    // so no need to store it in a temporary variable).
    auto templateInstantiationQueueCopy = _templateInstantiationQueue;
    auto assertionQueueCopy = _assertionQueue;
    file.exceptions(std::ios::failbit | std::ios::badbit);
    try {
        std::string templateName;
        std::size_t lineNumber = 0;
        // If std::getline() can't extract any characters for any reason, it will
        // set the fail bit. This is problematic because it will set the bit even
        // if it's reached the end of the file, and there's a blank line at the
        // very end. There are no characters after the final newline, so it will
        // panic. Luckily we can just peek the next character, and if it's EOF, we
        // can prevent getline from throwing in this circumstance.
        while (file.good() && file.peek() != EOF) {
            std::string line, directive, directiveText;
            std::getline(file, line);

            ++lineNumber;

            // If we're reading a template, just read each line straight into
            // scriptFile. We must preserve newline characters in case there are
            // single line comments.
            if (!templateName.empty()) {
                scriptFile += line + "\n";
                continue;
            }
            
            // Otherwise, look for directives. If the first non-whitespace
            // character is a #, it's a directive.
            const auto firstNonWhitespace = line.find_first_not_of(WHITESPACE);
            const auto isDirective = firstNonWhitespace != std::string::npos &&
                line[firstNonWhitespace] == '#';
            
            if (isDirective) {
                const auto endOfDirective = line.find_first_of(WHITESPACE,
                    firstNonWhitespace);
                if (endOfDirective == std::string::npos) {
                    directive = "#";
                } else {
                    directive = line.substr(firstNonWhitespace,
                        endOfDirective - firstNonWhitespace);
                    directiveText = line.substr(line.find_first_not_of(WHITESPACE,
                        endOfDirective));
                }

                if (directive == TEMPLATE_DIRECTIVE) {
                    _logger.write("Line {}: found {} directive.", lineNumber,
                        TEMPLATE_DIRECTIVE);
                    templateName = _parseTemplateDirective(directiveText,
                        lineNumber);
                    if (!templateName.empty()) continue;
                } else if (directive == INSTANTIATE_DIRECTIVE) {
                    _logger.write("Line {}: found {} directive.", lineNumber,
                        INSTANTIATE_DIRECTIVE);
                    const auto templateNameAndParameters =
                        _parseInstantiateDirective(directiveText, lineNumber);
                    if (!templateNameAndParameters.first.empty()) {
                        if (templateInstantiationQueueCopy[
                            templateNameAndParameters.first].count(
                                templateNameAndParameters.second)) {
                            _logger.warning("Line {}: this template instantiation "
                                "has already been queued!", lineNumber);
                        }
                        templateInstantiationQueueCopy[
                            templateNameAndParameters.first].insert(
                                templateNameAndParameters.second);
                        continue;
                    }
                } else if (directive == ASSERT_DIRECTIVE) {
                    _logger.write("Line {}: found {} directive.", lineNumber,
                        ASSERT_DIRECTIVE);
                    const auto assertion = _parseAssertDirective(directiveText,
                        lineNumber);
                    if (!assertion.code.empty()) {
                        assertionQueueCopy.push_back(assertion);
                        continue;
                    }
                }

                // If we've reached this point, the directive was invalid. Don't
                // add it to the final script file (we never add them anyway).
                _logger.warning("Invalid directive line {} is being excluded from "
                    "the build: {}", lineNumber, line);
            } else {
                // If there's no directive, it's a normal script line, add it.
                // We must preserve newline characters in case there are single
                // line comments.
                scriptFile += line + "\n";
            }
        }

        // If we just read a template, store it separately and do not add it to the
        // build.
        if (!templateName.empty()) {
            _templates[templateName] = scriptFile;
            scriptFile.clear();
        }
    } catch (const std::exception& e) {
        _logger.error("Couldn't read script file \"{}\", will not be adding it to "
            "the builder: {}. Any queued assertions and template instantiations "
            "from this script will not be evaluated.", filePath, e.what());
        return {};
    }
    _templateInstantiationQueue = std::move(templateInstantiationQueueCopy);
    _assertionQueue = std::move(assertionQueueCopy);
    return scriptFile;
}

std::string engine::scripts::_parseTemplateDirective(
    const std::string& directiveText, const std::size_t lineNumber) {
    if (lineNumber != 1) {
        _logger.warning("Line {}: {} directives after the first line have no "
            "effect.", lineNumber, TEMPLATE_DIRECTIVE);
        return {};
    }
    const auto endOfTemplateName = directiveText.find_first_of(WHITESPACE);
    const auto templateName = directiveText.substr(0, endOfTemplateName);
    if (templateName.empty()) {
        _logger.error("Line {}: no name given to template, this script will be "
            "treated like a normal script.", lineNumber);
        return {};
    }
    if (endOfTemplateName != std::string::npos &&
        directiveText.find_first_not_of(WHITESPACE, endOfTemplateName)
        != std::string::npos) {
        _logger.warning("Line {}: extra characters given to {} directive, these "
            "will be ignored: {}", lineNumber, TEMPLATE_DIRECTIVE,
            directiveText.substr(endOfTemplateName));
    }
    _logger.write("Line {}: this script is a template with the name {}.",
        lineNumber, templateName);
    if (_templates.count(templateName)) {
        _logger.warning("This will replace an existing template with the same "
            "name if it's loaded successfully!");
    }
    return templateName;
}

std::pair<std::string, std::vector<std::string>>
    engine::scripts::_parseInstantiateDirective(const std::string& directiveText,
        const std::size_t lineNumber) {
    const auto endOfTemplateName = directiveText.find_first_of(WHITESPACE);
    const auto templateName = directiveText.substr(0, endOfTemplateName);
    if (templateName.empty()) {
        _logger.error("Line {}: no template name given to {} directive.",
            lineNumber, INSTANTIATE_DIRECTIVE);
        return {};
    }
    // Now parse parameters. To keep this simple, let's assume parameters are only
    // separated by spaces, starting with the second character after the end of the
    // template name.
    // To include spaces within a parameter, simply prefix the space with a \.
    if (endOfTemplateName == std::string::npos ||
        directiveText.size() - endOfTemplateName < 1) {
        _logger.write("Line {}: this script will attempt to instantiate template "
            "{} with no parameters. To add parameters, insert a space after the "
            "template name, then write them out, each separated by a single "
            "space. Blank parameters are permitted.", lineNumber, templateName);
        return std::make_pair(templateName, std::vector<std::string>{});
    }
    // Now split the rest of the directive text into words delimited by spaces.
    auto words = stringSplit(directiveText.substr(endOfTemplateName + 1), " ");
    // Coalesce elements together if the element on the left ends with a \.
    for (std::size_t i = 0; i < words.size() - 1; ++i) {
        if (words[i].empty()) continue;
        if (*words[i].rbegin() == COALESCE) {
            words[i].erase(words[i].size() - 1);
            words[i] += words[i + 1];
            auto itr = words.begin();
            std::advance(itr, i + 1);
            words.erase(itr);
            // We'll need to reevaluate the current element in case it coalesced
            // with an element that had a \ at the end.
            --i;
        }
    }
    _logger.write("Line {}: this script will attempt to instantiate template {} "
        "with these parameters: {}.", lineNumber, templateName,
        engine::json::synthesiseKeySequence(words));
    return std::make_pair(templateName, words);
}

engine::scripts::assertion engine::scripts::_parseAssertDirective(
    const std::string& directiveText, const std::size_t lineNumber) {
    const auto startOfCode = directiveText.find_first_not_of(WHITESPACE);
    if (startOfCode == std::string::npos) {
        _logger.error("Line {}: {} directive was given no code to evaluate.",
            lineNumber, ASSERT_DIRECTIVE);
        return {};
    }
    const auto endOfCode = directiveText.find(';');
    if (endOfCode == std::string::npos) {
        _logger.warning("Line {}: {} directive's code is not terminated with a "
            "';'. No help text will be displayed if the assertion fails.",
            lineNumber, ASSERT_DIRECTIVE);
    }
    assertion ret;
    // Excludes the terminating ';'.
    ret.code = directiveText.substr(startOfCode, endOfCode);
    if (endOfCode != std::string::npos) {
        auto startOfHelpText = directiveText.find_first_not_of(WHITESPACE,
            endOfCode + 1);
        if (startOfHelpText != std::string::npos) {
            ret.helpText = directiveText.substr(startOfHelpText);
            const auto endOfHelpText = ret.helpText.find_last_not_of(WHITESPACE);
            if (endOfHelpText != std::string::npos) {
                ret.helpText = ret.helpText.substr(0, endOfHelpText + 1);
            }
            // Now check for quotes '"'. If the help text isn't surrounded by them,
            // insert them. If there are any other quotes within the string, escape
            // them.
            if (*ret.helpText.begin() != '"') {
                _logger.warning("Line {}: {} directive's help text didn't start "
                    "with a quote (\")! Adding one...", lineNumber,
                    ASSERT_DIRECTIVE);
                ret.helpText = "\"" + ret.helpText;
            }
            if (*ret.helpText.rbegin() != '"' || (ret.helpText.size() == 1 &&
                *ret.helpText.rbegin() == '"')) {
                _logger.warning("Line {}: {} directive's help text didn't end "
                    "with a quote (\")! Adding one...", lineNumber,
                    ASSERT_DIRECTIVE);
                ret.helpText += "\"";
            }
            std::size_t escapedQuotes = 0;
            assert(ret.helpText.size() >= 2);
            for (std::size_t i = ret.helpText.size() - 2; i > 0; --i) {
                if (ret.helpText[i] == '"') {
                    ++escapedQuotes;
                    ret.helpText.insert(i, 1, '\\');
                }
            }
            if (escapedQuotes) {
                _logger.warning("Line {}: found quote{} (\") within help text of "
                    "{} directive! Escaping them...", lineNumber,
                    (escapedQuotes == 1 ? "" : "s"), ASSERT_DIRECTIVE);
            }
        } else {
            _logger.warning("Line {}: {} directive was given no help text after "
                "the terminating ';'.", lineNumber, ASSERT_DIRECTIVE);
        }
    }
    _logger.write("Line {}: this script asserts that ({}) is TRUE. It has {}",
        lineNumber, ret.code, (ret.helpText.empty() ? "no help text." :
            std::string("the following help text: ").append(ret.helpText)));
    return ret;
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

void engine::scripts::_clearState() noexcept {
    _functionMetadata.clear();
    _variableMetadata.clear();
    _functionNamespaces.clear();
    _variableNamespaces.clear();
    _clearTemplatesAndAssertions();
}

void engine::scripts::_clearTemplatesAndAssertions() noexcept {
    _templates.clear();
    _templateInstantiationQueue.clear();
    _assertionQueue.clear();
}
