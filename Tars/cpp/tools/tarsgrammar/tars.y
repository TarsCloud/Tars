/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

%{
#include <iostream>
#include <memory>
#include <cassert>

using namespace std;

#define YYSTYPE GrammarBasePtr

#include "parse.h"
#define YYDEBUG 1
#define YYINITDEPTH 10000
%}

%defines
%debug

//keyword token
%token TARS_VOID
%token TARS_STRUCT
%token TARS_BOOL
%token TARS_BYTE
%token TARS_SHORT
%token TARS_INT
%token TARS_DOUBLE
%token TARS_FLOAT
%token TARS_LONG
%token TARS_STRING
%token TARS_VECTOR
%token TARS_MAP
%token TARS_NAMESPACE
%token TARS_INTERFACE
%token TARS_IDENTIFIER
%token TARS_OUT
%token TARS_OP
%token TARS_KEY
%token TARS_ROUTE_KEY
%token TARS_REQUIRE
%token TARS_OPTIONAL
%token TARS_CONST_INTEGER
%token TARS_CONST_FLOAT
%token TARS_FALSE
%token TARS_TRUE
%token TARS_STRING_LITERAL
%token TARS_SCOPE_DELIMITER
%token TARS_CONST
%token TARS_ENUM
%token TARS_UNSIGNED
%token BAD_CHAR

%%
start: definitions
;

// ----------------------------------------------------------------------
definitions
// ----------------------------------------------------------------------
: definition
{
}
';' definitions
| error ';'
{
    yyerrok;
}
definitions
| definition
{
    g_parse->error("`;' missing after definition");
}
|
{
}
;

// ----------------------------------------------------------------------
definition
// ----------------------------------------------------------------------
: namespace_def
{
    assert($1 == 0 || NamespacePtr::dynamicCast($1));
}
| interface_def
{
    assert($1 == 0 || InterfacePtr::dynamicCast($1));
}
| struct_def
{
    assert($1 == 0 || StructPtr::dynamicCast($1));
}
| key_def
{
}
| enum_def
{
    assert($1 == 0 || EnumPtr::dynamicCast($1));
}
| const_def
{
    assert($1 == 0 || ConstPtr::dynamicCast($1));
}
;

// ----------------------------------------------------------------------
enum_def
// ----------------------------------------------------------------------
: enum_id
{
    $$ = $1;
}
'{' enumerator_list '}'
{
    if($3)
    {
        g_parse->popContainer();
        $$ = $3;
    }
    else
    {
        $$ = 0;
    }

    $$ = $2;
}
;

// ----------------------------------------------------------------------
enum_id
// ----------------------------------------------------------------------
: TARS_ENUM TARS_IDENTIFIER
{
    NamespacePtr c = NamespacePtr::dynamicCast(g_parse->currentContainer());
    if(!c)
    {
        g_parse->error("enum must define in namespace");
    }
    StringGrammarPtr ident  = StringGrammarPtr::dynamicCast($2);
    EnumPtr e = c->createEnum(ident->v);
    g_parse->pushContainer(e);

    $$ = e;
}
| TARS_ENUM keyword
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);
    g_parse->error("keyword `" + ident->v + "' cannot be used as enumeration name");
    $$ = $2;
}
;

// ----------------------------------------------------------------------
enumerator_list
// ----------------------------------------------------------------------
: enumerator ',' enumerator_list
{
    $$ = $2;
}
| enumerator
{
}
;

// ----------------------------------------------------------------------
enumerator
// ----------------------------------------------------------------------
: TARS_IDENTIFIER
{
    TypePtr type        = TypePtr::dynamicCast(g_parse->createBuiltin(Builtin::KindLong));
    StringGrammarPtr ident  = StringGrammarPtr::dynamicCast($1);
    TypeIdPtr tPtr      = new TypeId(type, ident->v);
    tPtr->disableDefault();
    EnumPtr e = EnumPtr::dynamicCast(g_parse->currentContainer());
    assert(e);
    e->addMember(tPtr);
    $$ = e;
}
| keyword
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);
    g_parse->error("keyword `" + ident->v + "' cannot be used as enumerator");
}
| TARS_IDENTIFIER  '=' const_initializer 
{
    TypePtr type        = TypePtr::dynamicCast(g_parse->createBuiltin(Builtin::KindLong));
    StringGrammarPtr ident  = StringGrammarPtr::dynamicCast($1);
    TypeIdPtr tPtr      = new TypeId(type, ident->v);
    ConstGrammarPtr sPtr    = ConstGrammarPtr::dynamicCast($3);
    g_parse->checkConstValue(tPtr, sPtr->t);
    tPtr->setDefault(sPtr->v);
    EnumPtr e = EnumPtr::dynamicCast(g_parse->currentContainer());
    assert(e);
    e->addMember(tPtr);
    $$ = e;
}
| 
{
}
;

