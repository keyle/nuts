build:
	clang src/*.c -Wall -Wextra -pedantic -Wunused-variable -std=c2x -o nuts

release:
	clang src/*.c -std=c2x -pedantic -Wall -Wextra -Werror -Wshadow -flto -O2 -o nutsr
