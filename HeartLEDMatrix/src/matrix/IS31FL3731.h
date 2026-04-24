/**
 * @file IS31FL3731.h
 * @brief Driver for the IS31FL3731 LED matrix controller.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Bare-metal I2C driver for the IS31FL3731 LED matrix IC.
 * Implements double-buffering for flicker-free updates.
 */

#ifndef HEARTLEDMATRIX_IS31FL3731_H
#define HEARTLEDMATRIX_IS31FL3731_H

#include <Arduino.h>

/**
 * @class IS31FL3731
 * @brief Interface to the IS31FL3731 LED matrix driver IC.
 *
 * Uses bare-metal I2C on CH32V003 microcontroller with double-buffering
 * to achieve smooth, flicker-free LED animations.
 */
class IS31FL3731 {
private:
    uint8_t _i2c_addr;      ///< I2C address of the device
    uint8_t _pwmBuffer[144]; ///< PWM brightness buffer for all 144 LED outputs
    uint8_t _activeFrame;    ///< Currently displayed frame (0 or 1)

    /// Generate I2C START condition and address the device
    void i2c_start() {
        I2C1->CTLR1 |= I2C_CTLR1_START;
        while (!(I2C1->STAR1 & I2C_STAR1_SB));

        I2C1->DATAR = (_i2c_addr << 1);
        while (!(I2C1->STAR1 & I2C_STAR1_ADDR));
        (void)I2C1->STAR2;
    }

    /// Write a single byte over I2C
    void i2c_write(uint8_t data) {
        while (!(I2C1->STAR1 & I2C_STAR1_TXE));
        I2C1->DATAR = data;
    }

    /// Generate I2C STOP condition
    void i2c_stop() {
        while (!(I2C1->STAR1 & I2C_STAR1_TXE));
        while (!(I2C1->STAR1 & I2C_STAR1_BTF));
        I2C1->CTLR1 |= I2C_CTLR1_STOP;
    }

    /**
     * @brief Select a page on the IS31FL3731 chip.
     * @param page Page number (0-1 for frames, 0x0B for function registers)
     */
    void selectPage(uint8_t page) {
        i2c_start();
        i2c_write(0xFD);
        i2c_write(page);
        i2c_stop();
    }

    /**
     * @brief Write a value to an IS31FL3731 register.
     * @param reg Register address
     * @param data Value to write
     */
    void writeRegister(uint8_t reg, uint8_t data) {
        i2c_start();
        i2c_write(reg);
        i2c_write(data);
        i2c_stop();
    }

public:
    /**
     * @brief Construct an IS31FL3731 driver instance.
     * @param addr I2C address (default 0x74, adjustable via AD pin)
     */
    IS31FL3731(uint8_t addr = 0x74) : _i2c_addr(addr), _activeFrame(0) {}

    /**
     * @brief Initialize the IS31FL3731 and prepare for display updates.
     * @param ledMask Optional 18-byte mask to enable/disable specific LED outputs
     */
    void begin(const uint8_t* ledMask = nullptr) {
        // Initialize CH32V003 I2C hardware (PC1=SDA, PC2=SCL)
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;
        RCC->APB1PCENR |= RCC_APB1Periph_I2C1;

        GPIOC->CFGLR &= ~(0xFF << 4);
        GPIOC->CFGLR |= (0xFF << 4);

        RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
        RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;

        I2C1->CTLR2 = 48;
        I2C1->CKCFGR = I2C_CKCFGR_FS | 40;
        I2C1->CTLR1 |= I2C_CTLR1_PE;

        // Wake up and configure the LED controller
        selectPage(0x0B);
        writeRegister(0x0A, 0x00);
        delay(10);
        writeRegister(0x0A, 0x01);
        writeRegister(0x00, 0x00);
        writeRegister(0x01, 0x00);

        // Initialize both display frames
        for (uint8_t frame = 0; frame < 2; frame++) {
            selectPage(frame);

            for (uint8_t i = 0; i < 18; i++) {
                uint8_t mask = ledMask ? ledMask[i] : 0xFF;
                writeRegister(i, mask);
            }

            i2c_start();
            i2c_write(0x24);
            for (uint8_t j = 0; j < 144; j++) {
                i2c_write(0);
            }
            i2c_stop();
        }

        memset(_pwmBuffer, 0, sizeof(_pwmBuffer));
        _activeFrame = 0;
    }

    /**
     * @brief Set the brightness of an LED in the buffer.
     * @param index LED index (0-143)
     * @param brightness Brightness level (0-255)
     */
    void drawPixel(uint8_t index, uint8_t brightness) {
        if (index < 144) {
            _pwmBuffer[index] = brightness;
        }
    }

    /**
     * @brief Write the buffer to the display and swap frames for flicker-free updates.
     */
    void show() {
        uint8_t hiddenFrame = (_activeFrame == 0) ? 1 : 0;
        selectPage(hiddenFrame);

        i2c_start();
        i2c_write(0x24);
        for (uint8_t i = 0; i < 144; i++) {
            i2c_write(_pwmBuffer[i]);
        }
        i2c_stop();

        selectPage(0x0B);
        writeRegister(0x01, hiddenFrame);

        _activeFrame = hiddenFrame;
    }
};

#endif // HEARTLEDMATRIX_IS31FL3731_H