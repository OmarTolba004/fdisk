build: file.c file.h
	@gcc -g file.c -o file

clean: 
	@rm -f file
