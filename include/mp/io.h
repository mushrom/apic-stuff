#ifndef _MP_IO_H
#define _MP_IO_H 1

static inline void outb( uint16_t port, uint8_t value ){
	asm volatile( "outb %0, %1" :: "a"(value), "Nd"(port));
}

static inline uint8_t inb( uint16_t port ){
	uint8_t value = 0;
	asm volatile( "inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

#endif
