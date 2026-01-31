SRC       := src
BIN       := bin
CC        := clang
ASSEMBLY  := calc_sum_five
EXTENSION := 

INCLUDES     := 
LINKER_FLAGS := -lraylib -lm
COMPILER_FLAGS := -Wall -Werror -Wextra -g -O0 -Wno-system-headers -Wno-unused-but-set-variable -Wno-unused-variable -Wno-varargs -Wno-unused-private-field -Wno-unused-parameter -Wno-unused-function -fsanitize=undefined -fsanitize-trap

SRC_FILES := $(shell find $(src_dir) -type f -name '*.c')

all: 
	$(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o $(BIN)/$(ASSEMBLY)$(EXTENSION) $(INCLUDES) $(LINKER_FLAGS)
