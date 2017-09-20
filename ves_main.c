#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ARRLEN(a) (sizeof((a)) / sizeof((*(a))))

extern void put32(uint32_t, uint32_t);
extern uint32_t get32(uint32_t);

#define MAILBOX_BASE      0x2000B880
#define MAILBOX_READ      0x00
#define MAILBOX_STATUS    0x18
#define MAILBOX_WRITE     0x20
#define MAILBOX_EMPTY     0b1000000000000000000000000000000
#define MAILBOX_FULL      0b10000000000000000000000000000000

void write_mailbox(uint8_t channel, uint32_t data) {
	while ((get32(MAILBOX_BASE + MAILBOX_STATUS) & MAILBOX_FULL) != 0);
	put32(MAILBOX_BASE + MAILBOX_WRITE, data + channel);
}

uint32_t read_mailbox(uint8_t channel) {
	uint32_t r;
	while (true) {
		while ((get32(MAILBOX_BASE + MAILBOX_STATUS) & MAILBOX_EMPTY) != 0);

		uint32_t r = get32(MAILBOX_BASE + MAILBOX_READ);
		if ((r & 0xF) == channel) {
			break;
		}
	}
	return r;
}

int32_t ves_main(void) {

	#define GPU_BASE 0x40040000
	
	uint32_t properties[] = {
		640, 480,
		640, 480,
		0,   32,
		0,   0,
		0,   0,
		0,   0,
	};

	for (size_t i = 0; i < ARRLEN(properties); ++i) {
		put32(GPU_BASE + (i * 4), properties[i]);
	}

        write_mailbox(1, GPU_BASE);
        read_mailbox(1);

	uint32_t fb = get32(GPU_BASE + 0x20);

	int32_t i = 0;
	int32_t j = 0;
	put32(fb, 0x336699);
	
	return 0;
}
