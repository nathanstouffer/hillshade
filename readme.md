## hillshade

An exploration in hillshading for topographic maps

## running

1. Put tiffs in `./tiff`
1. `docker compose build && docker compose run --rm convert`
1. Generate VS project file with `make generate-vs-project`
    - note: relies on a symbolic link (you may need to enable `For developers` on Windows)
1. Build and run with VS