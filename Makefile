def:
	@gcc -Wall -g -Werror main.c problems/rev_strng.c framework/stack/stk.c -o elf
clean:
	@rm elf
