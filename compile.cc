#include "root.h"
#include "ast.h"
#include "gram.h"
#include "list.h"
#include "stack.h"
#include "insn.h"
#include "resource.h"
#include "symbol.h"
#include "type.h"
#include "opcode.h"
#include "output.h"
#include "util.h"
#include "main.h"
#include "optimize.h"
#include "local.h"
#include "type.h"

stack_t break_stack;
stack_t continue_stack;
list_t global_string_list;
int callee_saved_register_size;
int return_label;

operand_t *compile(ast_t *ast);

operand_t *evaluate(ast_t *ast)
{
    operand_t *source = compile(ast);
    if (source->is_indirect()) {
        operand_t *target = allocate_temporary();
        emit_insn(OPCODE_LOAD, "%o %o", source, target);
        return target; 
    }
    return source;
}

void branch_false(ast_t *ast, int true_label);

binary_express_t *is_logic_express(ast_t *ast, int operater)
{
    binary_express_t *binary = (binary_express_t*)ast;
    if (ast->rtti == BINARY_EXPRESS_T && binary->operater == operater)
        return binary;
    return NULL;
}

binary_express_t *is_compare_express(ast_t *ast)
{
    if (ast->rtti != BINARY_EXPRESS_T)
        return NULL;
    binary_express_t *binary = (binary_express_t*)ast;
    if (binary->opcode >= OPCODE_JUMP_EQUAL &&
        binary->opcode <= OPCODE_JUMP_GREAT_EQUAL)
        return binary;
    return NULL;
}

void branch_true(ast_t *ast, int true_label)
{
    binary_express_t *binary;

    if (binary = is_logic_express(ast, TOKEN_AND)) {
        int false_label = allocate_label();
        branch_false(binary->left, false_label);
        branch_true(binary->right, true_label);
        emit_label(false_label);
        return; 
    } 

    if (binary = is_logic_express(ast, TOKEN_OR)) {
        branch_true(binary->left, true_label);
        branch_true(binary->right, true_label);
        return;
    }

    if (binary = is_compare_express(ast)) {
        int is_signed = 1;
        operand_t *left = evaluate(binary->left);
        operand_t *right = evaluate(binary->right);
        emit_insn(binary->opcode, "%S %o %o %L",
                  is_signed, left, right, true_label);
        return;
    }

    operand_t *value = evaluate(ast);
    emit_insn(OPCODE_JUMP_NOT_EQUAL, "%o %d %L", value, 0, true_label);  
}

static int reverse_opcode(int opcode)
{
    switch (opcode) {
        case OPCODE_JUMP_EQUAL:
            return OPCODE_JUMP_NOT_EQUAL;

        case OPCODE_JUMP_NOT_EQUAL:
            return OPCODE_JUMP_EQUAL;

        case OPCODE_JUMP_LITTLE:
            return OPCODE_JUMP_GREAT_EQUAL;

        case OPCODE_JUMP_GREAT:
            return OPCODE_JUMP_LITTLE_EQUAL;

        case OPCODE_JUMP_LITTLE_EQUAL:
            return OPCODE_JUMP_GREAT;

        case OPCODE_JUMP_GREAT_EQUAL:
            return OPCODE_JUMP_LITTLE;

        default:
            assert(0);
    }
    return 0;
}

void branch_false(ast_t *ast, int false_label)
{
    binary_express_t *binary;

    if (binary = is_logic_express(ast, TOKEN_AND)) {
        branch_false(binary->left, false_label);
        branch_false(binary->right, false_label);
        return; 
    } 

    if (binary = is_logic_express(ast, TOKEN_OR)) {
        int true_label = allocate_label();
        branch_true(binary->left, true_label);
        branch_false(binary->right, false_label);
        emit_label(true_label);
        return;
    }

    if (binary = is_compare_express(ast)) {
        int is_signed = 1;
        operand_t *left = evaluate(binary->left);
        operand_t *right = evaluate(binary->right);
        int opcode = reverse_opcode(binary->opcode);
        emit_insn(opcode, "%S %o %o %L", is_signed, left, right, false_label);
        return;
    }

    operand_t *value = evaluate(ast);
    emit_insn(OPCODE_JUMP_EQUAL, "%o %d %L", value, 0, false_label);  
}

operand_t *compile_ast_list(ast_list_t *ast_list)
{
    ast_t *ast;
    for_each (ast, *ast_list) {
        compile(ast);
    }
    return NULL;
}

