#include "rcc.h"

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

void tokenize(char *p)
{
	Token head;
	head.next = NULL;
	Token *cur = &head;
	
	user_input = p;
	
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
	g_token = head.next;
}