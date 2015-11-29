#include "root.h"
#include "ast.h"

void check(ast_t *ast);

void check_assign_express(assign_express_t *ast)
{
}

void check_condition_express(condition_express_t *ast)
{
}

void check_binary_express(binary_express_t *ast)
{
}

void check_cast_express(cast_express_t *ast)
{
}

void check_unary_express(unary_express_t *ast)
{
}

void check_index_express(index_express_t *ast)
{
}

void check_call_express(call_express_t *ast)
{
}

void check_property_express(property_express_t *ast)
{
}

void check_postfix_express(postfix_express_t *ast)
{
}

void check_argument_expresss(argument_expresss_t *ast)
{
}

void check_id_express(id_express_t *ast)
{
}

void check_integer_express(integer_express_t *ast)
{
}

void check_string_express(string_express_t *ast)
{
}

void check_declare(declare_t *ast)
{
}

void check_decorator(decorator_t *ast)
{
}

void check_init(init_t *ast)
{
}

void check_declarator(declarator_t *ast)
{
}

void check_id_declarator(id_declarator_t *ast)
{
}

void check_pointer_declarator(pointer_declarator_t *ast)
{
}

void check_array_declarator(array_declarator_t *ast)
{
}

void check_function_declarator(function_declarator_t *ast)
{
}

void check_struct_specify(struct_specify_t *ast)
{
}

void check_union_specify(union_specify_t *ast)
{
}

#define CHECK(TYPE, type)                                                    \
    case TYPE##_T:                                                           \
        check_##type((type##_t *)ast);                                       \
        break

void check(ast_t *ast)
{
    switch (ast->rtti) {
        CHECK(ASSIGN_EXPRESS, assign_express);
        CHECK(CONDITION_EXPRESS, condition_express);
        CHECK(BINARY_EXPRESS, binary_express);
        CHECK(CAST_EXPRESS, cast_express);
        CHECK(UNARY_EXPRESS, unary_express);
        CHECK(INDEX_EXPRESS, index_express);
        CHECK(CALL_EXPRESS, call_express);
        CHECK(PROPERTY_EXPRESS, property_express);
        CHECK(POSTFIX_EXPRESS, postfix_express);
        CHECK(ARGUMENT_EXPRESSS, argument_expresss);
        CHECK(ID_EXPRESS, id_express);
        CHECK(INTEGER_EXPRESS, integer_express);
        CHECK(STRING_EXPRESS, string_express);
        CHECK(DECLARE, declare);
        CHECK(DECORATOR, decorator);
        CHECK(INIT, init);
        CHECK(POINTER_DECLARATOR, pointer_declarator);
        CHECK(ID_DECLARATOR, id_declarator);
        CHECK(ARRAY_DECLARATOR, array_declarator);
        CHECK(FUNCTION_DECLARATOR, function_declarator);
        CHECK(STRUCT_SPECIFY, struct_specify);
    }
}