operand_t *compile_assign_express(assign_express_t *ast)
{
    operand_t *source = evaluate(ast->right);
    operand_t *target = compile(ast->left);
    if (target->is_indirect())
        emit_insn(OPCODE_STORE, "%o %o", source, target);
    else
        emit_insn(OPCODE_MOVE, "%o %o", source, target);
    return target;
}

operand_t *compile_condition_express(condition_express_t *ast)
{
    return NULL;
}

operand_t *compile_binary_express(binary_express_t *ast)
{
    int is_signed = 1;
    int opcode = ast->opcode;
    operand_t *source0 = evaluate(ast->left);
    operand_t *source1 = evaluate(ast->right);
    operand_t *target = find_common_express(opcode, source0, source1, NULL);
    if (target)
        return target;

    target = allocate_temporary();
    emit_insn(opcode, "%S %o %o %o", is_signed, source0, source1, target);
    return target;
}

operand_t *compile_cast_express(cast_express_t *ast)
{
    return NULL;
}

operand_t *compile_unary_operation(ast_t *express, int opcode)
{
    operand_t *source = compile(express);
    operand_t *target = allocate_temporary();
    emit_insn(opcode, "%o %o", source, target);
    return target; 
}

operand_t *compile_unary_pointer(ast_t *express)
{
    operand_t *base = compile(express);
    operand_t *indirect = new operand_t();
    indirect->set_indirect(base, 0);
    return indirect;
}

operand_t *compile_unary_not(ast_t *express)
{
    return NULL;
}

operand_t *compile_prefix_delta(ast_t *express, int opcode)
{
    operand_t *source = compile(express);
    emit_insn(opcode, "%o %d %o", source, 1, source);
    return source;
}

operand_t *compile_postfix_delta(ast_t *express, int opcode)
{
    operand_t *source = compile(express);
    operand_t *target = allocate_temporary();
    emit_insn(opcode, "%o %o", source, target);
    return target;
}

operand_t *compile_unary_express(unary_express_t *ast)
{
    switch (ast->operater) {
        case TOKEN_INC:
            return compile_prefix_delta(ast->express, OPCODE_ADD);
        case TOKEN_DEC:
            return compile_prefix_delta(ast->express, OPCODE_SUBTRACT);
        case TOKEN_SIZEOF:
            break;
        case '&':
            return compile_unary_operation(ast->express, OPCODE_ADDRESS);
        case '*':
            return compile_unary_pointer(ast->express);
        case '+':
            return compile(ast);
        case '-':
            return compile_unary_operation(ast->express, OPCODE_NEGATIVE);
        case '~':
            return compile_unary_operation(ast->express, OPCODE_COMPLEMENT);
        case '!':
            return compile_unary_not(ast);
    }
    return NULL;
}

operand_t *compile_index_express(index_express_t *ast)
{
    operand_t *base = evaluate(ast->base);
    operand_t *index = evaluate(ast->index);
    operand_t *target = new operand_t();

    int scale = 0;
    type_t *base_type = base->type;
    if (base_type->is_pointer())
        scale = ((pointer_type_t*)base_type)->base_type->size;
    else if (base_type->is_array())
        scale = ((array_type_t*)base_type)->base_type->size;
    else
        error(ast, "subscripted value is neither array nor pointer");

    if (base->is_global_address() && index->is_integer()) {
        target->set_global(base->name, index->value * scale);
        return target;
    }

    if (scale == 1 || scale == 2 || scale == 4 || scale == 8) {
        target->set_indirect(base, index, scale);
        return target;
    }

    if (index->is_integer()) {
        int offset = index->value * scale; 
        target->set_indirect(base, offset);
        return target;
    }

    // scale is not power of tow, and index is not integer constant
    operand_t *offset = allocate_temporary();
    emit_insn(OPCODE_MULTIPLY, "%o %d %o", index, scale, offset);
    target->set_indirect(base, offset);
    return target;
}

operand_t *compile_call_express(call_express_t *ast)
{
    int argument_size = 0;
    ast_t *argument;
    for_each_reverse (argument, *ast->argument_list) {
        operand_t *operand = evaluate(argument);
        emit_insn(OPCODE_PUSH, "%o", operand);
        argument_size += 4;
    }

    operand_t *function = evaluate(ast->function);
    assert(function->type->is_function());
    function_type_t *function_type = (function_type_t*) function->type;

    type_t *return_type = function_type->base_type;
    operand_t *target = NULL; 

    if (ast->in_top_level || return_type->is_void()) {
        emit_insn(OPCODE_CALL, "%o", function);
    } else if (return_type->is_integer() || return_type->is_pointer()) {
        target = allocate_temporary();
        emit_insn(OPCODE_CALL, "%o %o", function, target);
    } else
        assert(0);

    emit_insn(OPCODE_POP, "%d", argument_size);
    return target;
}