// ----------------------------------------------------------------------
namespace_def
// ----------------------------------------------------------------------
: TARS_NAMESPACE TARS_IDENTIFIER
{
    StringGrammarPtr ident  = StringGrammarPtr::dynamicCast($2);
    ContainerPtr c      = g_parse->currentContainer();
    NamespacePtr n      = c->createNamespace(ident->v);
    if(n)
    {
        g_parse->pushContainer(n);
        $$ = GrammarBasePtr::dynamicCast(n);
    }
    else
    {
        $$ = 0;
    }
}
'{' definitions '}'
{
    if($3)
    {
        g_parse->popContainer();
        $$ = $3;
    }
    else
    {
        $$ = 0;
    }
}
;


//结构key------------------------------------------------------
key_def
// ----------------------------------------------------------------------
: TARS_KEY '[' scoped_name ','
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($3);
    StructPtr sp = StructPtr::dynamicCast(g_parse->findUserType(ident->v));
    if(!sp)
    {
        g_parse->error("struct '" + ident->v + "' undefined!");
    }

    g_parse->setKeyStruct(sp);
}
key_members ']'
{
}
;

// 结构key------------------------------------------------------
key_members
// ----------------------------------------------------------------------
: TARS_IDENTIFIER
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);
    StructPtr np = g_parse->getKeyStruct();
    if(np)
    {
        np->addKey(ident->v);
    }
    else
    {
        $$ = 0;
    }
}
| key_members ',' TARS_IDENTIFIER
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($3);
    StructPtr np = g_parse->getKeyStruct();
    if(np)
    {
        np->addKey(ident->v);
    }
    else
    {
        $$ = 0;
    }   
}
;


// ----------------------------------------------------------------------
interface_def
// ----------------------------------------------------------------------
: interface_id
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);

    NamespacePtr c = NamespacePtr::dynamicCast(g_parse->currentContainer());

    InterfacePtr cl = c->createInterface(ident->v);
    if(cl)
    {
        g_parse->pushContainer(cl);
        $$ = GrammarBasePtr::dynamicCast(cl);
    }
    else
    {
        $$ = 0;
    }
}
'{' interface_exports '}'
{
    if($2)
    {
       g_parse->popContainer();
       $$ = GrammarBasePtr::dynamicCast($2);
    }
    else
    {
       $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
interface_id
// ----------------------------------------------------------------------
: TARS_INTERFACE TARS_IDENTIFIER
{
    $$ = $2;
}
| TARS_INTERFACE keyword
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);
    g_parse->error("keyword `" + ident->v + "' cannot be used as interface name");
    $$ = $2;
}
;

// ----------------------------------------------------------------------
interface_exports
// ----------------------------------------------------------------------
: interface_export ';' interface_exports
{
}
| error ';' interface_exports
{
}
| interface_export
{
    g_parse->error("`;' missing after definition");
}
|
{
}
;

// ----------------------------------------------------------------------
interface_export
// ----------------------------------------------------------------------
: operation
;

