#ifdef _NVM_SAMC21_

#include "nvm.h"
#include "eeprom.h"

uint8_t eeprom_write (uint8_t *src, const uint32_t offset, const uint32_t size) {
	uint8_t cache[NVMCTRL_ROW_SIZE]; //NVMCTRL_ROW_SIZE = 256 Bytes
	uint32_t address_start = NVMCTRL_RWW_EEPROM_ADDR + offset;
	uint32_t address_end   = NVMCTRL_RWW_EEPROM_ADDR + offset + size;
	uint32_t current_row = 0;
	uint32_t tmp = 0;

	if (size > NVMCTRL_RWW_EEPROM_SIZE) {
#ifdef _NVM_DEBUG_
		printf ("EEPROM WRITE ERROR, SIZE=%d > EEPROM_SIZE\n", size, NVMCTRL_RWW_EEPROM_SIZE);
#endif
		return -1;
	}

	if (address_end > (NVMCTRL_RWW_EEPROM_ADDR + NVMCTRL_RWW_EEPROM_SIZE - 1)) {
#ifdef _NVM_DEBUG_
		printf ("EEPROM WRITE ERROR, OUT OF RANGE\n");
#endif
		return -1;
	}

#ifdef _NVM_DEBUG_
	printf ("---EEPROM WRITE START---\n");
	printf ("from:0x%08X to 0x%08X\n", address_start, address_end);
#endif

	for (uint32_t address = address_start; address < address_end; address++) {
		tmp = address & ~(NVMCTRL_ROW_SIZE - 1);
		if (current_row != tmp) {
			current_row = tmp;
			memcpy (cache, (void const *) current_row, NVMCTRL_ROW_SIZE);
			nvm_execute_cmd (NVMCTRL_CTRLA_CMD_ER_Val, current_row);

#ifdef _NVM_DEBUG_
			printf ("ROW ERASE @ 0x%08X\n", tmp);
#endif
		}

		//data copy
		*(cache + (address & (NVMCTRL_ROW_SIZE - 1))) = *(src + (address - address_start));

		tmp = address & (NVMCTRL_ROW_SIZE - 1); //MASK 0-255 Bytes in row
		if ((tmp == (NVMCTRL_ROW_SIZE - 1)) || (address == (address_end - 1))) { //end of row or end of write
			uint32_t *p = (uint32_t *) cache;

			for (uint8_t page = 0; page < NVMCTRL_ROW_PAGES; page++) { //page offset 0-3
				for (uint8_t index = 0; index < (NVMCTRL_PAGE_SIZE >> 2); index++) { //0-63 in dword
					while (!NVMCTRL->INTFLAG.bit.READY);

					*((volatile uint32_t *) (current_row + (page * NVMCTRL_PAGE_SIZE) + (index << 2))) = *(p + index + (page * 16));
				}
#ifdef _NVM_DEBUG_
				printf ("page end at 0x%08X, PAGE WRITE\n", (currentRow + (page * NVMCTRL_PAGE_SIZE)));
#endif
			}
		}
	}

#ifdef _NVM_DEBUG_
	printf ("---EEPROM WRITE DONE ---\n");
#endif

	return 0;
}

uint8_t eeprom_read (uint8_t *dst, const uint32_t offset, const uint32_t size) {\
	if (size > NVMCTRL_RWW_EEPROM_SIZE) {
#ifdef _NVM_DEBUG_
		printf ("EEPROM READ ERROR, SIZE=%d > EEPROM_SIZE\n", size, NVMCTRL_RWW_EEPROM_SIZE);
#endif
		return -1;
	}

	if ((NVMCTRL_RWW_EEPROM_ADDR + offset + size) > (NVMCTRL_RWW_EEPROM_ADDR + NVMCTRL_RWW_EEPROM_SIZE - 1)) {
#ifdef _NVM_DEBUG_
		printf ("EEPROM READ ERROR, OUT OF RANGE\n");
#endif
		return -1;
	}

	memcpy (dst, (void const *) (NVMCTRL_RWW_EEPROM_ADDR + offset), size);

#ifdef _NVM_DEBUG_
	printf ("EEPROM READ %d byte(s) of data @ 0x%08X\n", size, offset);
#endif

	return 0;
}

uint8_t erase_page (const uint8_t page) {
	uint32_t address = NVMCTRL_RWW_EEPROM_ADDR + (NVMCTRL_PAGE_SIZE * page);

	nvm_execute_cmd (
		NVMCTRL_CTRLA_CMD_RWWEEER_Val,
		address
	);

	return 0;
}

uint8_t erase_all_pages () {
	for (uint8_t page = 0; page < NVMCTRL_RWWEE_PAGES; page++) {
		nvm_execute_cmd (
			NVMCTRL_CTRLA_CMD_RWWEEER_Val,
			NVMCTRL_RWW_EEPROM_ADDR + (page * NVMCTRL_PAGE_SIZE)
		);
	}

	return 0;
}

#endif
