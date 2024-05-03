# Betacraft

[Website](https://betacraft.uk/) | [Discord](https://discord.gg/d4WvXeQ)

Betacraft aims to provide easy access to old Minecraft versions and improve the overall game experience.

## System Requirements

* OS
    * Windows (10 build 1809 or higher).
    * Linux.
    * macOS (11 or higher).

## Building

Betacraft requires [CMake](https://cmake.org/). Many libraries are bundled with Betacraft and used if they're not installed on your system. CMake will inform you if a bundled library is used or if you need to install any missing packages yourself.

1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make`

## Translations

1. Create a new file YOURLANGUAGE.json in the `lang` folder.
2. Copy contents of English.json to the newly created file and translate the content
3. Add the path to your translation to `src/ui/assets.qrc`
4. Make a pull request
