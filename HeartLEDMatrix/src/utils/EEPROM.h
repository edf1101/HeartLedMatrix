/*
 * Created by Ed Fillingham on 24/04/2026.
*/

#ifndef HEARTLEDMATRIX_EEPROM_H
#define HEARTLEDMATRIX_EEPROM_H


#define FLASH_PAGE_SIZE    64
#define FLASH_END_ADDR     0x08004000
#define SETTINGS_ADDR      (FLASH_END_ADDR - FLASH_PAGE_SIZE) // 0x08003FC0

uint8_t load_effect_index() {
    // Flash defaults to 0xFFFF when erased.
    // We read a 16-bit "half-word" from the start of the last page.
    uint16_t val = *(volatile uint16_t*)SETTINGS_ADDR;

    // If it's 0xFFFF (never written) or obviously wrong, return 0
    if (val >= 255) return 0;
    return (uint8_t)val;
}

void save_effect_index(uint8_t idx) {
    // 1. Unlock the Flash controller
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;

    // 2. Erase the page (Crucial: Flash can only flip bits from 1 to 0)
    // Erasing sets the whole 64-byte page to 0xFF.
    FLASH->CTLR |= FLASH_CTLR_PER;      // Page Erase mode
    FLASH->ADDR = SETTINGS_ADDR;        // Target address
    FLASH->CTLR |= FLASH_CTLR_STRT;     // Start erasing
    while(FLASH->STATR & FLASH_STATR_BSY); // Wait for hardware
    FLASH->CTLR &= ~FLASH_CTLR_PER;     // Clear mode

    // 3. Program the new value (16-bit half-word write)
    FLASH->CTLR |= FLASH_CTLR_PG;       // Program mode
    *(volatile uint16_t*)SETTINGS_ADDR = (uint16_t)idx;
    while(FLASH->STATR & FLASH_STATR_BSY); // Wait for hardware
    FLASH->CTLR &= ~FLASH_CTLR_PG;      // Clear mode

    // 4. Re-lock to prevent accidental corruption
    FLASH->CTLR |= FLASH_CTLR_LOCK;
}

#endif //HEARTLEDMATRIX_EEPROM_H