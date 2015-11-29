%{
#include "root.h"
#include "ast.h"
extern int yylex();
extern int line_number;
void yyerror(char *);
%}

%union {
    int integer;
    char *string;
    ast_t *ast;
    ast_list_t *ast_list;
    program_t *program;
    decorator_t *decorator;
    declarator_t *declarator;
}
 
%token<string> TOKEN_ID TOKEN_CONST_STRING TOKEN_USER_TYPE
%token<integer> TOKEN_CONST_INTEGER TOKEN_CONST_CHAR TOKEN_SIZEOF
%token<integer> TOKEN_POINTER TOKEN_INC TOKEN_DEC
%token<integer> TOKEN_MUL_ASSIGN TOKEN_DIV_ASSIGN TOKEN_MOD_ASSIGN 
%token<integer> TOKEN_ADD_ASSIGN TOKEN_SUB_ASSIGN
%token<integer> TOKEN_LSH_ASSIGN TOKEN_RSH_ASSIGN
%token<integer> TOKEN_AND_ASSIGN TOKEN_XOR_ASSIGN TOKEN_OR_ASSIGN 
%token<integer> TOKEN_EXTERN TOKEN_STATIC TOKEN_AUTO TOKEN_REGISTER
%token<integer> TOKEN_CHAR TOKEN_SHORT TOKEN_INTEGER TOKEN_LONG 
%token<integer> TOKEN_SIGNED TOKEN_UNSIGNED TOKEN_FLOAT TOKEN_DOUBLE 
%token<integer> TOKEN_TYPEDEF TOKEN_CONST TOKEN_VOLATILE TOKEN_VOID
%token<integer> TOKEN_STRUCT TOKEN_UNION TOKEN_ENUM TOKEN_ELLIPSIS
%token<integer> TOKEN_CASE TOKEN_DEFAULT TOKEN_IF TOKEN_ELSE TOKEN_SWITCH 
%token<integer> TOKEN_WHILE TOKEN_DO TOKEN_FOR 
%token<integer> TOKEN_GOTO TOKEN_CONTINUE TOKEN_BREAK TOKEN_RETURN

%right<integer> '=' 
%left<integer> TOKEN_OR
%left<integer> TOKEN_AND
%left<integer> '|'
%left<integer> '^'
%left<integer> '&'
%left<integer> TOKEN_EQ TOKEN_NE
%left<integer> '<' TOKEN_LE '>' TOKEN_GE
%left<integer> TOKEN_LSH TOKEN_RSH
%left<integer> '+' '-'
%left<integer> '*' '/' '%'
%token<integer> '~' '!'

%type<ast> express assign_express condition_express const_express 
%type<ast> binary_express cast_express unary_express 
%type<ast> postfix_express primary_express
%type<integer> assign_operator unary_operator

%type<ast> declare init
%type<decorator> decorator 
%type<declarator> declarator direct_declarator 
%type<declarator> abstract_declarator direct_abstract_declarator
%type<ast> type_specify atom_type struct_specify union_specify 
%type<ast> enum_specify enum_item
%type<ast> type_name prototype argument_declare
%type<program> start program
%type<ast_list> declare_list declarator_list 
%type<ast_list> enum_item_list argument_list argument_declare_list
%type<ast_list> init_list statement_list
%type<integer> pointer kuark_type storage_specify type_qualify

%type<ast> if_statement switch_statement label_statement
%type<ast> for_statement while_statement jump_statement
%type<ast> function statement express_statement
%type<ast> for_init for_condition for_continue group_statement

%start start
%%
/* express {{{ */
express
    : assign_express                                            
    | express ',' assign_express     
            { $$ = new binary_express_t($1, ',', $3);                        }
    ;

assign_express
    : condition_express 
    | unary_express assign_operator assign_express      
            { $$ = new assign_express_t($1, $2, $3);                         }
    ; 

assign_operator 
    : '=' 
    | TOKEN_MUL_ASSIGN
    | TOKEN_DIV_ASSIGN
    | TOKEN_MOD_ASSIGN
    | TOKEN_ADD_ASSIGN
    | TOKEN_SUB_ASSIGN
    | TOKEN_LSH_ASSIGN
    | TOKEN_RSH_ASSIGN
    | TOKEN_AND_ASSIGN
    | TOKEN_XOR_ASSIGN
    | TOKEN_OR_ASSIGN
    ; 

condition_express
    : binary_express
    | binary_express '?' express ':' condition_express   
            { $$ = new condition_express_t($1, $3, $5);                      }
    ; 

