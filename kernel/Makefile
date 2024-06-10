# Nuke built-in rules and variables.
override MAKEFLAGS += -rR
 
override KERNEL := limix
 
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef
 
override DEFAULT_CC := x86_64-elf-gcc
$(eval $(call DEFAULT_VAR,CC,$(DEFAULT_CC)))
 
override DEFAULT_LD := x86_64-elf-ld
$(eval $(call DEFAULT_VAR,LD,$(DEFAULT_LD)))
 
override DEFAULT_CFLAGS := -g -Os -pipe
$(eval $(call DEFAULT_VAR,CFLAGS,$(DEFAULT_CFLAGS)))
 
override DEFAULT_CPPFLAGS :=
$(eval $(call DEFAULT_VAR,CPPFLAGS,$(DEFAULT_CPPFLAGS)))
 
override DEFAULT_NASMFLAGS := -F dwarf -g
$(eval $(call DEFAULT_VAR,NASMFLAGS,$(DEFAULT_NASMFLAGS)))
 
override DEFAULT_LDFLAGS :=
$(eval $(call DEFAULT_VAR,LDFLAGS,$(DEFAULT_LDFLAGS)))
 
override CFLAGS += \
    -Wall \
    -Wextra \
		-Werror \
		-Wno-unused-parameter \
		-Wno-unused-function \
		-Wno-implicit-fallthrough \
		-Wno-multichar \
		-Wno-write-strings \
		-std=gnu2x \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fPIE \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone

# Internal C preprocessor flags that should not be changed by the user.
override CPPFLAGS := \
		-I ../include \
    $(CPPFLAGS) \
    -MMD \
    -MP
 
# Internal linker flags that should not be changed by the user.
override LDFLAGS += \
    -m elf_x86_64 \
    -nostdlib \
    -static \
    -pie \
    --no-dynamic-linker \
    -z text \
    -z max-page-size=0x1000 \
    -T linker.ld -Map linker.map
 
# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -Wall \
    -f elf64

override ARFLAGS = qc
override AR = x86_64-elf-ar
 
# Use "find" to glob all *.c, *.S, and *.asm files in the tree and obtain the
# object and header dependency file names.
override CFILES := $(shell cd src && find -L * -type f -name '*.c')
override ASFILES := $(shell cd src && find -L * -type f -name '*.S')
override NASMFILES := $(shell cd src && find -L * -type f -name '*.asm')
override OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))
 
# Default target.
.PHONY: all
all: bin/$(KERNEL).gz

bin/$(KERNEL).a: Makefile $(OBJ)
	@echo "[AR] Packing kernel"
	@mkdir -p "$$(dirname $@)"
	@rm -f $@
	@$(AR) $(ARFLAGS) $@ $(OBJ)

# Link rules for the final kernel executable.
bin/$(KERNEL): Makefile linker.ld bin/$(KERNEL).a
	@echo "[LD] Linking kernel"
	@mkdir -p "$$(dirname $@)"
	@$(LD) obj/main.c.o $@.a ../libk/bin/libk.a $(LDFLAGS) -o $@

bin/$(KERNEL).gz: bin/$(KERNEL)
	@echo "[GZIP] Compressing kernel"
	@rm -f $@
	@gzip -k $<

# Include header dependencies.
-include $(HEADER_DEPS)
 
# Compilation rules for *.c files.
obj/%.c.o: src/%.c Makefile
	@echo "[CC] Building kernel component $@"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
 
# Compilation rules for *.S files.
obj/%.S.o: src/%.S Makefile
	@echo "[CC] Building kernel component $@"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
 
# Compilation rules for *.asm (nasm) files.
obj/%.asm.o: src/%.asm Makefile
	@echo "[CC] Building kernel component $@"
	@mkdir -p "$$(dirname $@)"
	@nasm $(NASMFLAGS) $< -o $@
 
# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf bin obj
