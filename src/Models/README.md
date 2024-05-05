# Models
Models describe what the pixels should look like at a given position at a given time.

# FUTURE IDEAS

## Dim
Adjust the brightness of the color down by the provided percent
 
Constructors:
```
Dim(dimPercent, model)
```
Position and time independent

## Brighten
Increases the brightness of the color up by the provided percent, no R, G, or B will exceed 255
 
Constructors:
```
Brighten(brightenPercent, model)
```

Position and time independent

## Firefly
A firely (small light band? needs definition) flits around in a specified range with specified speed parameters (TBD)

Position and time dependent

## Matrix
Green spots flow from one end of the strip to the other.
Can experiment with varying rates, sizes, brightnesses, hues.

Position and time dependent

## Blend
Blend two models together. Details TBD, but options include LERP, add, etc.
 
Requires two input models

Position and time independent

## Blur
Performs some sort of convolution around a position to blur the colors.

Requires input model

Position and time independent

## Lava lamp
Simulate a lava lamp.

Direction - up/down
Color

## Jacob's ladder
Simulate the rising electrical arc of a Jacob's ladder.
 
Color?
Speed?

## Noise
Simulate static/random noise.

Color or BW
Speed? (how often the static updates?)

## Knight Rider
Strobe light back and forth

Color?
Speed?
Velocity shape? (e.g., linear, faster on edges, faster in the middle, etc.)

## Strobe
Only update the underlying model every nnn ms

Requires input model

Position independent
