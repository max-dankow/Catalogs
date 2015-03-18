all:result

result:main.c
	gcc -std=gnu99 main.c -o result

main.c:
