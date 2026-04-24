# Heart LED Matrix Animation System

A high-performance LED animation controller for a heart shaped LED matrix display, built for the CH32V003 microcontroller.

## Hardware

### Components
- **Microcontroller**: CH32V003 RISC-V (32-bit, 48MHz)
- **LED Driver**: IS31FL3731 (144-LED matrix controller with I2C)
- **LED Array**: 79 custom heart-shaped RGB LEDs

## Project Structure

```
src/
├── main.cpp              # Entry point, effect initialization, main loop
├── effects/
│   ├── Effect.h          # Abstract base class for all effects
│   ├── DotEffect.[h/cpp]  # Expanding circular dots
│   ├── PulseEffect.[h/cpp] # Shape-respecting expanding pulse
│   ├── WaveEffect.[h/cpp]  # Traveling sinusoidal waves
│   ├── RainEffect.[h/cpp]  # Falling raindrops with anti-aliasing
│   └── SpiralEffect.[h/cpp] # Rotating spiral patterns
├── matrix/
│   ├── IS31FL3731.[h/cpp] # LED driver with double-buffering
│   └── LedMapper.[h/cpp]  # Coordinate mapping system
└── utils/
    └── FastMaths.h       # Optimized math library (sin, cos, atan2, distance)
```

## Animation Effects

### 1. Dot Effect
Multiple expanding circles that fade in and out at random positions. Uses Euclidean distance for perfect circular gradients.

**Parameters**: Number of dots, expansion radius, fade speed

### 2. Pulse Effect
A smooth expanding wave that respects the shape of the heart. Uses a precomputed signed distance field (SDF) for efficient edge detection.

**Parameters**: Pulse speed, wave thickness/sharpness

### 3. Wave Effect
Traveling sinusoidal waves across the LED array. Supports rotation for dynamic directional movement.

**Parameters**: Direction (degrees), wavelength, propagation speed, rotation speed

### 4. Rain Effect
Raindrops falling vertically with sub-pixel anti-aliasing for smooth motion and fading trails.

**Parameters**: Number of drops, fall speed, trail fade amount

### 5. Spiral Effect
Multi-armed spiral patterns rotating around the center point (5, 5). Combines angular and radial components.

**Parameters**: Rotation speed, spiral tightness, number of arms
## Building and Uploading

### Requirements
- Arduino IDE or PlatformIO with CH32V003 board support
- USB-to-Serial adapter (for programming)

### Build Steps
```bash
# Using PlatformIO:
pio run -t upload
```

## Usage

The system automatically initializes and selects a random effect on startup. To change effects at runtime, modify `currentEffect` in `main.cpp` or implement a selection mechanism (e.g., button input).