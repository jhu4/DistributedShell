CC      = gcc
CFLAGS  = -Wall -g
LDFLAGS = -g
TARGET  = copy_time
SRCEXT  = c

SOURCES = $(wildcard src/*.$(SRCEXT))
OBJECTS = $(patsubst src/%, build/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS	= $(OBJECTS:.o=.d)
DOXYLOG = "doxygen.log"
CCFLAGS = -MP -MD -I include

default: all

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

build/%.o : src/%.$(SRCEXT)
	mkdir -p build 
	$(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS) $(DOXYLOG)
	rm -rf build
	
docs:
	@doxygen > $(DOXYLOG)
	@echo "Doxygen Generated"

-include $(DEPS)