// ----------------------------------------------------------------------
operation
// ----------------------------------------------------------------------
: operation_preamble parameters ')'
{
    if($1)
    {
        g_parse->popContainer();
        $$ = GrammarBasePtr::dynamicCast($1);
    }
    else
    {
        $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
operation_preamble
// ----------------------------------------------------------------------
: return_type TARS_OP
{
    TypePtr returnType = TypePtr::dynamicCast($1);
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);
    string name        = ident->v;
    InterfacePtr cl    = InterfacePtr::dynamicCast(g_parse->currentContainer());
    if(cl)
    {
         OperationPtr op = cl->createOperation(name, returnType);
         if(op)
         {
             g_parse->pushContainer(op);
             $$ = GrammarBasePtr::dynamicCast(op);
         }
         else
         {
             $$ = 0;
         }
    }
    else
    {
        $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
return_type
// ----------------------------------------------------------------------
: type
| TARS_VOID
{
    $$ = 0;
}
;


// ----------------------------------------------------------------------
parameters
// ----------------------------------------------------------------------
: // empty
{
}
| type_id
{
    TypeIdPtr  tsp         = TypeIdPtr::dynamicCast($1);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    assert(op);
    if(op)
    {
        op->createParamDecl(tsp, false, false);
    }
}
| parameters ',' type_id
{
    TypeIdPtr  tsp         = TypeIdPtr::dynamicCast($3);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    assert(op);
    if(op)
    {
        op->createParamDecl(tsp, false, false);
    }
}
| out_qualifier type_id
{
    BoolGrammarPtr isOutParam  = BoolGrammarPtr::dynamicCast($1);
    TypeIdPtr  tsp         = TypeIdPtr::dynamicCast($2);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    assert(op);
    if(op)
    {
        op->createParamDecl(tsp, isOutParam->v, false);
    }
}
| parameters ',' out_qualifier type_id
{
    BoolGrammarPtr isOutParam  = BoolGrammarPtr::dynamicCast($3);
    TypeIdPtr  tsp         = TypeIdPtr::dynamicCast($4);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    assert(op);
    if(op)
    {
        op->createParamDecl(tsp, isOutParam->v, false);
    }
}
| routekey_qualifier type_id
{
    BoolGrammarPtr isRouteKeyParam  = BoolGrammarPtr::dynamicCast($1);
    TypeIdPtr  tsp              = TypeIdPtr::dynamicCast($2);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    assert(op);
    if(op)
    {
         op->createParamDecl(tsp, false, isRouteKeyParam->v);
    }
}
| parameters ',' routekey_qualifier type_id
{
    BoolGrammarPtr isRouteKeyParam = BoolGrammarPtr::dynamicCast($3);
    TypeIdPtr  tsp             = TypeIdPtr::dynamicCast($4);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    assert(op);
    if(op)
    {
         op->createParamDecl(tsp, false, isRouteKeyParam->v);
    }
}
| out_qualifier 
{
    g_parse->error("'out' must be defined with a type");
}
| routekey_qualifier 
{
    g_parse->error("'routekey' must be defined with a type");
}
;

// ----------------------------------------------------------------------
routekey_qualifier
// ----------------------------------------------------------------------
: TARS_ROUTE_KEY
{
    BoolGrammarPtr routekey = new BoolGrammar;
    routekey->v = true;
    $$ = GrammarBasePtr::dynamicCast(routekey);
}
;

// ----------------------------------------------------------------------
out_qualifier
// ----------------------------------------------------------------------
: TARS_OUT
{
    BoolGrammarPtr out = new BoolGrammar;
    out->v = true;
    $$ = GrammarBasePtr::dynamicCast(out);
}
;

// 结构定义--------------------------------------------------------------
struct_def
// ----------------------------------------------------------------------
: struct_id
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);
    NamespacePtr np = NamespacePtr::dynamicCast(g_parse->currentContainer());
    if(np)
    {
         StructPtr sp = np->createStruct(ident->v);
         if(sp)
         {
             g_parse->pushContainer(sp);
             $$ = GrammarBasePtr::dynamicCast(sp);
         }
         else
         {
             $$ = 0;
         }
    }
    else
    {
       g_parse->error("struct '" + ident->v + "' must definition in namespace");
    }
}
'{' struct_exports '}'
{
    if($2)
    {
        g_parse->popContainer();
    }
    $$ = $2;

    //不能有空结构
    StructPtr st = StructPtr::dynamicCast($$);
    assert(st);
    if(st->getAllMemberPtr().size() == 0)
    {
        g_parse->error("struct `" + st->getSid() + "' must have at least one member");
    }
}
;

// 结构名称定义----------------------------------------------------------
struct_id
// ----------------------------------------------------------------------
: TARS_STRUCT TARS_IDENTIFIER
{
    $$ = $2;
}
| TARS_STRUCT keyword
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);

    g_parse->error("keyword `" + ident->v + "' cannot be used as struct name");
}
| TARS_STRUCT error
{
    g_parse->error("abstract declarator '<anonymous struct>' used as declaration");
}
;

// 结构成员变量部分------------------------------------------------------
struct_exports
// ----------------------------------------------------------------------
: data_member ';' struct_exports
{

}
| data_member
{
   g_parse->error("';' missing after definition");
}
|
{
}
;

// 
// 
// 数据成员--------------------------------------------------------------
data_member
// ----------------------------------------------------------------------
: struct_type_id
{
    $$ = GrammarBasePtr::dynamicCast($1);
}
;