const_express
    : condition_express 
    ; 

binary_express
    : cast_express
    | binary_express TOKEN_OR binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express TOKEN_AND binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '|' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '^' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '&' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express TOKEN_EQ binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express TOKEN_NE binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '<' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '>' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express TOKEN_LE binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express TOKEN_GE binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express TOKEN_LSH binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express TOKEN_RSH binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '+' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '-' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '*' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '/' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    | binary_express '%' binary_express
            { $$ = new binary_express_t($1, $2, $3);                         }
    ;

cast_express 
    : unary_express 
    | '(' type_name ')' cast_express       
            { $$ = new cast_express_t($2, $4);                               }
    ; 

unary_express 
    : postfix_express 
    | TOKEN_INC unary_express                
            { $$ = new unary_express_t($1, $2);                              }
    | TOKEN_DEC unary_express                
            { $$ = new unary_express_t($1, $2);                              }
    | unary_operator cast_express                
            { $$ = new unary_express_t($1, $2);                              }
    | TOKEN_SIZEOF unary_express             
            { $$ = new unary_express_t($1, $2);                              }
    | TOKEN_SIZEOF '(' type_name ')'    
            { $$ = new unary_express_t($1, $3);                              }
    ;

unary_operator 
    : '&' 
    | '*' 
    | '+' 
    | '-' 
    | '~' 
    | '!' 
    ; 

postfix_express 
    : primary_express 
    | postfix_express '[' express ']'         
            { $$ = new index_express_t($1, $3);                              }
    | postfix_express '(' ')'              
            { $$ = new call_express_t($1, NULL);                             }
    | postfix_express '(' argument_list ')'    
            { $$ = new call_express_t($1, $3);                               }
    | postfix_express '.' TOKEN_ID           
            { $$ = new property_express_t($1, '.', $3);                      }
    | postfix_express TOKEN_POINTER TOKEN_ID       
            { $$ = new property_express_t($1, $2, $3);                       }
    | postfix_express TOKEN_INC              
            { $$ = new postfix_express_t($1, $2);                            }
    | postfix_express TOKEN_DEC              
            { $$ = new postfix_express_t($1, $2);                            }
    ;

argument_list
    : assign_express                         
            { $$ = new ast_list_t($1);                                       }
    | argument_list ',' assign_express           
            { $$ = $1; $$->push_back($3);                                    }
    ; 

primary_express 
    : TOKEN_ID                       { $$ = new id_express_t($1);            }
    | TOKEN_CONST_CHAR               { $$ = new integer_express_t($1);       }
    | TOKEN_CONST_INTEGER            { $$ = new integer_express_t($1);       }
    | TOKEN_CONST_STRING             { $$ = new string_express_t($1);        }
    | '(' express ')'                { $$ = $2;                              }
    ;
/*  }}} */

/* declare {{{ */
declare
    : decorator ';'                    
            { $$ = new declare_t($1, NULL);                                  }
    | decorator declarator_list ';'
            { $$ = new declare_t($1, $2);                                    }
    ;

declare_list
    : declare                        { $$ = new ast_list_t($1);              }
    | declare_list declare           { $$ = $1; $$->push_back($2);           }
    ;

decorator
    : type_specify
            { $$ = new decorator_t(-1, $1);                                  }
    | storage_specify type_specify
            { $$ = new decorator_t($1, $2);                                  }
    ;

declarator_list
    : declarator                       
            { $$ = new ast_list_t($1);                                       }
    | declarator '=' init 
            { $$ = new ast_list_t($1); 
              $1->init = $3;                                                 }
    | declarator_list ',' declarator      
            { $$ = $1; 
              $$->push_back($3);                                             }
    | declarator_list ',' declarator '=' init
            { $$ = $1;
              $$->push_back($3);
              $3->init = $5;                                                 }
    ; 

init
    : const_express                     { }
    | '{' init_list '}'                 { }
    | '{' init_list ',' '}'             { }
    ; 

init_list
    : init                              { $$ = new ast_list_t($1);           }
    | init_list ',' init                { $$ = $1; $$->push_back($3);        }
    ; 
/*  }}} */

/* decorator {{{ */
storage_specify
    : TOKEN_TYPEDEF
    | TOKEN_EXTERN
    | TOKEN_STATIC
    | TOKEN_AUTO
    | TOKEN_REGISTER
    ;

type_qualify
    : TOKEN_CONST 
    | TOKEN_VOLATILE 
    ; 

