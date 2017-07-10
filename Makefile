k-obj   = entry.o main.o debug.o serial.o pit.o smp_patch.o \
	      display.o apic.o mp.o bios.o interrupts.o idt.o
LDFLAGS = -melf_i386 -Tlinker.ld
CFLAGS  = -O0 -m32 -ffreestanding -nostdlib -nodefaultlibs -nostartfiles \
          -fno-builtin -I./include -Wall -g
ASFLAGS = -felf

kernel: $(k-obj)
	$(LD) $(LDFLAGS) -o $@ $(k-obj)

%.o: %.s
	nasm $(ASFLAGS) $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.bin: %.s
	nasm -fbin $< -o $@

smp_patch.o: smp.bin
	$(LD) -melf_i386 -r -b binary -o $@.tmp.o $<
	binary_name="_binary_$$(echo $< | tr '\\/\-. ' _)"; \
	$(CROSS)objcopy --redefine-sym $${binary_name}_start=smp_boot_start \
	                --redefine-sym $${binary_name}_end=smp_boot_end \
	                --redefine-sym $${binary_name}_size=smp_boot_size \
	                $@.tmp.o $@
	rm $@.tmp.o

.PHONY: clean
clean:
	rm $(k-obj) kernel smp.bin

.PHONY: test
test:
	qemu-system-i386 -kernel kernel -smp cpus=6 \
					 -serial stdio -enable-kvm
					 #-d cpu_reset,guest_errors -serial stdio
