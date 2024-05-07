# Nuke built-in rules and variables.
override MAKEFLAGS += -rR
 
# Convenience macro to reliably declare user overridable variables.
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef
 
# It is suggested to use a custom built cross toolchain to build a kernel.
# We are using the standard "cc" here, it may work by using
# the host system's toolchain, but this is not guaranteed.
override DEFAULT_CC := x86_64-elf-gcc
$(eval $(call DEFAULT_VAR,CC,$(DEFAULT_CC)))
 
# User controllable C flags.
override DEFAULT_CFLAGS := -g -O2 -pipe
$(eval $(call DEFAULT_VAR,CFLAGS,$(DEFAULT_CFLAGS)))
 
# User controllable C preprocessor flags. We set none by default.
override DEFAULT_CPPFLAGS :=
$(eval $(call DEFAULT_VAR,CPPFLAGS,$(DEFAULT_CPPFLAGS)))
 
# User controllable nasm flags.
override DEFAULT_NASMFLAGS := -F dwarf -g
$(eval $(call DEFAULT_VAR,NASMFLAGS,$(DEFAULT_NASMFLAGS)))
 
# Internal C flags that should not be changed by the user.
override CFLAGS += \
    -Wall \
    -Wextra \
		-Wno-implicit-fallthrough \
		-Werror \
    -std=gnu11 \
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
    -mno-red-zone \
		-std=gnu2x
 
# Internal C preprocessor flags that should not be changed by the user.
override CPPFLAGS := \
    -I include \
    $(CPPFLAGS) \
    -MMD \
    -MP
 
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
all: bin/klibc.a
 
# Link rules for the final kernel executable.
bin/klibc.a: Makefile $(OBJ)
	@echo "[AR] Packing klibc"
	@mkdir -p "$$(dirname $@)"
	@rm -f $@
	@$(AR) $(ARFLAGS) $@ $(OBJ)
 
# Include header dependencies.
-include $(HEADER_DEPS)
 
# Compilation rules for *.c files.
obj/%.c.o: src/%.c Makefile
	@echo "[CC] Building klibc component $@"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
 
# Compilation rules for *.S files.
obj/%.S.o: src/%.S Makefile
	@echo "[CC] Building klibc component $@"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
 
# Compilation rules for *.asm (nasm) files.
obj/%.asm.o: src/%.asm Makefile
	@echo "[CC] Building klibc component $@"
	@mkdir -p "$$(dirname $@)"
	@nasm $(NASMFLAGS) $< -o $@
 
# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf bin obj