type_specify
    : atom_type 
    | TOKEN_USER_TYPE                   { $$ = new user_type_t($1);          }
    | struct_specify
    | union_specify
    | enum_specify
    ;

atom_type
    : kuark_type                        { $$ = new atom_type_t($1);          }
    | atom_type kuark_type              { $$ = $1;                           }
    ; 

kuark_type
    : TOKEN_VOID
    | TOKEN_CHAR
    | TOKEN_SHORT
    | TOKEN_INTEGER
    | TOKEN_LONG
    | TOKEN_FLOAT
    | TOKEN_DOUBLE
    | TOKEN_SIGNED
    | TOKEN_UNSIGNED
    ;

struct_specify
    : TOKEN_STRUCT TOKEN_ID '{' declare_list '}' 
            { $$ = new struct_specify_t($2, $4);                             }
    | TOKEN_STRUCT '{' declare_list '}'        
            { $$ = new struct_specify_t(NULL, $3);                           }
    | TOKEN_STRUCT TOKEN_ID               
            { $$ = new struct_specify_t($2, NULL);                           }
    ;

union_specify
    : TOKEN_UNION TOKEN_ID '{' declare_list '}'  
            { $$ = new union_specify_t($2, $4);                              }
    | TOKEN_UNION '{' declare_list '}'         
            { $$ = new union_specify_t(NULL, $3);                            }
    | TOKEN_UNION TOKEN_ID                
            { $$ = new union_specify_t($2, NULL);                            }
    ; 

enum_specify
    : TOKEN_ENUM '{' enum_item_list '}'       
            { $$ = new enum_specify_t(NULL, $3);                             }
    | TOKEN_ENUM TOKEN_ID '{' enum_item_list 
            '}'{ $$ = new enum_specify_t($2, $4);                            }
    | TOKEN_ENUM TOKEN_ID                   
            { $$ = new enum_specify_t($2, NULL);                             }
    ; 

enum_item_list
    : enum_item                         { $$ = new ast_list_t();             }
    | enum_item_list ',' enum_item      { $$ = $1; $$->push_back($3);        }
    ;

enum_item
    : TOKEN_ID                            
            { $$ = new enum_item_t($1, NULL);                                }
    | TOKEN_ID '=' const_express             
            { $$ = new enum_item_t($1, $3);                                  }
    ; 
/*  }}} */

/* declarator {{{ */
declarator 
    : pointer direct_declarator                   
            { $$ = new pointer_declarator_t($1, $2);                         }
    | direct_declarator                           
    ;

direct_declarator
    : TOKEN_ID                            
            { $$ = new id_declarator_t($1);                                  }
    | '(' declarator ')'                    
            { $$ = $2;                                                       }
    | direct_declarator '[' const_express ']'        
            { $$ = new array_declarator_t($1, $3);                           }
    | direct_declarator '[' ']'                   
            { $$ = new array_declarator_t($1, NULL);                         }
    | direct_declarator '(' prototype ')'             
            { $$ = new function_declarator_t($1, $3);                        }
    ;

prototype
    :                                   
            { $$ = new prototype_t(NULL, 0);                                 }
    | TOKEN_ELLIPSIS                      
            { $$ = new prototype_t(NULL, 1);                                 }
    | argument_declare_list                         
            { $$ = new prototype_t($1, 0);                                   }
    | argument_declare_list ',' TOKEN_ELLIPSIS            
            { $$ = new prototype_t($1, 1);                                   }
    ;

argument_declare_list
    : argument_declare                          
            { $$ = new ast_list_t($1);                                       }
    | argument_declare_list ',' argument_declare            
            { $$ = $1; $$->push_back($3);                                    }
    ;

argument_declare
    : decorator declarator                 
            { $$ = new argument_declare_t($1, $2);                           }
    | decorator abstract_declarator                 
            { $$ = new argument_declare_t($1, $2);                           }
    | decorator
            { $$ = new argument_declare_t($1, NULL);                         }
    ;

pointer 
    : '*'                               { $$ = 1;                            }
    | '*' type_qualify                  { $$ = 1;                            }
    | '*' pointer                       { $$ = 1 + $2;                       }
    | '*' type_qualify pointer          { $$ = 1 + $3;                       }
    ; 

type_name
    : decorator
            { $$ = new type_name_t($1, NULL);                                }
    | decorator abstract_declarator                 
            { $$ = new type_name_t($1, $2);                                  }
    ; 

abstract_declarator 
    : pointer                           
            { }
    | direct_abstract_declarator
    | pointer direct_abstract_declarator
            { $$ = new pointer_declarator_t($1, $2);                         }
    ;

