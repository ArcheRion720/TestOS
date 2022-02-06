KERNEL := kernel.elf
CC ?= gcc
LD ?= lld
ASM ?= nasm
CFLAGS ?= -Wall -Wextra -O2 -pipe -iquote src/intf
LDFLAGS ?=
LIMINE ?= target/limine

INTERNALCFLAGS :=		 \
	-I.                  \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-pie                 \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-m64			     \
    -MMD				 \
	-mmanual-endbr       \
	-mgeneral-regs-only  \
	-fcf-protection=branch
 
INTERNALLDFLAGS :=		   \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static                \
	-pie                   \
	--no-dynamic-linker    \
	-ztext				   \
	-T target/linker.ld 

CFILES := $(shell find src/impl -name *.c)
ASMFILES := $(shell find src/asm -name *.asm)
OBJ := $(patsubst src/impl/%.c, build/%.o, $(CFILES))
OBJASM := $(patsubst src/asm/%.asm, build/%.o, $(ASMFILES))
RESOBJ := $(shell find res -name *.o)

 $(OBJ): build/%.o : src/impl/%.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

$(OBJASM): build/%.o : src/asm/%.asm
	$(ASM) -f elf64 $(patsubst build/%.o, src/asm/%.asm, $@) -o $@

$(KERNEL): $(OBJASM) $(OBJ) 
	$(LD) $(OBJASM) $(OBJ) $(RESOBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o dist/${@F}

.PHONY: setup
setup:
	mkdir -p build/memory
	mkdir -p build/ahci
	mkdir -p dist
	mkdir -p iso_root

.PHONY: build-kernel
build-kernel: $(KERNEL)
	cp -v dist/$(KERNEL) target/limine.cfg $(LIMINE)/limine.sys $(LIMINE)/limine-cd.bin $(LIMINE)/limine-eltorito-efi.bin iso_root/ && \
	xorriso -as mkisofs -b limine-cd.bin \
			-no-emul-boot -boot-load-size 4 -boot-info-table \
			--efi-boot limine-eltorito-efi.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
			iso_root -o dist/TestOS.iso && \
	./$(LIMINE)/limine-install-linux-x86_64 dist/TestOS.iso