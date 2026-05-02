/**
 * @file main.cpp
 * @brief Heart LED Matrix animation controller.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Main entry point for the LED heart animation system. Initializes hardware,
 * sets up effects, and runs the main animation loop at 60 FPS.
 */

#include <Arduino.h>

#include "effects/DotEffect.h"
#include "effects/PulseEffect.h"
#include "effects/RainEffect.h"
#include "effects/SpiralEffect.h"
#include "effects/WaveEffect.h"
#include "effects/StarEffect.h"
#include "matrix/IS31FL3731.h"
#include "matrix/LedMapper.h"
#include "utils/EEPROM.h"
#include "utils/Watchdog.h"

// Provide dummy implementations for C++ runtime functions to avoid linking issues
extern "C" {
void* __dso_handle = (void*)&__dso_handle;
int __cxa_atexit(void (*f)(void*), void* p, void* d) { return 0; }
}

void operator delete(void* ptr, unsigned int size) noexcept
{
}

/// LED matrix driver instance (I2C address 0x74)
IS31FL3731 matrix = IS31FL3731(0x74);

/// LED enable mask for IS31FL3731: specifies which outputs are active
const uint8_t activeLEDs[18] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Outputs 0-8 fully enabled
    0b01111111, // Output 9: 7 of 8 enabled
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Outputs 10-17: disabled
};

/// Heart-shaped LED coordinate layout (79 total LEDs)
LedMapper::Coordinate rawCoords[] = {
    {2, 0}, {8, 0},
    {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1},
    {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2},
    {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3},
    {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4},
    {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5},
    {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6},
    {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7},
    {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8},
    {4, 9}, {5, 9}, {6, 9},
    {5, 10}
};


/// Coordinate mapper for heart-shaped LED array
LedMapper matrixMapper(rawCoords, sizeof(rawCoords) / sizeof(rawCoords[0]));

/// Center coordinate of the heart shape
LedMapper::Coordinate center = matrixMapper.getCoord(49);

// Animation effects
DotEffect dotEffect(&matrix, &matrixMapper, 3, 384, 8);
PulseEffect pulseEffect(&matrix, &matrixMapper, 16, 18);
RainEffect rainEffect(&matrix, &matrixMapper, 4, 30, 35);
SpiralEffect spiralEffect(&matrix, &matrixMapper, 3, 6, 1);
WaveEffect waveEffect1(&matrix, &matrixMapper, 0, 20, 2, 1);
WaveEffect waveEffect2(&matrix, &matrixMapper, 45, 20, 2, 0);
StarEffect starEffect(&matrix, &matrixMapper, 6, 250, 100,5,2);
DotEffect dotEffect2(&matrix, &matrixMapper, 6, 50, 9);



/// Array of all available effects
Effect* effectList[] = {
    &dotEffect,
    &pulseEffect,
    &waveEffect2,
    &rainEffect,
    &dotEffect2,
    &spiralEffect,
    &waveEffect1,
    &starEffect,
};

const uint8_t NUM_EFFECTS = sizeof(effectList) / sizeof(effectList[0]);
Effect* currentEffect = nullptr;

// power latch variables
const uint32_t POWER_LATCH_TIMEOUT = 15 * 1000; // 15 seconds
const uint8_t POWER_LATCH_PIN = A2;

void setup()
{

    // set the power latch pin high to keep power on, and configure it as an output
    pinMode(POWER_LATCH_PIN, OUTPUT);
    digitalWrite(POWER_LATCH_PIN, HIGH);

    if (RCC->RSTSCKR & RCC_IWDGRSTF)
    {
        // KILL POWER
        while (1)
        {
            RCC->RSTSCKR |= RCC_RMVF;

            // try a few times to cut power then cut leds after, as dont want that to cause an issue if it fails
            pinMode(POWER_LATCH_PIN, OUTPUT);
            digitalWrite(POWER_LATCH_PIN, LOW);
            delay(100);
            pinMode(POWER_LATCH_PIN, OUTPUT);
            digitalWrite(POWER_LATCH_PIN, LOW);
            delay(100);
            matrix.begin(activeLEDs);

            // clear the matrix
            for (int pix = 0; pix < LedMapper::MAX_LEDS; pix++)
            {
                matrix.drawPixel(pix, 0);
            }
            matrix.show();
        }
    }


    RCC->RSTSCKR |= RCC_RMVF;
    init_watchdog();

    // Initialize serial on alternate pins (TX on PD6, RX disabled)
    Serial.begin(115200);
    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 = (AFIO->PCFR1 & ~(1 << 2)) | (1 << 21);
    USART1->CTLR1 &= ~(1 << 2);
    GPIOD->CFGLR &= ~(0xF << (4 * 6));
    GPIOD->CFGLR |= (0xB << (4 * 6));

    // Initialize the LED matrix
    matrix.begin(activeLEDs);
    
    Serial.println("Matrix initialized!");


    // 1. Load the last played effect
    uint8_t lastIndex = load_effect_index();

    // 2. Increment and wrap around
    // NUM_EFFECTS is the size of your effectList array
    uint8_t nextIndex = (lastIndex + 1) % NUM_EFFECTS;

    // 3. Save it immediately for the NEXT boot
    save_effect_index(nextIndex);

    // 4. Run the current effect
    uint8_t currentEffectIndex = lastIndex; // Or use nextIndex if you want it to move forward immediately
    currentEffect = effectList[currentEffectIndex];
    currentEffect->setupEffect();


}


void loop()
{
    feed_watchdog(); // Reset the watchdog timer to prevent reset

    static uint32_t lastFrameTime = 0;
    const uint32_t TARGET_FPS = 60;
    const uint32_t FRAME_MICROS = 1000000 / TARGET_FPS;

    uint32_t now = micros();

    // Run effect at fixed 60 FPS rate
    if (now - lastFrameTime >= FRAME_MICROS)
    {
        lastFrameTime = now;

        if (currentEffect != nullptr)
        {
            currentEffect->loopEffect();
        }
    }

    if (micros() > POWER_LATCH_TIMEOUT * 1000)
    {
        while (1)
        {
            // clear the matrix before cutting power
            for (int pix = 0; pix < LedMapper::MAX_LEDS; pix++)
            {
                matrix.drawPixel(pix, 0);
            }
            matrix.show();

            digitalWrite(POWER_LATCH_PIN, LOW); // Cut power after timeout
            delay(100);
        }
    }
}
