# -*- Makefile -*-

# builds the final executable, adapted from https://stackoverflow.com/questions/40621451/makefile-automatically-compile-all-c-files-keeping-o-files-in-separate-folde

# source folder, object files folder, executable folder and executable name
SRC := ./src
INC := ../include
OBJ := ./build
DEBUG := ./debug
BIN := ./bin
EXECUTABLE_NAME := flightDreams
SOURCE_EXTENSION := c
HEADER_EXTENSION := h

# compiler
CC := gcc

# source file and object file names
SOURCES := $(wildcard $(SRC)/*.$(SOURCE_EXTENSION))
OBJECTS := $(patsubst $(SRC)/%.$(SOURCE_EXTENSION), $(OBJ)/%.o, $(SOURCES))
DEBUG_OBJ := $(patsubst $(SRC)/%.$(SOURCE_EXTENSION), $(DEBUG)/%.dbg, $(SOURCES))

#debug and valgrind flags:
COMPILE_FLAGS = -Wall -Wextra -Wshadow -pedantic -O2 #-Werror
DEBUG_FLAGS = -g -ggdb3
VALGRING_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt

# compiles every object into the final executable
all: makeBuild makeBin $(EXECUTABLE_NAME)
	@echo -e '\n\033[1;33mfinished compiling\033[0m'

$(EXECUTABLE_NAME): $(OBJECTS)
	$(CC) $(COMPILE_FLAGS) $(OBJECTS) -o $(BIN)/$@

debug: makeDebug makeBin $(EXECUTABLE_NAME).dbg

$(EXECUTABLE_NAME).dbg: $(DEBUG_OBJ)
	$(CC) $(COMPILE_FLAGS) $(DEBUG_FLAGS) $(DEBUG_OBJ) -o $(BIN)/$@

# compiles every source file into its respective object file
# $@ = name of rule/recipe target
# $< = name of first dependency of this recipe
$(OBJ)/%.o: $(SRC)/%.$(SOURCE_EXTENSION)
	$(CC) $(COMPILE_FLAGS) -c $< -o $@
#	$(CC)  -c $< -o $@

$(DEBUG)/%.dbg: $(SRC)/%.$(SOURCE_EXTENSION)
	$(CC) $(COMPILE_FLAGS) $(DEBUG_FLAGS) -c $< -o $@
#	$(CC) $(DEBUG_FLAGS) -c $< -o $@

# create the build folder if it doesn't already exist
makeBuild:
	mkdir -p $(OBJ)/

makeDebug:
	mkdir -p $(DEBUG)/

makeBin:
	mkdir -p $(BIN)/

# remove the build and debug folder
clean:
	rm -rf $(OBJ)/
	rm -rf $(DEBUG)/
	rm -f valgrind-out.txt

# remove the bin folder
uninstall: clean
	rm -rf $(BIN)/

valgrind:
	valgrind $(VALGRING_FLAGS) $(BIN)/$(EXECUTABLE_NAME).dbg
