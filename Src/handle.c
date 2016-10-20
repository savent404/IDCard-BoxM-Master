#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "box_bsp.h"
#include "nv_040c.h"
//#define DEBUGE 1
uint32_t BOX_NUM = 1;
uint32_t FULL_MASK = 0x80000000;
static uint8_t leap[32][9] = {""};
unsigned char Check_Func(unsigned char* pt) {
	int num = 8;
	unsigned char ans = 0;
	
	while (num--)
		ans += *pt++;
	return ans;
}
static uint8_t get_emtpy_bos(uint32_t box_status);

/* cmp 8 Bytes */
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

/* cpy 8 Bytes */
static void leap_cpy(uint8_t *des, uint8_t *src) {
	int i = 8;
	while (i--) {
		*des++ = *src++;
	}
}

/**
  * @Retvl: if a locationg is full, each bit will be @1
  */
static uint32_t leap_read(void) {
	uint32_t buf = 0;
	uint32_t loc = 0;
	uint32_t cnt = BOX_NUM;
	
	while (cnt--) {
		buf |= (*leap[loc] & 1) << loc;
		loc++;
	}
	return buf;
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
	memset(leap, 0, (9*32));
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
            @FF"通讯错误"
  */	uint32_t box_status;
	uint8_t status = 0,i;
	uint8_t recive_buff[8] = "";
	uint8_t rec_flag[5];
uint32_t ID_Check_Handle(void *arg) {

	rec_flag[0] = getchar();
	rec_flag[1] = getchar();
	/* waiting there, until data comming */
	for (i = 0; i < 8; i++) {
		recive_buff[i] = getchar();
	}
	rec_flag[2] = getchar();
	rec_flag[3] = getchar();
	rec_flag[4] = getchar();
	
	/* rec_flag check */
	if (rec_flag[0] != rec_flag[1]) {
		memset(rec_flag, 0, 5);
		return 0xFF;
	}
	if (rec_flag[2] != rec_flag[3]) {
		memset(rec_flag, 0, 5);
		return 0xFF;
	}
	if (rec_flag[0] != 0xAA) {
		memset(rec_flag, 0, 5);
		return 0xFF;
	}
	if (rec_flag[2] != 0x55) {
		memset(rec_flag, 0, 5);
		return 0xFF;
	}
	if (rec_flag[4] != Check_Func(recive_buff)) {
		memset(rec_flag, 0, 5);
		return 0xFF;
	}
	memset(rec_flag, 0, 4);
	
//	#if (DEBUGE)
//	#else
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
//	#endif
	/* search the same id in leap */
	status = 0;
	for (i = 0; i < BOX_NUM; i++) {
		if (!leap_cmp(leap[i] + 1, recive_buff)) {
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
		i = get_emtpy_bos(leap_read());
		
		// If there are some empty box, it'd not return a 0xFF
		if ( i == 0xFF) {
			/* voice memssage: "No box for you" */
			SoundPlay(33);
			return 0x02;
		}
		else {
			/* regist UID first */
			leap_cpy(leap[i] + 1, recive_buff);
			*leap[i] = 1;
			
			/* open box */
			if (BSP_Box_Ctl(Box_ID(i)))
				return 0x03;
			SoundPlay(i);
			return 0x00;
		}
	}
	
	// every thing is ok
	return 0;
	
}

__weak uint32_t getseed(void) {
	static uint32_t code = 0x6713A1FD;
	uint32_t buf;
	if (code & 0x80000000)
		buf = 1;
	else
		buf = 0;
	
	code <<= 1;
	code |= buf;
	code += 1;
	return code;
}

/**
  * @Para:  box_status - each bit @1-notempty, @0-empty
  * @Retvl: @0xFF, didn't have a empty box
            #else return a num means location
  */
static uint8_t get_emtpy_bos(uint32_t box_status) {
	uint8_t loc = getseed() % BOX_NUM; // loc = (0 ~ BOX_NUM -1 )
	uint32_t max_cnt = BOX_NUM;        // it will search BOX_NUM times(max)
	
	while (max_cnt--) {
		if ((box_status & Box_ID(loc)) == 0) {
			return loc;
		}
		loc += 1;
		if (loc >= BOX_NUM) {
			loc = 0;
		}
	}
	return 0xFF;
}
