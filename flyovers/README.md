# flyovers

A `flyover` is an animation of the map defined by a camera path.

## Specification

A `flyover` is a JSON object with two root level properties: `dem` and `anchors`

The value of `dem` should be a filename relative to `./terrarium` that excludes the file extension.

The value of `anchors` should be an array of [anchor](#Anchor) objects.

### Anchor

A JSON object with three keys:

1. `delta` - The time (in ms) since the last anchor
2. `camera` - The camera state to use at this anchor (position/heading/pitch)
    * `position` - A vector of length 3 defining the position
    * `heading` - Heading angle (in degrees)
    * `pitch` - Pitch angle (in degrees)
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