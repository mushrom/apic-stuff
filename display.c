#include <stdint.h>
#include <stddef.h>

void *memset( void *dest, int c, size_t n );

struct vga_char {
	uint8_t character, color;
};

static struct vga_char *vga = (void *)0xb8000;
static unsigned display_x = 0;
static unsigned display_y = 0;

void display_clear( void ){
	memset( vga, 0, sizeof( struct vga_char[80 * 25] ));
}

void display_putchar( char c ){
	if ( display_x >= 80 ){
		display_y++;
		display_x = 0;
	}

	if ( c == '\n' ){
		display_y++;
		display_x = 0;
		return;
	}

	if ( display_y >= 24 ){
		display_y = 0;
		display_x = 0;
	}

	struct vga_char *vc = vga + display_y*80 + display_x;

	vc->character = c;
	vc->color     = 0x7;

	display_x++;
}

void display_puts( const char *s ){
	for (; *s; s++ ){
		display_putchar(*s);
	}
}

