ENTRY (loader)

SECTIONS {
	/* . = 0x00100000; */
	. = 0x100000;

	.__mbHeader : AT(ADDR(.__mbHeader)) {
		*(.__mbHeader)
	}

	. = 0x101000;
	.__smp_entry : AT(ADDR(.__smp_entry)) {
		*(.__smp_entry)
	}

	.text ALIGN (0x1000) : AT(ADDR(.text)) {
		*(.text)
	}

	.rodata ALIGN (0x1000) : AT(ADDR(.rodata)) {
		*(.rodata*)
	}

	.data ALIGN (0x1000) : AT(ADDR(.data)) {
		*(.data)
	}

	.bss ALIGN (0x1000) : AT(ADDR(.bss)) {
		sbss = .;
		*(COMMON)
		*(.bss)
		ebss = .;
	}

	end = .; _end = .; __end = .;
}
