#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

/* Simple compier test          
	Usage: ./rcc 123 > temp.s 
	
	Test with gcc:               
	./gcc -o tmp tmp.s           
	./tmp                        
	echo $?                     
	123
 */

/* Ref:https://koshizuow.gitbook.io/compilerbook/calculator_level_language/step1 */

/* Simple BNF for computing            */
/* expr = mul ('+' mul || '-' mul )*   */
/* mul = term ('*' term || '/' term )* */
/* term = num | '(' expr ')'           */

typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV.
	ND_NUM
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind,
	Node *lhs,
	Node *rhs,
	int val,
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_num_node(int val)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}
(1+2) * 3

/* expr = mul ('+' mul || '-' mul )*   */
Node *expr()
{
	Node *node = mul();
	
	for(;;) // Did we reach the end node ?
	{
		if(cosume('+'))
			node = new_node(ND_ADD, node, mul());
		else if(cosume('-'))
			node = new_node(ND_SUB, node, mul());
		else 
			return node;
	}	
}


/* mul = term ('*' term || '/' term )* */
Node *mul()
{
	Node *node = term();
	
	for(;;) // Did we reach the end node ?
	{
		if(cosume('*'))
			node = new_node(ND_MUL, node, term());
		else if(cosume('/'))
			node = new_node(ND_DIV, node, term());
		else 
			return node;
	}	
}

/* term = num | '(' expr ')'           */
Node *term()
{
	if(cosume('('))
	{
		Node *node = expr();
		expected(')')
		return node;
	}
	else
	{
		return *new_num_node(expected_number());
	}	
}

typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF
} TokenKind;

typedef struct Token Token;

struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char *str;
};

Token *g_token;
char *user_input;

void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s\n", pos, "");
	fprintf(stderr, "^ ");	
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

bool cosume(char op)
{
	if(g_token->kind != TK_RESERVED || g_token->str[0] != op)
		return false;
	g_token = g_token->next;	
	return true;
}

void expected(char op)
{
	if(g_token->kind != TK_RESERVED || g_token->str[0] != op)
		error_at(g_token->str, "Not %c", op);
	g_token = g_token->next;	
}

int expected_number(void)
{
	if(g_token->kind != TK_NUM)
		error_at(g_token->str, "Not number");
	int val = g_token->val;
	g_token = g_token->next;	
	return val;
}

bool at_eof(void)
{
	return g_token->kind == TK_EOF;	
}

Token *new_token(TokenKind kind, Token *cur, char *str)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next =tok;
	return tok;
}

Token *tokenize(char *p)
{
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p)
	{
		if(isspace(*p)) {
			p++;
			continue;
		}/* Skip space */
		
		if (*p == '+' || *p =='-') {
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}/* Add as RESERVE node */
		
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p); // Since the strtol will automatic update p, don't need p++ here.
			cur->val = strtol(p, &p,10);
			continue;
		}/* Add as NUM node */
		
		error_at(g_token->str, "Error parsing");
	}
	
	// Add the EOF node 
	new_token(TK_EOF, cur, p);
	return head.next;
}

int main(int argc, char ** argv)
{
	if(argc != 2) {
		fprintf(stderr, "Wrong parameter!!\n");
		return 1;
	}
	
	// Tokenize the input args
	g_token = tokenize(argv[1]);
	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	
	// Expected start with number
	printf("mov rax, %d\n", expected_number());
	
	while(!at_eof())
	{
		if(cosume('+')){
			printf("add rax, %d\n", expected_number());
			continue;
		}	
		
		expected('-');
		printf("sub rax, %d\n", expected_number());
	}
	
	printf("ret\n");
	
	return 0;
}