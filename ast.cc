#include "root.h"
#include "ast.h"
#include "dump.h"
#include "gram.h"
#include "type.h"
#include "opcode.h"
#include "insn.h"
#include "util.h"

extern int line_number;
program_t *program;
function_t *current_function;

void error(ast_t *ast, char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "%d:error:", ast->location);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(0);
}

ast_t::ast_t()
{
    in_top_level = 0;
}

ast_t::~ast_t()
{
}

ast_list_t::ast_list_t()
{
}

ast_list_t::ast_list_t(ast_t *ast)
{
    push_back(ast);
}

void ast_list_t::dump(char *tag)
{
    if (tag)
        push_tag(tag);

    ast_t *ast;
    for_each (ast, *this)
        ast->dump();

    if (tag)
        pop_tag(tag);
}

assign_express_t::assign_express_t(ast_t *left, int operater, ast_t *right)
{
    rtti = ASSIGN_EXPRESS_T;
    location = left->location;
    this->left = left;
    this->operater = operater;
    this->right = right;
}

assign_express_t::~assign_express_t()
{
}

void assign_express_t::dump()
{
    push_tag("assign_express");
    left->dump();
    right->dump();
    pop_tag("assign_express");
}

condition_express_t::condition_express_t(
        ast_t *condition, ast_t *then_part, ast_t *else_part)
{
    rtti = CONDITION_EXPRESS_T;
    location = condition->location;
    this->condition = condition;
    this->then_part = then_part;
    this->else_part = else_part;
}

condition_express_t::~condition_express_t()
{
}

void condition_express_t::dump()
{
}

binary_express_t::binary_express_t(ast_t *left, int operater, ast_t *right)
{
    rtti = BINARY_EXPRESS_T;
    location = left->location;
    this->left = left;
    this->operater = operater;
    this->right = right;

#define OP(token, text, code)                  \
    case token:                                \
        operator_text = text;                  \
        opcode = code;                         \
        break;

    switch (operater) {
        OP(TOKEN_OR, "||", -1);
        OP(TOKEN_AND, "&&", -1);

        OP('|', "|", OPCODE_OR);
        OP('^', "^", OPCODE_XOR);
        OP('&', "&", OPCODE_AND);
        OP(TOKEN_LSH, "<<", OPCODE_LEFT_SHIFT);
        OP(TOKEN_RSH, ">>", OPCODE_RIGHT_SHIFT);

        OP(TOKEN_EQ, "==", OPCODE_JUMP_EQUAL);
        OP(TOKEN_NE, "!=", OPCODE_JUMP_NOT_EQUAL);
        OP('<', "<", OPCODE_JUMP_LITTLE);
        OP('>', ">", OPCODE_JUMP_GREAT);
        OP(TOKEN_LE, "<=", OPCODE_JUMP_GREAT_EQUAL);
        OP(TOKEN_GE, ">=", OPCODE_JUMP_GREAT_EQUAL);

        OP('+', "+", OPCODE_ADD);
        OP('-', "-", OPCODE_SUBTRACT);
        OP('*', "*", OPCODE_MULTIPLY);
        OP('/', "/", OPCODE_DIVIDE);
        OP('%', "%", OPCODE_MODULAR);
    }
#undef OP
}

binary_express_t::~binary_express_t()
{
}

void binary_express_t::dump()
{
    push_tag(operator_text);
    left->dump();
    right->dump();
    pop_tag(operator_text);
}

cast_express_t::cast_express_t(ast_t *type_name, ast_t *express)
{
    rtti = CAST_EXPRESS_T;
    this->type_name = type_name;
    this->express = express;
}

cast_express_t::~cast_express_t()
{
}

void cast_express_t::dump()
{
}

unary_express_t::unary_express_t(int operater, ast_t *express)
{
    rtti = UNARY_EXPRESS_T;
    this->operater = operater;
    this->express = express;
}

unary_express_t::~unary_express_t()
{
}

void unary_express_t::dump()
{
}

