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

/*#include "dialogue.h"
#include "terrain.h"
#include <iostream>
#include "angelscript.h"
#include "scriptstdstring.h"
#include "scriptbuilder.h"

/*Changes


/*Next steps


test terrain tile bank and tile classes

for virtual methods defined in final classes, remove "virtual"


// Implement a simple message callback function
void MessageCallback(const asSMessageInfo* msg, void* param) {
    const char* type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";
    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void print(std::string& in) {
    std::cout << in;
}
*/

/* Tracker
New JSON method: getTypeName()
*/

#include "game.h"

int main() {
    // create the sink all loggers output to
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);
    // initialise game loop
    awe::game gameLoop;
    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run();
}

/*int main() {
    global::sink::Get("Application", "Developer", "log", false);

    sfx::spritesheet ssTiles;
    ssTiles.load("png/tiles/tiles.json");



    /*asIScriptEngine* engine = asCreateScriptEngine();
    int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert(r >= 0);
    RegisterStdString(engine);
    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert(r >= 0);

    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "MyModule");
    if (r < 0) {
        std::cout << "Unrecoverable error while starting a new module." << std::endl;
        return -1;
    }
    r = builder.AddSectionFromFile("test.as");
    if (r < 0) {
        std::cout << "Please correct the errors in the script and try again." << std::endl;
        return -1;
    }
    r = builder.BuildModule();
    if (r < 0) {
        std::cout << "Please correct the errors in the script and try again (2)." << std::endl;
        return -1;
    }
    asIScriptModule* mod = engine->GetModule("MyModule");
    asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
    if (!func) {
        std::cout << "The script must have the function 'void main()'. Please add it and try again.\n";
        return -1;
    }
    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(func);
    r = ctx->Execute();
    if (r != asEXECUTION_FINISHED) {
        if (r == asEXECUTION_EXCEPTION) {
            std::cout << "An exception '" << ctx->GetExceptionString() << "' occurred. Please correct the code and try again.\n";
            return -1;
        }
    }
    ctx->Release();
    engine->ShutDownAndRelease();
    return 0;

    awe::country country;
    country.load("json/country.json");
    awe::weather weather;
    weather.load("json/weather.json");
    awe::environment environment;
    environment.load("json/environment.json");
    awe::movement movement;
    movement.load("json/movement.json");

    awe::terrain_bank terrainBank(&movement);
    terrainBank.load("json/terrain.json");
    std::cout << terrainBank[1].movecost[3] << std::endl;
    
    i18n::language lang;
    lang.load("lang/DE_test.json");
    lang.toNativeLanguage(true);

    sfx::renderer window;
    window.load("json/renderer.json");
    window.openWindow();

    sfx::spritesheet sprite;
    sprite.load("png/smiley.json");

    sfx::audio audio;
    audio.load("audio/script.json");
    audio.play("bg");

    sfx::user_input input(window);
    input.load("json/control.json");

    sfx::fonts font;
    font.load("json/fonts.json");

    awe::dialogue_sequence seq(&window, &font, &input, &sprite, &lang, &audio);
    seq.load("dialogue/test.json");

    // The main loop - ends as soon as the window is closed
    while (window.isOpen()) {
        input.update();

        // Event processing
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Request for closing the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Controls
        if (input["up"]) {
            audio.play("up");
            sprite++;
            //window.moveCamera(16, 0);
        }
        if (input["down"]) {
            audio.play("down");
            sprite--;
            //window.moveCamera(-16, 0);
        }

        // Clear the whole window before rendering a new frame
        window.clear(sf::Color::White);

        // Render frame
        window.drawToScale(sprite[0]);
        window.drawToScale(sprite[1], 200);
        window.drawToScale(sprite[2], 0, 200);
        window.drawToScale(sprite[3], 200, 200);

        seq.animate(lang("User"));

        // End the current frame and display its contents on screen
        window.display();

        //if (ret == awe::dialogue_status::Closed) break;

        // Custom control to shut the app down
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

        //sf::Keyboard::Key mb;
        //if (input.listenForInput(mb)) std::cout << mb << std::endl;
    }

    //input.save();
}*/