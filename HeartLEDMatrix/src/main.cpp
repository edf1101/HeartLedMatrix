#include <Arduino.h>

#include "effects/DotEffect.h"
#include "effects/PulseEffect.h"
#include "effects/RainEffect.h"
#include "effects/SpiralEffect.h"
#include "effects/WaveEffect.h"
#include "matrix/IS31FL3731.h"
#include "matrix/LedMapper.h"
#include "utils/FastMaths.h"


extern "C" {
    // Stub for dso_handle
    void* __dso_handle = (void*) &__dso_handle;

    // Stub for atexit (prevents cleanup logic bloat)
    int __cxa_atexit(void (*f)(void *), void *p, void *d) { return 0; }
}

// Stub for the specific delete operator the linker is looking for
void operator delete(void* ptr, unsigned int size) noexcept {
    // Do nothing - we aren't using dynamic memory!
}

IS31FL3731 matrix = IS31FL3731(0x74); // Default I2C address for the J4M6 board

const uint8_t activeLEDs[18] = {
    0xFF,       // Reg 0x00: CA1 block (Matrix A)
    0xFF,       // Reg 0x01: CB1 block (Matrix B)
    0xFF,       // Reg 0x02: CA2 block (Matrix A) -
    0xFF,       // Reg 0x03: CB2 block (Matrix B)
    0xFF,       // Reg 0x04: CA3 block (Matrix A)
    0xFF,       // Reg 0x05: CB3 block (Matrix B)
    0xFF,       // Reg 0x06: CA4 block (Matrix A)
    0xFF,       // Reg 0x07: CB4 block (Matrix B)
    0xFF,       // Reg 0x08: CA5 block (Matrix A)
    0b01111111, // Reg 0x09: CB5 block (Matrix B)
    0x00,       // Reg 0x0A: CA6 block (Matrix A)
    0x00,       // Reg 0x0B: CB6 block (Matrix B)
    0x00,       // Reg 0x0C: CA7 block (Matrix A)
    0x00,       // Reg 0x0D: CB7 block (Matrix B)
    0x00,       // Reg 0x0E: CA8 block (Matrix A)
    0x00,       // Reg 0x0F: CB8 block (Matrix B)
    0x00,       // Reg 0x10: CA9 block (Matrix A)
    0x00        // Reg 0x11: CB9 block (Matrix B)
};

LedMapper::Coordinate rawCoords[] = {
    {2, 0}, {8, 0},                                                                          // D1, D2
    {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1},                                                  // D3 - D7
    {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1},                                                 // D8 - D12
    {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, // D13 - D23
    {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, // D24 - D34
    {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, // D35 - D45
    {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5},                  // D46 - D54
    {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6},                  // D55 - D63
    {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7},                                  // D64 - D70
    {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8},                                                  // D71 - D75
    {4, 9}, {5, 9}, {6, 9},                                                                  // D76 - D78
    {5, 10}                                                                                  // D79
};

LedMapper matrixMapper(rawCoords, sizeof(rawCoords) / sizeof(rawCoords[0]));

LedMapper::Coordinate center = matrixMapper.getCoord(49); // D50 is the center LED in our 79-LED arrangement}

//EFFECTS here
DotEffect dotEffect(&matrix, &matrixMapper, 3, 384, 8);
PulseEffect pulseEffect(&matrix, &matrixMapper, 16, 18);
RainEffect rainEffect(&matrix, &matrixMapper,4, 30, 35);
SpiralEffect spiralEffect(&matrix, &matrixMapper, 3, 6, 1);
WaveEffect waveEffect1(&matrix, &matrixMapper, 0, 20, 2, 1);
WaveEffect waveEffect2(&matrix, &matrixMapper, 45, 20, 2, 0);
Effect* effectList[] = {
    &dotEffect,
    &pulseEffect,
    &rainEffect,
    &spiralEffect,
    &waveEffect1,
    &waveEffect2
};
const uint8_t NUM_EFFECTS = sizeof(effectList) / sizeof(effectList[0]);

Effect* currentEffect = nullptr;

uint16_t get_adc_noise() {
    RCC->APB2PCENR |= RCC_APB2Periph_ADC1;
    ADC1->CTLR2 |= (uint32_t)0x00000001; // ADON

    // Add a simple timeout counter to every while loop
    volatile uint32_t timeout = 2000;

    ADC1->CTLR2 |= (uint32_t)0x00000008; // RSTCAL
    while((ADC1->CTLR2 & (uint32_t)0x00000008) && --timeout);

    timeout = 2000;
    ADC1->CTLR2 |= (uint32_t)0x00000004; // CAL
    while((ADC1->CTLR2 & (uint32_t)0x00000004) && --timeout);

    ADC1->RSQR3 = 1; // Channel 1
    ADC1->CTLR2 |= (uint32_t)0x00400000; // SWSTART

    timeout = 2000;
    while(!(ADC1->STATR & (uint32_t)0x00000002) && --timeout);

    return (uint16_t)ADC1->RDATAR;
}

void setup()
{
    // enable serial on alternate pin so can still upload
    Serial.begin(115200);
    // 2. Enable the Alternate Function I/O (AFIO) clock
    // RCC_AFIOEN is bit 0 in the APB2 peripheral clock enable register
    RCC->APB2PCENR |= RCC_AFIOEN;

    // 3. Apply USART1 Partial Remap 2 (Moves TX to PD6, RX to PD5)
    // According to the CH32V003 Reference Manual, Remap 2 is active when
    // AFIO_PCFR1 bit 21 is 1, and bit 2 is 0.
    AFIO->PCFR1 = (AFIO->PCFR1 & ~(1 << 2)) | (1 << 21);

    // 4. Disable the RX functionality completely (Free up PD5 for SWIO)
    // Bit 2 in USART1->CTLR1 is the Receiver Enable (RE) bit.
    USART1->CTLR1 &= ~(1 << 2);

    // 5. Manually reconfigure PD6 as an Alternate Function Push-Pull pin
    // Clear the 4 configuration bits for Pin 6 (bits 24-27)
    GPIOD->CFGLR &= ~(0xF << (4 * 6));
    // Set to 0xB (Binary 1011: Mode=11 for 50MHz Output, CNF=10 for AF Push-Pull)
    GPIOD->CFGLR |= (0xB << (4 * 6));
    matrix.begin(activeLEDs);

    Serial.println("Matrix initialized!");

    pinMode(A2,OUTPUT);
    pinMode(C4,OUTPUT);

    uint32_t seed = 0;
    for(uint8_t i = 0; i < 16; i++) {
        // Shift and XOR to mix the noise bits thoroughly
        seed = (seed << 2) ^ get_adc_noise();
    }
    seed_rand(seed);
    uint8_t currentEffectIndex = fast_rand() % NUM_EFFECTS;
    currentEffect = effectList[currentEffectIndex];
    currentEffect->setupEffect();
}


void loop() {
    static uint32_t lastFrameTime = 0;
    const uint32_t TARGET_FPS = 60;
    const uint32_t FRAME_MICROS = 1000000 / TARGET_FPS; // 16,666 micros

    uint32_t now = micros();

    // This ensures the effect runs exactly every 16.6ms,
    // regardless of how long the math took to calculate.
    if (now - lastFrameTime >= FRAME_MICROS) {
        lastFrameTime = now;

        if (currentEffect != nullptr) {
            currentEffect->loopEffect();
        }
    }
}