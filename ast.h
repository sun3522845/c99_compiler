#ifndef _AST_H
#define _AST_H

#include "list.h"

enum {
    ASSIGN_EXPRESS_T,
    CONDITION_EXPRESS_T,
    BINARY_EXPRESS_T,
    CAST_EXPRESS_T,
    UNARY_EXPRESS_T,
    INDEX_EXPRESS_T,
    CALL_EXPRESS_T,
    PROPERTY_EXPRESS_T,
    POSTFIX_EXPRESS_T,
    ARGUMENT_EXPRESSS_T,
    ID_EXPRESS_T,
    INTEGER_EXPRESS_T,
    STRING_EXPRESS_T,
    DECLARE_T,
    DECORATOR_T,
    INIT_T,
    POINTER_DECLARATOR_T,
    ID_DECLARATOR_T,
    ARRAY_DECLARATOR_T,
    FUNCTION_DECLARATOR_T,
    PROTOTYPE_T,
    ARGUMENT_DECLARE_T,
    ATOM_TYPE_T,
    USER_TYPE_T,
    STRUCT_SPECIFY_T,
    UNION_SPECIFY_T,
    ENUM_SPECIFY_T,
    ENUM_ITEM_T,
    TYPE_NAME_T,
    EXPRESS_STATEMENT_T,
    IF_STATEMENT_T,
    WHILE_STATEMENT_T,
    FOR_STATEMENT_T,
    SWITCH_STATEMENT_T,
    RETURN_STATEMENT_T,
    LABEL_STATEMENT_T,
    JUMP_STATEMENT_T,
    GROUP_STATEMENT_T,
    FUNCTION_DEFINE_T,
    UNIT_T,
};

struct insn_t;
struct type_t;
struct struct_type_t;
struct prototype_t;

struct ast_t {
    int rtti;
    int in_top_level;
    int location;

    ast_t();
    virtual ~ast_t();
    virtual void dump() = 0;
};

extern void error(ast_t *ast, char *format, ...);

struct ast_list_t: list_t {
    ast_list_t();
    ast_list_t(ast_t *ast);
    void dump(char *tag);
};

struct express_t: ast_t {
};

struct assign_express_t: express_t {
    ast_t *left;
    int operater;
    ast_t *right;

    assign_express_t(ast_t *left, int operater, ast_t *right);
    ~assign_express_t();
    void dump();
};

struct condition_express_t: express_t {
    ast_t *condition;
    ast_t *then_part;
    ast_t *else_part;

    condition_express_t(ast_t *condition, ast_t *then_part, 
                        ast_t *else_part);
    ~condition_express_t();
    void dump();
};

struct binary_express_t: express_t {
    ast_t *left; 
    int operater;
    ast_t *right;
    char *operator_text;
    int opcode;

    binary_express_t(ast_t *left, int operater, ast_t *right);
    ~binary_express_t();
    void dump();
};

struct cast_express_t: express_t {
    ast_t *type_name;
    ast_t *express;

    cast_express_t(ast_t *type_name, ast_t *express);
    ~cast_express_t();
    void dump();
};

struct unary_express_t: express_t {
    int operater;
    ast_t *express;

    unary_express_t(int operater, ast_t *express);
    ~unary_express_t();
    void dump();
};

struct index_express_t: express_t {
    ast_t *base;
    ast_t *index;

    index_express_t(ast_t *base, ast_t *index);
    ~index_express_t();
    void dump();
};

struct call_express_t: express_t {
    ast_t *function;
    ast_list_t *argument_list;

    call_express_t(ast_t *function, ast_list_t *argument_list);
    ~call_express_t();
    void dump();
};

struct property_express_t: express_t {
    ast_t *object;
    int operater;
    char *property;

    property_express_t(ast_t *object, int operater, char *property);
    ~property_express_t();
    void dump();
};

struct postfix_express_t: express_t {
    ast_t *express;
    int operater;

    postfix_express_t(ast_t *express, int operater);
    ~postfix_express_t();
    void dump();
};

struct argument_expresss_t: ast_t {
    argument_expresss_t();
    ~argument_expresss_t();
    void dump();
};

struct id_express_t: express_t {
    char *value;

    id_express_t(char *value);
    ~id_express_t();
    void dump();
};

struct integer_express_t: express_t {
    int value;

    integer_express_t(int value);
    ~integer_express_t();
    void dump();
};

struct string_express_t: express_t {
    char *value;

    string_express_t(char *value);
    ~string_express_t();
    void dump();
};

struct declarator_t; 
struct decorator_t: ast_t {
    int storage_specify;
    ast_t *type_specify;

    decorator_t(int storage_specify, ast_t *type_specify);
    ~decorator_t();
    void dump();
};

struct declare_t: ast_t {
    decorator_t *decorator;
    ast_list_t *declarator_list;

    declare_t(decorator_t *decorator, ast_list_t *declarator_list);
    ~declare_t();
    void dump();
    declarator_t *first_declarator()
    {
        return (declarator_t*) declarator_list->front();
    }
};

struct init_t: ast_t {
    init_t();
    ~init_t();
    void dump();
};

struct declarator_t: ast_t {
    ast_t *init;

    declarator_t();
    ~declarator_t();
    void dump();
    virtual char *get_name();
    virtual type_t *deduce_type(type_t *base_type) = 0;
    type_t *deduce_type(decorator_t *decorator);
};

