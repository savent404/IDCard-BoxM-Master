#include "NV_040C.H"

void SoundPlay(uint32_t voice_addr) {
	int i = 8;
	
	/* Waiting for voice complet */
	
	/* Reset Pin 5ms */
	DATA_PIN_RESET();
	HAL_Delay(5);
	
	while (i--) {
		
		DATA_PIN_SET();
		
		if (voice_addr & 0x01) {
			HAL_Delay(3);
			DATA_PIN_RESET();
			HAL_Delay(1);
		}
		else {
			HAL_Delay(1);
			DATA_PIN_RESET();
			HAL_Delay(3);
		}
		voice_addr >>= 1;
	}
	
	DATA_PIN_SET();
}