operand_t *compile_property_express(property_express_t *ast)
{
    return NULL;
}

operand_t *compile_postfix_express(postfix_express_t *ast)
{
    int opcode;

    if (ast->in_top_level) {
        opcode = (ast->operater == TOKEN_INC) ? OPCODE_ADD: OPCODE_SUBTRACT;
        return compile_prefix_delta(ast->express, opcode);
    }
    opcode = (ast->operater == TOKEN_INC) ? OPCODE_INCREASE: OPCODE_DECREASE;
    return compile_postfix_delta(ast->express, opcode);
}

operand_t *compile_argument_expresss(argument_expresss_t *ast)
{
    return NULL;
}

operand_t *compile_id_express(id_express_t *ast)
{
    operand_t *id = lookup_symbol(ast->value);
    if (!id)
        error(ast, "identifier '%s' is undeclared.\n", ast->value);
    if (!id->type->is_array())
        return id;

    if (id->level == 0) {
        operand_t *target = new operand_t();
        target->set_global_address(id->name);
        target->type = id->type;
        return target;
    } else {
        operand_t *target = allocate_temporary();
        emit_insn(OPCODE_ADDRESS, "%o %o", id, target);
        target->type = id->type;
        return target;
    }
}

operand_t *compile_integer_express(integer_express_t *ast)
{
    operand_t *operand = new operand_t();
    operand->set_integer(ast->value);
    return operand;
}

operand_t *compile_string_express(string_express_t *ast)
{
    char *name = (char *) malloc(16);
    sprintf(name, ".S%d", global_string_list.count());
    global_string_list.push_back(ast->value);
    operand_t *operand = new operand_t();
    operand->set_global_address(name);
    return operand;
}

void compile_declare(declare_t *ast)
{
    declarator_t *declarator;
    for_each (declarator, *ast->declarator_list) {
        current_scope->insert_declare(ast->decorator, declarator, -1);
    }
}

void output_declare(declare_t *ast)
{
    declarator_t *declarator;
    for_each (declarator, *ast->declarator_list) {
        char *name = declarator->get_name();
        type_t *type = declarator->deduce_type(ast->decorator);
        if (type->size)
            output(".comm\t%s,%d\n", name, type->size);
    }
}

void compile_init(init_t *ast)
{
}

void compile_struct_specify(struct_specify_t *ast)
{
}

void compile_union_specify(union_specify_t *ast)
{
}

void compile_enum_specify(enum_specify_t *ast)
{
}

void compile_enum_item(enum_item_t *ast)
{
}

void compile_type_name(type_name_t *ast)
{
}

void compile_express_statement(express_statement_t *ast)
{
    ast_t *express = ast->express;
    if (express) {
        express->in_top_level = 1;
        compile(express);
    }
}

/**
 * Translation of if statement
 *               branch_false exit
 * then_part:    ...
 * exit:
 */
void compile_if_statement(ast_t *condition, ast_t *then_part)
{
    int exit_label = allocate_label();
    branch_false(condition, exit_label);
    compile(then_part);
    emit_label(exit_label);
}

/**
 * Translation of if statement
 *               branch_false else_part
 * then_part:    ...
 *               jump exit
 * else_part:    ... 
 * exit:
 */
void compile_if_statement(if_statement_t *ast)
{
    ast_t *condition = ast->condition;
    ast_t *then_part = ast->then_part; 
    ast_t *else_part = ast->else_part;

    if (!else_part) {
        compile_if_statement(condition, then_part);
        return;
    }

    int else_part_label = allocate_label();
    int exit_label = allocate_label();
    branch_false(condition, else_part_label);
    compile(then_part); 
    emit_jump(exit_label);
    emit_label(else_part_label);
    compile(else_part);
    emit_label(exit_label);
}

/**
 * Translation of for statement
 *
 *               init
 *               jump condition
 * body:         ...
 *               ...
 * next:         ...
 *               ...
 * condition:    branch_true body 
 *               
 * exit:
 */
void compile_loop_statement(int is_while_statement, 
                            ast_t *body, ast_t *next, ast_t *condition)
{
    int body_label = allocate_label();
    int condition_label = allocate_label();
    int next_label = -1;
    if (next) {
        next_label = allocate_label();
        continue_stack.push(next_label);
    } else 
        continue_stack.push(condition_label);
    int exit_label = allocate_label();
    break_stack.push(exit_label);

    if (is_while_statement)
        emit_jump(condition_label);

    enter_loop();
    emit_label(body_label);
    compile(body);
    if (next) {
        emit_label(next_label);
        next->in_top_level = 1;
        compile(next);
    }
    emit_label(condition_label);
    branch_true(condition, body_label);
    leave_loop();

    emit_label(exit_label);
    break_stack.pop();
    continue_stack.pop();
}

