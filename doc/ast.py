#!/usr/bin/python
def print_operation(operation, type):
    print """
void %s_%s(%s_ast_t *ast)
{
}""" % (operation, type, type)

def print_ast():
    print """
struct ast_t {
    int rtti;
    type_t *type;

    ast_t();
    virtual ~ast_t() = 0;
    virtual void dump() = 0;
};
"""

def print_enum():
    global types
    print "enum {"
    for type in types:
        type = type + "_ast_t"
        print "    %s," % type.upper()
    print "};"
    
def print_head(type):
    print """
struct %s: ast_t {
    %s();
    ~%s();
    void dump();
};""" % (type, type, type)

def print_body(type):
    print """
%s::%s()
{
    rtti = %s;
}

%s::~%s()
{
}

void %s::dump()
{
}""" % (type, type, type.upper(), type, type, type)

types = """
assign_express
condition_express
binary_express
cast_express
unary_express
array_express
call_express
property_express
postfix_express
argument_expresss
id_express
integer_express
string_express
declare
declare_specify
init_declarer
init
declarer
id_declarer
array_declarer
function_declarer
prototype
argument_declare
atom_type
user_type
struct_specify
union_specify
enum_specify
enum_item
type_name
express_statement
if_statement
while_statement
for_statement
switch_statement
return_statement
break_statement
continue_statement
group_statement
function_define
""".split("\n")
types = types[1:-1]

print_enum()
print_ast()
for type in types:
    type = type + "_ast_t"
    print_head(type)

for type in types:
    type = type + "_ast_t"
    print_body(type)

for type in types:
    print_operation("compile", type)

for type in types:
    print_operation("check", type)

def switch_operation(operation):
    print "void %s(ast_t *ast)" % operation
    print "{"
    print "    switch (ast->rtti) {"
    for type in types:
        print "        %s(%s, %s);" % (operation.upper(), type.upper(), type)
    print "    }"
    print "}"

switch_operation("compile")
switch_operation("check")
