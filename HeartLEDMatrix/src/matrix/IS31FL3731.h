#ifndef CH32TEST_IS31FL3731_H
#define CH32TEST_IS31FL3731_H

#include <Arduino.h> // Gives us access to the CH32V003 hardware registers

class IS31FL3731 {
  private:
    uint8_t _i2c_addr;
    uint8_t _pwmBuffer[144];
    uint8_t _activeFrame = 0; // 0 for Frame 1, 1 for Frame 2

    // --- BARE METAL I2C HARDWARE DRIVER ---

    void i2c_start() {
        I2C1->CTLR1 |= I2C_CTLR1_START; // Generate Start condition
        while(!(I2C1->STAR1 & I2C_STAR1_SB)); // Wait for Start bit

        // Send the 7-bit address shifted left by 1 (Write mode = bit 0 is low)
        I2C1->DATAR = (_i2c_addr << 1);

        while(!(I2C1->STAR1 & I2C_STAR1_ADDR)); // Wait for address match
        (void)I2C1->STAR2; // Clear ADDR flag by reading STAR2
    }

    void i2c_write(uint8_t data) {
        while(!(I2C1->STAR1 & I2C_STAR1_TXE)); // Wait until transmit buffer is empty
        I2C1->DATAR = data;
    }

    void i2c_stop() {
        while(!(I2C1->STAR1 & I2C_STAR1_TXE)); // Wait for last byte to leave buffer
        while(!(I2C1->STAR1 & I2C_STAR1_BTF)); // Wait for byte transfer to physically finish
        I2C1->CTLR1 |= I2C_CTLR1_STOP; // Generate Stop condition
    }

    // --------------------------------------

    void selectPage(uint8_t page) {
      i2c_start();
      i2c_write(0xFD); // Command Register
      i2c_write(page);
      i2c_stop();
    }

    void writeRegister(uint8_t reg, uint8_t data) {
      i2c_start();
      i2c_write(reg);
      i2c_write(data);
      i2c_stop();
    }

  public:
    // Default I2C address is usually 0x74 when AD pin is tied to GND
    IS31FL3731(uint8_t addr = 0x74) : _i2c_addr(addr) {}

    void begin(const uint8_t* ledMask = nullptr) {
      // 1. INITIALIZE CH32V003 HARDWARE I2C (PC1 = SDA, PC2 = SCL)
      RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO; // Enable GPIOC & AFIO clocks
      RCC->APB1PCENR |= RCC_APB1Periph_I2C1; // Enable I2C1 clock

      // Configure PC1 and PC2 as Alternate Function Open Drain (50MHz)
      GPIOC->CFGLR &= ~(0xFF << 4); // Clear config for PC1 and PC2
      GPIOC->CFGLR |= (0xFF << 4);  // 0xF per pin = AF Open Drain

      // Reset and configure the I2C1 Peripheral
      RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
      RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;

      // Set peripheral clock to 48MHz (default system clock)
      I2C1->CTLR2 = 48;
      // Fast Mode (400kHz): I2C_CKCFG_FS bit | (48MHz / (3 * 400kHz) = 40)
      I2C1->CKCFGR = I2C_CKCFGR_FS | 40;
      // Enable I2C hardware
      I2C1->CTLR1 |= I2C_CTLR1_PE;


      // 2. IS31FL3731 HARDWARE WAKEUP & SETUP
      selectPage(0x0B); // Enter Function Register Page
      writeRegister(0x0A, 0x00); // Software shutdown to reset
      delay(10);
      writeRegister(0x0A, 0x01); // Normal operation
      writeRegister(0x00, 0x00); // Set to Picture Mode
      writeRegister(0x01, 0x00); // Display Frame 1


      // 3. INITIALIZE FRAME 1 AND FRAME 2
      for (uint8_t frame = 0; frame < 2; frame++) {
        selectPage(frame);

        // Write the 18-byte LED Control Mask (0x00 to 0x11)
        for (uint8_t i = 0; i < 18; i++) {
          uint8_t mask = ledMask ? ledMask[i] : 0xFF; // Default all ON
          writeRegister(i, mask);
        }

        // Clear all PWM registers (0x24 to 0xB3) to 0
        // Because we bypass Wire, we can send all 144 bytes in ONE massive transaction!
        i2c_start();
        i2c_write(0x24);
        for (uint8_t j = 0; j < 144; j++) {
            i2c_write(0);
        }
        i2c_stop();
      }

      // Clear local SRAM buffer
      memset(_pwmBuffer, 0, sizeof(_pwmBuffer));
      _activeFrame = 0;
    }

    void drawPixel(uint8_t index, uint8_t brightness) {
      if (index < 144) {
        _pwmBuffer[index] = brightness;
      }
    }

    void show() {
      // Determine which frame is currently hidden
      uint8_t hiddenFrame = (_activeFrame == 0) ? 1 : 0;
      selectPage(hiddenFrame);

      // STREAM THE ENTIRE BUFFER INSTANTLY
      i2c_start();
      i2c_write(0x24); // 0x24 is the start of the PWM registers
      for (uint8_t i = 0; i < 144; i++) {
        i2c_write(_pwmBuffer[i]);
      }
      i2c_stop();

      // Swap the active frame on the physical display
      selectPage(0x0B);
      writeRegister(0x01, hiddenFrame);

      // Update our local state
      _activeFrame = hiddenFrame;
    }
};

#endif //CH32TEST_IS31FL3731_H