index_express_t::index_express_t(ast_t *base, ast_t *index)
{
    rtti = INDEX_EXPRESS_T;
    this->base = base;
    this->index = index;
}

index_express_t::~index_express_t()
{
}

void index_express_t::dump()
{
    push_tag("index_express");
    base->dump();
    index->dump();
    pop_tag("index_express");
}

call_express_t::call_express_t(ast_t *function, ast_list_t *argument_list)
{
    rtti = CALL_EXPRESS_T;
    this->function = function;
    this->argument_list = argument_list;
}

call_express_t::~call_express_t()
{
}

void call_express_t::dump()
{
}

property_express_t::property_express_t(ast_t *object, int operater, 
                                       char *property)
{
    rtti = PROPERTY_EXPRESS_T;
    this->object = object;
    this->operater = operater;
    this->property = property;
}

property_express_t::~property_express_t()
{
}

void property_express_t::dump()
{
    push_tag("property_express");
    object->dump();
    indent(property);
    pop_tag("property_express");
}

postfix_express_t::postfix_express_t(ast_t *express, int operater)
{
    rtti = POSTFIX_EXPRESS_T;
    this->express = express;
    this->operater = operater;
}

postfix_express_t::~postfix_express_t()
{
}

void postfix_express_t::dump()
{
    const char *operater_text = (operater == TOKEN_INC) ? "++" : "--";
    push_dump_pop(operater_text, express);
}

argument_expresss_t::argument_expresss_t()
{
    rtti = ARGUMENT_EXPRESSS_T;
}

argument_expresss_t::~argument_expresss_t()
{
}

void argument_expresss_t::dump()
{
}

id_express_t::id_express_t(char *value)
{
    rtti = ID_EXPRESS_T;
    location = line_number;
    this->value = value;
}

id_express_t::~id_express_t()
{
}

void id_express_t::dump()
{
    push_dump_pop("id_express", value);
}

integer_express_t::integer_express_t(int value)
{
    rtti = INTEGER_EXPRESS_T;
    location = line_number;
    this->value = value;
}

integer_express_t::~integer_express_t()
{
}

void integer_express_t::dump()
{
    push_dump_pop("integer_express", value);
}

string_express_t::string_express_t(char *value)
{
    rtti = STRING_EXPRESS_T;
    location = line_number;
    this->value = value;
}

string_express_t::~string_express_t()
{
}

void string_express_t::dump()
{
}

decorator_t::decorator_t(int storage_speicfy, ast_t *type_specify)
{
    rtti = DECORATOR_T;
    this->storage_specify = storage_specify;
    this->type_specify = type_specify;
}

decorator_t::~decorator_t()
{
}

void decorator_t::dump()
{
    type_specify->dump();
}

declare_t::declare_t(decorator_t *decorator, ast_list_t *declarator_list)
{
    rtti = DECLARE_T;
    this->decorator = decorator;
    this->declarator_list = declarator_list;
}

declare_t::~declare_t()
{
}

void declare_t::dump()
{
    push_tag("declare");
    decorator->dump();
    declarator_list->dump("declarator_list");
    pop_tag("declare");
}

init_t::init_t()
{
    rtti = INIT_T;
}

init_t::~init_t()
{
}

void init_t::dump()
{
}

declarator_t::declarator_t()
{
    init = NULL;
}

declarator_t::~declarator_t()
{
}

void declarator_t::dump()
{
}

char *declarator_t::get_name()
{
    return NULL;
}

type_t *declarator_t::deduce_type(decorator_t *decorator)
{
    type_t *base_type = NULL;
    ast_t *type_specify = decorator->type_specify;
    int  type_name;
    switch (type_specify->rtti) {
        case ATOM_TYPE_T:
            type_name = ((atom_type_t *)type_specify)->type_name;
            if (type_name == TOKEN_VOID)
                base_type = new void_type_t();
            else
                base_type = new integer_type_t(type_name);
            break;
        case STRUCT_SPECIFY_T:
            base_type = ((struct_specify_t*) type_specify)->deduce_type();
            break;
        case UNION_SPECIFY_T:
            break;
        case ENUM_SPECIFY_T:
            break;
    }

    return deduce_type(base_type);
}

