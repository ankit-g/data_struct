def:
	@gcc -Wall -g -Werror main.c problems/*.c framework/*.c -o elf
clean:
	@rm elf
