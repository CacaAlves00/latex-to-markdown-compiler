#include "latex-to-markdown.h"

void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

struct ast *newast(enum NodeType nodetype, struct ast *n1, struct ast *n2, struct ast *n3, struct ast *n4)
{
    struct ast *a = malloc(sizeof(struct ast));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->n1 = n1;
    a->n2 = n2;
    a->n3 = n3;
    a->n4 = n4;

    return a;
}

struct ast *newclass(enum NodeType nodetype, char *content1, char *content2)
{
    struct StructClass *a = malloc(sizeof(struct StructClass));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->content1 = content1;
    a->content2 = content2;

    return ((struct ast *)a);
}

struct ast *newpackage(enum NodeType nodetype, char *content1, char *content2, struct ast *next)
{
    struct StructPackage *a = malloc(sizeof(struct StructPackage));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->content1 = content1;
    a->content2 = content2;
    a->next = (struct StructPackage *)next;

    return ((struct ast *)a);
}

struct ast *newidentification(enum NodeType nodetype, char *title, char *author)
{
    struct StructIdentification *a = malloc(sizeof(struct StructIdentification));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->title = title;
    a->author = author;

    return ((struct ast *)a);
}

struct ast *newtextsubdivision(enum NodeType nodetype, char *content, struct ast *n1, struct ast *n2)
{
    struct StructTextSubdivision *a = malloc(sizeof(struct StructTextSubdivision));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->content = content;
    a->n1 = n1;
    a->n2 = n2;

    return ((struct ast *)a);
}

struct ast *newtext(enum NodeType nodetype, char *content, struct ast *next)
{
    struct StructText *a = malloc(sizeof(struct StructText));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->content = content;
    a->next = (struct StructText *)next;

    return ((struct ast *)a);
}

struct ast *newtextstyle(enum NodeType nodetype, char *content, enum TextStyle textStyle)
{
    struct StructTextStyle *a = malloc(sizeof(struct StructTextStyle));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->content = content;
    a->textStyle = textStyle;

    return ((struct ast *)a);
}

struct ast *newitens(enum NodeType nodetype, char *content, struct ast *next)
{
    struct StructItens *a = malloc(sizeof(struct StructItens));

    if (!a)
    {
        yyerror("sem espaço");
        exit(0);
    }

    a->nodetype = nodetype;
    a->content = content;
    a->next = next;

    return ((struct ast *)a);
}

