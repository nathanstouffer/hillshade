## hillshade

An exploration in hillshading for topographic maps

## running

1. Put any tiffs in `./tiffs`
1. `docker compose build && docker compose run --rm convert` (optional)
1. Generate VS project file with `make gnerate-vs-project`
    - note: relies on a symbolic link (you may need to enable `For developers` on Windows)
1. Build and run with VS