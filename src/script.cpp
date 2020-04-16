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

awe::scripts::scripts(const std::string& name) noexcept : _logger(name) {
    _engine = asCreateScriptEngine();
    if (_engine) {
        if (_engine->SetMessageCallback(asMETHOD(awe::scripts, scriptMessageCallback), this, asCALL_THISCALL) < 0) {
            _logger.error("Fatal error: failed to assign the message callback routine - this is likely a faulty engine build.");
        }
        RegisterStdString(_engine);
    } else {
        _logger.error("Fatal error: script engine failed to load. Ensure that version \"{}\" of AngelScript is being loaded (DLL).", ANGELSCRIPT_VERSION_STRING);
    }
}

awe::scripts::~scripts() noexcept {
    if (_engine) _engine->ShutDownAndRelease();
}

void awe::scripts::scriptMessageCallback(const asSMessageInfo* msg, void* param) noexcept {
    if (msg->type == asMSGTYPE_INFORMATION) {
        _logger.write("INFO: (@{}:{},{}): {}.", msg->section, msg->row, msg->col, msg->message);
    } else {
        _logger.error("{}: (@{}:{},{}): {}.", (msg->type==asMSGTYPE_WARNING?"WARN":"ERR "), msg->section, msg->row, msg->col, msg->message);
    }
}