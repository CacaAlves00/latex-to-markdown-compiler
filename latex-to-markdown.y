%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "latex-to-markdown.h"
%}

%union
{   
    struct ast *a;
    char *string;
};

%token <string> NAME
%token <string> CONTENT
%token NUMBER
// keywords
%token DOCUMENT
%token _BEGIN
%token _END
%token CLASS
%token PACKAGE
%token AUTHOR 
%token TITLE
%token CHAPTER
%token SECTION
%token SUBSECTION
%token PAR
%token BF
%token UNDERLINE
%token IT
%token <string> ITEM
%token ENUMERATE
%token ITEMIZE

%type <a> documentLatex identification settings class package main begin end bodyList chapter section subsection body text textStyle lists numberedList itemList itens

%%

documentLatex: settings identification main {
    $$ = newast(NT_DOCUMENT, $1, $2, $3, NULL);
    eval($$);
}
| settings main {
    $$ = newast(NT_DOCUMENT, $1, $2, NULL, NULL);
    eval($$);
}
;

settings: class package {
    $$ = newast(NT_SETTINGS, $1, $2, NULL, NULL);
}
| class {
    $$ = newast(NT_SETTINGS, $1, NULL, NULL, NULL);
}
;

class: CLASS NAME NAME {
    $$ = newclass(NT_CLASS, $2, $3);   
}
;

package: PACKAGE NAME {
    $$ = newpackage(NT_PACKAGE, $2, NULL, NULL);  
}
| PACKAGE NAME NAME {
    $$ = newpackage(NT_PACKAGE, $2, $3, NULL);
} 
| PACKAGE NAME package {
    $$ = newpackage(NT_PACKAGE, $2, NULL, $3);
}
| PACKAGE NAME NAME package {
    $$ = newpackage(NT_PACKAGE, $2, $3, $4);
}
;

identification: TITLE CONTENT AUTHOR CONTENT {
    $$ = newidentification(NT_IDENTIFICATION, $2, $4);
} 
| TITLE NAME AUTHOR CONTENT {
    $$ = newidentification(NT_IDENTIFICATION, $2, $4);
}
| TITLE CONTENT AUTHOR NAME {
    $$ = newidentification(NT_IDENTIFICATION, $2, $4);
}
| TITLE NAME AUTHOR NAME {
    $$ = newidentification(NT_IDENTIFICATION, $2, $4);
}
| TITLE CONTENT {
    $$ = newidentification(NT_IDENTIFICATION, $2, NULL);
} 
| TITLE NAME {
    $$ = newidentification(NT_IDENTIFICATION, $2, NULL);
}
;

main: begin end {
    $$ = newast(NT_MAIN, $1, $2, NULL, NULL);
}
| begin bodyList end {
    $$ = newast(NT_MAIN, $1, $3, $2, NULL);
}
;

begin: _BEGIN DOCUMENT {
    $$ = newast(NT_BEGIN, NULL, NULL, NULL, NULL);
}
;


end: _END DOCUMENT {
    $$ = newast(NT_END, NULL, NULL, NULL, NULL);
}
;

bodyList: chapter bodyList {
    $$ = newast(NT_BODYLIST, $1, $2, NULL, NULL);
}
| chapter {
    $$ = newast(NT_BODYLIST, $1, NULL, NULL, NULL);
}
| section bodyList {
    $$ = newast(NT_BODYLIST, $1, $2, NULL, NULL);
}
| section {
    $$ = newast(NT_BODYLIST, $1, NULL, NULL, NULL);
}
| subsection bodyList {
    $$ = newast(NT_BODYLIST, $1, $2, NULL, NULL);
}
| subsection {
    $$ = newast(NT_BODYLIST, $1, NULL, NULL, NULL);
}
| body bodyList {
    $$ = newast(NT_BODYLIST, $1, $2, NULL, NULL);
}
| body {
    $$ = newast(NT_BODYLIST, $1, NULL, NULL, NULL);
}
;

chapter: CHAPTER CONTENT {
    $$ = newtextsubdivision(NT_CHAPTER, $2, NULL, NULL);
}
| CHAPTER NAME {
    $$ = newtextsubdivision(NT_CHAPTER, $2, NULL, NULL);
}
;

section: SECTION CONTENT {
    $$ = newtextsubdivision(NT_SECTION, $2, NULL, NULL);    
}
| SECTION NAME {
    $$ = newtextsubdivision(NT_SECTION, $2, NULL, NULL);    
}
;

subsection: SUBSECTION CONTENT {
    $$ = newtextsubdivision(NT_SUBSECTION, $2, NULL, NULL);
} 
| SUBSECTION NAME {
    $$ = newtextsubdivision(NT_SUBSECTION, $2, NULL, NULL);
} 
;

body: text {
    $$ = newast(NT_BODY, $1, NULL, NULL, NULL);
} 
| textStyle {
    $$ = newast(NT_BODY, $1, NULL, NULL, NULL);
} 
| lists {
    $$ = newast(NT_BODY, $1, NULL, NULL, NULL);
}
;

text: NAME text {
    $$ = newtext(NT_TEXT, $1, $2);
}
| NAME PAR {
    $$ = newtext(NT_TEXT, $1, NULL);
}
| NAME {
    $$ = newtext(NT_TEXT, $1, NULL);
}
| PAR {
    char *space = (char *) malloc(sizeof(char));
    (*space) = ' '; 
    $$ = newtext(NT_TEXT, space, NULL);
}
;

textStyle: BF NAME {
    $$ = newtextstyle(NT_TEXTSTYLE, $2, TS_BOLD);
} 
| BF CONTENT {
    $$ = newtextstyle(NT_TEXTSTYLE, $2, TS_BOLD);
} 
| UNDERLINE NAME {
    $$ = newtextstyle(NT_TEXTSTYLE, $2, TS_UNDERLINE);
} 
| UNDERLINE CONTENT {
    $$ = newtextstyle(NT_TEXTSTYLE, $2, TS_UNDERLINE);
} 
| IT NAME {
    $$ = newtextstyle(NT_TEXTSTYLE, $2, TS_ITALIC);
}
| IT CONTENT {
    $$ = newtextstyle(NT_TEXTSTYLE, $2, TS_ITALIC);
}
;

lists: numberedList {
    $$ = newast(NT_LIST, $1, NULL, NULL, NULL);
} | itemList {
    $$ = newast(NT_LIST, $1, NULL, NULL, NULL);
}
;

numberedList: _BEGIN ENUMERATE itens _END ENUMERATE {
    $$ = newast(NT_NUMBEREDLIST, $3, NULL, NULL, NULL);
}
;

itemList: _BEGIN ITEMIZE itens _END ITEMIZE {
    $$ = newast(NT_ITEMLIST, $3, NULL, NULL, NULL);
}
;

itens: ITEM {
    $$ = newitens(NT_ITENS, $1, NULL);
} | ITEM itens {
    $$ = newitens(NT_ITENS, $1, $2);
}
;