void eval(struct ast *a)
{
    if (!a)
    {
        return;
    }

    switch (a->nodetype)
    {
    case NT_DOCUMENT:
        eval(a->n1);
        treefree(a->n1);
        eval(a->n2);
        treefree(a->n2);
        eval(a->n3);
        treefree(a->n3);
        break;
    case NT_SETTINGS:
        eval(a->n1);
        eval(a->n2);
        break;
    case NT_CLASS:;
        struct StructClass *class = (struct StructClass *)a;

        appendoutput("[//]: # \""); /* um comentário em Markdown */

        appendoutput("\\documentclass[");
        appendoutput(class->content1);
        appendoutput("]");

        appendoutput("{");
        appendoutput(class->content2);
        appendoutput("}");

        appendoutput("}\"");

        appendoutput(ELEMENT_PADDING);

        break;
    case NT_PACKAGE:;
        struct StructPackage *package = (struct StructPackage *)a;

        appendoutput("[//]: # \"\n"); /* um comentário em Markdown */

        while (package != NULL)
        {

            appendoutput("\\package[");
            appendoutput(package->content1);
            appendoutput("]");

            if (package->content2)
            {
                appendoutput("{");
                appendoutput(package->content2);
                appendoutput("}");
            }
            appendoutput(ELEMENT_PADDING);

            package = (struct StructPackage *)package->next;
        }

        appendoutput("}\"");

        appendoutput(ELEMENT_PADDING);

        break;

    case NT_IDENTIFICATION:;
        struct StructIdentification *id = (struct StructIdentification *)a;

        appendoutput("# ");
        appendoutput(id->title);
        appendoutput(ELEMENT_PADDING);

        if (id->author)
        {
            appendoutput("\t");
            appendoutput(id->author);
            appendoutput(ELEMENT_PADDING);
            appendoutput(ELEMENT_PADDING);
        }

        break;

    case NT_MAIN:
        eval(a->n3); /* eval bodyList */
        break;

    case NT_BODYLIST:
        eval(a->n1);
        if (a->n2) /* se tem pelo menos o nó 2, tem todos os 4 */
            eval(a->n2);

        break;

    case NT_CHAPTER:;
        struct StructTextSubdivision *chap = (struct StructTextSubdivision *)a;

        curSection = 1;
        curSubSection = 1;

        appendoutput("## ");
        appendoutput(chap->content);

        appendoutput(ELEMENT_PADDING);

        curChapter++;

        break;
    case NT_SECTION:;
        struct StructTextSubdivision *sec = (struct StructTextSubdivision *)a;

        curSubSection = 1;

        appendoutput("#### **");
        appendoutput(number_to_str(curChapter));
        appendoutput(".");
        appendoutput(number_to_str(curSection));
        appendoutput("\t");
        appendoutput(sec->content);
        appendoutput("**\n");

        appendoutput(ELEMENT_PADDING);

        curSection++;

        break;
    case NT_SUBSECTION:;
        struct StructTextSubdivision *subsec = (struct StructTextSubdivision *)a;

        appendoutput("##### **");
        appendoutput(number_to_str(curChapter));
        appendoutput(".");
        appendoutput(number_to_str(curSection));
        appendoutput(".");
        appendoutput(number_to_str(curSubSection));
        appendoutput("\t");
        appendoutput(subsec->content);
        appendoutput("**\n");

        appendoutput(ELEMENT_PADDING);

        curSubSection++;

        break;
    case NT_BODY:
        eval(a->n1);
        break;

    case NT_TEXT:;
        struct StructText *txt = (struct StructText *)a;
        while (txt != NULL)
        {
            appendoutput(txt->content);
            txt = (struct StructText *)txt->next;
            appendoutput(" ");
        }

        appendoutput(ELEMENT_PADDING);

        break;

    case NT_TEXTSTYLE:;
        struct StructTextStyle *txtst = (struct StructTextStyle *)a;

        switch (txtst->textStyle)
        {
        case TS_BOLD:
            appendoutput("**");
            appendoutput(txtst->content);
            appendoutput("**");
            appendoutput(" ");
            break;
        case TS_ITALIC:
            appendoutput("*");
            appendoutput(txtst->content);
            appendoutput("*");
            appendoutput(" ");
            break;
        case TS_UNDERLINE:
            appendoutput("<ins>");
            appendoutput(txtst->content);
            appendoutput("</ins>");
            appendoutput(" ");
            break;
        default:
            break;
        }

        break;

    case NT_LIST:
        eval(a->n1);

        break;

    case NT_NUMBEREDLIST:;
        struct StructItens *nlist = ((struct StructItens *)a->n1);

        while (nlist != NULL)
        {
            appendoutput("1. ");
            appendoutput(nlist->content);
            appendoutput("\n");
            nlist = (struct StructItens *)nlist->next;
        }

        appendoutput(ELEMENT_PADDING);

        break;

    case NT_ITEMLIST:;
        struct StructItens *ilist = ((struct StructItens *)a->n1);

        while (ilist != NULL)
        {
            appendoutput("* ");
            appendoutput(ilist->content);
            appendoutput("\n");
            ilist = (struct StructItens *)ilist->next;
        }

        appendoutput(ELEMENT_PADDING);

        break;
    case NT_ITENS:
        break;

    default:
        printf("erro interno: bad node %d\n", a->nodetype);
    }
}

