# Computer Wars
Advance Wars engine for the Windows, macOS, and Linux platforms.
If you've somehow stumbled across this project, then please stick around. This is one of my active projects as of right now, and I hope to keep working on it for the coming few years.

### Planned Features
- Customisable Advance Wars engine: countries, graphics, units, terrains, COs, etc. will all be customisable, as well as a lot more.
- The ability to not only modify the game using scripts, but also the ability to modify the engine itself.
- Internet play.
- Campaign engine.
- Cross-platform: Windows, macOS, and Linux.
- Controllable via mouse, keyboard, or gamepad/joystick.
- Ability to easily add in Artificial Intelligence after-the-fact (actually implementing the AI is not a main goal, but it will be invaluable for the campaign side of the engine).
- In-depth documentation, along with tutorials and examples once the project is ready for release.

### July 7th 2021
Okay, I've officially converted the project to use CMake. This should theoretically make it possible to build this game on more systems besides Windows, **however**, I have only tested on Windows up to this point, and there are one or two areas where Windows-only code or project settings have been written, so I'm more or less convinced that the code won't work for other platforms yet. At least you should now be able to use other IDEs besides Visual Studio 2019 :smile:.

### June 5th 2021
The project has now entered the alpha stage :smile:! [Here's a small demo](https://youtu.be/p96ly3l8YBo) of the project so far. Not much, true, but development should be fairly rapid once I find more time to dedicate to the project in the near future. The engine can load basic maps and display/animate them, but not much else yet. It's far from a complete product, so if you really want an Advance Wars engine **now**, then you can check out [Robosturm's project](https://github.com/Robosturm/Commander_Wars): it might be one of the only AW engine out there of this flavour that's still being actively developed, so go check it out. [This project by ThislsAUsername](https://github.com/ThislsAUsername/DefendPeace) is also worth mentioning. [Custom Wars Tactics](https://github.com/ctomni231/cwtactics) is a web-based Advance Wars engine that you should check out, too :smile:!

### Dependencies
For information on the versions of libraries I have used for this project, please check the `FetchContent_Declare` statements in the `CMakeLists.txt`, `engine/CMakeLists.txt`, and `sfx/CMakeLists.txt` files.
- [SFML](https://github.com/SFML/SFML)
- [TGUI](https://github.com/texus/TGUI)
- [AngelScript](https://www.angelcode.com/angelscript/)
- [Boxer](https://github.com/aaronmjacobs/Boxer)
- [SPDLOG](https://github.com/gabime/spdlog)
- [JSON for C++](https://github.com/nlohmann/json)
- [GoogleTest](https://github.com/google/googletest)
- [SystemProperties](https://github.com/CasualYT31/SystemProperties)

### Links
- [Trello Board](https://trello.com/b/GN25IV5j/computer-wars)
- [Discord Server](https://discord.gg/SxaMn2n)

### Licence
Despite its incomplete state, you are free to work with some or all of my code for your own projects, so long as you follow the MIT licence attached to this repository.
