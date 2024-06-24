#include "nvm.h"
#include "eeprom.h"

Nvm nvm;

void init_nvm () {
	Nvm *p;
	uint32_t tmp_hi = 0;
	uint32_t tmp_lo = 0;

	p = &nvm;

#ifdef _NVM_SAMC21_
	MCLK->AHBMASK.bit.NVMCTRL_ = 1;
	MCLK->APBBMASK.bit.NVMCTRL_ = 1;

	GCLK->PCHCTRL[NVMCTRL_GCLK_ID].bit.GEN = 0;
	GCLK->PCHCTRL[NVMCTRL_GCLK_ID].bit.CHEN = 1;

	p->eeprom.write = eeprom_write;
	p->eeprom.read  = eeprom_read;
	p->eeprom.erase_page      = erase_page;
	p->eeprom.erase_all_pages = erase_all_pages;

#elif _NVM_SAMD21_
	PM->AHBMASK.bit.NVMCTRL_ = 1;
#endif

	NVMCTRL->CTRLB.bit.MANW = 0; //AUTO-WRITE

#ifdef _NVM_SAMC21_
	//load TSENS calibration 0x00806030
	tmp_lo = *((volatile uint32_t *) 0x00806030);
	tmp_hi = *((volatile uint32_t *) 0x00806034);
	p->temp_sensor_calibration.bit.TSENS_TCAL   = tmp_lo >> 0 & 0x3F;
	p->temp_sensor_calibration.bit.TSENS_FCAL   = tmp_lo >> 6 & 0x3F;
	p->temp_sensor_calibration.bit.TSENS_GAIN   = (tmp_hi >> 0 & 0xF) << 20 | (tmp_lo >> 12 & 0xFFFFF);
	p->temp_sensor_calibration.bit.TSENS_OFFSET = (tmp_hi >> 4 & 0xFFFFFF);

	//load software calibration 0x00806020
	tmp_lo = *((volatile uint32_t *) 0x00806020);
	tmp_hi = *((volatile uint32_t *) 0x00806024);
	p->software_calibration.bit.ADC0_LINEARITY = tmp_lo >> 0 & 0x7;
	p->software_calibration.bit.ADC0_BIASCAL   = tmp_lo >> 3 & 0x7;
	p->software_calibration.bit.ADC1_LINEARITY = tmp_lo >> 6 & 0x7;
	p->software_calibration.bit.ADC1_BIASCAL   = tmp_lo >> 9 & 0x7;
	p->software_calibration.bit.OSC32K_CAL     = tmp_lo >> 12 & 0x7F;
	p->software_calibration.bit.CAL48M_5V      = (tmp_hi >> 0 & 0x1FF) << 13 | (tmp_lo >> 19 & 0x1FFF);
	p->software_calibration.bit.CAL48M_3V3     = (tmp_hi >> 9 & 0x1FFFFF);
#elif _NVM_SAMD21_
	//load software calibration 0x00806020
	tmp_lo = *((volatile uint32_t *) 0x00806020);
	tmp_hi = *((volatile uint32_t *) 0x00806024);
	p->software_calibration.bit.ADC_LINEARITY = (tmp_hi >> 0 & 0x7) << 5 | (tmp_lo >> 27 & 0x1F);
	p->software_calibration.bit.ADC_BIASCAL   = (tmp_hi >> 3 & 0x7);
	p->software_calibration.bit.OSC32K_CAL    = (tmp_hi >> 6 & 0x7F);
	p->software_calibration.bit.USB_TRANSN    = (tmp_hi >> 13 & 0x1F);
	p->software_calibration.bit.USB_TRANSP    = (tmp_hi >> 18 & 0x1F);
	p->software_calibration.bit.USB_TRIM      = (tmp_hi >> 23 & 0x7);
	p->software_calibration.bit.DFLL48M_COARSE_CAL = (tmp_hi >> 26 & 0x3F);
#endif
	p->serial.reg[0] = *((volatile uint32_t *) 0x0080A00C);
	p->serial.reg[1] = *((volatile uint32_t *) 0x0080A040);
	p->serial.reg[2] = *((volatile uint32_t *) 0x0080A044);
	p->serial.reg[3] = *((volatile uint32_t *) 0x0080A048);
}

void nvm_lock_page (uint8_t p) {
}

void nvm_unlock_page (uint8_t p) {
}

void nvm_execute_cmd (const uint8_t cmd, const uint32_t address) {
	while (!NVMCTRL->INTFLAG.bit.READY);

	NVMCTRL->ADDR.reg = address;
	NVMCTRL->CTRLA.reg = ((NVMCTRL_KEY << 8) | (cmd & 0x7F));
}
