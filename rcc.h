#ifndef _RCC_H_
#define _RCC_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

/* Tokenizer section */
typedef enum {
	TK_RESERVED,
	TK_IDENT,
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

bool cosume(char *op);
Token *cosume_ident(void);
bool at_eof(void);
void expected(char *op);
int expected_number(void);
void tokenize(char *p);

/* Code gen section */
typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // <
	ND_LE, // <=
	ND_NUM,
	ND_ASSIGN,
	ND_LVAR
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
	int offset;
};

Node *stmt(void);
Node *expr(void);
Node *assign(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *term(void);

void gen(Node *node);
void program(void);

Node* g_code[100];

#endif