// 结构的数据成员--------------------------------------------------------------
struct_type_id
// ----------------------------------------------------------------------
: TARS_CONST_INTEGER TARS_REQUIRE type_id
{
    StructPtr np = StructPtr::dynamicCast(g_parse->currentContainer());
    if(np)
    {
        IntergerGrammarPtr iPtr = IntergerGrammarPtr::dynamicCast($1);
        g_parse->checkTag(iPtr->v);

        TypeIdPtr tPtr  = TypeIdPtr::dynamicCast($3);
        tPtr->setRequire(iPtr->v);
        np->addTypeId(tPtr);
        $$ = GrammarBasePtr::dynamicCast($3);
    }
    else
    {
        $$ = 0;
    }
}
| TARS_CONST_INTEGER TARS_REQUIRE type_id '=' const_initializer
{
    StructPtr np = StructPtr::dynamicCast(g_parse->currentContainer());
    if(np)
    {
        IntergerGrammarPtr iPtr = IntergerGrammarPtr::dynamicCast($1);
        g_parse->checkTag(iPtr->v);

        //判断类型和数值类型是否一致
        TypeIdPtr tPtr   = TypeIdPtr::dynamicCast($3);
        ConstGrammarPtr sPtr = ConstGrammarPtr::dynamicCast($5);
        g_parse->checkConstValue(tPtr, sPtr->t);

        tPtr->setRequire(iPtr->v);
        tPtr->setDefault(sPtr->v);
        np->addTypeId(tPtr);
        $$ = GrammarBasePtr::dynamicCast($3);
    }
    else
    {
        $$ = 0;
    }
}
| TARS_CONST_INTEGER TARS_OPTIONAL type_id '=' const_initializer
{
    StructPtr np = StructPtr::dynamicCast(g_parse->currentContainer());
    if(np)
    {
        IntergerGrammarPtr iPtr = IntergerGrammarPtr::dynamicCast($1);
        g_parse->checkTag(iPtr->v);

        //判断类型和数值类型是否一致
        TypeIdPtr tPtr   = TypeIdPtr::dynamicCast($3);
        ConstGrammarPtr sPtr = ConstGrammarPtr::dynamicCast($5);
        g_parse->checkConstValue(tPtr, sPtr->t);

        tPtr->setOptional(iPtr->v);
        tPtr->setDefault(sPtr->v);
        np->addTypeId(tPtr);
        $$ = GrammarBasePtr::dynamicCast($3);
    }
    else
    {
        $$ = 0;
    }
}
| TARS_CONST_INTEGER TARS_OPTIONAL type_id
{
    StructPtr np = StructPtr::dynamicCast(g_parse->currentContainer());
    if(np)
    {
        IntergerGrammarPtr iPtr = IntergerGrammarPtr::dynamicCast($1);
        g_parse->checkTag(iPtr->v);
        TypeIdPtr tPtr = TypeIdPtr::dynamicCast($3);
        tPtr->setOptional(iPtr->v);
        np->addTypeId(tPtr);
        $$ = GrammarBasePtr::dynamicCast($3);
    }
    else
    {
        $$ = 0;
    }
}
| TARS_REQUIRE type_id
{
    g_parse->error("struct member need 'tag'");
}
| TARS_OPTIONAL type_id
{
    g_parse->error("struct member need 'tag'");
}
| TARS_CONST_INTEGER type_id
{
    g_parse->error("struct member need 'require' or 'optional'");
}
| type_id
{
    g_parse->error("struct member need 'tag' or 'require' or 'optional'");
}
;

// ----------------------------------------------------------------------
const_initializer
// ----------------------------------------------------------------------
: TARS_CONST_INTEGER
{
    IntergerGrammarPtr intVal = IntergerGrammarPtr::dynamicCast($1);
    ostringstream sstr;
    sstr << intVal->v;
    ConstGrammarPtr c = new ConstGrammar();
    c->t = ConstGrammar::VALUE;
    c->v = sstr.str();
    $$ = c;
}
| TARS_CONST_FLOAT
{
    FloatGrammarPtr floatVal = FloatGrammarPtr::dynamicCast($1);
    ostringstream sstr;
    sstr << floatVal->v;
    ConstGrammarPtr c = new ConstGrammar();
    c->t = ConstGrammar::VALUE;
    c->v = sstr.str();
    $$ = c;
}
| TARS_STRING_LITERAL
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);
    ConstGrammarPtr c = new ConstGrammar();
    c->t = ConstGrammar::STRING;
    c->v = ident->v;
    $$ = c;
}
| TARS_FALSE
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);
    ConstGrammarPtr c = new ConstGrammar();
    c->t = ConstGrammar::BOOL;
    c->v = ident->v;
    $$ = c;
}
| TARS_TRUE
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);
    ConstGrammarPtr c = new ConstGrammar();
    c->t = ConstGrammar::BOOL;
    c->v = ident->v;
    $$ = c;
}
| TARS_IDENTIFIER
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);

    if (g_parse->checkEnum(ident->v) == false)
    {
        g_parse->error("error enum default value, not defined yet");
    }
    ConstGrammarPtr c = new ConstGrammar();
    c->t = ConstGrammar::ENUM;
    c->v = ident->v;
    $$ = c;
}
| scoped_name TARS_SCOPE_DELIMITER TARS_IDENTIFIER
{

    StringGrammarPtr scoped = StringGrammarPtr::dynamicCast($1);
    StringGrammarPtr ident  = StringGrammarPtr::dynamicCast($3);
    
    if (g_parse->checkEnum(ident->v) == false)
    {
        g_parse->error("error enum default value, not defined yet");
    }
    ConstGrammarPtr c = new ConstGrammar();
    c->t = ConstGrammar::ENUM;
    c->v = scoped->v + "::" + ident->v;
    $$ = c;
}
;

