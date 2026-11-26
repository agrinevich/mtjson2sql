DEBUG = 1

TARGET = mtjson2sql
STANDARD = c99
WARNINGS = -Wall -Wextra -Wpedantic
CC = gcc
CFLAGS = $(WARNINGS) -std=$(STANDARD)
# CPPFLAGS = -I$(INC_DIR)
CPPFLAGS =
LDFLAGS =

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0
else
CFLAGS += -O3
endif

COMPILER_CALL = $(CC) $(CFLAGS) $(CPPFLAGS)

SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

#
# TARGETS
#

build: $(OBJECTS)
	$(COMPILER_CALL) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

execute:
	./$(TARGET)
	
clean:
	rm -f *.o
	rm -f $(TARGET)

#
# PATTERNS
#

%.o: %.c
	$(COMPILER_CALL) -c $< -o $@
