%option noyywrap nodefault yylineno
%{
    #include "latex-to-markdown.h"    
    #include "latex-to-markdown.tab.h"    
%}

LETTER [a-zA-Z]
WORD [a-zA-Z]+
DIGIT [0-9]
NUMBER [0-9]+
SPACE " " 

%%

"%".* // comentários são ignorados

\\begin {
    return _BEGIN;
}

\\end {
    return _END;
}

"{"document"}" {
    return DOCUMENT;
}

\\documentclass {
    return CLASS;
}

\\usepackage {
    return PACKAGE;
}

\\title {
    return TITLE;
}

\\author {
    return AUTHOR;
}

\\chapter {
    return CHAPTER;
}

\\section {
    return SECTION;
}

\\subsection {
    return SUBSECTION;
}

\\par {
    return PAR;
}

\\bf {
    return BF;
}

\\underline {
    return UNDERLINE;
}

\\it {
    return IT;
}

\\item" "["" "a-zA-Z0-9"\\"\.@\-"<"">""("")"","\''"!""?"";"":"]+ {
    copystring(&yylval.string, &yytext[6], false);
    return ITEM;
}

"{"enumerate"}" {
    return ENUMERATE;
}

"{"itemize"}" {
    return ITEMIZE;
}



"{"[a-zA-Z0-9"\\"\.@\n\-"<"">""("")"","\''"!""?"";"":"]+"}" {
    copystring(&yylval.string, yytext, true);
    return NAME;
}

"{"[a-zA-Z0-9" ""\\"\.@\n\-"<"">""("")"","\''"!""?"";"":"]+"}" {
    copystring(&yylval.string, yytext, true);
    return CONTENT;
}

"["[a-zA-Z0-9"\\"\.@\n\-"<"">""("")"","\''"!""?"";"":"]+"]" {
    copystring(&yylval.string, yytext, true);
    return NAME;
}



[a-zA-Z0-9\.@\-"<"">""("")"","\''"!""?"";"":"]+ {
    copystring(&yylval.string, yytext, false);
    return NAME;
}

\t|\n|{SPACE} {
    /* Ignora o espaço em branco */
}
. {
    yyerror("Caracter não reconhecido %c\n", *yytext);
}

%%