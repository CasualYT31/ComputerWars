# Computer Wars
Advance Wars engine for the Windows, macOS, and Linux platforms.
If you've somehow stumbled across this project, then please stick around. This is one of my active projects as of right now, and I hope to keep working on it for the coming few years.

### Planned Features
- Customisable Advance Wars engine: countries, graphics, units, terrains, COs, etc. will all be customisable, as well as a lot more.
- The ability to not only modify the game using scripts, but also the ability to modify the engine itself if you so wish.
- Map maker.
- Internet play.
- Campaign engine.
- Cross-platform: Windows, macOS, and Linux.
- Controllable via mouse, keyboard, or gamepad/joystick.
- Ability to easily add in Artificial Intelligence after-the-fact (actually implementing the AI is not a main goal, but it will be invaluable for the campaign side of the engine).
- In-depth documentation, along with tutorials and examples once the project is ready for release.

### May 10th 2024 ~ Update on Version 0.0.5 and Development Moving Forward
[Here's the progress I've made on the features I intend to add to Version 0.0.5](https://trello.com/c/QX9kYO6U/269-version-005):
- I've completely reworked the GUI code, it's a lot more object-oriented friendly now :+1:
- I've figured out how to get animations to work cleanly in the game engine and maps. The only reason I haven't ticked off that item is due to [this ticket](https://trello.com/c/wQPg0omq/446-complete-black-obelisk-animations), but once I complete Black Obelisk animations, there'd only be the Volcano and the Death Ray animations to complete, and I'm postponing those to a later version.
- I've added sounds and music to the game.
- I've added support for weathers, Fog-of-War, and environments (though not the graphics for environments).
- Only one task left: begin to implement CO logic...

But the final piece of the puzzle for weather support (logic) is where I ran into issues...

It's come to my attention that the way I've written the game engine does not lend itself at all to custom effects that come about with, e.g. certain weathers being active. Things like reducing the attack range of units in sandstorms, changing the way movement works in snow, etc. This becomes an even bigger problem for COs. I had come up with a decent approach in the `bank-rework` branch that would resolve this big limitation. It took quite a while to implement, and after implementing it and trying to integrate it with the rest of the codebase, it gradually became obvious that the entire architecture of the game didn't accommodate it well at all. At some point within the past day or so, I finally acknowledged that I'd need to rework the entire architecture of the game very soon... I already need to carry out so many changes just to get the new `banks` implementation running, and it will inevitably crash/not work once I get it to that stage. I'm not looking forward to debugging that mess. And after all that, I'd need to rewrite a lot of the code anyway. All to jump over one hurdle at the very end of the 0.0.5 iteration.

Needless to say, I'm seriously considering starting from "scratch," _again_... This project was already a fresh start after my first attempt at writing an Advance Wars game failed (the code for that was truly horrid, though, at least this project is somewhat passable). It's taken five years to get to this point, and in the end, it falls short. I don't think it would take as long to get back up and running, thankfully, but it's still a massive blow to the project. I'm not quite at the stage where I want to give up, but I came pretty close today. I may end up officially throwing in the towel within the next year anyway to pursue other endeavours, but for now, I'll keep working on this side project. I'm going to create a whole new repository for it, and I'm already in the process of switching to Jira + Confluence from Trello.

Unsure if I'll create a 0.0.5 tag: it will just point to the current commit of `master` anyway. Feel free to clone it and give it a go though, it pretty much is 0.0.5, just without weather logic and without proper CO support.

### September 14th 2023 ~ Version 0.0.4
The map maker has now been added! For a full rundown of the features that have been added, please see the 0.0.4 release tag.

### December 25th 2022 ~ Version 0.0.3
The basics of the game have now been written up! You can currently play full games on three maps: Island X, Moon Isle, and Keyhole Cove. There are a few minor bugs, but otherwise the engine is in a good playable state! Notable features I haven't coded yet include: weather, FoW, CO powers, and day-to-day abilities. My next goals are to complete a map maker for version 0.0.4, before including the aforementioned features—and more—in 0.0.5. It's best to operate the game with the mouse only for now: full keyboard and gamepad support hasn't been implemented yet w.r.t. navigating GUIs.

### July 7th 2021 ~ Version 0.0.2
Okay, I've officially converted the project to use CMake. This should theoretically make it possible to build this game on more systems besides Windows, **however**, I have only tested on Windows up to this point, and there are one or two areas where Windows-only code or project settings have been written, so I'm more or less convinced that the code won't work for other platforms yet. At least you should now be able to use other IDEs besides Visual Studio 2019 :smile:.

### June 5th 2021 ~ Version 0.0.1
The project has now entered the alpha stage :smile:! [Here's a small demo](https://youtu.be/p96ly3l8YBo) of the project so far. Not much, true, but development should be fairly rapid once I find more time to dedicate to the project in the near future. The engine can load basic maps and display/animate them, but not much else yet. It's far from a complete product, so if you really want an Advance Wars engine **now**, then you can check out [Robosturm's project](https://github.com/Robosturm/Commander_Wars): it might be one of the only AW engine out there of this flavour that's still being actively developed, so go check it out. [This project by ThislsAUsername](https://github.com/ThislsAUsername/DefendPeace) is also worth mentioning. [Custom Wars Tactics](https://github.com/ctomni231/cwtactics) is a web-based Advance Wars engine that you should check out, too :smile:!

### Build Issue With My Fork of TGUI At Version 0.9
When I upgraded my fork to the latest 1.x version, I also accidentally synchronised my 0.9 branch, which introduced a CMake configuration error to versions of this codebase that relied on 0.9 of my fork. I cba to fix it properly, so if you want to build old versions of this codebase that use that branch of my fork, you will have to apply the `fix-0.9.patch` to the root of the repository.

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

### Links
- [Trello Board](https://trello.com/b/GN25IV5j/computer-wars)
- [Discord Server](https://discord.gg/SxaMn2n)

### Licence
Despite its incomplete state, you are free to work with some or all of my code for your own projects, so long as you follow the MIT licence attached to this repository.

### Gitmoji
I like the idea of Gitmoji, but I find that there are a lot of emoji for the same thing, and some that are missing. So I decided to use my own emoji:

| Coding | Emoji | Name | Meaning |
| --- | ---: | :---: | :--- |
| | :sparkles: | sparkles | Add new feature/s. |
| | :hammer: | hammer | Update behaviour of existing feature/s. |
| | :bug: | bug | Fix a bug. |
| | :art: | art | Improve structure/format of the code. |
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
| | :twisted_rightwards_arrows: | twisted_rightwards_arrows | Merge branches. |
| | :bookmark: | bookmark | Release new version. |
