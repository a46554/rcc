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
/* Operation code piority: () -> +a,-b -> a*b, a/b -> a+b,a-b->>,<,>=,<= -> ==,!= */
/* expr = relational */
/* equality = relational ('==' relational | '!='relational)*   */
/* relational = add ('>'add | '<'add | '>=' add| '<='add)*  */
/* add = mul ('+' mul || '-' mul )*   */
/* mul = unary ('*' unary || '/' unary )* */
/* unary = ('+' || '-')? term          */
/* term = num | '(' expr ')'           */

typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // <
	ND_LE, // <=
	ND_NUM
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};

// Function delcaretion
bool cosume(char *op);
void expected(char *op);
int expected_number(void);

Node *expr(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *term(void);

void gen(Node *node);

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

/* expr = equality */
Node *expr(void)
{
	return equality();
}

/* equality = relational ('==' relational | '!='relational)*   */
Node *equality(void)
{
	Node *node = relational();
	
	for(;;) // Did we reach the end node ?
	{
		if(cosume("=="))
			node = new_node(ND_EQ, node, relational());
		if(cosume("!="))
			node = new_node(ND_NE, node, relational());
		else 
			return node;
	}
}

/* relational = add ('<'add | '<='add | '>' add| '>='add)*  */
Node *relational(void)
{
	Node *node = add();
	
	for(;;)
	{
		if(cosume("<"))
			node = new_node(ND_LT, node, add());
		else if(cosume("<="))
			node = new_node(ND_LE, node, add());
		else if(cosume(">"))
			node = new_node(ND_LT, add(), node);
		else if(cosume(">="))
			node = new_node(ND_LE, add(), node);
		else 
			return node;
	}
}

// (1+2) * 3
/* add = mul ('+' mul || '-' mul )*   */
Node *add(void)
{
	Node *node = mul();
	
	for(;;)
	{
		if(cosume("+"))
			node = new_node(ND_ADD, node, mul());
		else if(cosume("-"))
			node = new_node(ND_SUB, node, mul());
		else 
			return node;
	}
}

/* mul = unary ('*' unary || '/' unary )* */
Node *mul(void)
{
	Node *node = unary();
	
	for(;;)
	{
		if(cosume("*"))
			node = new_node(ND_MUL, node, unary());
		else if(cosume("/"))
			node = new_node(ND_DIV, node, unary());
		else 
			return node;
	}
}

/* unary = ('+' || '-')? term -> represent +3 or -2 case*/
Node *unary(void)
{
	if(cosume("+"))
		return term();// no need for i = +3
	else if(cosume("-"))
		return new_node(ND_SUB, new_num_node(0), term()); // Rplace -2 to 0 - 2 
	
	return term();
}

/* term = num | '(' expr ')'           */
Node *term(void)
{
	if(cosume("("))
	{
		Node *node = expr();
		expected(")");
		return node;
	}
	else
	{
		return new_num_node(expected_number());
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
	int len;
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

bool cosume(char *op)
{
	if(g_token->kind != TK_RESERVED ||
	   strlen(op) != g_token->len ||
	   memcmp(g_token->str, op, g_token->len))
		return false;
		
	g_token = g_token->next;	
	return true;
}

void expected(char *op)
{
	if(g_token->kind != TK_RESERVED ||
	   strlen(op) != g_token->len ||
	   memcmp(g_token->str, op, g_token->len))
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

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next =tok;
	return tok;
}

bool startswitch(char *p, char *q)
{
	return memcmp(p, q, strlen(q)) == 0;
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
		
		/* Multi-letter operation code */
		if(startswitch(p, "==") || startswitch(p, "!=") ||
		   startswitch(p, ">=") || startswitch(p, "<=")) {
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}/* Add as RESERVE node */
		
		if (strchr("+-*/()<>", *p)) { // Did p been fund at string 1?
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}/* Add as RESERVE node */
		
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0); // Since the strtol will automatic update p, don't need p++ here.
			char *q = p;
			cur->val = strtol(p, &p,10);
			cur->len = p - q; // How many digits 
			continue;
		}/* Add as NUM node */
		
		error_at(g_token->str, "Error parsing");
	}
	
	// Add the EOF node 
	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

int main(int argc, char ** argv)
{
	if(argc != 2) {
		fprintf(stderr, "Wrong parameter!!\n");
		return 1;
	}
	
	user_input = argv[1];
	
	// Tokenize the input args
	g_token = tokenize(user_input);
	
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

void gen(Node *node)
{
	if(node->kind == ND_NUM) {
		printf(" push %d\n", node->val);
		return;
	}
	
	gen(node->lhs);
	gen(node->rhs);
	
	// POP top 2 element and do compute
	printf(" pop rdi\n");
	printf(" pop rax\n");
	
	switch(node->kind)
	{
		case ND_ADD:
			printf(" add rax, rdi\n");
			break;
		case ND_SUB:
			printf(" sub rax, rdi\n");
			break;
		case ND_MUL:
			printf(" imul rax, rdi\n");
			break;
		case ND_DIV:
			printf(" cqo\n");
			printf(" idiv rdi\n");
			break;
		case ND_EQ:
			printf(" cmp rax, rdi\n");
			printf(" sete al\n");
			printf(" movzb rax, al\n");
			break;
		case ND_NE:
			printf(" cmp rax, rdi\n");
			printf(" setne al\n");
			printf(" movzb rax, al\n");
			break;
		case ND_LT:
			printf(" cmp rax, rdi\n");
			printf(" setl al\n");
			printf(" movzb rax, al\n");
			break;
		case ND_LE:		
			printf(" cmp rax, rdi\n");
			printf(" setle al\n");
			printf(" movzb rax, al\n");
			break;
	}
	
	// PUSH back final result 
	printf("push rax\n");
}