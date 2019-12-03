

CC = gcc

#LDFLAGS = -Xlinker -L$(ROOT)

#LOADLIBES = -lm

#CFLAGS = -I$(ROOT)/include

% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -lm \
        $? $(LOADLIBES) dynamixel.o
