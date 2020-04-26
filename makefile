dailygame: main.cpp config.h
	g++ -Wall -Wextra main.cpp -o dailygame

run: dailygame
	./dailygame

config.h:
	cp config.def.h config.h

.PHONY: run
