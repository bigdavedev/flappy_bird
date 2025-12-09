Flappy Bird
===========

This repository is part of a presentation I gave for the Gothenburg C++ User Group. You can find
the presentation material [here](presentation/index.html).

Building
========

In order to build this project it is recommended to create a file called `CMakeUserPresets.json`
in the root folder and add the following:

### Windows
```json
{
  "version": 6,
  "configurePresets": [
    {
      "name": "dev",
      "inherits": "dev-default-windows"
    }
  ]
}
```
VisualStudio 2022 should then pick this up and generate the entire project and set `flappy_bird.exe`
as the run target.

### Linux
```json
{
  "version": 6,
  "configurePresets": [
    {
      "name": "dev",
      "inherits": "dev-default-linux"
    }
  ],
  "buildPresets": [
    {
      "name": "debug",
      "configurePreset": "dev",
      "configuration": "Debug"
    },
    {
      "name": "release",
      "configurePreset": "dev",
      "configuration": "Release"
    }
  ]
}
```

Then configure and build with:
```bash
cmake --preset dev
cmake --build --preset debug
```

Attributions
------------

This repository makes use of the following open source projects:

| Project   | Homepage                                | License                                       |
|-----------|-----------------------------------------|-----------------------------------------------|
| SDL3      | https://libsdl.org/                     | [ZLib](data/licences/sdl3_zlib_licence.txt)   |
| SDL_image | https://github.com/libsdl-org/SDL_image | [ZLib](data/licences/sdl3_zlib_licence.txt)   |
| SDL_mixer | https://github.com/libsdl-org/SDL_mixer | [ZLib](data/licences/sdl3_zlib_licence.txt)   |
| fmt       | https://fmt.dev/                        | [MIT](data/licences/fmt_mit_licence.txt)      |
| SpdLog    | https://github.com/gabime/spdlog        | [MIT](data/licences/spdlog_mit_licence.txt)   |
| Freetype  | https://www.freetype.org/               | [FTL](data/licences/freetype_ftl_licence.txt) |
| EnTT      | https://github.com/skypjack/entt        | [MIT](data/licences/entt_mit_licence.txt)     |
