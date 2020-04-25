dailygame: main.cpp
	g++ -Wall -Wextra main.cpp -o dailygame

run: dailygame
	./dailygame

.PHONY: run
