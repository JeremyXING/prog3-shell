%{
#include "Shell.h"
#include "y.tab.h"
%}

ID	([-.$/\\*?A-Za-z0-9]+)

%%

[ \t]+			;
^[ \t]*\n		;
{ID}|\"{ID}\"|\'{ID}\' {
  strcpy (yylval.identificateur, yytext);
  return IDENTIFICATEUR;
  }
\<			return IN;
\>			return OUT;
"2>"			return ERR;
"&>"			return ERR_OUT;
">>"			return OUT_APPEND;
"||"			return OU;
"&&"			return ET;
<<EOF>>			EndOfFile();
.|\n			return yytext[0];
