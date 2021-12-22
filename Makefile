latex-to-markdown: latex-to-markdown.l latex-to-markdown.y latex-to-markdown.h
				bison -d latex-to-markdown.y
				flex -o latex-to-markdown.lex.c latex-to-markdown.l
				gcc -o $@ latex-to-markdown.tab.c latex-to-markdown.lex.c latex-to-markdown-functions.c -lm -g
				@echo Parser da Linguagem estah pronto!