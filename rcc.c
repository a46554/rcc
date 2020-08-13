#include "rcc.h"

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
	
	// Tokenize the input args
	tokenize(argv[1]);
	
	// Generate syntax tree 
	Node *node = expr();
	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	
	// Walking through syntax tree and generate command
	gen(node);
	
	printf(" pop rax\n");	
	printf(" ret\n");
	
	return 0;
}
