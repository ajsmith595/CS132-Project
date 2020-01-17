EXE = robotarm
BASEDIR := $(abspath ./)
OBJPATH := $(BASEDIR)/obj
INCLUDE_PATH = $(BASEDIR)/include
LIB_PATH = $(BASEDIR)/lib
SRC_PATH = $(BASEDIR)/src


CC = gcc
CFLAGS = -c -Wall
LDFLAGS = -Xlinker
LIBS = $(LIB_PATH)/dynamixel.o -lm
INCLUDES = -I$(INCLUDE_PATH) -I$(LIB_PATH)/include

# list of all src and obj files
SRC = $(wildcard $(SRC_PATH)/*.c)
OBJ = $(SRC:$(SRC_PATH)/%.c=$(OBJPATH)/%.o)

.PHONY: all clean

all: dir $(EXE)

# links object files and libraries into one
$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@ 

# compiles .c files to object files
$(OBJPATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJ)

dir:
	mkdir -p $(OBJPATH)