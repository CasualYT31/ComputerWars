# Computer Wars

Advance Wars engine for the Windows and Linux platforms. Planned features:

- Customisable Advance Wars engine: countries, graphics, units, terrains, COs, etc. will all be customisable, as well as a lot more.
- The ability to not only modify the game using AngelScript but also the ability to modify the engine itself if you so wish.
- Map maker.
- Internet play.
- Campaign engine.
- Cross-platform: Windows and Linux. I may also support macOS if the stars align.
- Controllable via mouse, keyboard, or gamepad/joystick.
- Ability to easily add in Artificial Intelligence (implementing the AI is not a main goal, but it will be invaluable for the campaign side of the engine).
- In-depth documentation, tutorials, and examples once the project is ready for release.

## Starting From "Scratch"

Since the release of version 0.0.5, I have been reworking the game engine from the ground up. I'm hoping to copy over a lot of existing code, so hopefully, it will not take extremely long to get to where I left off (0.0.5).

Until version 0.0.6 comes out, I can't guarantee a functioning game. Check out the [0.0.5 tag](https://github.com/CasualYT31/ComputerWars/tree/0.0.5) instead if you want to play around with the latest version.

## Tracking Progress

I've moved over to Jira and Confluence from Trello. Unfortunately, Jira is not publicly accessible in the free version, so I will have to grit my teeth and make use of the GitHub Projects feature to provide [a high-level overview of my progress publicly](https://github.com/users/CasualYT31/projects/1). The epics and stories in my Jira will also be given cards on this repository's project page.

## Dependencies

You can check the `Fetch dependencies` section of `CMakeLists.txt` to know which version of each dependency is used.

- [GoogleTest](https://github.com/google/googletest).
- [Nlohmann::Json](https://github.com/nlohmann/json).

## Tools

This is a C++20, CMake project. I use Doxygen to generate documentation and clang-format to format all of the code. Check the `CMakeLists.txt`, `Doxyfile.ini` and `.clang-format` scripts for the versions of these tools I use (or the minimum version supported).

For Windows, I use Visual Studio 2022 + MSVC. For Linux, I use WSL2 + Visual Studio Code + GCC. I develop on Ubuntu 22.04, but hopefully other flavours of Linux can run this game, too.

## Links

- [GitHub Project](https://github.com/users/CasualYT31/projects/1)
- [Discord Server](https://discord.gg/SxaMn2n)

## Gitmoji
I like the idea of Gitmoji, but I find that there are a lot of emoji for the same thing, and some that are missing. So I decided to use my own emoji:

### Coding

| Emoji | Name | Meaning |
| ---: | :---: | :--- |
| :sparkles: | sparkles | Add new feature/s. |
| :hammer: | hammer | Update/improve behaviour of existing feature/s. |
| :bug: | bug | Fix a bug. |
| :art: | art | Improve the structure/format of the code. |
| :construction: | construction | Commit code that is a WIP. |
| :recycle: | recycle | Refactor code. |
| :boom: | boom | Introduce breaking changes. |
| :fire: | fire | Remove code, tests, or files. |
| :warning: | warning | Fix code causing compiler warnings. |
| :no_entry: | no_entry | Fix code causing compiler errors. |

### Testing

| Emoji | Name | Meaning |
| ---: | :---: | :--- |
| :white_check_mark: | white_check_mark | Add passing test. |
| :x: | x | Add failing test. |
| :test_tube: | test_tube | Write tests that have not yet been run. |

### Documentation

| Emoji | Name | Meaning |
| ---: | :---: | :--- |
| :memo: | memo | Add or update documentation. |
| :bulb: | bulb | Add or update source code comments. |
| :page_facing_up: | page_facing_up | Add or update markdown files (readmes, licence, etc.). |

### Assets

| Emoji | Name | Meaning |
| ---: | :---: | :--- |
| :wrench: | wrench | Add or update CMake, Doxygen, and other development scripts. |
| :framed_picture: | framed_picture | Add or update art assets. |
| :musical_note: | musical_note | Add or update audio assets. |
| :scroll: | scroll | Add or update JSON assets, or AngelScript assets that override JSON assets during execution. |
| :gear: | gear | Add or update binary assets. |

### Dependencies

| Emoji | Name | Meaning |
| ---: | :---: | :--- |
| :arrow_up: | arrow_up | Upgrade dependency. |
| :arrow_down: | arrow_down | Downgrade dependency. |
| :heavy_plus_sign: | heavy_plus_sign | Add dependency. |
| :heavy_minus_sign: | heavy_minus_sign | Remove dependency. |

### Git

| Emoji | Name | Meaning |
| ---: | :---: | :--- |
| :twisted_rightwards_arrows: | twisted_rightwards_arrows | Merge branches. |
| :bookmark: | bookmark | Release new version. |