/**
 * Translation of while statement:
 *
 *               jump condition
 * body:         ...
 *               ...
 * condition:    branch_true body 
 * exit:
 */
void compile_while_statement(while_statement_t *ast)
{
    compile_loop_statement(ast->is_while, ast->body, NULL, ast->condition);
}

void compile_for_statement(for_statement_t *ast)
{
    compile(ast->init);
    compile_loop_statement(1, ast->body, ast->next, ast->condition);
}

void compile_switch_statement(switch_statement_t *ast)
{
}

void compile_label_statement(label_statement_t *ast)
{
    emit_insn(OPCODE_LABEL, "%L", ast->value);
}

void compile_jump_statement(jump_statement_t *ast)
{
    if (ast->type == TOKEN_GOTO) {
        int label = current_function->lookup_label(ast->label);
        if (label < 0)
            error(ast, "label '%s' is not defined", ast->label);
        emit_insn(OPCODE_JUMP, "%L", label);
        return;
    }

    if (ast->type == TOKEN_BREAK) {
        if (break_stack.count == 0)
            error(ast, "break statement not within loop or switch");
        int label = break_stack.get_top();
        emit_insn(OPCODE_JUMP, "%L", label); 
        return;
    }

    if (ast->type == TOKEN_CONTINUE) {
        if (continue_stack.count == 0)
            error(ast, "continue statement not within loop");
        int label = continue_stack.get_top();
        emit_insn(OPCODE_JUMP, "%L", label); 
        return;
    }
}

void compile_return_statement(return_statement_t *ast)
{
    if (ast->express) {
        operand_t *result = evaluate(ast->express);
        emit_insn(OPCODE_RETURN, "%o", result);
    } else
        emit_insn(OPCODE_RETURN, "");
}

void compile_group_statement(group_statement_t *ast)
{
    enter_scope();
    compile_ast_list(ast->list);
    leave_scope();
}

void compile_prototype(prototype_t *ast)
{
    current_scope->current_offset = 8;
    argument_declare_t *declare; 
    for_each (declare, *ast->argument_declare_list) {
        decorator_t *decorator = declare->decorator;
        declarator_t *declarator = declare->declarator;
        current_scope->insert_declare(decorator, declarator, +1);
    }
    current_scope->current_offset = 0;
}

void compile_argument_declare(argument_declare_t *ast)
{
}

void enter_function()
{
    enter_scope();
    assert(break_stack.count == 0);
    assert(continue_stack.count == 0);
    reset_temporary_list();
    reset_register_array();
    reset_transient_array();
    reset_local();
    callee_saved_register_size = 0;
    return_label = allocate_label();
}

void leave_function()
{
    leave_scope();
}

void dump_insn_list(list_t *block_list)
{
    list_t *block;
    for_each (block, *block_list) {
        insn_t *insn;
        for_each (insn, *block)
            insn->dump();
    }
    printf("\n");
    dump_local_usage();
}

void compile_function(function_t *ast)
{
    current_scope->insert_function(ast);
    enter_function();

    // translate ast to IR
    compile_prototype(ast->declarator->prototype);
    compile_ast_list(ast->body->list);
    ast_t *back = (ast_t*) ast->body->list->back();
    if (!back || back->rtti != RETURN_STATEMENT_T)
        emit_insn(OPCODE_RETURN, "");
    emit_label(return_label);
    optimize();

    if (option_dump_insn) {
        printf("function %s()\n", ast->get_name());
        dump_insn_list(&ast->block_list);
        leave_function();
        return;
    }

    // translate IR to x86
    int register_count = allocate_register_for_local();
    callee_saved_register_size = register_count * 4;
    callee_save_register(); 
    load_argument();

    list_t *block;
    for_each (block, ast->block_list) {
        insn_t *insn;
        for_each (insn, *block) {
            insn_handler_array[insn->opcode](insn);
            insn->decrease_life();
        }
    }
    callee_restore_register(); 

    leave_function();
}

void trim(char *string)
{
    int length = strlen(string);
    assert(length);
    string[length - 1] = 0;
}

