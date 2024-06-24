#include <string.h>

#ifdef _NVM_SAMC21_
#include <samc21.h>
#elif _NVM_SAMD21_
#include <samd21.h>
#else
#error NVM_CHIPSET_NOT_DEFINE
#endif

#define NVMCTRL_KEY 0xA5

typedef union {
	struct { //fix spec p104
		uint32_t WORD0;
		uint32_t WORD1;
		uint32_t WORD2;
		uint32_t WORD3;
	} bit;
	uint32_t reg[4];
} SerialNumber;

#ifdef _NVM_SAMC21_
typedef union { // @ 0x00806030
	struct {
		uint8_t TSENS_TCAL : 6;
		uint8_t TSENS_FCAL : 6;
		uint32_t TSENS_GAIN : 24;
		uint32_t TSENS_OFFSET : 24;
		uint8_t : 4;
	} bit;
} TemperatureCalibration;
#endif

typedef union { // @ 0x00806020
#ifdef _NVM_SAMC21_
	struct {
		uint8_t ADC0_LINEARITY : 3;
		uint8_t ADC0_BIASCAL : 3;
		uint8_t ADC1_LINEARITY : 3;
		uint8_t ADC1_BIASCAL : 3;
		uint8_t OSC32K_CAL : 7;
		uint32_t CAL48M_5V : 22;
		uint32_t CAL48M_3V3: 22;
	} bit;
#elif _NVM_SAMD21_
	struct {
		uint8_t ADC_LINEARITY : 8;
		uint8_t ADC_BIASCAL : 3;
		uint8_t OSC32K_CAL : 7;
		uint8_t USB_TRANSN : 5;
		uint8_t USB_TRANSP : 5;
		uint8_t USB_TRIM : 3;
		uint8_t DFLL48M_COARSE_CAL : 6;
	} bit;
#endif
} SoftwareCalibration;

typedef struct _eeprom_func {
	uint8_t (*write) (uint8_t *src, const uint32_t offset, const uint32_t size);
	uint8_t (*read) (uint8_t *dest, const uint32_t offset, const uint32_t size);
	uint8_t (*erase_page) (const uint8_t p);
	uint8_t (*erase_all_pages) (void );
} Eeprom;

typedef struct _nvm {
	SerialNumber serial;
	SoftwareCalibration software_calibration;

#ifdef _NVM_SAMC21_
	Eeprom eeprom;
	TemperatureCalibration temp_sensor_calibration;
#endif

} Nvm;
extern Nvm nvm;

void init_nvm (void );
void nvm_lock_page (uint8_t );
void nvm_unlock_page (uint8_t );
void nvm_execute_cmd (const uint8_t , const uint32_t );
