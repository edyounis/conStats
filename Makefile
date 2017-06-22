all:
	gcc -Wall -Werror -fPIC -c src/constats.c -o build/constats.o  -lm
	gcc -shared -o libstats.so build/constats.o