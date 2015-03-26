all:result

result:main.c
	gcc -std=gnu99 main.c Hash-Table.c -o result

main.c:

Hash-Table.c:
