/*	simple Hello World for OpenShift nes-s2i builder for cc65, for NES
 *      Based on example code by Doug Fraker
 *	using neslib
 */	


#include "neslib.h"
#include "MoreLib.c"

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)

#define FP_BITS	4

const unsigned char palette[16]={ 0x0f,0x00,0x16,0x30,0x0f,0x01,0x21,0x31,0x0f,0x06,0x16,0x26,0x0f,0x09,0x19,0x29 };

int index;

void write_str(unsigned char x, unsigned char y, unsigned char *str) {
	while( *str != 0) {
		vram_adr(NTADR_A(x, y));
		PPU_DATA = *str;
		++str;
		++x;
	}
}

void render_logo(unsigned int x, unsigned int y) {
	unsigned char *tile = (unsigned char *) 0x80; // start of logo in charset A
	unsigned int left = 16; // width of logo

	vram_adr(NTADR_A(x, y));
	while(tile <= (unsigned char *) 0xFF) { // end of logo in charset A
		PPU_DATA = tile;
		++tile;
		--left;
		if(left == 0) {
			y++;
			vram_adr(NTADR_A(x, y));
			left = 16;
		}
	}
}

unsigned char buf[256]={};

void type_str(unsigned char x, unsigned char y, unsigned char *str, unsigned char _delay) {
	while( *str != 0) {
		buf[0] = MSB(NTADR_A(x,y))|NT_UPD_HORZ;
		buf[1] = LSB(NTADR_A(x,y));
		buf[2] = 1; // 1 write
		buf[3] = *str; // tile #
		buf[4] = NT_UPD_EOF;

		set_vram_update(buf);

		++str;
		++x;
		if(_delay > 0) {
			delay(_delay);
		} else {
			delay(1);		
		}
	}
	set_vram_update(NULL);
}

void clear_line(unsigned char row) {
	type_str(1, row, "                                      ", 0);
}

unsigned int s;

void main (void) {
	pal_bg(palette);
	render_logo(1, 1);
	ppu_on_all();

	scroll(0, 0);

	while (1) {
		delay(40);
		type_str(1, 10, (unsigned char *) "Red Hat [ OpenShift", 3); // square bracket is actually defined as dash symbol in tileset
		
		delay(40);
		clear_line(10);

		s=0;
		while(s<256) {
			delay(1);
			scroll(s, 0);
			++s;
		}

		delay(20);
	}
};
