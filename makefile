
run: dailygame
	./dailygame

dailygame: main.cpp
	g++ -Wall -Wextra main.cpp -o dailygame -lncurses

.PHONY: run
