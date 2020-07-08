#include <stdio.h>
#include <stdlib.h>

/* Simple compier test          
	Usage: ./rcc 123 > temp.s 
	
	Test with gcc:               
	./gcc -o tmp tmp.s           
	./tmp                        
	echo $?                     
	123
 */

/* Ref:https://koshizuow.gitbook.io/compilerbook/calculator_level_language/step1 */
int main(int argc, char ** argv)
{
	if(argc != 2) {
		fprintf(stderr, "Wrong parameter!!\n");
		return 1;
	}
	
	char *p = argv[1];
	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	printf("mov rax, %ld\n", strtol(p, &p, 10));
	
	while(*p)
	{
		if(*p == '+'){
			p++;
			printf("add rax, %ld\n", strtol(p, &p, 10));
			continue;
		}	
		else if(*p == '-'){
			p++;
			printf("sub rax, %ld\n", strtol(p, &p, 10));
			continue;
		}
		else {
			printf("Unkown operator\n");
			return 1;
		}	
	}
	
	printf("ret\n");
	
	return 0;
}