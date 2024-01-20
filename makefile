COMPILER = clang
APP = nuts
EXT_LIBS =
WARNINGS = -Wall -Wextra
DEBUG_BUILD = -g -std=c2x -Wunused-variable
LEAKS_BUILD  = -fsanitize=address
RELEASE_FLAGS = -std=c2x -O3
CFILES = $(wildcard src/*.c)

# Discover all .c files in the libs directory
LIB_CFILES = $(wildcard libs/*.c)
OBJ_FLAGS = -g -Wunused-variable -std=c2x
export ASAN_OPTIONS := allocator_may_return_null=1

# Create object files list from CFILES and LIB_CFILES
OBJ = $(patsubst src/%.c, src/%.o, $(CFILES)) $(patsubst libs/%.c, libs/%.o, $(LIB_CFILES))

# Rule for compiling .c files into .o files
src/%.o: src/%.c
	$(COMPILER) $(OBJ_FLAGS) -c $< -o $@

# Rule for compiling library .c files into .o files
libs/%.o: libs/%.c
	$(COMPILER) $(OBJ_FLAGS) -c $< -o $@

$(APP): $(OBJ)
	$(COMPILER) $^ -o $@ $(EXT_LIBS) $(DEBUG_BUILD) $(WARNINGS)
	./nuts

#leaks: OBJ_FLAGS = -ggdb
leaks: export ASAN_OPTIONS := allocator_may_return_null=1 # for debug -fsanitize=address
leaks: $(OBJ)
	$(COMPILER) $^ -o $(APP)-$@ $(EXT_LIBS) $(DEBUG_BUILD) $(WARNINGS) $(LEAKS_BUILD)


# NOTE: make clean first
release: OBJ_FLAGS = -O3
release: $(OBJ)
	$(COMPILER) $^ -o $(APP)-$@ $(RELEASE_FLAGS) $(EXT_LIBS)

clean:
	rm -rf $(APP).* $(APP)* **/*.o *.o

check:
	cppcheck --check-level=exhaustive --enable=style -j 8 ./src
