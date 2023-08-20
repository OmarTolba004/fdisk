build: file.c file.h
	@gcc file.c -o file

clean: 
	@rm -f file
