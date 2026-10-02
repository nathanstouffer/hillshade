# hillshade

An exploration in hillshading for topographic maps

## Building

1. Put desired tiffs in `./tiff` directory
1. `docker compose build && docker compose run --rm convert`
1. Generate VS project via CMake preset `dest-win`
1. Build and run `.build/dest-win/hillshade.sln` with VS

## Attribution

Many thanks to the open-source software that enables this project.

* [cmake](https://cmake.org/)
* [vcpkg](https://vcpkg.io/en/)
* [Diligent Engine](https://github.com/DiligentGraphics/DiligentEngine)
* [nlohmann::json](https://github.com/nlohmann/json)
* [stb](https://github.com/nothings/stb)
* [stf](https://github.com/nathanstouffer/stf)