// 常量定义--------------------------------------------------------------
const_def
// ----------------------------------------------------------------------
: TARS_CONST type_id '=' const_initializer
{
    NamespacePtr np = NamespacePtr::dynamicCast(g_parse->currentContainer());
    if(!np)
    {
        g_parse->error("const type must define in namespace");
    }

    TypeIdPtr t   = TypeIdPtr::dynamicCast($2);
    ConstGrammarPtr c = ConstGrammarPtr::dynamicCast($4);
    ConstPtr cPtr = np->createConst(t, c);
    $$ = cPtr;
}
;

// 变量定义--------------------------------------------------------------
type_id
// ----------------------------------------------------------------------
: type TARS_IDENTIFIER
{
    TypePtr type = TypePtr::dynamicCast($1);
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);

    TypeIdPtr typeIdPtr = new TypeId(type, ident->v);

    $$ = GrammarBasePtr::dynamicCast(typeIdPtr);
}
|type TARS_IDENTIFIER  '[' TARS_CONST_INTEGER ']'
{
    TypePtr type = g_parse->createVector(TypePtr::dynamicCast($1));
    IntergerGrammarPtr iPtrSize = IntergerGrammarPtr::dynamicCast($4);
    g_parse->checkArrayVaid(type,iPtrSize->v);
    type->setArray(iPtrSize->v);
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);
    TypeIdPtr typeIdPtr = new TypeId(type, ident->v);
    $$ = GrammarBasePtr::dynamicCast(typeIdPtr);
}
|type '*' TARS_IDENTIFIER  
{
    TypePtr type = g_parse->createVector(TypePtr::dynamicCast($1));
    //IntergerGrammarPtr iPtrSize = IntergerGrammarPtr::dynamicCast($4);
    g_parse->checkPointerVaid(type);
    type->setPointer(true);
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($3);
    TypeIdPtr typeIdPtr = new TypeId(type, ident->v);
    $$ = GrammarBasePtr::dynamicCast(typeIdPtr);
}
|type TARS_IDENTIFIER  ':' TARS_CONST_INTEGER 
{
    TypePtr type = TypePtr::dynamicCast($1);
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);
    TypeIdPtr typeIdPtr = new TypeId(type, ident->v);
    IntergerGrammarPtr iPtrSize = IntergerGrammarPtr::dynamicCast($3);
    g_parse->checkArrayVaid(type,iPtrSize->v);
    $$ = GrammarBasePtr::dynamicCast(typeIdPtr);
}
| type keyword
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);
    g_parse->error("keyword `" + ident->v + "' cannot be used as data member name");
}
| type
{
    g_parse->error("missing data member name");
}
| error
{
    g_parse->error("unkown type");
}
;

// 类型------------------------------------------------------------------
type
// ----------------------------------------------------------------------
: type_no ':' TARS_CONST_INTEGER 
{

    TypePtr type = TypePtr::dynamicCast($1);
    IntergerGrammarPtr iPtrSize = IntergerGrammarPtr::dynamicCast($3);
    g_parse->checkArrayVaid(type,iPtrSize->v);
    type->setArray(iPtrSize->v);
    $$ = type;
}
| type_no
{
    $$ = $1;
}
| type_no ':' error
{
   g_parse->error("array missing size");
}
;

