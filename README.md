# Computer Wars
Advance Wars engine for the Windows, macOS, and Linux platforms.
There are periods when I can't work on this project, but it is being actively developed, so if you've stumbled across this repository, [please stick around](https://github.com/CasualYT31/ComputerWars)!

### Planned Features
- Customisable Advance Wars engine: countries, graphics, units, terrains, COs, etc. will all be customisable, as well as a lot more.
- The ability to not only modify the game using scripts but also the ability to modify the engine itself if you so wish.
- Map maker.
- Internet play.
- Campaign engine.
- Cross-platform: Windows, macOS, and Linux.
- Controllable via mouse, keyboard, or gamepad/joystick.
- Ability to easily add in Artificial Intelligence after the fact (implementing the AI is not a main goal, but it will be invaluable for the campaign side of the engine).
- In-depth documentation, tutorials and examples once the project is ready for release.

### September 14th 2023 ~ Version 0.0.4
The map maker has now been added! For a full rundown of the features that have been added, please peruse the 0.0.4 release tag.

### December 25th 2022 ~ Version 0.0.3
The basics of the game have now been written up! You can currently play full games on three maps: Island X, Moon Isle, and Keyhole Cove. There are a few minor bugs, but the engine is in a good playable state! Notable features I haven't coded yet include weather, FoW, CO powers, and day-to-day abilities. My next goals are to complete a map maker for version 0.0.4, before including the aforementioned features—and more—in 0.0.5. It's best to operate the game with the mouse only for now: full keyboard and gamepad support haven't been implemented yet w.r.t. navigating GUIs.

### July 7th 2021 ~ Version 0.0.2
Okay, I've officially converted the project to use CMake. This should theoretically make it possible to build this game on more systems besides Windows, **however**, I have only tested on Windows up to this point, and there are one or two areas where Windows-only code or project settings have been written, so I'm more or less convinced that the code won't work for other platforms yet. At least you should now be able to use other IDEs besides Visual Studio 2019 :smile:.

### June 5th 2021 ~ Version 0.0.1
The project has now entered the alpha stage :smile:! [Here's a small demo](https://youtu.be/p96ly3l8YBo) of the project so far. Not much, true, but development should be fairly rapid once I find more time to dedicate to the project in the near future. The engine can load basic maps and display/animate them, but not much else yet. It's far from a complete product, so if you really want an Advance Wars engine **now**, then you can check out [Robosturm's project](https://github.com/Robosturm/Commander_Wars): it might be one of the only AW engines out there of this flavour that's still being actively developed, so go check it out. [This project by ThislsAUsername](https://github.com/ThislsAUsername/DefendPeace) is also worth mentioning. [Custom Wars Tactics](https://github.com/ctomni231/cwtactics) is a web-based Advance Wars engine that you should check out, too :smile:!

### Build Issue With My Fork of TGUI At Version 0.9
When I upgraded my fork to the latest 1.x version, I also accidentally synchronised my 0.9 branch, which introduced a CMake configuration error to versions of this codebase that relied on 0.9 of my fork. I can't be arsed to fix it properly, so if you want to build old versions of this codebase that use that branch of my fork, you have to apply the `fix-0.9.patch` to the root of the repository.

### Build Dependencies
Besides a C++17 compiler and CMake >= 3.14, you'll also require Python >= 3.2 (for `argparse` support) if you wish to work on the C++ macro code. I use Python to generate some macros. `python/requirements.txt` mentions `pyperclip`, but you won't need this module to get a build going.

### Dependencies
For information on the versions of libraries I have used for this project, please check the `FetchContent_Declare` statements in the referenced `CMakeLists.txt` files.
- [SFML](https://github.com/SFML/SFML) (check `engine/CMakeLists.txt`)
- [TGUI](https://github.com/CasualYT31/TGUI) (check `sfx/CMakeLists.txt`)
- [AngelScript](https://www.angelcode.com/angelscript/) (check `engine/CMakeLists.txt`)
- [Boxer](https://github.com/aaronmjacobs/Boxer) (check `engine/CMakeLists.txt`)
- [FMT](https://github.com/fmtlib/fmt) (check `engine/CMakeLists.txt`)
- [SPDLOG](https://github.com/gabime/spdlog) (check `engine/CMakeLists.txt`)
- [JSON for C++](https://github.com/nlohmann/json) (check `engine/CMakeLists.txt`)
- [GoogleTest](https://github.com/google/googletest) (check `test/CMakeLists.txt`)
- [SystemProperties](https://github.com/CasualYT31/SystemProperties) (check `engine/CMakeLists.txt`)
- [AngelScript Docgen](https://github.com/CasualYT31/angelscript-docgen) (check `engine/CMakeLists.txt`)
- [Boost](https://www.boost.org/users/download/)* (check `engine/CMakeLists.txt`)

_*: You'll need to install Boost yourself, and ensure it's accessible to the project by setting `BOOST_ROOT`._

### Links
- [Trello Board](https://trello.com/b/GN25IV5j/computer-wars)
- [Discord Server](https://discord.gg/SxaMn2n)

### Licence
Despite its incomplete state, you are free to work with some or all of my code for your projects, so long as you follow the MIT licence attached to this repository.

### Gitmoji
I like the idea of Gitmoji, but I find that there are a lot of emoji for the same thing and some that are missing. So I decided to use my own emoji:

| Coding | Emoji | Name | Meaning |
| --- | ---: | :---: | :--- |
| | :sparkles: | sparkles | Add new feature/s. |
| | :hammer: | hammer | Update behaviour of existing feature/s. |
| | :bug: | bug | Fix a bug. |
| | :art: | art | Improve the structure/format of the code. |
| | :zap: | zap | Improve performance. |
| | :construction: | construction | Commit code that is a WIP. |
| | :recycle: | recycle | Refactor code. |
| | :boom: | boom | Introduce breaking changes. |
| | :fire: | fire | Remove code, tests, or files. |
| | :warning: | warning | Fix code causing compiler warnings. |
| | :no_entry: | no_entry | Fix code causing compiler errors. |
| | :stethoscope: | stethoscope | Improve and/or document exception safety. |

| Testing | Emoji | Name | Meaning |
| --- | ---: | :---: | :--- |
| | :white_check_mark: | white_check_mark | Add passing test. |
| | :x: | x | Add failing test. |
| | :test_tube: | test_tube | Write tests that have not yet been run. |

| Documentation | Emoji | Name | Meaning |
| --- | ---: | :---: | :--- |
| | :memo: | memo | Add or update documentation. |
| | :bulb: | bulb | Add or update source code comments. |
| | :page_facing_up: | page_facing_up | Add or update markdown files (readmes, licence, etc.). |

| Assets | Emoji | Name | Meaning |
| --- | ---: | :---: | :--- |
| | :wrench: | wrench | Add or update development scripts. |
| | :framed_picture: | framed_picture | Add or update art assets. |
| | :musical_note: | musical_note | Add or update audio assets. |
| | :scroll: | scroll | Add or update JSON assets. |
| | :gear: | gear | Add or update binary assets. |

| Dependencies | Emoji | Name | Meaning |
| --- | ---: | :---: | :--- |
| | :arrow_up: | arrow_up | Upgrade dependency. |
| | :arrow_down: | arrow_down | Downgrade dependency. |
| | :heavy_plus_sign: | heavy_plus_sign | Add dependency. |
| | :heavy_minus_sign: | heavy_minus_sign | Remove dependency. |

| Git | Emoji | Name | Meaning |
| --- | ---: | :---: | :--- |
| | :twisted_rightwards_arrows: | twisted_rightwards_arrows | Merge branches (should be paired with another icon to document the nature of the merge. |
| | :bookmark: | bookmark | Release new version. |
