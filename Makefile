dest/a.out: src/main.cpp
	g++ -std=c++11 $< -o $@