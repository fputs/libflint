CMAKE_OPTS=-DCMAKE_EXPORT_COMPILE_COMMANDS=1

all:
	mkdir -p build
	cd build && \
	cmake ${CMAKE_OPTS} .. && \
	$(MAKE) && \
	cp compile_commands.json ..

clean:
	rm -rf build
	rm -f compile_commands.json

