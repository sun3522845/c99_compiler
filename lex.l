%{
#include "root.h"
#include "ast.h"
#include "gram.h"
#include "intern.h"
int line_number = 1;
static void read_comment();
static void read_cpp_comment();
static int read_integer();
static int read_id();
static int read_string();
#define __T(token) yylval.integer = token; return token
%}

D               [0-9]
L               [a-zA-Z_]
H               [a-fA-F0-9]

%%
"/*"            { read_comment();                                            }
"//"            { read_cpp_comment();                                        }
[ \t\v\f]       { ;                                                          }
"\n"            { line_number++;                                             }

"if"            { __T(TOKEN_IF);                                             }
"else"          { __T(TOKEN_ELSE);                                           }
"switch"        { __T(TOKEN_SWITCH);                                         }
"case"          { __T(TOKEN_CASE);                                           }
"default"       { __T(TOKEN_DEFAULT);                                        }
"goto"          { __T(TOKEN_GOTO);                                           }
"for"           { __T(TOKEN_FOR);                                            }
"while"         { __T(TOKEN_WHILE);                                          }
"do"            { __T(TOKEN_DO);                                             }
"continue"      { __T(TOKEN_CONTINUE);                                       }
"break"         { __T(TOKEN_BREAK);                                          }
"return"        { __T(TOKEN_RETURN);                                         }

"struct"        { __T(TOKEN_STRUCT);                                         }
"union"         { __T(TOKEN_UNION);                                          }
"enum"          { __T(TOKEN_ENUM);                                           }
"const"         { __T(TOKEN_CONST);                                          }
"volatile"      { __T(TOKEN_VOLATILE);                                       }

"void"          { __T(TOKEN_VOID);                                           }
"long"          { __T(TOKEN_LONG);                                           }
"int"           { __T(TOKEN_INTEGER);                                        }
"short"         { __T(TOKEN_SHORT);                                          }
"char"          { __T(TOKEN_CHAR);                                           }
"signed"        { __T(TOKEN_SIGNED);                                         }
"unsigned"      { __T(TOKEN_UNSIGNED);                                       }

"&&"            { __T(TOKEN_AND);                                            }
"||"            { __T(TOKEN_OR);                                             }
"=="            { __T(TOKEN_EQ);                                             }
"!="            { __T(TOKEN_EQ);                                             }
">="            { __T(TOKEN_GE);                                             }
"<="            { __T(TOKEN_LE);                                             } 
"<<"            { __T(TOKEN_LSH);                                            } 
">>"            { __T(TOKEN_RSH);                                            } 
"++"            { __T(TOKEN_INC);                                            } 
"--"            { __T(TOKEN_DEC);                                            } 

"|="            { __T(TOKEN_OR_ASSIGN);                                      }
"^="            { __T(TOKEN_XOR_ASSIGN);                                     }
"&="            { __T(TOKEN_AND_ASSIGN);                                     }
"+="            { __T(TOKEN_ADD_ASSIGN);                                     }
"-="            { __T(TOKEN_SUB_ASSIGN);                                     }
"*="            { __T(TOKEN_MUL_ASSIGN);                                     }
"/="            { __T(TOKEN_DIV_ASSIGN);                                     }
"%="            { __T(TOKEN_MOD_ASSIGN);                                     }
"..."           { __T(TOKEN_ELLIPSIS);                                       }
[?:=|^&<>+\-*/%!~.,;(){}\[\]] {
                  __T(yytext[0]);                                            }

{D}+            { return read_integer();                                     }
{L}({L}|{D})*   { return read_id();                                          }
"\""            { return read_string();                                      }
%%

int yywrap()
{
    return 1;
}

static void read_comment()
{
    int state = 0;
    int c;

    while ((c = yyinput()) != 0) {
        if (c == '\n')
            line_number++;
        switch (state) {
            case 0:
                if (c == '*')
                     state = 1;
                break;
            case 1:
                if (c == '/')
                     return;
                if (c != '*')
                     state = 0;
                break;
        }
    }
}

static void read_cpp_comment()
{
    int c;

    while ((c = yyinput()) != 0) {
        if (c == '\n') { 
            line_number++;
            break;
        }
    }
}

static int read_integer()
{
    yylval.integer= atoi(yytext);
    return TOKEN_CONST_INTEGER;
}

static int read_id()
{
    yylval.string = intern(yytext);
    return TOKEN_ID;
}

static int read_string()
{
    char buffer[512];
    char *b = buffer;
    int c;

    while (c = yyinput(), c != 0 && c != '"')
        *b++ = c;
    *b = 0;
    yylval.string = intern(buffer);
    return TOKEN_CONST_STRING;
}

void keep_compiler_silent()
{
    yyunput(123, NULL);
}