direct_abstract_declarator
    : '(' abstract_declarator ')'                  
            { $$ = $2;                                                       }
    | '[' ']'                         
            { $$ = new array_declarator_t(NULL, NULL);                       }
    | '[' const_express ']'              
            { $$ = new array_declarator_t(NULL, $2);                         }
    | direct_abstract_declarator '[' ']'                 
            { $$ = new array_declarator_t($1, NULL);                         }
    | direct_abstract_declarator '[' const_express ']'      
            { $$ = new array_declarator_t($1, $3);                           }
    | '(' prototype ')'                   
            { $$ = new function_declarator_t(NULL, $2);                      }
    | direct_abstract_declarator '(' prototype ')'           
            { $$ = new function_declarator_t($1, $3);                        }
    ;
/* }}} */

/* statement {{{ */
start
    : program  { program = $$ = $1;                                          }
    ;

program
    : declare
            { $$ = new program_t(); $$->add_declare($1);                     }
    | function 
            { $$ = new program_t(); $$->add_function($1);                    }
    | program declare                    
            { $$ = $1; $$->add_declare($2);                                  }
    | program function
            { $$ = $1; $$->add_function($2);                                 }
    ;

function
    : decorator declarator setup_current_function group_statement
            { $$ = current_function;
              current_function->init($1, $2, $4);    
              current_function = NULL;                                       }
    ;

setup_current_function
    :       { current_function = new function_t();                           }
    ;

statement_list
    : statement                        { $$ = new ast_list_t($1);            }
    | statement_list statement         { $$ = $1; $$->push_back($2);         }
    ; 

statement 
    : declare                              
    | express_statement
    | if_statement
    | switch_statement
    | label_statement
    | for_statement
    | while_statement
    | jump_statement
    | group_statement
    ;

express_statement
    : ';'                              
            { $$ = new express_statement_t(NULL);                            }
    | express ';'                      
            { $$ = new express_statement_t($1);                              }
    ;
    
if_statement
    : TOKEN_IF '(' express ')' statement              
            { $$ = new if_statement_t($3, $5, NULL);                         }
    | TOKEN_IF '(' express ')' statement TOKEN_ELSE statement
            { $$ = new if_statement_t($3, $5, $7);                           }
    ;

switch_statement
    : TOKEN_SWITCH '(' express ')' group_statement
            { $$ = new switch_statement_t($3, $5);                           }
    ;

label_statement
    : TOKEN_ID ':' 
            { $$ = new label_statement_t($1);                                } 
    | TOKEN_CASE const_express ':' 
            { }
    | TOKEN_DEFAULT ':'
            { }
    ;

jump_statement
    : TOKEN_GOTO TOKEN_ID ';'            
            { $$ = new jump_statement_t($1, $2);                             }
    | TOKEN_CONTINUE ';'                 
            { $$ = new jump_statement_t($1, NULL);                           }
    | TOKEN_BREAK ';'                    
            { $$ = new jump_statement_t($1, NULL);                           }
    | TOKEN_RETURN ';'                   
            { $$ = new return_statement_t(NULL);                             }
    | TOKEN_RETURN express ';'           
            { $$ = new return_statement_t($2);                               }
    ;

while_statement
    : TOKEN_WHILE '(' express ')' statement       
            { $$ = new while_statement_t($1, $3, $5);                        }
    | TOKEN_DO statement TOKEN_WHILE '(' express ')' ';'
            { $$ = new while_statement_t($1, $2, $5);                        }
    ;

for_statement
    : TOKEN_FOR '(' for_init for_condition for_continue ')' statement 
            { $$ = new for_statement_t($3, $4, $5, $7);                      }
    ;

for_init
    : ';'                              { $$ = NULL;                          }
    | declare                          { $$ = $1;                            }
    | express ';'                      { $$ = $1;                            }
    ;

for_condition
    : ';'                              { $$ = NULL;                          }
    | express ';'                      { $$ = $1;                            }
    ;

for_continue
    :                                  { $$ = NULL;                          }
    | express                          { $$ = $1;                            }
    ;

group_statement 
    : '{' '}'                          
            { $$ = new group_statement_t(new ast_list_t());                  }
    | '{' statement_list '}'           
            { $$ = new group_statement_t($2);                                }
    ;
/* }}} */
%%
void yyerror(char *message)
{
    fprintf(stderr, "Syntax error near %d: %s\n", line_number, message);
    exit(0);
}