id_declarator_t::id_declarator_t(char *id)
{
    rtti = ID_DECLARATOR_T;
    this->id = id;
}

id_declarator_t::~id_declarator_t()
{
}

void id_declarator_t::dump()
{
    push_tag("id_declarator");
    indent(id);
    pop_tag("id_declarator");
}

char *id_declarator_t::get_name()
{
    return id;
}

type_t *id_declarator_t::deduce_type(type_t *base_type)
{
    return base_type;
}

pointer_declarator_t::pointer_declarator_t(int pointer, declarator_t *base)
{
    rtti = POINTER_DECLARATOR_T;
    this->pointer = pointer;
    this->base = base;
}

pointer_declarator_t::~pointer_declarator_t()
{
}

void pointer_declarator_t::dump()
{
    push_dump_pop("pointer_declarator", base);
}

char *pointer_declarator_t::get_name()
{
    return base->get_name();
}

type_t *pointer_declarator_t::deduce_type(type_t *base_type)
{
    type_t *pointer_type = new pointer_type_t(base_type);
    return base->deduce_type(pointer_type);
}

array_declarator_t::array_declarator_t(declarator_t *base, ast_t *capacity)
{
    rtti = ARRAY_DECLARATOR_T;
    this->base = base;
    assert(capacity->rtti == INTEGER_EXPRESS_T);
    this->capacity = capacity;
}

array_declarator_t::~array_declarator_t()
{
}

void array_declarator_t::dump()
{
    push_tag("array_declarator");
    base->dump();
    if (capacity)
        push_dump_pop("capacity", capacity);
    pop_tag("array_declarator");
}

int array_declarator_t::get_capacity()
{
    return ((integer_express_t*)capacity)->value;
}

char *array_declarator_t::get_name()
{
    return base->get_name();
}

type_t *array_declarator_t::deduce_type(type_t *base_type)
{
    type_t *array_type = new array_type_t(base_type, get_capacity());
    return base->deduce_type(array_type);
}

function_declarator_t::function_declarator_t(declarator_t *base, ast_t *prototype)
{
    rtti = FUNCTION_DECLARATOR_T;
    this->base = base;
    this->prototype = (prototype_t *)prototype;
}

function_declarator_t::~function_declarator_t()
{
}

void function_declarator_t::dump()
{
    base->dump();
    prototype->dump();
}

char *function_declarator_t::get_name()
{
    return base->get_name(); 
}

type_t *function_declarator_t::deduce_type(type_t *base_type)
{
    function_type_t *function_type = new function_type_t(base_type);

    argument_declare_t *argument_declare;
    for_each (argument_declare, *prototype->argument_declare_list) {
        type_t *argument_type = argument_declare->deduce_type();
        function_type->argument_type_list.push_back(argument_type);
    }
    return base->deduce_type(function_type);
}

argument_declare_t::argument_declare_t(decorator_t *decorator, ast_t *declarator)
{
    rtti = ARGUMENT_DECLARE_T;
    this->decorator = decorator;
    this->declarator = (declarator_t*) declarator;
}

argument_declare_t::~argument_declare_t()
{
}

void argument_declare_t::dump()
{
    push_tag("argument_declare");
    decorator->dump();
    declarator->dump();
    pop_tag("argument_declare");
}

type_t *argument_declare_t::deduce_type()
{
    return declarator->deduce_type(decorator);
}

prototype_t::prototype_t(ast_list_t *argument_declare_list, int with_ellipsis)
{
    rtti = PROTOTYPE_T;
    if (argument_declare_list == NULL)
        argument_declare_list = new ast_list_t();
    this->argument_declare_list = argument_declare_list;
    this->with_ellipsis = with_ellipsis;
}