/* libera uma árvore de AST */
void treefree(struct ast *a)
{
    if (!a)
        return;

    switch (a->nodetype)
    {
    case NT_DOCUMENT:
        treefree(a->n1);
        treefree(a->n2);
        treefree(a->n3);
        break;
    case NT_SETTINGS:
        treefree(a->n1);
        treefree(a->n2);
        break;
    case NT_CLASS:;
        struct StructClass *class = (struct StructClass *)a;

        if (class->content1)
        {
            free(class->content1);
            class->content1 = NULL;
        }
        if (class->content2)
        {
            free(class->content2);
            class->content2 = NULL;
        }

        break;
    case NT_PACKAGE:;
        struct StructPackage *package = (struct StructPackage *)a;

            if (package->content1)
            {
                free(package->content1);
                package->content1 = NULL;
            }
            if (package->content2)
            {
                free(package->content2);
                package->content2 = NULL;
            }

            eval((struct ast *)package->next);
            package->next = NULL;

        break;
    case NT_IDENTIFICATION:;
        struct StructIdentification *id = (struct StructIdentification *)a;

        if (id->title)
        {
            free(id->title);
            id->title = NULL;
        }
        if (id->author)
        {
            free(id->author);
            id->author = NULL;
        }

        break;
    case NT_MAIN:
        treefree(a->n1);
        treefree(a->n2);
        treefree(a->n3);
        break;
    case NT_BEGIN:
        /* sem nós filhos */
        break;
    case NT_END:
        /* sem nós filhos */
        break;
    case NT_BODYLIST:
        treefree(a->n1);

        if (a->n2)
            treefree(a->n2);

        break;
    case NT_CHAPTER:;
        struct StructTextSubdivision *chapter = (struct StructTextSubdivision *)a;

        if (chapter->content)
        {
            free(chapter->content);
            chapter->content = NULL;
        }

        if (chapter->n1) /* se tiver n1, tem n2 */
        {
            treefree(chapter->n1);
            treefree(chapter->n2);
        }
        break;
    case NT_SUBSECTION:;
        struct StructTextSubdivision *subsection = (struct StructTextSubdivision *)a;

        if (subsection->content)
        {
            free(subsection->content);
            subsection->content = NULL;
        }

        treefree(subsection->n1); /* sempre vai ter n1 */

        if (subsection->n2) /* se tiver n1, tem n2 */
        {
            treefree(subsection->n2);
        }
        break;
    case NT_SECTION:;
        struct StructTextSubdivision *section = (struct StructTextSubdivision *)a;

        if (section->content)
        {
            free(section->content);
            section->content = NULL;
        }

        treefree(section->n1); /* sempre vai ter n1 */

        if (section->n2) /* se tiver n1, tem n2 */
        {
            treefree(section->n2);
        }
        break;
    case NT_BODY:
        treefree(a->n1);
        treefree(a->n2);
        break;
    case NT_TEXT:;
        struct StructText *text = (struct StructText *)a;

        if (text->content)
        {
            free(text->content);
            text->content = NULL;
        }

        treefree((struct ast *)text->next);
        break;
    case NT_TEXTSTYLE:;
        struct StructTextStyle *textStyle = (struct StructTextStyle *)a;

        if (textStyle->content)
        {
            free(textStyle->content);
            textStyle->content = NULL;
        }
        break;
    case NT_LIST:
        treefree(a->n1);
        break;
    case NT_NUMBEREDLIST:
        treefree(a->n1);
        break;
    case NT_ITEMLIST:
        treefree(a->n1);
        break;
    case NT_ITENS:;
        struct StructItens *itens = (struct StructItens *)a;

        if (itens->content)
        {
            free(itens->content);
            itens->content = NULL;
        }

        if (itens->next)
        {
            treefree(itens->next);
            itens->next = NULL;
        }

        break;

    default:;
        // printf("erro interno: free bad node\n");
    }

    if (a)
        free(a); /* sempre libera o próprio nó */
}

void copystring(char **dest, char *src, bool takeOffBrackets)
{
    const int n = (takeOffBrackets ? (strlen(src) - 2) /* ignoring '{''}' */ : strlen(src));
    (*dest) = (char *)malloc((sizeof(char) * n) + 1); /* adding 1 for the '\0' */

    strncpy((*dest), &src[(takeOffBrackets ? 1 : 0)], n);
    (*dest)[n] = '\0';
}