// eliminate jump in two case:
// case 1)
// jmp .L1
// jmp .L2
//
// case 2)
// jmp .L1
// .L1:
int eliminate_jump(char *this_insn, char *next_insn)
{
    if (memcmp(this_insn, "\tjmp", 4) != 0)
        return 0;
    char this_label[32];
    strcpy(this_label, this_insn + 5);
    trim(this_label);

    if (next_insn[0] == '.') {
        char next_label[32];
        strcpy(next_label, next_insn);
        trim(next_label);
        trim(next_label);
        if (strcmp(this_label, next_label))
            return 0;
        output("%s", next_insn);
        return 1;
    }

    if (memcmp(next_insn, "\tjmp", 4) == 0) {
        output("%s", this_insn);
        return 1;
    }

    return 0;
}

void output_function(function_t *ast)
{
    int max_stack_depth = ast->max_local_size + transient_count * 4;
    output_epilogue(ast->get_name(), max_stack_depth);

    list_node_t *this_node = ast->machine_insn_list.begin();
    list_node_t *next_node;
    list_node_t *end_node = ast->machine_insn_list.end(); 

    while (this_node != end_node) {
        next_node = this_node->next;
        char *this_insn = (char *) this_node->item;
        char *next_insn = (char *) next_node->item;
        if (!option_eliminate_jump) {
        normal:     
            output("%s", this_insn);
            this_node = next_node;
            continue;
        }
        if (next_node == end_node)
            goto normal;
        if (!eliminate_jump(this_insn, next_insn))
            goto normal;
        // skip this_node and next_node
        this_node = next_node->next;
    }

    output_prologue();
    output("\n");
}

void compile_program()
{
    enter_scope();

    ast_t *ast;
    for_each (ast, program->declare_list)
        compile(ast);

    for_each (ast, program->function_list) {
        current_function = (function_t *) ast;
        compile(ast);
    }

    leave_scope();
}

void output_program()
{
    if (option_dump_symbol || option_dump_insn)
        return;

    output("\t.data\n");
    int index = 0;
    char *string;
    for_each (string, global_string_list) {
        output(".S%d:\t.string \"%s\"\n", index, string);
        index++;
    }
    output("\n");

    declare_t *declare;
    for_each (declare, program->declare_list)
        output_declare(declare);

    output("\t.text\n");
    function_t *function;
    for_each (function, program->function_list) {
        current_function = (function_t *) function;
        output_function(function);
    }
}

#define EVALUATE(TYPE, type)                                                 \
    case TYPE##_T:                                                           \
        return compile_##type((type##_t *)ast);

#define COMPILE(TYPE, type)                                                  \
    case TYPE##_T:                                                           \
        compile_##type((type##_t *)ast);                                     \
        return NULL;

operand_t *compile(ast_t *ast)
{
    switch (ast->rtti) {
        EVALUATE(ASSIGN_EXPRESS, assign_express);
        EVALUATE(CONDITION_EXPRESS, condition_express);
        EVALUATE(BINARY_EXPRESS, binary_express);
        EVALUATE(CAST_EXPRESS, cast_express);
        EVALUATE(UNARY_EXPRESS, unary_express);
        EVALUATE(INDEX_EXPRESS, index_express);
        EVALUATE(CALL_EXPRESS, call_express);
        EVALUATE(PROPERTY_EXPRESS, property_express);
        EVALUATE(POSTFIX_EXPRESS, postfix_express);
        EVALUATE(ARGUMENT_EXPRESSS, argument_expresss);
        EVALUATE(ID_EXPRESS, id_express);
        EVALUATE(INTEGER_EXPRESS, integer_express);
        EVALUATE(STRING_EXPRESS, string_express);

        COMPILE(DECLARE, declare);
        COMPILE(INIT, init);
        COMPILE(STRUCT_SPECIFY, struct_specify);
        COMPILE(UNION_SPECIFY, union_specify);
        COMPILE(ENUM_SPECIFY, enum_specify);
        COMPILE(ENUM_ITEM, enum_item);
        COMPILE(TYPE_NAME, type_name);
        COMPILE(EXPRESS_STATEMENT, express_statement);
        COMPILE(IF_STATEMENT, if_statement);
        COMPILE(WHILE_STATEMENT, while_statement);
        COMPILE(FOR_STATEMENT, for_statement);
        COMPILE(SWITCH_STATEMENT, switch_statement);
        COMPILE(LABEL_STATEMENT, label_statement);
        COMPILE(JUMP_STATEMENT, jump_statement);
        COMPILE(RETURN_STATEMENT, return_statement);
        COMPILE(GROUP_STATEMENT, group_statement);
        COMPILE(FUNCTION_DEFINE, function);
    }
    return NULL;
}
