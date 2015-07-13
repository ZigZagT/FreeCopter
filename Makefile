SRC := $(wildcard src/*.cpp)
TAR = $(patsubst src/%.cpp,dest/%.o,$(SRC))

.PHONY: all clean

#dest/a: src/gpio_mem_dump.c
#	gcc -std=c11 $< -o $@

all:
	@if [ ! -d ./dest ]; then \
	   mkdir ./dest ; \
	fi
	
	@for file in src/*.cpp; do \
		export objfile="$${file//src\//dest\/}";\
		export objfile="$${objfile//.cpp/.o}";\
		echo "$$objfile";\
		gcc -Wall -std=c++11 -pthread -c $$file -o $$objfile;\
	done

	@gcc -Wall -std=c++11 -pthread -lstdc++ dest/*.o -o dest/fc

dest/%.o: src/%.cpp
	echo $<
	echo $@
	g++ -Wall -std=c++11 -c $< -o $@

clean:
	rm -rf dest/