void clearoutput()
{
    output = fopen(outputfilename, "w");
    fclose(output);
}

void appendoutput(char *str)
{
    fputs(str, output);
}

struct StackChar *new_node_stack_char(char data)
{
    struct StackChar *node = (struct StackChar *)malloc(sizeof(struct StackChar));
    node->data = data;
    node->next = NULL;

    return node;
}

void push_stack_char(struct StackChar **stack, char data)
{
    if (stack == NULL)
        return;

    struct StackChar *node = new_node_stack_char(data);

    if ((*stack) == NULL)
    {
        (*stack) = node;
    }
    else
    {
        struct StackChar *aux = (*stack);
        (*stack) = node;
        node->next = aux;
    }
}

char top_stack_char(struct StackChar *stack)
{
    if (stack == NULL)
    {
        // printf("Can't return top! Stack is empty.\n");
    }
    else
    {
        return stack->data;
    }
}

void pop_stack_char(struct StackChar **stack)
{
    if (stack == NULL || (*stack) == NULL)
    {
        // printf("Can't pop! Empty stack.\n");
        return;
    }

    struct StackChar *aux = (*stack);
    (*stack) = (*stack)->next;
    free(aux);
    aux = NULL;
}

bool is_empty_stack_char(struct StackChar *stack)
{
    return (stack == NULL);
}

void print_stack_char(struct StackChar **stack)
{
    if (stack == NULL || (*stack) == NULL)
    {
        // printf("Can't print! Empty stack.\n");
        return;
    }

    while ((*stack) != NULL)
    {
        printf("%c", top_stack_char(*stack));
        pop_stack_char(stack);
    }
    printf("\n");
}

char *get_string(long long unsigned int value, bool isNegative)
{
    if (value == 0)
    {
        char *zero = (char *)malloc(sizeof(char) * 2);
        zero[0] = '0';
        zero[1] = '\0';
        
        return zero;
    }

    struct StackChar *strStack = NULL;
    int strStackSize = (isNegative ? 1 : 0);

    while (value > 0)
    {
        int digit = (value % 10);

        char digitChar = (digit + '0');
        push_stack_char(&strStack, digitChar);
        value /= 10;
        strStackSize++;
    }
    char *str = (char *)malloc(sizeof(char) * (strStackSize + 1));

    int i = (isNegative ? 1 : 0);
    if (isNegative)
    {
        str[0] = '-';
    }

    for (i; !is_empty_stack_char(strStack); i++)
    {
        str[i] = top_stack_char(strStack);
        pop_stack_char(&strStack);
    }
    const int lastStrPosition = strStackSize;
    str[lastStrPosition] = '\0'; /*Last position*/

    return str;
}

char *number_to_str(long long int value)
{
    bool isNegative = (value < 0);

    if (isNegative)
    {
        value *= -1;
    }

    char *str = get_string(value, isNegative);
}

long long int str_to_number(const char *str)
{
    long long int number = 0;
    bool isNegative = (str[0] == '-');

    int strLength = strlen(str);
    long long unsigned int decimalPlace = 1;
    const int stopFlag = (isNegative ? 0 : -1);
    for (int i = strLength - 1; i != stopFlag; i--)
    {
        int digit = (str[i] - '0');
        number += (digit * decimalPlace);
        decimalPlace *= 10;
    }

    if (isNegative)
    {
        number *= -1;
    }

    return number;
}

int main(int argc, char **argv)
{

    if (argc > 1)
    {
        if (!(yyin = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return (1);
        }
    }

    int n = strlen(argv[1]); /* filename */
    n += 5;                  /* filename + .out\0 */
    outputfilename = (char *)malloc(sizeof(char) * n);
    strcat(outputfilename, argv[1]);
    strcat(outputfilename, ".out");
    outputfilename[n - 1] = '\0';

    clearoutput();

    output = fopen(outputfilename, "a");

    curChapter = 0;
    curSection = 1;
    curSubSection = 1;

    return yyparse();
}