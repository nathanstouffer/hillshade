# flyovers

A `flyover` is an animation of the map defined by a camera path.

## Specification

A `flyover` is a JSON object with the following root level properties:

1. `dem` - The name of the DEM to use (filename relative to repo root)
1. `initial` - A [camera](#camera) object defining the initial position of the animation
1. `delay` - A non-negative delay before beginning the path
1. `anchors` - An array of [anchor](#anchor) objects

### Camera

A JSON object with the following keys:

1. `x` - X position of the camera
1. `y` - Y position of the camera
1. `z` - Z position of the camera
1. `heading` - Heading angle (in degrees)
1. `pitch` - Pitch angle (in degrees)

### Anchor

A JSON object with three keys:

1. `delta` - The positive time since the last anchor (in ms)
1. `camera` - The camera state to use at this anchor
1. `deriv` - The derivative to use at this anchor. Each property can be specified optionally. If not specified, the value will be computed internally.

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