#ifndef LATEX_TO_MARKDOWN
#define LATEX_TO_MARKDOWN

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#define ELEMENT_PADDING "\n\n"

/* interface com o lexer */

extern int yylineno; /* do lexer */
extern FILE *yyin;   /* do lexer */
void yyerror(char *s, ...);

char *outputfilename;
FILE *output;

int curChapter;
int curSection;
int curSubSection;

enum NodeType
{
    NT_DOCUMENT = 0,
    NT_SETTINGS,
    NT_CLASS,
    NT_PACKAGE,
    NT_IDENTIFICATION,
    NT_MAIN,
    NT_BEGIN,
    NT_END,
    NT_BODYLIST,
    NT_CHAPTER,
    NT_SUBSECTION,
    NT_SECTION,
    NT_BODY,
    NT_TEXT,
    NT_TEXTSTYLE,
    NT_LIST,
    NT_NUMBEREDLIST,
    NT_ITEMLIST,
    NT_ITENS
};

enum TextStyle
{
    TS_BOLD,
    TS_ITALIC,
    TS_UNDERLINE
};

struct ast /* abstractic syntactic list */
{
    enum NodeType nodetype;
    struct ast *n1;
    struct ast *n2;
    struct ast *n3;
    struct ast *n4;
};

struct StructClass
{
    enum NodeType nodetype;
    char *content1;
    char *content2;
};

struct StructPackage
{
    enum NodeType nodetype;
    char *content1;
    char *content2;
    struct StructPackage *next;
};

struct StructIdentification
{
    enum NodeType nodetype;
    char *title;
    char *author;
};

struct StructBody
{
    enum NodeType nodetype;
    char *content;
    struct ast *n1;
    struct ast *n2;
};

struct StructTextSubdivision
{
    enum NodeType nodetype;
    char *content;
    struct ast *n1;
    struct ast *n2;
};

struct StructText
{
    enum NodeType nodetype;
    char *content;
    struct StructText *next;
};

struct StructTextStyle
{
    enum NodeType nodetype;
    char *content;
    enum TextStyle textStyle;
};

struct StructItens
{
    enum NodeType nodetype;
    char *content;
    struct ast *next;
};

struct StackChar
{
    char data;
    struct StackChar *next;
} StackChar;

/* construção de uma ast */
struct ast *newast(enum NodeType nodetype, struct ast *n1, struct ast *n2, struct ast *n3, struct ast *n4);
struct ast *newclass(enum NodeType nodetype, char *content1, char *content2);
struct ast *newpackage(enum NodeType nodetype, char *content1, char *content2, struct ast *next);
struct ast *newidentification(enum NodeType nodetype, char *n1, char *n2);
struct ast *newtextsubdivision(enum NodeType nodetype, char *content, struct ast *n1, struct ast *n2);
struct ast *newtext(enum NodeType nodetype, char *content, struct ast *next);
struct ast *newtextstyle(enum NodeType nodetype, char *content, enum TextStyle textStyle);
struct ast *newitens(enum NodeType nodetype, char *content, struct ast *next);

/* avaliação de uma AST */
void eval(struct ast *);
/* deletar e liberar uma AST */
void treefree(struct ast *);

/* cria uma nova string e copia */
void copystring(char **dest, char *src, bool takeOffBrackets);
/* limpa o arquivo da saída do programa */
void clearoutput();
/* acrescenta uma string na saída do programa */
void appendoutput(char *str);

/* funções da stack */
void push_stack_char(struct StackChar **stack, char data); /*Insere no final*/
char top_stack_char(struct StackChar *stack);              /*Retorna o dado do último nó*/
void pop_stack_char(struct StackChar **stack);             /*Remove no fim*/
void print_stack_char(struct StackChar **stack);           /*Printa a stack*/
bool is_empty_stack_char(struct StackChar *stack);         /*Return se está vazia*/

/* funções para conversão número - string */
char *number_to_str(long long int value);
long long int str_to_number(const char *str);
char *get_string(long long unsigned int value, bool isNegative);

#endif