prototype_t::~prototype_t()
{
}

void prototype_t::dump()
{
    argument_declare_list->dump("argument_declare_list");
}

atom_type_t::atom_type_t(int type_name)
{
    rtti = ATOM_TYPE_T;
    this->type_name = type_name;
}

atom_type_t::~atom_type_t()
{
}

void atom_type_t::dump()
{
    push_tag("atom_type");
    switch (type_name) {
        case TOKEN_CHAR:
            indent("char");
            break;
        case TOKEN_SHORT:
            indent("short");
            break;
        case TOKEN_INTEGER:
            indent("int");
            break;
        case TOKEN_VOID:
            indent("void");
            break;
    }
    pop_tag("atom_type");
}

user_type_t::user_type_t(char *type_name)
{
    rtti = USER_TYPE_T;
    this->type_name = type_name;
}

user_type_t::~user_type_t()
{
}

void user_type_t::dump()
{
}

struct_specify_t::struct_specify_t(char *name, ast_list_t *declare_list)
{
    rtti = STRUCT_SPECIFY_T;
    this->name = name;
    this->declare_list = declare_list;
}

struct_specify_t::~struct_specify_t()
{
}

void struct_specify_t::dump()
{
    push_tag("struct");
    declare_t *declare;
    for_each (declare, *declare_list)
        declare->dump();
    pop_tag("struct");
}

struct_type_t *struct_specify_t::deduce_type()
{
    struct_type_t *struct_type = new struct_type_t();
    declare_t *declare;
    for_each (declare, *declare_list) {
        declarator_t *declarator = declare->first_declarator();
        type_t *type = declarator->deduce_type(declare->decorator);
        struct_type->add_field(declarator->get_name(), type);
    }
    struct_type->compute_layout();
    return struct_type;
}

union_specify_t::union_specify_t(char *name, ast_list_t *declare_list)
{
    rtti = UNION_SPECIFY_T;
    this->name = name;
    this->declare_list = declare_list;
}

union_specify_t::~union_specify_t()
{
}

void union_specify_t::dump()
{
}

enum_specify_t::enum_specify_t(char *name, ast_list_t *enum_item_list)
{
    rtti = ENUM_SPECIFY_T;
    this->name = name;
    this->enum_item_list = enum_item_list;
}

enum_specify_t::~enum_specify_t()
{
}

void enum_specify_t::dump()
{
}

enum_item_t::enum_item_t(char *name, ast_t *value)
{
    rtti = ENUM_ITEM_T;
    this->name = name;
    this->value = value;
}

enum_item_t::~enum_item_t()
{
}

void enum_item_t::dump()
{
}

type_name_t::type_name_t(decorator_t *decorator, ast_t *abstract_declarator)
{
    rtti = TYPE_NAME_T;
    this->decorator = decorator;
    this->abstract_declarator = abstract_declarator;
}

type_name_t::~type_name_t()
{
}

void type_name_t::dump()
{
}

express_statement_t::express_statement_t(ast_t *express)
{
    rtti = EXPRESS_STATEMENT_T;
    if (express)
        location = express->location;
    this->express = express;
}

express_statement_t::~express_statement_t()
{
}

void express_statement_t::dump()
{
    if (express)
        express->dump();
}

if_statement_t::if_statement_t(ast_t *condition, ast_t *then_part, 
                                       ast_t *else_part)

{
    rtti = IF_STATEMENT_T;
    this->condition = condition;
    this->then_part = then_part;
    this->else_part = else_part;
}

if_statement_t::~if_statement_t()
{
}

void if_statement_t::dump()
{
}

while_statement_t::while_statement_t(int is_while, ast_t *condition, 
                                             ast_t *body)
{
    rtti = WHILE_STATEMENT_T;
    this->is_while = is_while;
    this->condition = condition;
    this->body = body;
}

while_statement_t::~while_statement_t()
{
}

void while_statement_t::dump()
{
    push_tag("while_statement");
    condition->dump();
    body->dump();
    pop_tag("while_statement");
}