struct id_declarator_t: declarator_t {
    char *id;

    id_declarator_t(char *id);
    ~id_declarator_t();
    void dump();
    char *get_name();
    type_t *deduce_type(type_t *base_type);
};

struct pointer_declarator_t: declarator_t {
    int pointer;
    declarator_t *base;

    pointer_declarator_t(int pointer, declarator_t *base);
    ~pointer_declarator_t();
    void dump();
    char *get_name();
    type_t *deduce_type(type_t *base_type);
};

struct array_declarator_t: declarator_t {
    declarator_t *base;
    ast_t *capacity;

    array_declarator_t(declarator_t *base, ast_t *capacity);
    ~array_declarator_t();
    void dump();
    int get_capacity();
    char *get_name();
    type_t *deduce_type(type_t *base_type);
};

struct function_declarator_t: declarator_t {
    declarator_t *base;
    prototype_t *prototype;

    function_declarator_t(declarator_t *base, ast_t *prototype);
    ~function_declarator_t();
    void dump();
    char *get_name();
    type_t *deduce_type(type_t *base_type);
};

struct argument_declare_t: ast_t {
    decorator_t *decorator;
    declarator_t *declarator;

    argument_declare_t(decorator_t *decorator, ast_t *declarator);
    ~argument_declare_t();
    void dump();
    type_t *deduce_type();
};

struct prototype_t: ast_t {
    ast_list_t *argument_declare_list;
    int with_ellipsis;

    prototype_t(ast_list_t *argument_declare_list, int with_ellipsis);
    ~prototype_t();
    void dump();
};

struct atom_type_t: ast_t {
    int type_name;

    atom_type_t(int type_name);
    ~atom_type_t();
    void dump();
};

struct user_type_t: ast_t {
    char *type_name;

    user_type_t(char *type_name);
    ~user_type_t();
    void dump();
};

struct struct_specify_t: ast_t {
    char *name;
    ast_list_t *declare_list;

    struct_specify_t(char *name, ast_list_t *declare_list);
    ~struct_specify_t();
    void dump();
    struct_type_t *deduce_type();
};

struct union_specify_t: ast_t {
    char *name;
    ast_list_t *declare_list;

    union_specify_t(char *name, ast_list_t *declare_list);
    ~union_specify_t();
    void dump();
};

struct enum_specify_t: ast_t {
    char *name;
    ast_list_t *enum_item_list;

    enum_specify_t(char *name, ast_list_t *enum_item_list);
    ~enum_specify_t();
    void dump();
};

struct enum_item_t: ast_t {
    char *name;
    ast_t *value;

    enum_item_t(char *name, ast_t *value);
    ~enum_item_t();
    void dump();
};

struct type_name_t: ast_t {
    decorator_t *decorator;
    ast_t *abstract_declarator;

    type_name_t(decorator_t *decorator, ast_t *abstract_declarator);
    ~type_name_t();
    void dump();
};

struct express_statement_t: ast_t {
    ast_t *express;

    express_statement_t(ast_t *express);
    ~express_statement_t();
    void dump();
};

struct if_statement_t: ast_t {
    ast_t *condition;
    ast_t *then_part;
    ast_t *else_part;

    if_statement_t(ast_t *condition, ast_t *then_part, ast_t *else_part);
    ~if_statement_t();
    void dump();
};

struct while_statement_t: ast_t {
    int is_while;
    ast_t *condition;
    ast_t *body;

    while_statement_t(int is_while, ast_t *condition, ast_t *body);
    ~while_statement_t();
    void dump();
};

struct for_statement_t: ast_t {
    ast_t *init;
    ast_t *condition;
    ast_t *next;
    ast_t *body;

    for_statement_t(ast_t *init, ast_t *condition, ast_t *next, ast_t *body);
    ~for_statement_t();
    void dump();
};

struct switch_statement_t: ast_t {
    ast_t *condition;
    ast_t *body;

    switch_statement_t(ast_t *condition, ast_t *body);
    ~switch_statement_t();
    void dump();
};

struct return_statement_t: ast_t {
    ast_t *express;

    return_statement_t(ast_t *express);
    ~return_statement_t();
    void dump();
};

struct label_statement_t: ast_t {
    char *name;
    int value;

    label_statement_t(char *name);
    ~label_statement_t();
    void dump();
};

struct jump_statement_t: ast_t {
    int type;
    char *label;

    jump_statement_t(int type, char *label);
    ~jump_statement_t();
    void dump();
};

struct group_statement_t: ast_t {
    ast_list_t *list;

    group_statement_t(ast_list_t *list);
    ~group_statement_t();
    void dump();
};

struct function_t: ast_t {
    decorator_t *decorator;
    function_declarator_t *declarator;
    group_statement_t *body;

    int max_local_size;
    list_t label_statement_list;
    list_t block_list;
    list_t *current_block;
    list_t machine_insn_list;

    function_t();
    ~function_t();
    void init(decorator_t *decorator, ast_t *declarator, ast_t *body);
    void dump();
    char *get_name();
    void add_insn(insn_t *insn);
    void add_label_statement(label_statement_t *label_statement);
    int lookup_label(char *name);
};
extern function_t *current_function;

struct program_t {
    ast_list_t declare_list;
    ast_list_t function_list;

    program_t();
    ~program_t();
    void dump();
    void add_declare(ast_t *declare);
    void add_function(ast_t *function);
};
extern program_t *program;

#endif
