#include <stdbool.h>
#include <stdint.h>

// listing of com io addresses
enum {
	SERIAL_COM1 = 0x3f8,
	SERIAL_COM2 = 0x2f8,
	SERIAL_COM3 = 0x3e8,
	SERIAL_COM4 = 0x2e8,

	SERIAL_DEFAULT = SERIAL_COM1,
};

// address offsets for various info
enum {
	// these two sets of ports alternate depending on the
	// uppermost value of the SERIAL_LINE_CONTROL register
	// this set is used when 0
	SERIAL_DATA              = 0,
	SERIAL_INTERRUPT_ENABLE  = 1,

	// and this when it's 1
	SERIAL_DIVISOR_UPPER     = 0,
	SERIAL_DIVISOR_LOWER     = 1,

	// and the rest are unaffected
	SERIAL_IDENT_CONTROL     = 2,
	SERIAL_LINE_CONTROL      = 3,
	SERIAL_MODEM_CONTROL     = 4,
	SERIAL_LINE_STATUS       = 5,
	SERIAL_MODEM_STATUS      = 6,
	SERIAL_SCRATCH_REGISTER  = 7,
};

// fields in SERIAL_LINE_CONTROL register
enum {
	SERIAL_LINECTRL_DATASIZE_LOWER = (1 << 0),
	SERIAL_LINECTRL_DATASIZE_UPPER = (1 << 1),
	SERIAL_LINECTRL_STOPBITS       = (1 << 2),
	SERIAL_LINECTRL_DLAB           = (1 << 7),
};

// fields in SERIAL_LINE_STATUS register
enum {
	SERIAL_LINESTAT_HAVE_DATA      = (1 << 0),
	SERIAL_LINESTAT_ERROR_OVERRUN  = (1 << 1),
	SERIAL_LINESTAT_ERROR_PARITY   = (1 << 2),
	SERIAL_LINESTAT_ERROR_FRAMING  = (1 << 3),
	SERIAL_LINESTAT_BREAK_INTR     = (1 << 4),
	SERIAL_LINESTAT_TRANSMIT_EMPTY = (1 << 5),
	SERIAL_LINESTAT_ERROR_FIFO     = (1 << 7),
};

// fields in SERIAL_INTERRUPT_ENABLE register
enum {
	SERIAL_INTERRUPT_HAS_DATA       = (1 << 0),
	SERIAL_INTERRUPT_TRANSMIT_EMPTY = (1 << 1),
	SERIAL_INTERRUPT_ERROR          = (1 << 2),
	SERIAL_INTERRUPT_STATUS_CHANGE  = (1 << 3),
};

static inline void outb( uint16_t port, uint8_t value ){
	asm volatile( "outb %0, %1" :: "a"(value), "Nd"(port));
}

static inline uint8_t inb( uint16_t port ){
	uint8_t value = 0;
	asm volatile( "inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

static uint8_t read_register( unsigned port, unsigned reg ){
	return inb( port + reg );
}

static void write_register( unsigned port, unsigned reg, uint8_t value ){
	outb( port + reg, value );
}

static bool serial_can_transmit( void ){
	unsigned temp = read_register( SERIAL_DEFAULT, SERIAL_LINE_STATUS );

	return temp & SERIAL_LINESTAT_TRANSMIT_EMPTY;
}

void init_serial() {
   outb(SERIAL_DEFAULT + 1, 0x00);    // Disable all interrupts
   outb(SERIAL_DEFAULT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(SERIAL_DEFAULT + 0, 0x01);    // Set divisor to 3 (lo byte) 115200 baud
   outb(SERIAL_DEFAULT + 1, 0x00);    //                  (hi byte)
   outb(SERIAL_DEFAULT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(SERIAL_DEFAULT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(SERIAL_DEFAULT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void debug_putchar( int c ){
	static bool initialized = false;

	if ( !initialized ){
		init_serial();
		initialized = true;
	}

	while ( !serial_can_transmit( ));
	write_register( SERIAL_DEFAULT, SERIAL_DATA, c );
}
