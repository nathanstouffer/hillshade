# hillshade

An exploration in hillshading for topographic maps

## Building

1. Put desired tiffs in `./tiff` directory
1. `docker compose build && docker compose run --rm convert`
1. Generate VS project file with `make generate-vs-project`
    - note: relies on a symbolic link (you may need to enable `For developers` on Windows)
1. Build and run `.build/Win64/hillshade.sln` with VS

## Attribution

Many thanks to the open-source software that enables this project.

* [cmake](https://cmake.org/)
* [vcpkg](https://vcpkg.io/en/)
* [Diligent Engine](https://github.com/DiligentGraphics/DiligentEngine)
* [nlohmann::json](https://github.com/nlohmann/json)
* [stb](https://github.com/nothings/stb)
* [stf](https://github.com/nathanstouffer/stf)