for_statement_t::for_statement_t(ast_t *init, ast_t *condition, 
                                         ast_t *next, ast_t *body)
{
    rtti = FOR_STATEMENT_T;
    this->init = init;
    this->condition = condition;
    this->next = next;
    this->body = body;
}

for_statement_t::~for_statement_t()
{
}

void for_statement_t::dump()
{
}

switch_statement_t::switch_statement_t(ast_t *condition, ast_t *body)
{
    rtti = SWITCH_STATEMENT_T;
    this->condition = condition;
    this->body = body;
}

switch_statement_t::~switch_statement_t()
{
}

void switch_statement_t::dump()
{
}

label_statement_t::label_statement_t(char *name)
{
    rtti = LABEL_STATEMENT_T;
    location = line_number;
    this->name = name;
    value = allocate_label();
    current_function->add_label_statement(this);
}

label_statement_t::~label_statement_t()
{
}

void label_statement_t::dump()
{
}

jump_statement_t::jump_statement_t(int type, char *label)
{
    rtti = JUMP_STATEMENT_T;
    location = line_number;
    this->type = type;
    this->label = label;
}

jump_statement_t::~jump_statement_t()
{
}

void jump_statement_t::dump()
{
}

return_statement_t::return_statement_t(ast_t *express)
{
    rtti = RETURN_STATEMENT_T;
    this->express = express;
}

return_statement_t::~return_statement_t()
{
}

void return_statement_t::dump()
{
}

group_statement_t::group_statement_t(ast_list_t *list)
{
    rtti = GROUP_STATEMENT_T;
    this->list = list;
}

group_statement_t::~group_statement_t()
{
}

void group_statement_t::dump()
{
    if (list != NULL)
        list->dump("group");
}

function_t::function_t()
{
}

void function_t::init(decorator_t *decorator, ast_t *declarator, 
                             ast_t *body)
{
    rtti = FUNCTION_DEFINE_T;
    this->decorator = decorator;
    assert(declarator->rtti == FUNCTION_DECLARATOR_T);
    this->declarator = (function_declarator_t*) declarator;
    assert(body->rtti == GROUP_STATEMENT_T);
    this->body = ((group_statement_t*)body);

    current_block = NULL;
    max_local_size = 0;
}

function_t::~function_t()
{
    list_t *block;
    for_each (block, block_list) {
        insn_t *insn;
        for_each (insn, *block)
            delete insn;
    }

    char *machine_insn;
    for_each (machine_insn, machine_insn_list) 
        delete machine_insn;
}

void function_t::dump()
{
    push_tag("function");
    decorator->dump();
    declarator->dump();
    body->dump();
    pop_tag("function");
}

char *function_t::get_name()
{
    return declarator->get_name();
}

void function_t::add_insn(insn_t *insn)
{
    int opcode = insn->opcode;
    if (opcode == OPCODE_LABEL)
        current_block = NULL;
    if (current_block == NULL) {
        current_block = new list_t();
        block_list.push_back(current_block);
    }
    current_block->push_back(insn);
    if (opcode >= OPCODE_JUMP && opcode <= OPCODE_JUMP_GREAT_EQUAL)
        current_block = NULL;
}

void function_t::add_label_statement(label_statement_t *ast)
{
    if (lookup_label(ast->name) != -1)
        error(ast, "duplicate ast '%s'", ast->name);
    label_statement_list.push_back(ast);
}

int function_t::lookup_label(char *name)
{
    label_statement_t *ast;
    for_each (ast, label_statement_list) {
        if (ast->name == name)
            return ast->value;
    }
    return -1;
}

program_t::program_t()
{
}

program_t::~program_t()
{
}

void program_t::dump()
{
    declare_list.dump("declare_list");
    function_list.dump("function_list");
}

void program_t::add_declare(ast_t *declare)
{
    declare_list.push_back(declare);
}

void program_t::add_function(ast_t *function)
{
    function_list.push_back(function);
}
