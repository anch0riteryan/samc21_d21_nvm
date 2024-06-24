#include <stdint.h>

uint8_t eeprom_write (uint8_t *, const uint32_t , const uint32_t );
uint8_t eeprom_read  (uint8_t *, const uint32_t , const uint32_t );
uint8_t erase_page (const uint8_t p);
uint8_t erase_all_pages (void );