// 没有默认大小的类型------------------------------------------------------------------
type_no
// ----------------------------------------------------------------------
: TARS_BOOL
{
    $$ = g_parse->createBuiltin(Builtin::KindBool);
}
| TARS_BYTE
{
    $$ = g_parse->createBuiltin(Builtin::KindByte);
}
| TARS_UNSIGNED TARS_BYTE //unsigned char 对应 short
{
    $$ = g_parse->createBuiltin(Builtin::KindShort,true);
}
| TARS_SHORT
{
    $$ = g_parse->createBuiltin(Builtin::KindShort);
}
| TARS_UNSIGNED TARS_SHORT
{
    $$ = g_parse->createBuiltin(Builtin::KindInt,true);
}
| TARS_INT
{
    $$ = g_parse->createBuiltin(Builtin::KindInt);
}
| TARS_UNSIGNED TARS_INT
{
    $$ = g_parse->createBuiltin(Builtin::KindLong,true);
}
| TARS_LONG
{
    $$ = g_parse->createBuiltin(Builtin::KindLong);
}
| TARS_FLOAT
{
    $$ = g_parse->createBuiltin(Builtin::KindFloat);
}
| TARS_DOUBLE
{
    $$ = g_parse->createBuiltin(Builtin::KindDouble);
}
| TARS_STRING
{
    $$ = g_parse->createBuiltin(Builtin::KindString);
}
| vector
{
   $$ = GrammarBasePtr::dynamicCast($1);
}
| map
{
   $$ = GrammarBasePtr::dynamicCast($1);
}
| scoped_name
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($1);
    TypePtr sp = g_parse->findUserType(ident->v);
    if(sp)
    {
        $$ = GrammarBasePtr::dynamicCast(sp);
    }
    else
    {
        g_parse->error("'" + ident->v + "' undefined!");
    }
}
;

// 关键字----------------------------------------------------------------
vector
// ----------------------------------------------------------------------
: TARS_VECTOR '<' type '>'
{
   $$ = GrammarBasePtr::dynamicCast(g_parse->createVector(TypePtr::dynamicCast($3)));
}
| TARS_VECTOR '<' error
{
   g_parse->error("vector error");
}
| TARS_VECTOR '<' type error
{
   g_parse->error("vector missing '>'");
}
| TARS_VECTOR error
{
   g_parse->error("vector missing type");
}
;

// 关键字----------------------------------------------------------------
map
// ----------------------------------------------------------------------
: TARS_MAP '<' type ',' type '>'
{
   $$ = GrammarBasePtr::dynamicCast(g_parse->createMap(TypePtr::dynamicCast($3), TypePtr::dynamicCast($5)));
}
| TARS_MAP '<' error
{
   g_parse->error("map error");
}
;

// ----------------------------------------------------------------------
scoped_name
// ----------------------------------------------------------------------
: TARS_IDENTIFIER
{
}
| TARS_SCOPE_DELIMITER TARS_IDENTIFIER
{
    StringGrammarPtr ident = StringGrammarPtr::dynamicCast($2);
    ident->v = "::" + ident->v;
    $$ = GrammarBasePtr::dynamicCast(ident);
}
| scoped_name TARS_SCOPE_DELIMITER TARS_IDENTIFIER
{
    StringGrammarPtr scoped = StringGrammarPtr::dynamicCast($1);
    StringGrammarPtr ident  = StringGrammarPtr::dynamicCast($3);
    scoped->v += "::";
    scoped->v += ident->v;
    $$ = GrammarBasePtr::dynamicCast(scoped);
}
;

// 关键字----------------------------------------------------------------
keyword
// ----------------------------------------------------------------------
: TARS_STRUCT
{
}
| TARS_VOID
{
}
| TARS_BOOL
{
}
| TARS_BYTE
{
}
| TARS_SHORT
{
}
| TARS_INT
{
}
| TARS_FLOAT
{
}
| TARS_DOUBLE
{
}
| TARS_STRING
{
}
| TARS_VECTOR
{
}
| TARS_KEY
{
}
| TARS_MAP
{
}
| TARS_NAMESPACE
{
}
| TARS_INTERFACE
{
}
| TARS_OUT
{
}
| TARS_REQUIRE
{
}
| TARS_OPTIONAL
{
}
| TARS_CONST_INTEGER
{
}
| TARS_CONST_FLOAT
{
}
| TARS_FALSE
{
}
| TARS_TRUE
{
}
| TARS_STRING_LITERAL
{
}
| TARS_CONST
{
}
| TARS_ENUM
{
}
| TARS_UNSIGNED
{
}
;

%%


