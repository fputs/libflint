.PHONY : clean

CFLAGS =		-std=c99 -Iinclude -pedantic -Wall -Wextra
LDFLAGS =		-fPIC -shared

TARGET = 	libflint.so
SRC !=		ls src/*.c
OBJ =			$(SRC:./src/$.c=./obj/%.o)

PREFIX =	$(DESTDIR)/usr/local
LIBDIR =	$(PREFIX)/lib

all: $(TARGET)

$(TARGET): $(OBJ)
	cc $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJ)

./obj/%.o: ./src/%.c
	cc $(CFLAGS) -c $< -o $@

install: $(TARGET)
	cp $(TARGET) $(LIBDIR)

uninstall:
	rm -f $(LIBDIR)/$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f test

test:
	cc $(CFLAGS) -o test tests/tests.c src/*.c
