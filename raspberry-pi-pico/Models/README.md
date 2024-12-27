# Models
Models describe what the pixels should look like at a given position at a given time.

# FUTURE IDEAS

## Blink
Blink switches between the provided models at the provided rate/interval

## Breathe
Similar to Blink, but fades between the provided models at the provided rate/interval

## Brighten
Increases the brightness of the color up by the provided percent, no R, G, or B will exceed 255
 
Constructors:
```
Brighten(brightenPercent, model)
```

Position and time independent

## Dim
Adjust the brightness of the color down by the provided percent
 
Constructors:
```
Dim(dimPercent, model)
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

## Theater Chase
Theater chase is an animation effect where LEDs light up in a sequential,
moving pattern that resembles the lights commonly seen on the borders of
theater signs or marquees.

Customizations:
- Speed (how quickly the pattern moves)
- Colors (single color, alternating colors, rainbow, etc.)
- Group size (how many LEDs are lit up in each step), or alternatively how many groups
- Direction (forward, backward, alternating, etc.)

# More "Action" Ideas
These ideas are from an earlier version of the project and may be worth revisiting.
In this version of the project, "Actions" were models that changed with time.

## Delayed Start
    DelayedStart(long delayMS, Action *action) : Action(strip), delayMS(delayMS), action(action) { }

## FadeTo    
    FadeTo(Pixels pixels, long durationMS, int firstPixel, int lastPixel, Color c);
    FadeTo(Pixels pixels, long durationMS, Color c) : FadeTo(strip, pixels, durationMS, 0, strip.numPixels(), c) { }

## Flicker    
    Flicker(int firstPixel, int lastPixel, Color color) : Action(strip, firstPixel, lastPixel), color(color) { }
    Flicker(int pixel, Color color) : Flicker(strip, pixel, pixel + 1, color) { }
    Flicker(Color color) : Flicker(strip, 0, strip.numPixels(), color) { }
    
## Fuse
    Fuse(int pixelsPerSecond) : Fuse(strip, pixelsPerSecond, 0, strip.numPixels(), Colors::fade(WHITE, 3), ORANGE) { }
    Fuse(int pixelsPerSecond, Color fuseColor, Color burnColor) : Fuse(strip, pixelsPerSecond, 0, strip.numPixels(), fuseColor, burnColor) { }
    Fuse(int pixelsPerSecond, int firstPixel, int lastPixel, Color fuseColor, Color burnColor);

## Lightning    
    Lightning(Color color) : Lightning(strip, 0, strip.numPixels(), color) { }
    Lightning(int firstPixel, int lastPixel, Color color);
    
## Noise
    Noise(int firstPixel, int lastPixel, Color color) : Action(strip, firstPixel, lastPixel), color(color) { }
    Noise(Color color) : Noise(strip, 0, strip.numPixels(), color) { }
    Noise() : Noise(strip, 0, strip.numPixels(), WHITE) { }

# More "Pattern" Ideas
These ideas are from an earlier version of the project and may be worth revisiting.
In this version of the project, "Patterns" were models that did not change with time.

## Blend
Similar to sum, but averages between two or more models.

## SineWave
Fade between two colors in a sine wave pattern.