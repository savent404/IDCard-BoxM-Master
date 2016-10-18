#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "box_bsp.h"
#include "nv_040c.h"

uint32_t BOX_NUM = 1;
uint32_t FULL_MASK = 0x80000000;
static uint8_t leap[32][9] = {""};

static uint8_t leap_cmp(uint8_t *src1, uint8_t *src2) {
	int i = 8;
	while (i--) {
		if (*src1 == *src2) {
			src1++,src2++;
			continue;
		}
		else
			return 1;
	}
	return 0;
}

static void leap_cpy(uint8_t *des, uint8_t *src) {
	int i = 8;
	while (i--) {
		*des++ = *src++;
	}
}

/**
  * @Retvl: if a locationg is full, that bit will be @1
  */
static uint32_t leap_read(void) {
	uint32_t buf = 0;
	uint32_t loc = 0;
	uint32_t cnt = BOX_NUM;
	while (cnt--) {
		buf >>= 1;
		if (*leap[loc++]) {
			buf |= 0x80000000;
		}
	} return buf;
}

static void leap_set(uint32_t loc) {
	*leap[loc] = 1;
}
static void leap_clear(uint32_t loc) {
	*leap[loc] = 0;
}

void Handle_Init(void) {
	int i = 0;
	BSP_Box_Init();
	/* box number check */
	BOX_NUM = (GPIOA->IDR >> 2) & 0x1F;
	BOX_NUM += 1;
	/* define BOX_FULL_MASK */
	FULL_MASK = 0;
	for (i = 0; i < BOX_NUM; i++) {
		FULL_MASK >>= 1;
		FULL_MASK |= 0x80000000;
	}
	memset(leap, 0, sizeof(leap));
	SoundPlay(0xE7);
	for (i = 0; i < BOX_NUM; i++) {
		if(!BSP_Box_Ctl(Box_ID(i))) {
			SoundPlay(i);
			HAL_Delay(2000);
		}
	}
}

/**
  * @Retvl: @0 "操作成功"
            @1 "有柜子未关闭，拒绝任何操作"
            @2 "柜子已满"
            @3 "开锁错误"
  */	uint32_t box_status;
	uint8_t status = 0,i;
	uint8_t recive_buff[8] = "";
	uint8_t buf[2];
uint32_t ID_Check_Handle(void *arg) {

	buf[0] = getchar();
	/* waiting there, until data comming */
	for (i = 0; i < 8; i++) {
		recive_buff[i] = getchar();
	}
	buf[1] = getchar();
	
	/* check box status */
	box_status = BSP_Box_Check_L();
	if (box_status != 0xFFFFFFFF) {
		/* voice memssage: "Close all box first" */
		SoundPlay(32);
		/* return error */
		return 0x01;
	}
	
	/* check void box */
	box_status = BSP_Box_Check_IF();
	
	/* clear bit */
	for (i = 0; i < BOX_NUM; i++) {
		if (box_status & 0x01) {
			memset(leap[i], 0, sizeof(uint8_t)*9);
		}
		box_status >>= 1;
	}
	
	/* search the same id in leap */
	status = 0;
	for (i = 0; i < BOX_NUM; i++) {
		if (!leap_cmp(leap[i] + 1, recive_buff)) {
			SoundPlay(i);
			HAL_Delay(2000);
			status = 1;
			break;
		}
	}
	
	/* already has the id */
	if (status) {
		/* open the old box */
		if (BSP_Box_Ctl(Box_ID(i)))
			return 0x03;
		SoundPlay(i);
	}
	
	/* add new id */
	else {
		/* search a void box */
		for (i = 0; i < BOX_NUM; i++) {
			if (*leap[i] == 0) {
				/* find a empty box */
				/* regist UID first */
				leap_cpy(leap[i] + 1, recive_buff);
				leap_set(i);
				/* open box */
				if (BSP_Box_Ctl(Box_ID(i)))
					return 0x03;
				SoundPlay(i);
				return 0x00;
			}
		}
		/* voice memssage : "No box for you" */
		SoundPlay(33);
		return 0x02;
	}
	
	return 0;
	
}
