#include <stdbool.h>
#include <stddef.h>
#include <arm_neon.h>

// M-z: zap-to-char

#define ARRLEN(a) (sizeof((a)) / sizeof((*(a))))

#define MAILBOX_BASE   0x2000B880
#define MAILBOX_READ   0x00
#define MAILBOX_STATUS 0x18
#define MAILBOX_WRITE  0x20
#define MAILBOX_EMPTY  0b1000000000000000000000000000000
#define MAILBOX_FULL   0b10000000000000000000000000000000
#define GPUINFO_BASE   0x40040000

#define GPIO_LEV0 0x20200034
#define GPIO_LEV1 0x20200038
#define GPFSEL4   0x20200010
#define GPSET1    0x20200020
#define GPCLR1    0x2020002C

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

extern void put32(uint32_t, uint32_t);
extern uint32_t get32(uint32_t);

void write_mailbox(uint8_t channel, uint32_t data) {
	while ((get32(MAILBOX_BASE + MAILBOX_STATUS) & MAILBOX_FULL) != 0);
	put32(MAILBOX_BASE + MAILBOX_WRITE, data + channel);
}

uint32_t read_mailbox(uint8_t channel) {
	uint32_t r = 0;
	while (true) {
		while ((get32(MAILBOX_BASE + MAILBOX_STATUS) & MAILBOX_EMPTY) != 0);

		uint32_t r = get32(MAILBOX_BASE + MAILBOX_READ);
		if ((r & 0xF) == channel) {
			break;
		}
	}
	return r;
}

// @NOTE: 0x30000000 should be free memory based on the BCM-2835 memory map
void *heap = (void *) 0x30000000;

// @NOTE: Simple watermark allocator
void *malloc(size_t size) {
	void *m = heap;
	heap += size;
	return m;
}

void memcpy(void *dest, const void *src, size_t count, size_t add) {
        char *dst8 = (char *) dest;
        char *src8 = (char *) src;
        
        if (count & 1) {
		dst8[0] = src8[0];
		dst8 += 1;
		src8 += 1;
        }
        
        count >>= 1;
        while (count--) {
		dst8[0] = src8[0] + add;
		dst8[1] = src8[1] + add;
        
		dst8 += 2;
		src8 += 2;
        }
}

#include "image.h"

int32_t ves_main(void) {

	/* uint8x8_t vec_a, vec_b, vec_dest; // a vector of 8 8bit ints */
	/* vec_a = vdup_n_u8(9); */
	/* vec_b = vdup_n_u8(10); */
	/* vec_dest = vec_a * vec_b; //90 */
	/* vec_a = vec_dest * vec_b; //90*10 = 900 */
	/* vec_dest = vec_a * vec_b; //900*10 = 9000 */
	
	const uint32_t properties[] = {
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		0,   32,
		0,   0,
		0,   0,
		0,   0,
	};

	for (size_t i = 0; i < ARRLEN(properties); ++i) {
		put32(GPUINFO_BASE + (i * 4), properties[i]);
	}

	write_mailbox(1, GPUINFO_BASE);
	read_mailbox(1); // @TODO: error?
	
	uint32_t *fb = (uint32_t *) get32(GPUINFO_BASE + 0x20);
       
	// exacerbate

	uint8_t tick = 0;

	void memset(uint32_t *b, uint32_t x, size_t n) {
		for (size_t i = 0; i < n; i++) {
			b[i] = x;
		}
	}
	
	while (true) {
		if (tick % 2) {
			memset(fb, tick << 16, SCREEN_WIDTH * SCREEN_HEIGHT);
		} else  {
			memset(fb, tick, SCREEN_WIDTH * SCREEN_HEIGHT);
		}
		++tick;
	}

	
	return 0;
}

/* uint32_t button_lev0 = get32(GPIO_LEV0); */

/* if (button_lev0 & (1 << 14)) { */
/*	for (size_t i = 0; i < properties[0] * properties[1]; ++i) { */
/*		fb[i] = 0xFF0000; */
/*	} */
/* } */

/* uint32_t ra = get32(GPFSEL4); */
/* ra &= ~(6 << 21); */
/* put32(GPFSEL4, ra); */
/* 	put32(GPSET1, 1 << (47 - 32)); */
		
/* 	for (volatile int32_t i = 0; i < 0x100000; ++i); */
		
/* 	put32(GPCLR1, 1 << (47 - 32)); */
		
/* 	for (volatile int32_t i = 0; i < 0x100000; ++i); */
