TARGET = grandmas-agenda

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = Makefile.depend

INCLUDES = -I./include
CFLAGS = -O2 -Wall $(INCLUDES) -DLINUX_MACHINE -Wall -Wextra -pthread
LDFLAGS = -lm -pthread

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

run: all
	@./$(TARGET)

.PHONY: depend clean
depend:
	$(CC) $(INCLUDES) -MM $(SRCS) > $(DEPS)
	@sed -i -E "s/^(.+?).o: ([^ ]+?)\1/\2\1.o: \2\1/g" $(DEPS)

clean:
	$(RM) $(OBJS) $(TARGET)

-include $(DEPS)