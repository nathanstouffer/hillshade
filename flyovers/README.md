# flyovers

A `flyover` is an animation of the map defined by a camera path.

## Specification

A `flyover` is a JSON object with three root level properties: `dem`, `initial`, and `anchors`

1. `dem` - The name of the DEM to use (filename relative to `./terrarium` and excludes the file extension)
2. `initial` - A [camera](#camera) object defining the initial position of the animation
3. `anchors` - An array of [anchor](#anchor) objects

### Camera

A JSON object with three keys:

1. `position` - A vector of length 3 defining the position
2. `heading` - Heading angle (in degrees)
3. `pitch` - Pitch angle (in degrees)

### Anchor

A JSON object with three keys:

1. `delta` - The positivenegative time since the last anchor (in ms)
2. `camera` - The camera state to use at this
3. `deriv` - The derivative to use at this anchor. Each property can be specified optionally. If not specified, the value will be computed internally.

```JSON
{
    "delta": 2000,
    "camera": {
        "position": [0, 0, 100],
        "heading": 50,
        "pitch": 40
    },
    "deriv": {
        "heading": 10,
        "pitch": -5
    }
}
```