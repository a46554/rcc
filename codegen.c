#include "rcc.h"

/* Simple BNF for computing            */
/* Operation code piority: () -> +a,-b -> a*b, a/b -> a+b,a-b->>,<,>=,<= -> ==,!= */
/* expr = relational */
/* equality = relational ('==' relational | '!='relational)*   */
/* relational = add ('>'add | '<'add | '>=' add| '<='add)*  */
/* add = mul ('+' mul || '-' mul )*   */
/* mul = unary ('*' unary || '/' unary )* */
/* unary = ('+' || '-')? term          */
/* term = num | '(' expr ')'           */


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
