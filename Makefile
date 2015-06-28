SRC := $(wildcard src/*.cpp)
TAR = $(patsubst src/%.cpp,dest/%.o,$(SRC))

.PHONY: all clean

all:
	@for file in src/*.cpp; do \
		export objfile="$${file//src\//dest\/}";\
		export objfile="$${objfile//.cpp/.o}";\
		echo "$$objfile";\
		gcc -Wall --std=c++11 -c $$file -o $$objfile;\
	done

	@gcc -Wall -lstdc++ dest/*.o -o dest/fc

dest/%.o: src/%.cpp
	echo $<
	echo $@
	g++ -Wall -std=c++11 -c $< -o $@

clean:
	rm -rf dest/