/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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
package com.qq.tars.maven.parse;
import com.qq.tars.maven.parse.ast.*;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

import org.antlr.runtime.tree.*;


@SuppressWarnings("all")
public class TarsParser extends Parser {
	public static final String[] tokenNames = new String[] {
		"<invalid>", "<EOR>", "<DOWN>", "<UP>", "COLON", "COMMA", "COMMENT", "DOT", 
		"EQ", "ESC_SEQ", "GT", "HEX_DIGIT", "LBRACE", "LBRACKET", "LPAREN", "LT", 
		"OCTAL_ESC", "QUOTE", "RBRACE", "RBRACKET", "RPAREN", "SEMI", "TARS_BOOL", 
		"TARS_BYTE", "TARS_CONST", "TARS_DOUBLE", "TARS_ENUM", "TARS_FALSE", "TARS_FLOAT", 
		"TARS_FLOATING_POINT_LITERAL", "TARS_IDENTIFIER", "TARS_INCLUDE", "TARS_INT", 
		"TARS_INTEGER_LITERAL", "TARS_INTERFACE", "TARS_KEY", "TARS_LONG", "TARS_MAP", 
		"TARS_NAMESPACE", "TARS_OPTIONAL", "TARS_OUT", "TARS_REQUIRE", "TARS_ROUTE_KEY", 
		"TARS_SHORT", "TARS_STRING", "TARS_STRING_LITERAL", "TARS_STRUCT", "TARS_TRUE", 
		"TARS_UNSIGNED", "TARS_VECTOR", "TARS_VOID", "UNICODE_ESC", "WS", "TARS_OPERATION", 
		"TARS_PARAM", "TARS_REF", "TARS_ROOT", "TARS_STRUCT_MEMBER"
	};
	public static final int EOF=-1;
	public static final int COLON=4;
	public static final int COMMA=5;
	public static final int COMMENT=6;
	public static final int DOT=7;
	public static final int EQ=8;
	public static final int ESC_SEQ=9;
	public static final int GT=10;
	public static final int HEX_DIGIT=11;
	public static final int LBRACE=12;
	public static final int LBRACKET=13;
	public static final int LPAREN=14;
	public static final int LT=15;
	public static final int OCTAL_ESC=16;
	public static final int QUOTE=17;
	public static final int RBRACE=18;
	public static final int RBRACKET=19;
	public static final int RPAREN=20;
	public static final int SEMI=21;
	public static final int TARS_BOOL=22;
	public static final int TARS_BYTE=23;
	public static final int TARS_CONST=24;
	public static final int TARS_DOUBLE=25;
	public static final int TARS_ENUM=26;
	public static final int TARS_FALSE=27;
	public static final int TARS_FLOAT=28;
	public static final int TARS_FLOATING_POINT_LITERAL=29;
	public static final int TARS_IDENTIFIER=30;
	public static final int TARS_INCLUDE=31;
	public static final int TARS_INT=32;
	public static final int TARS_INTEGER_LITERAL=33;
	public static final int TARS_INTERFACE=34;
	public static final int TARS_KEY=35;
	public static final int TARS_LONG=36;
	public static final int TARS_MAP=37;
	public static final int TARS_NAMESPACE=38;
	public static final int TARS_OPTIONAL=39;
	public static final int TARS_OUT=40;
	public static final int TARS_REQUIRE=41;
	public static final int TARS_ROUTE_KEY=42;
	public static final int TARS_SHORT=43;
	public static final int TARS_STRING=44;
	public static final int TARS_STRING_LITERAL=45;
	public static final int TARS_STRUCT=46;
	public static final int TARS_TRUE=47;
	public static final int TARS_UNSIGNED=48;
	public static final int TARS_VECTOR=49;
	public static final int TARS_VOID=50;
	public static final int UNICODE_ESC=51;
	public static final int WS=52;
	public static final int TARS_OPERATION=53;
	public static final int TARS_PARAM=54;
	public static final int TARS_REF=55;
	public static final int TARS_ROOT=56;
	public static final int TARS_STRUCT_MEMBER=57;

	// delegates
	public Parser[] getDelegates() {
		return new Parser[] {};
	}

	// delegators


	public TarsParser(TokenStream input) {
		this(input, new RecognizerSharedState());
	}
	public TarsParser(TokenStream input, RecognizerSharedState state) {
		super(input, state);
	}

	protected TreeAdaptor adaptor = new CommonTreeAdaptor();

	public void setTreeAdaptor(TreeAdaptor adaptor) {
		this.adaptor = adaptor;
	}
	public TreeAdaptor getTreeAdaptor() {
		return adaptor;
	}
	@Override public String[] getTokenNames() { return TarsParser.tokenNames; }
	@Override public String getGrammarFileName() { return "TarsParser.g"; }


	public static class start_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "start"
	// TarsParser.g:22:1: start : ( include_def )* ( namespace_def )+ -> ^( TARS_ROOT ( include_def )* ( namespace_def )+ ) ;
	public final TarsParser.start_return start() throws RecognitionException {
		TarsParser.start_return retval = new TarsParser.start_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		ParserRuleReturnScope include_def1 =null;
		ParserRuleReturnScope namespace_def2 =null;

		RewriteRuleSubtreeStream stream_include_def=new RewriteRuleSubtreeStream(adaptor,"rule include_def");
		RewriteRuleSubtreeStream stream_namespace_def=new RewriteRuleSubtreeStream(adaptor,"rule namespace_def");

		try {
			// TarsParser.g:23:2: ( ( include_def )* ( namespace_def )+ -> ^( TARS_ROOT ( include_def )* ( namespace_def )+ ) )
			// TarsParser.g:23:4: ( include_def )* ( namespace_def )+
			{
			// TarsParser.g:23:4: ( include_def )*
			loop1:
			while (true) {
				int alt1=2;
				int LA1_0 = input.LA(1);
				if ( (LA1_0==TARS_INCLUDE) ) {
					alt1=1;
				}

				switch (alt1) {
				case 1 :
					// TarsParser.g:23:4: include_def
					{
					pushFollow(FOLLOW_include_def_in_start70);
					include_def1=include_def();
					state._fsp--;

					stream_include_def.add(include_def1.getTree());
					}
					break;

				default :
					break loop1;
				}
			}

			// TarsParser.g:23:17: ( namespace_def )+
			int cnt2=0;
			loop2:
			while (true) {
				int alt2=2;
				int LA2_0 = input.LA(1);
				if ( (LA2_0==TARS_NAMESPACE) ) {
					alt2=1;
				}

				switch (alt2) {
				case 1 :
					// TarsParser.g:23:17: namespace_def
					{
					pushFollow(FOLLOW_namespace_def_in_start73);
					namespace_def2=namespace_def();
					state._fsp--;

					stream_namespace_def.add(namespace_def2.getTree());
					}
					break;

				default :
					if ( cnt2 >= 1 ) break loop2;
					EarlyExitException eee = new EarlyExitException(2, input);
					throw eee;
				}
				cnt2++;
			}

			// AST REWRITE
			// elements: include_def, namespace_def
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 24:3: -> ^( TARS_ROOT ( include_def )* ( namespace_def )+ )
			{
				// TarsParser.g:24:6: ^( TARS_ROOT ( include_def )* ( namespace_def )+ )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsRoot(TARS_ROOT), root_1);
				// TarsParser.g:24:28: ( include_def )*
				while ( stream_include_def.hasNext() ) {
					adaptor.addChild(root_1, stream_include_def.nextTree());
				}
				stream_include_def.reset();

				if ( !(stream_namespace_def.hasNext()) ) {
					throw new RewriteEarlyExitException();
				}
				while ( stream_namespace_def.hasNext() ) {
					adaptor.addChild(root_1, stream_namespace_def.nextTree());
				}
				stream_namespace_def.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "start"


	public static class include_def_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "include_def"
	// TarsParser.g:27:1: include_def : TARS_INCLUDE TARS_STRING_LITERAL -> ^( TARS_INCLUDE[$TARS_STRING_LITERAL.text] ) ;
	public final TarsParser.include_def_return include_def() throws RecognitionException {
		TarsParser.include_def_return retval = new TarsParser.include_def_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_INCLUDE3=null;
		Token TARS_STRING_LITERAL4=null;

		Object TARS_INCLUDE3_tree=null;
		Object TARS_STRING_LITERAL4_tree=null;
		RewriteRuleTokenStream stream_TARS_STRING_LITERAL=new RewriteRuleTokenStream(adaptor,"token TARS_STRING_LITERAL");
		RewriteRuleTokenStream stream_TARS_INCLUDE=new RewriteRuleTokenStream(adaptor,"token TARS_INCLUDE");

		try {
			// TarsParser.g:28:2: ( TARS_INCLUDE TARS_STRING_LITERAL -> ^( TARS_INCLUDE[$TARS_STRING_LITERAL.text] ) )
			// TarsParser.g:28:4: TARS_INCLUDE TARS_STRING_LITERAL
			{
			TARS_INCLUDE3=(Token)match(input,TARS_INCLUDE,FOLLOW_TARS_INCLUDE_in_include_def102);  
			stream_TARS_INCLUDE.add(TARS_INCLUDE3);

			TARS_STRING_LITERAL4=(Token)match(input,TARS_STRING_LITERAL,FOLLOW_TARS_STRING_LITERAL_in_include_def104);  
			stream_TARS_STRING_LITERAL.add(TARS_STRING_LITERAL4);

			// AST REWRITE
			// elements: TARS_INCLUDE
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 29:3: -> ^( TARS_INCLUDE[$TARS_STRING_LITERAL.text] )
			{
				// TarsParser.g:29:6: ^( TARS_INCLUDE[$TARS_STRING_LITERAL.text] )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsInclude(TARS_INCLUDE, (TARS_STRING_LITERAL4!=null?TARS_STRING_LITERAL4.getText():null)), root_1);
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "include_def"


	public static class namespace_def_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "namespace_def"
	// TarsParser.g:32:1: namespace_def : TARS_NAMESPACE TARS_IDENTIFIER LBRACE ( definition SEMI )+ RBRACE -> ^( TARS_NAMESPACE[$TARS_IDENTIFIER.text] ( definition )+ ) ;
	public final TarsParser.namespace_def_return namespace_def() throws RecognitionException {
		TarsParser.namespace_def_return retval = new TarsParser.namespace_def_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_NAMESPACE5=null;
		Token TARS_IDENTIFIER6=null;
		Token LBRACE7=null;
		Token SEMI9=null;
		Token RBRACE10=null;
		ParserRuleReturnScope definition8 =null;

		Object TARS_NAMESPACE5_tree=null;
		Object TARS_IDENTIFIER6_tree=null;
		Object LBRACE7_tree=null;
		Object SEMI9_tree=null;
		Object RBRACE10_tree=null;
		RewriteRuleTokenStream stream_TARS_NAMESPACE=new RewriteRuleTokenStream(adaptor,"token TARS_NAMESPACE");
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_RBRACE=new RewriteRuleTokenStream(adaptor,"token RBRACE");
		RewriteRuleTokenStream stream_SEMI=new RewriteRuleTokenStream(adaptor,"token SEMI");
		RewriteRuleTokenStream stream_LBRACE=new RewriteRuleTokenStream(adaptor,"token LBRACE");
		RewriteRuleSubtreeStream stream_definition=new RewriteRuleSubtreeStream(adaptor,"rule definition");

		try {
			// TarsParser.g:33:2: ( TARS_NAMESPACE TARS_IDENTIFIER LBRACE ( definition SEMI )+ RBRACE -> ^( TARS_NAMESPACE[$TARS_IDENTIFIER.text] ( definition )+ ) )
			// TarsParser.g:33:4: TARS_NAMESPACE TARS_IDENTIFIER LBRACE ( definition SEMI )+ RBRACE
			{
			TARS_NAMESPACE5=(Token)match(input,TARS_NAMESPACE,FOLLOW_TARS_NAMESPACE_in_namespace_def127);  
			stream_TARS_NAMESPACE.add(TARS_NAMESPACE5);

			TARS_IDENTIFIER6=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_namespace_def129);  
			stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER6);

			LBRACE7=(Token)match(input,LBRACE,FOLLOW_LBRACE_in_namespace_def131);  
			stream_LBRACE.add(LBRACE7);

			// TarsParser.g:33:42: ( definition SEMI )+
			int cnt3=0;
			loop3:
			while (true) {
				int alt3=2;
				int LA3_0 = input.LA(1);
				if ( (LA3_0==TARS_CONST||LA3_0==TARS_ENUM||(LA3_0 >= TARS_INTERFACE && LA3_0 <= TARS_KEY)||LA3_0==TARS_STRUCT) ) {
					alt3=1;
				}

				switch (alt3) {
				case 1 :
					// TarsParser.g:33:43: definition SEMI
					{
					pushFollow(FOLLOW_definition_in_namespace_def134);
					definition8=definition();
					state._fsp--;

					stream_definition.add(definition8.getTree());
					SEMI9=(Token)match(input,SEMI,FOLLOW_SEMI_in_namespace_def136);  
					stream_SEMI.add(SEMI9);

					}
					break;

				default :
					if ( cnt3 >= 1 ) break loop3;
					EarlyExitException eee = new EarlyExitException(3, input);
					throw eee;
				}
				cnt3++;
			}

			RBRACE10=(Token)match(input,RBRACE,FOLLOW_RBRACE_in_namespace_def140);  
			stream_RBRACE.add(RBRACE10);

			// AST REWRITE
			// elements: definition, TARS_NAMESPACE
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 34:3: -> ^( TARS_NAMESPACE[$TARS_IDENTIFIER.text] ( definition )+ )
			{
				// TarsParser.g:34:6: ^( TARS_NAMESPACE[$TARS_IDENTIFIER.text] ( definition )+ )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsNamespace(TARS_NAMESPACE, (TARS_IDENTIFIER6!=null?TARS_IDENTIFIER6.getText():null)), root_1);
				if ( !(stream_definition.hasNext()) ) {
					throw new RewriteEarlyExitException();
				}
				while ( stream_definition.hasNext() ) {
					adaptor.addChild(root_1, stream_definition.nextTree());
				}
				stream_definition.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "namespace_def"


	public static class definition_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "definition"
	// TarsParser.g:37:1: definition : ( const_def | enum_def | struct_def | key_def | interface_def );
	public final TarsParser.definition_return definition() throws RecognitionException {
		TarsParser.definition_return retval = new TarsParser.definition_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		ParserRuleReturnScope const_def11 =null;
		ParserRuleReturnScope enum_def12 =null;
		ParserRuleReturnScope struct_def13 =null;
		ParserRuleReturnScope key_def14 =null;
		ParserRuleReturnScope interface_def15 =null;


		try {
			// TarsParser.g:38:2: ( const_def | enum_def | struct_def | key_def | interface_def )
			int alt4=5;
			switch ( input.LA(1) ) {
			case TARS_CONST:
				{
				alt4=1;
				}
				break;
			case TARS_ENUM:
				{
				alt4=2;
				}
				break;
			case TARS_STRUCT:
				{
				alt4=3;
				}
				break;
			case TARS_KEY:
				{
				alt4=4;
				}
				break;
			case TARS_INTERFACE:
				{
				alt4=5;
				}
				break;
			default:
				NoViableAltException nvae =
					new NoViableAltException("", 4, 0, input);
				throw nvae;
			}
			switch (alt4) {
				case 1 :
					// TarsParser.g:38:4: const_def
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_const_def_in_definition167);
					const_def11=const_def();
					state._fsp--;

					adaptor.addChild(root_0, const_def11.getTree());

					}
					break;
				case 2 :
					// TarsParser.g:39:6: enum_def
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_enum_def_in_definition174);
					enum_def12=enum_def();
					state._fsp--;

					adaptor.addChild(root_0, enum_def12.getTree());

					}
					break;
				case 3 :
					// TarsParser.g:40:4: struct_def
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_struct_def_in_definition179);
					struct_def13=struct_def();
					state._fsp--;

					adaptor.addChild(root_0, struct_def13.getTree());

					}
					break;
				case 4 :
					// TarsParser.g:41:4: key_def
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_key_def_in_definition184);
					key_def14=key_def();
					state._fsp--;

					adaptor.addChild(root_0, key_def14.getTree());

					}
					break;
				case 5 :
					// TarsParser.g:42:4: interface_def
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_interface_def_in_definition189);
					interface_def15=interface_def();
					state._fsp--;

					adaptor.addChild(root_0, interface_def15.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "definition"


	public static class const_def_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "const_def"
	// TarsParser.g:45:1: const_def : TARS_CONST type_primitive TARS_IDENTIFIER EQ v= const_initializer -> ^( TARS_CONST[$TARS_IDENTIFIER.text, $v.text] type_primitive ) ;
	public final TarsParser.const_def_return const_def() throws RecognitionException {
		TarsParser.const_def_return retval = new TarsParser.const_def_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_CONST16=null;
		Token TARS_IDENTIFIER18=null;
		Token EQ19=null;
		ParserRuleReturnScope v =null;
		ParserRuleReturnScope type_primitive17 =null;

		Object TARS_CONST16_tree=null;
		Object TARS_IDENTIFIER18_tree=null;
		Object EQ19_tree=null;
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_EQ=new RewriteRuleTokenStream(adaptor,"token EQ");
		RewriteRuleTokenStream stream_TARS_CONST=new RewriteRuleTokenStream(adaptor,"token TARS_CONST");
		RewriteRuleSubtreeStream stream_type_primitive=new RewriteRuleSubtreeStream(adaptor,"rule type_primitive");
		RewriteRuleSubtreeStream stream_const_initializer=new RewriteRuleSubtreeStream(adaptor,"rule const_initializer");

		try {
			// TarsParser.g:46:2: ( TARS_CONST type_primitive TARS_IDENTIFIER EQ v= const_initializer -> ^( TARS_CONST[$TARS_IDENTIFIER.text, $v.text] type_primitive ) )
			// TarsParser.g:46:4: TARS_CONST type_primitive TARS_IDENTIFIER EQ v= const_initializer
			{
			TARS_CONST16=(Token)match(input,TARS_CONST,FOLLOW_TARS_CONST_in_const_def202);  
			stream_TARS_CONST.add(TARS_CONST16);

			pushFollow(FOLLOW_type_primitive_in_const_def204);
			type_primitive17=type_primitive();
			state._fsp--;

			stream_type_primitive.add(type_primitive17.getTree());
			TARS_IDENTIFIER18=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_const_def206);  
			stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER18);

			EQ19=(Token)match(input,EQ,FOLLOW_EQ_in_const_def208);  
			stream_EQ.add(EQ19);

			pushFollow(FOLLOW_const_initializer_in_const_def212);
			v=const_initializer();
			state._fsp--;

			stream_const_initializer.add(v.getTree());
			// AST REWRITE
			// elements: type_primitive, TARS_CONST
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 47:3: -> ^( TARS_CONST[$TARS_IDENTIFIER.text, $v.text] type_primitive )
			{
				// TarsParser.g:47:6: ^( TARS_CONST[$TARS_IDENTIFIER.text, $v.text] type_primitive )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsConst(TARS_CONST, (TARS_IDENTIFIER18!=null?TARS_IDENTIFIER18.getText():null), (v!=null?input.toString(v.start,v.stop):null)), root_1);
				adaptor.addChild(root_1, stream_type_primitive.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "const_def"


	public static class enum_def_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "enum_def"
	// TarsParser.g:50:1: enum_def : ( TARS_ENUM n= TARS_IDENTIFIER LBRACE m+= TARS_IDENTIFIER ( COMMA m+= TARS_IDENTIFIER )* ( COMMA )? RBRACE -> ^( TARS_ENUM[$n.text] ( $m)+ ) | TARS_ENUM n= TARS_IDENTIFIER LBRACE m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL ( COMMA m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL )* ( COMMA )? RBRACE -> ^( TARS_ENUM[$n.text] ( $m)+ ( $v)+ ) );
	public final TarsParser.enum_def_return enum_def() throws RecognitionException {
		TarsParser.enum_def_return retval = new TarsParser.enum_def_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token n=null;
		Token TARS_ENUM20=null;
		Token LBRACE21=null;
		Token COMMA22=null;
		Token COMMA23=null;
		Token RBRACE24=null;
		Token TARS_ENUM25=null;
		Token LBRACE26=null;
		Token EQ27=null;
		Token COMMA28=null;
		Token EQ29=null;
		Token COMMA30=null;
		Token RBRACE31=null;
		Token m=null;
		Token v=null;
		List<Object> list_m=null;
		List<Object> list_v=null;

		Object n_tree=null;
		Object TARS_ENUM20_tree=null;
		Object LBRACE21_tree=null;
		Object COMMA22_tree=null;
		Object COMMA23_tree=null;
		Object RBRACE24_tree=null;
		Object TARS_ENUM25_tree=null;
		Object LBRACE26_tree=null;
		Object EQ27_tree=null;
		Object COMMA28_tree=null;
		Object EQ29_tree=null;
		Object COMMA30_tree=null;
		Object RBRACE31_tree=null;
		Object m_tree=null;
		Object v_tree=null;
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_TARS_ENUM=new RewriteRuleTokenStream(adaptor,"token TARS_ENUM");
		RewriteRuleTokenStream stream_RBRACE=new RewriteRuleTokenStream(adaptor,"token RBRACE");
		RewriteRuleTokenStream stream_EQ=new RewriteRuleTokenStream(adaptor,"token EQ");
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleTokenStream stream_TARS_INTEGER_LITERAL=new RewriteRuleTokenStream(adaptor,"token TARS_INTEGER_LITERAL");
		RewriteRuleTokenStream stream_LBRACE=new RewriteRuleTokenStream(adaptor,"token LBRACE");

		try {
			// TarsParser.g:51:2: ( TARS_ENUM n= TARS_IDENTIFIER LBRACE m+= TARS_IDENTIFIER ( COMMA m+= TARS_IDENTIFIER )* ( COMMA )? RBRACE -> ^( TARS_ENUM[$n.text] ( $m)+ ) | TARS_ENUM n= TARS_IDENTIFIER LBRACE m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL ( COMMA m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL )* ( COMMA )? RBRACE -> ^( TARS_ENUM[$n.text] ( $m)+ ( $v)+ ) )
			int alt9=2;
			int LA9_0 = input.LA(1);
			if ( (LA9_0==TARS_ENUM) ) {
				int LA9_1 = input.LA(2);
				if ( (LA9_1==TARS_IDENTIFIER) ) {
					int LA9_2 = input.LA(3);
					if ( (LA9_2==LBRACE) ) {
						int LA9_3 = input.LA(4);
						if ( (LA9_3==TARS_IDENTIFIER) ) {
							int LA9_4 = input.LA(5);
							if ( (LA9_4==EQ) ) {
								alt9=2;
							}
							else if ( (LA9_4==COMMA||LA9_4==RBRACE) ) {
								alt9=1;
							}

							else {
								int nvaeMark = input.mark();
								try {
									for (int nvaeConsume = 0; nvaeConsume < 5 - 1; nvaeConsume++) {
										input.consume();
									}
									NoViableAltException nvae =
										new NoViableAltException("", 9, 4, input);
									throw nvae;
								} finally {
									input.rewind(nvaeMark);
								}
							}

						}

						else {
							int nvaeMark = input.mark();
							try {
								for (int nvaeConsume = 0; nvaeConsume < 4 - 1; nvaeConsume++) {
									input.consume();
								}
								NoViableAltException nvae =
									new NoViableAltException("", 9, 3, input);
								throw nvae;
							} finally {
								input.rewind(nvaeMark);
							}
						}

					}

					else {
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 9, 2, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 9, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				NoViableAltException nvae =
					new NoViableAltException("", 9, 0, input);
				throw nvae;
			}

			switch (alt9) {
				case 1 :
					// TarsParser.g:51:5: TARS_ENUM n= TARS_IDENTIFIER LBRACE m+= TARS_IDENTIFIER ( COMMA m+= TARS_IDENTIFIER )* ( COMMA )? RBRACE
					{
					TARS_ENUM20=(Token)match(input,TARS_ENUM,FOLLOW_TARS_ENUM_in_enum_def238);  
					stream_TARS_ENUM.add(TARS_ENUM20);

					n=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_enum_def242);  
					stream_TARS_IDENTIFIER.add(n);

					LBRACE21=(Token)match(input,LBRACE,FOLLOW_LBRACE_in_enum_def244);  
					stream_LBRACE.add(LBRACE21);

					m=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_enum_def248);  
					stream_TARS_IDENTIFIER.add(m);

					if (list_m==null) list_m=new ArrayList<Object>();
					list_m.add(m);
					// TarsParser.g:51:59: ( COMMA m+= TARS_IDENTIFIER )*
					loop5:
					while (true) {
						int alt5=2;
						int LA5_0 = input.LA(1);
						if ( (LA5_0==COMMA) ) {
							int LA5_1 = input.LA(2);
							if ( (LA5_1==TARS_IDENTIFIER) ) {
								alt5=1;
							}

						}

						switch (alt5) {
						case 1 :
							// TarsParser.g:51:60: COMMA m+= TARS_IDENTIFIER
							{
							COMMA22=(Token)match(input,COMMA,FOLLOW_COMMA_in_enum_def251);  
							stream_COMMA.add(COMMA22);

							m=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_enum_def255);  
							stream_TARS_IDENTIFIER.add(m);

							if (list_m==null) list_m=new ArrayList<Object>();
							list_m.add(m);
							}
							break;

						default :
							break loop5;
						}
					}

					// TarsParser.g:51:87: ( COMMA )?
					int alt6=2;
					int LA6_0 = input.LA(1);
					if ( (LA6_0==COMMA) ) {
						alt6=1;
					}
					switch (alt6) {
						case 1 :
							// TarsParser.g:51:87: COMMA
							{
							COMMA23=(Token)match(input,COMMA,FOLLOW_COMMA_in_enum_def259);  
							stream_COMMA.add(COMMA23);

							}
							break;

					}

					RBRACE24=(Token)match(input,RBRACE,FOLLOW_RBRACE_in_enum_def262);  
					stream_RBRACE.add(RBRACE24);

					// AST REWRITE
					// elements: m, TARS_ENUM
					// token labels: 
					// rule labels: retval
					// token list labels: m
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleTokenStream stream_m=new RewriteRuleTokenStream(adaptor,"token m", list_m);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 52:3: -> ^( TARS_ENUM[$n.text] ( $m)+ )
					{
						// TarsParser.g:52:6: ^( TARS_ENUM[$n.text] ( $m)+ )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsEnum(TARS_ENUM, (n!=null?n.getText():null)), root_1);
						if ( !(stream_m.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_m.hasNext() ) {
							adaptor.addChild(root_1, stream_m.nextNode());
						}
						stream_m.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 2 :
					// TarsParser.g:53:4: TARS_ENUM n= TARS_IDENTIFIER LBRACE m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL ( COMMA m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL )* ( COMMA )? RBRACE
					{
					TARS_ENUM25=(Token)match(input,TARS_ENUM,FOLLOW_TARS_ENUM_in_enum_def284);  
					stream_TARS_ENUM.add(TARS_ENUM25);

					n=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_enum_def288);  
					stream_TARS_IDENTIFIER.add(n);

					LBRACE26=(Token)match(input,LBRACE,FOLLOW_LBRACE_in_enum_def290);  
					stream_LBRACE.add(LBRACE26);

					m=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_enum_def294);  
					stream_TARS_IDENTIFIER.add(m);

					if (list_m==null) list_m=new ArrayList<Object>();
					list_m.add(m);
					EQ27=(Token)match(input,EQ,FOLLOW_EQ_in_enum_def296);  
					stream_EQ.add(EQ27);

					v=(Token)match(input,TARS_INTEGER_LITERAL,FOLLOW_TARS_INTEGER_LITERAL_in_enum_def300);  
					stream_TARS_INTEGER_LITERAL.add(v);

					if (list_v==null) list_v=new ArrayList<Object>();
					list_v.add(v);
					// TarsParser.g:53:85: ( COMMA m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL )*
					loop7:
					while (true) {
						int alt7=2;
						int LA7_0 = input.LA(1);
						if ( (LA7_0==COMMA) ) {
							int LA7_1 = input.LA(2);
							if ( (LA7_1==TARS_IDENTIFIER) ) {
								alt7=1;
							}

						}

						switch (alt7) {
						case 1 :
							// TarsParser.g:53:86: COMMA m+= TARS_IDENTIFIER EQ v+= TARS_INTEGER_LITERAL
							{
							COMMA28=(Token)match(input,COMMA,FOLLOW_COMMA_in_enum_def303);  
							stream_COMMA.add(COMMA28);

							m=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_enum_def307);  
							stream_TARS_IDENTIFIER.add(m);

							if (list_m==null) list_m=new ArrayList<Object>();
							list_m.add(m);
							EQ29=(Token)match(input,EQ,FOLLOW_EQ_in_enum_def309);  
							stream_EQ.add(EQ29);

							v=(Token)match(input,TARS_INTEGER_LITERAL,FOLLOW_TARS_INTEGER_LITERAL_in_enum_def313);  
							stream_TARS_INTEGER_LITERAL.add(v);

							if (list_v==null) list_v=new ArrayList<Object>();
							list_v.add(v);
							}
							break;

						default :
							break loop7;
						}
					}

					// TarsParser.g:53:140: ( COMMA )?
					int alt8=2;
					int LA8_0 = input.LA(1);
					if ( (LA8_0==COMMA) ) {
						alt8=1;
					}
					switch (alt8) {
						case 1 :
							// TarsParser.g:53:140: COMMA
							{
							COMMA30=(Token)match(input,COMMA,FOLLOW_COMMA_in_enum_def317);  
							stream_COMMA.add(COMMA30);

							}
							break;

					}

					RBRACE31=(Token)match(input,RBRACE,FOLLOW_RBRACE_in_enum_def320);  
					stream_RBRACE.add(RBRACE31);

					// AST REWRITE
					// elements: v, TARS_ENUM, m
					// token labels: 
					// rule labels: retval
					// token list labels: v, m
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleTokenStream stream_v=new RewriteRuleTokenStream(adaptor,"token v", list_v);
					RewriteRuleTokenStream stream_m=new RewriteRuleTokenStream(adaptor,"token m", list_m);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 54:3: -> ^( TARS_ENUM[$n.text] ( $m)+ ( $v)+ )
					{
						// TarsParser.g:54:6: ^( TARS_ENUM[$n.text] ( $m)+ ( $v)+ )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsEnum(TARS_ENUM, (n!=null?n.getText():null)), root_1);
						if ( !(stream_m.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_m.hasNext() ) {
							adaptor.addChild(root_1, stream_m.nextNode());
						}
						stream_m.reset();

						if ( !(stream_v.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_v.hasNext() ) {
							adaptor.addChild(root_1, stream_v.nextNode());
						}
						stream_v.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;

			}
			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "enum_def"


	public static class struct_def_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "struct_def"
	// TarsParser.g:57:1: struct_def : TARS_STRUCT TARS_IDENTIFIER LBRACE ( struct_member SEMI )+ RBRACE -> ^( TARS_STRUCT[$TARS_IDENTIFIER.text] ( struct_member )+ ) ;
	public final TarsParser.struct_def_return struct_def() throws RecognitionException {
		TarsParser.struct_def_return retval = new TarsParser.struct_def_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_STRUCT32=null;
		Token TARS_IDENTIFIER33=null;
		Token LBRACE34=null;
		Token SEMI36=null;
		Token RBRACE37=null;
		ParserRuleReturnScope struct_member35 =null;

		Object TARS_STRUCT32_tree=null;
		Object TARS_IDENTIFIER33_tree=null;
		Object LBRACE34_tree=null;
		Object SEMI36_tree=null;
		Object RBRACE37_tree=null;
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_RBRACE=new RewriteRuleTokenStream(adaptor,"token RBRACE");
		RewriteRuleTokenStream stream_SEMI=new RewriteRuleTokenStream(adaptor,"token SEMI");
		RewriteRuleTokenStream stream_LBRACE=new RewriteRuleTokenStream(adaptor,"token LBRACE");
		RewriteRuleTokenStream stream_TARS_STRUCT=new RewriteRuleTokenStream(adaptor,"token TARS_STRUCT");
		RewriteRuleSubtreeStream stream_struct_member=new RewriteRuleSubtreeStream(adaptor,"rule struct_member");

		try {
			// TarsParser.g:58:2: ( TARS_STRUCT TARS_IDENTIFIER LBRACE ( struct_member SEMI )+ RBRACE -> ^( TARS_STRUCT[$TARS_IDENTIFIER.text] ( struct_member )+ ) )
			// TarsParser.g:58:4: TARS_STRUCT TARS_IDENTIFIER LBRACE ( struct_member SEMI )+ RBRACE
			{
			TARS_STRUCT32=(Token)match(input,TARS_STRUCT,FOLLOW_TARS_STRUCT_in_struct_def351);  
			stream_TARS_STRUCT.add(TARS_STRUCT32);

			TARS_IDENTIFIER33=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_struct_def353);  
			stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER33);

			LBRACE34=(Token)match(input,LBRACE,FOLLOW_LBRACE_in_struct_def355);  
			stream_LBRACE.add(LBRACE34);

			// TarsParser.g:58:39: ( struct_member SEMI )+
			int cnt10=0;
			loop10:
			while (true) {
				int alt10=2;
				int LA10_0 = input.LA(1);
				if ( (LA10_0==TARS_INTEGER_LITERAL) ) {
					alt10=1;
				}

				switch (alt10) {
				case 1 :
					// TarsParser.g:58:40: struct_member SEMI
					{
					pushFollow(FOLLOW_struct_member_in_struct_def358);
					struct_member35=struct_member();
					state._fsp--;

					stream_struct_member.add(struct_member35.getTree());
					SEMI36=(Token)match(input,SEMI,FOLLOW_SEMI_in_struct_def360);  
					stream_SEMI.add(SEMI36);

					}
					break;

				default :
					if ( cnt10 >= 1 ) break loop10;
					EarlyExitException eee = new EarlyExitException(10, input);
					throw eee;
				}
				cnt10++;
			}

			RBRACE37=(Token)match(input,RBRACE,FOLLOW_RBRACE_in_struct_def364);  
			stream_RBRACE.add(RBRACE37);

			// AST REWRITE
			// elements: TARS_STRUCT, struct_member
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 59:3: -> ^( TARS_STRUCT[$TARS_IDENTIFIER.text] ( struct_member )+ )
			{
				// TarsParser.g:59:6: ^( TARS_STRUCT[$TARS_IDENTIFIER.text] ( struct_member )+ )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsStruct(TARS_STRUCT, (TARS_IDENTIFIER33!=null?TARS_IDENTIFIER33.getText():null)), root_1);
				if ( !(stream_struct_member.hasNext()) ) {
					throw new RewriteEarlyExitException();
				}
				while ( stream_struct_member.hasNext() ) {
					adaptor.addChild(root_1, stream_struct_member.nextTree());
				}
				stream_struct_member.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "struct_def"


	public static class struct_member_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "struct_member"
	// TarsParser.g:62:1: struct_member : TARS_INTEGER_LITERAL (r= TARS_REQUIRE |r= TARS_OPTIONAL ) type TARS_IDENTIFIER ( EQ v= const_initializer )? -> ^( TARS_STRUCT_MEMBER[$TARS_INTEGER_LITERAL.text, $r, $TARS_IDENTIFIER.text, $v.result] type ) ;
	public final TarsParser.struct_member_return struct_member() throws RecognitionException {
		TarsParser.struct_member_return retval = new TarsParser.struct_member_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token r=null;
		Token TARS_INTEGER_LITERAL38=null;
		Token TARS_IDENTIFIER40=null;
		Token EQ41=null;
		ParserRuleReturnScope v =null;
		ParserRuleReturnScope type39 =null;

		Object r_tree=null;
		Object TARS_INTEGER_LITERAL38_tree=null;
		Object TARS_IDENTIFIER40_tree=null;
		Object EQ41_tree=null;
		RewriteRuleTokenStream stream_TARS_OPTIONAL=new RewriteRuleTokenStream(adaptor,"token TARS_OPTIONAL");
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_EQ=new RewriteRuleTokenStream(adaptor,"token EQ");
		RewriteRuleTokenStream stream_TARS_REQUIRE=new RewriteRuleTokenStream(adaptor,"token TARS_REQUIRE");
		RewriteRuleTokenStream stream_TARS_INTEGER_LITERAL=new RewriteRuleTokenStream(adaptor,"token TARS_INTEGER_LITERAL");
		RewriteRuleSubtreeStream stream_type=new RewriteRuleSubtreeStream(adaptor,"rule type");
		RewriteRuleSubtreeStream stream_const_initializer=new RewriteRuleSubtreeStream(adaptor,"rule const_initializer");

		try {
			// TarsParser.g:63:2: ( TARS_INTEGER_LITERAL (r= TARS_REQUIRE |r= TARS_OPTIONAL ) type TARS_IDENTIFIER ( EQ v= const_initializer )? -> ^( TARS_STRUCT_MEMBER[$TARS_INTEGER_LITERAL.text, $r, $TARS_IDENTIFIER.text, $v.result] type ) )
			// TarsParser.g:63:4: TARS_INTEGER_LITERAL (r= TARS_REQUIRE |r= TARS_OPTIONAL ) type TARS_IDENTIFIER ( EQ v= const_initializer )?
			{
			TARS_INTEGER_LITERAL38=(Token)match(input,TARS_INTEGER_LITERAL,FOLLOW_TARS_INTEGER_LITERAL_in_struct_member391);  
			stream_TARS_INTEGER_LITERAL.add(TARS_INTEGER_LITERAL38);

			// TarsParser.g:63:25: (r= TARS_REQUIRE |r= TARS_OPTIONAL )
			int alt11=2;
			int LA11_0 = input.LA(1);
			if ( (LA11_0==TARS_REQUIRE) ) {
				alt11=1;
			}
			else if ( (LA11_0==TARS_OPTIONAL) ) {
				alt11=2;
			}

			else {
				NoViableAltException nvae =
					new NoViableAltException("", 11, 0, input);
				throw nvae;
			}

			switch (alt11) {
				case 1 :
					// TarsParser.g:63:26: r= TARS_REQUIRE
					{
					r=(Token)match(input,TARS_REQUIRE,FOLLOW_TARS_REQUIRE_in_struct_member396);  
					stream_TARS_REQUIRE.add(r);

					}
					break;
				case 2 :
					// TarsParser.g:63:43: r= TARS_OPTIONAL
					{
					r=(Token)match(input,TARS_OPTIONAL,FOLLOW_TARS_OPTIONAL_in_struct_member402);  
					stream_TARS_OPTIONAL.add(r);

					}
					break;

			}

			pushFollow(FOLLOW_type_in_struct_member405);
			type39=type();
			state._fsp--;

			stream_type.add(type39.getTree());
			TARS_IDENTIFIER40=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_struct_member407);  
			stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER40);

			// TarsParser.g:63:81: ( EQ v= const_initializer )?
			int alt12=2;
			int LA12_0 = input.LA(1);
			if ( (LA12_0==EQ) ) {
				alt12=1;
			}
			switch (alt12) {
				case 1 :
					// TarsParser.g:63:82: EQ v= const_initializer
					{
					EQ41=(Token)match(input,EQ,FOLLOW_EQ_in_struct_member410);  
					stream_EQ.add(EQ41);

					pushFollow(FOLLOW_const_initializer_in_struct_member414);
					v=const_initializer();
					state._fsp--;

					stream_const_initializer.add(v.getTree());
					}
					break;

			}

			// AST REWRITE
			// elements: type
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 64:3: -> ^( TARS_STRUCT_MEMBER[$TARS_INTEGER_LITERAL.text, $r, $TARS_IDENTIFIER.text, $v.result] type )
			{
				// TarsParser.g:64:6: ^( TARS_STRUCT_MEMBER[$TARS_INTEGER_LITERAL.text, $r, $TARS_IDENTIFIER.text, $v.result] type )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsStructMember(TARS_STRUCT_MEMBER, (TARS_INTEGER_LITERAL38!=null?TARS_INTEGER_LITERAL38.getText():null), r, (TARS_IDENTIFIER40!=null?TARS_IDENTIFIER40.getText():null), (v!=null?((TarsParser.const_initializer_return)v).result:null)), root_1);
				adaptor.addChild(root_1, stream_type.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "struct_member"


	public static class key_def_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "key_def"
	// TarsParser.g:67:1: key_def : TARS_KEY LBRACKET n= TARS_IDENTIFIER ( COMMA k+= TARS_IDENTIFIER )+ RBRACKET -> ^( TARS_KEY[$n.text] ( $k)+ ) ;
	public final TarsParser.key_def_return key_def() throws RecognitionException {
		TarsParser.key_def_return retval = new TarsParser.key_def_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token n=null;
		Token TARS_KEY42=null;
		Token LBRACKET43=null;
		Token COMMA44=null;
		Token RBRACKET45=null;
		Token k=null;
		List<Object> list_k=null;

		Object n_tree=null;
		Object TARS_KEY42_tree=null;
		Object LBRACKET43_tree=null;
		Object COMMA44_tree=null;
		Object RBRACKET45_tree=null;
		Object k_tree=null;
		RewriteRuleTokenStream stream_LBRACKET=new RewriteRuleTokenStream(adaptor,"token LBRACKET");
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_RBRACKET=new RewriteRuleTokenStream(adaptor,"token RBRACKET");
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleTokenStream stream_TARS_KEY=new RewriteRuleTokenStream(adaptor,"token TARS_KEY");

		try {
			// TarsParser.g:68:2: ( TARS_KEY LBRACKET n= TARS_IDENTIFIER ( COMMA k+= TARS_IDENTIFIER )+ RBRACKET -> ^( TARS_KEY[$n.text] ( $k)+ ) )
			// TarsParser.g:68:5: TARS_KEY LBRACKET n= TARS_IDENTIFIER ( COMMA k+= TARS_IDENTIFIER )+ RBRACKET
			{
			TARS_KEY42=(Token)match(input,TARS_KEY,FOLLOW_TARS_KEY_in_key_def443);  
			stream_TARS_KEY.add(TARS_KEY42);

			LBRACKET43=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_key_def445);  
			stream_LBRACKET.add(LBRACKET43);

			n=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_key_def449);  
			stream_TARS_IDENTIFIER.add(n);

			// TarsParser.g:68:41: ( COMMA k+= TARS_IDENTIFIER )+
			int cnt13=0;
			loop13:
			while (true) {
				int alt13=2;
				int LA13_0 = input.LA(1);
				if ( (LA13_0==COMMA) ) {
					alt13=1;
				}

				switch (alt13) {
				case 1 :
					// TarsParser.g:68:42: COMMA k+= TARS_IDENTIFIER
					{
					COMMA44=(Token)match(input,COMMA,FOLLOW_COMMA_in_key_def452);  
					stream_COMMA.add(COMMA44);

					k=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_key_def456);  
					stream_TARS_IDENTIFIER.add(k);

					if (list_k==null) list_k=new ArrayList<Object>();
					list_k.add(k);
					}
					break;

				default :
					if ( cnt13 >= 1 ) break loop13;
					EarlyExitException eee = new EarlyExitException(13, input);
					throw eee;
				}
				cnt13++;
			}

			RBRACKET45=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_key_def460);  
			stream_RBRACKET.add(RBRACKET45);

			// AST REWRITE
			// elements: TARS_KEY, k
			// token labels: 
			// rule labels: retval
			// token list labels: k
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleTokenStream stream_k=new RewriteRuleTokenStream(adaptor,"token k", list_k);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 69:3: -> ^( TARS_KEY[$n.text] ( $k)+ )
			{
				// TarsParser.g:69:6: ^( TARS_KEY[$n.text] ( $k)+ )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsKey(TARS_KEY, (n!=null?n.getText():null)), root_1);
				if ( !(stream_k.hasNext()) ) {
					throw new RewriteEarlyExitException();
				}
				while ( stream_k.hasNext() ) {
					adaptor.addChild(root_1, stream_k.nextNode());
				}
				stream_k.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "key_def"


	public static class interface_def_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "interface_def"
	// TarsParser.g:72:1: interface_def : TARS_INTERFACE TARS_IDENTIFIER LBRACE ( operation SEMI )+ RBRACE -> ^( TARS_INTERFACE[$TARS_IDENTIFIER.text] ( operation )+ ) ;
	public final TarsParser.interface_def_return interface_def() throws RecognitionException {
		TarsParser.interface_def_return retval = new TarsParser.interface_def_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_INTERFACE46=null;
		Token TARS_IDENTIFIER47=null;
		Token LBRACE48=null;
		Token SEMI50=null;
		Token RBRACE51=null;
		ParserRuleReturnScope operation49 =null;

		Object TARS_INTERFACE46_tree=null;
		Object TARS_IDENTIFIER47_tree=null;
		Object LBRACE48_tree=null;
		Object SEMI50_tree=null;
		Object RBRACE51_tree=null;
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_TARS_INTERFACE=new RewriteRuleTokenStream(adaptor,"token TARS_INTERFACE");
		RewriteRuleTokenStream stream_RBRACE=new RewriteRuleTokenStream(adaptor,"token RBRACE");
		RewriteRuleTokenStream stream_SEMI=new RewriteRuleTokenStream(adaptor,"token SEMI");
		RewriteRuleTokenStream stream_LBRACE=new RewriteRuleTokenStream(adaptor,"token LBRACE");
		RewriteRuleSubtreeStream stream_operation=new RewriteRuleSubtreeStream(adaptor,"rule operation");

		try {
			// TarsParser.g:73:2: ( TARS_INTERFACE TARS_IDENTIFIER LBRACE ( operation SEMI )+ RBRACE -> ^( TARS_INTERFACE[$TARS_IDENTIFIER.text] ( operation )+ ) )
			// TarsParser.g:73:4: TARS_INTERFACE TARS_IDENTIFIER LBRACE ( operation SEMI )+ RBRACE
			{
			TARS_INTERFACE46=(Token)match(input,TARS_INTERFACE,FOLLOW_TARS_INTERFACE_in_interface_def488);  
			stream_TARS_INTERFACE.add(TARS_INTERFACE46);

			TARS_IDENTIFIER47=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_interface_def490);  
			stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER47);

			LBRACE48=(Token)match(input,LBRACE,FOLLOW_LBRACE_in_interface_def492);  
			stream_LBRACE.add(LBRACE48);

			// TarsParser.g:73:42: ( operation SEMI )+
			int cnt14=0;
			loop14:
			while (true) {
				int alt14=2;
				int LA14_0 = input.LA(1);
				if ( ((LA14_0 >= TARS_BOOL && LA14_0 <= TARS_BYTE)||LA14_0==TARS_DOUBLE||LA14_0==TARS_FLOAT||LA14_0==TARS_IDENTIFIER||LA14_0==TARS_INT||(LA14_0 >= TARS_LONG && LA14_0 <= TARS_MAP)||(LA14_0 >= TARS_SHORT && LA14_0 <= TARS_STRING)||(LA14_0 >= TARS_UNSIGNED && LA14_0 <= TARS_VOID)) ) {
					alt14=1;
				}

				switch (alt14) {
				case 1 :
					// TarsParser.g:73:43: operation SEMI
					{
					pushFollow(FOLLOW_operation_in_interface_def495);
					operation49=operation();
					state._fsp--;

					stream_operation.add(operation49.getTree());
					SEMI50=(Token)match(input,SEMI,FOLLOW_SEMI_in_interface_def497);  
					stream_SEMI.add(SEMI50);

					}
					break;

				default :
					if ( cnt14 >= 1 ) break loop14;
					EarlyExitException eee = new EarlyExitException(14, input);
					throw eee;
				}
				cnt14++;
			}

			RBRACE51=(Token)match(input,RBRACE,FOLLOW_RBRACE_in_interface_def501);  
			stream_RBRACE.add(RBRACE51);

			// AST REWRITE
			// elements: operation, TARS_INTERFACE
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 74:3: -> ^( TARS_INTERFACE[$TARS_IDENTIFIER.text] ( operation )+ )
			{
				// TarsParser.g:74:6: ^( TARS_INTERFACE[$TARS_IDENTIFIER.text] ( operation )+ )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsInterface(TARS_INTERFACE, (TARS_IDENTIFIER47!=null?TARS_IDENTIFIER47.getText():null)), root_1);
				if ( !(stream_operation.hasNext()) ) {
					throw new RewriteEarlyExitException();
				}
				while ( stream_operation.hasNext() ) {
					adaptor.addChild(root_1, stream_operation.nextTree());
				}
				stream_operation.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "interface_def"


	public static class operation_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "operation"
	// TarsParser.g:77:1: operation : type TARS_IDENTIFIER LPAREN ( param ( COMMA param )* )? RPAREN -> ^( TARS_OPERATION[$TARS_IDENTIFIER.text] type ( param )* ) ;
	public final TarsParser.operation_return operation() throws RecognitionException {
		TarsParser.operation_return retval = new TarsParser.operation_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_IDENTIFIER53=null;
		Token LPAREN54=null;
		Token COMMA56=null;
		Token RPAREN58=null;
		ParserRuleReturnScope type52 =null;
		ParserRuleReturnScope param55 =null;
		ParserRuleReturnScope param57 =null;

		Object TARS_IDENTIFIER53_tree=null;
		Object LPAREN54_tree=null;
		Object COMMA56_tree=null;
		Object RPAREN58_tree=null;
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleSubtreeStream stream_param=new RewriteRuleSubtreeStream(adaptor,"rule param");
		RewriteRuleSubtreeStream stream_type=new RewriteRuleSubtreeStream(adaptor,"rule type");

		try {
			// TarsParser.g:78:2: ( type TARS_IDENTIFIER LPAREN ( param ( COMMA param )* )? RPAREN -> ^( TARS_OPERATION[$TARS_IDENTIFIER.text] type ( param )* ) )
			// TarsParser.g:78:4: type TARS_IDENTIFIER LPAREN ( param ( COMMA param )* )? RPAREN
			{
			pushFollow(FOLLOW_type_in_operation528);
			type52=type();
			state._fsp--;

			stream_type.add(type52.getTree());
			TARS_IDENTIFIER53=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_operation530);  
			stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER53);

			LPAREN54=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_operation532);  
			stream_LPAREN.add(LPAREN54);

			// TarsParser.g:78:32: ( param ( COMMA param )* )?
			int alt16=2;
			int LA16_0 = input.LA(1);
			if ( ((LA16_0 >= TARS_BOOL && LA16_0 <= TARS_BYTE)||LA16_0==TARS_DOUBLE||LA16_0==TARS_FLOAT||LA16_0==TARS_IDENTIFIER||LA16_0==TARS_INT||(LA16_0 >= TARS_LONG && LA16_0 <= TARS_MAP)||LA16_0==TARS_OUT||(LA16_0 >= TARS_ROUTE_KEY && LA16_0 <= TARS_STRING)||(LA16_0 >= TARS_UNSIGNED && LA16_0 <= TARS_VOID)) ) {
				alt16=1;
			}
			switch (alt16) {
				case 1 :
					// TarsParser.g:78:33: param ( COMMA param )*
					{
					pushFollow(FOLLOW_param_in_operation535);
					param55=param();
					state._fsp--;

					stream_param.add(param55.getTree());
					// TarsParser.g:78:39: ( COMMA param )*
					loop15:
					while (true) {
						int alt15=2;
						int LA15_0 = input.LA(1);
						if ( (LA15_0==COMMA) ) {
							alt15=1;
						}

						switch (alt15) {
						case 1 :
							// TarsParser.g:78:40: COMMA param
							{
							COMMA56=(Token)match(input,COMMA,FOLLOW_COMMA_in_operation538);  
							stream_COMMA.add(COMMA56);

							pushFollow(FOLLOW_param_in_operation540);
							param57=param();
							state._fsp--;

							stream_param.add(param57.getTree());
							}
							break;

						default :
							break loop15;
						}
					}

					}
					break;

			}

			RPAREN58=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_operation547);  
			stream_RPAREN.add(RPAREN58);

			// AST REWRITE
			// elements: param, type
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 79:3: -> ^( TARS_OPERATION[$TARS_IDENTIFIER.text] type ( param )* )
			{
				// TarsParser.g:79:6: ^( TARS_OPERATION[$TARS_IDENTIFIER.text] type ( param )* )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsOperation(TARS_OPERATION, (TARS_IDENTIFIER53!=null?TARS_IDENTIFIER53.getText():null)), root_1);
				adaptor.addChild(root_1, stream_type.nextTree());
				// TarsParser.g:79:66: ( param )*
				while ( stream_param.hasNext() ) {
					adaptor.addChild(root_1, stream_param.nextTree());
				}
				stream_param.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "operation"


	public static class param_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "param"
	// TarsParser.g:82:1: param : ( TARS_ROUTE_KEY )? ( TARS_OUT )? type TARS_IDENTIFIER -> ^( TARS_PARAM[$TARS_IDENTIFIER.text, $TARS_OUT, $TARS_ROUTE_KEY] type ) ;
	public final TarsParser.param_return param() throws RecognitionException {
		TarsParser.param_return retval = new TarsParser.param_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_ROUTE_KEY59=null;
		Token TARS_OUT60=null;
		Token TARS_IDENTIFIER62=null;
		ParserRuleReturnScope type61 =null;

		Object TARS_ROUTE_KEY59_tree=null;
		Object TARS_OUT60_tree=null;
		Object TARS_IDENTIFIER62_tree=null;
		RewriteRuleTokenStream stream_TARS_ROUTE_KEY=new RewriteRuleTokenStream(adaptor,"token TARS_ROUTE_KEY");
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");
		RewriteRuleTokenStream stream_TARS_OUT=new RewriteRuleTokenStream(adaptor,"token TARS_OUT");
		RewriteRuleSubtreeStream stream_type=new RewriteRuleSubtreeStream(adaptor,"rule type");

		try {
			// TarsParser.g:83:2: ( ( TARS_ROUTE_KEY )? ( TARS_OUT )? type TARS_IDENTIFIER -> ^( TARS_PARAM[$TARS_IDENTIFIER.text, $TARS_OUT, $TARS_ROUTE_KEY] type ) )
			// TarsParser.g:83:4: ( TARS_ROUTE_KEY )? ( TARS_OUT )? type TARS_IDENTIFIER
			{
			// TarsParser.g:83:4: ( TARS_ROUTE_KEY )?
			int alt17=2;
			int LA17_0 = input.LA(1);
			if ( (LA17_0==TARS_ROUTE_KEY) ) {
				alt17=1;
			}
			switch (alt17) {
				case 1 :
					// TarsParser.g:83:4: TARS_ROUTE_KEY
					{
					TARS_ROUTE_KEY59=(Token)match(input,TARS_ROUTE_KEY,FOLLOW_TARS_ROUTE_KEY_in_param576);  
					stream_TARS_ROUTE_KEY.add(TARS_ROUTE_KEY59);

					}
					break;

			}

			// TarsParser.g:83:20: ( TARS_OUT )?
			int alt18=2;
			int LA18_0 = input.LA(1);
			if ( (LA18_0==TARS_OUT) ) {
				alt18=1;
			}
			switch (alt18) {
				case 1 :
					// TarsParser.g:83:20: TARS_OUT
					{
					TARS_OUT60=(Token)match(input,TARS_OUT,FOLLOW_TARS_OUT_in_param579);  
					stream_TARS_OUT.add(TARS_OUT60);

					}
					break;

			}

			pushFollow(FOLLOW_type_in_param582);
			type61=type();
			state._fsp--;

			stream_type.add(type61.getTree());
			TARS_IDENTIFIER62=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_param584);  
			stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER62);

			// AST REWRITE
			// elements: type
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 84:3: -> ^( TARS_PARAM[$TARS_IDENTIFIER.text, $TARS_OUT, $TARS_ROUTE_KEY] type )
			{
				// TarsParser.g:84:6: ^( TARS_PARAM[$TARS_IDENTIFIER.text, $TARS_OUT, $TARS_ROUTE_KEY] type )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsParam(TARS_PARAM, (TARS_IDENTIFIER62!=null?TARS_IDENTIFIER62.getText():null), TARS_OUT60, TARS_ROUTE_KEY59), root_1);
				adaptor.addChild(root_1, stream_type.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "param"


	public static class const_initializer_return extends ParserRuleReturnScope {
		public String result;
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "const_initializer"
	// TarsParser.g:87:1: const_initializer returns [String result] : ( TARS_INTEGER_LITERAL | TARS_FLOATING_POINT_LITERAL | TARS_STRING_LITERAL | TARS_FALSE | TARS_TRUE );
	public final TarsParser.const_initializer_return const_initializer() throws RecognitionException {
		TarsParser.const_initializer_return retval = new TarsParser.const_initializer_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_INTEGER_LITERAL63=null;
		Token TARS_FLOATING_POINT_LITERAL64=null;
		Token TARS_STRING_LITERAL65=null;
		Token TARS_FALSE66=null;
		Token TARS_TRUE67=null;

		Object TARS_INTEGER_LITERAL63_tree=null;
		Object TARS_FLOATING_POINT_LITERAL64_tree=null;
		Object TARS_STRING_LITERAL65_tree=null;
		Object TARS_FALSE66_tree=null;
		Object TARS_TRUE67_tree=null;

		try {
			// TarsParser.g:88:2: ( TARS_INTEGER_LITERAL | TARS_FLOATING_POINT_LITERAL | TARS_STRING_LITERAL | TARS_FALSE | TARS_TRUE )
			int alt19=5;
			switch ( input.LA(1) ) {
			case TARS_INTEGER_LITERAL:
				{
				alt19=1;
				}
				break;
			case TARS_FLOATING_POINT_LITERAL:
				{
				alt19=2;
				}
				break;
			case TARS_STRING_LITERAL:
				{
				alt19=3;
				}
				break;
			case TARS_FALSE:
				{
				alt19=4;
				}
				break;
			case TARS_TRUE:
				{
				alt19=5;
				}
				break;
			default:
				NoViableAltException nvae =
					new NoViableAltException("", 19, 0, input);
				throw nvae;
			}
			switch (alt19) {
				case 1 :
					// TarsParser.g:88:4: TARS_INTEGER_LITERAL
					{
					root_0 = (Object)adaptor.nil();


					TARS_INTEGER_LITERAL63=(Token)match(input,TARS_INTEGER_LITERAL,FOLLOW_TARS_INTEGER_LITERAL_in_const_initializer613); 
					TARS_INTEGER_LITERAL63_tree = (Object)adaptor.create(TARS_INTEGER_LITERAL63);
					adaptor.addChild(root_0, TARS_INTEGER_LITERAL63_tree);

					 retval.result = (TARS_INTEGER_LITERAL63!=null?TARS_INTEGER_LITERAL63.getText():null);
					}
					break;
				case 2 :
					// TarsParser.g:89:4: TARS_FLOATING_POINT_LITERAL
					{
					root_0 = (Object)adaptor.nil();


					TARS_FLOATING_POINT_LITERAL64=(Token)match(input,TARS_FLOATING_POINT_LITERAL,FOLLOW_TARS_FLOATING_POINT_LITERAL_in_const_initializer620); 
					TARS_FLOATING_POINT_LITERAL64_tree = (Object)adaptor.create(TARS_FLOATING_POINT_LITERAL64);
					adaptor.addChild(root_0, TARS_FLOATING_POINT_LITERAL64_tree);

					 retval.result = (TARS_FLOATING_POINT_LITERAL64!=null?TARS_FLOATING_POINT_LITERAL64.getText():null);
					}
					break;
				case 3 :
					// TarsParser.g:90:4: TARS_STRING_LITERAL
					{
					root_0 = (Object)adaptor.nil();


					TARS_STRING_LITERAL65=(Token)match(input,TARS_STRING_LITERAL,FOLLOW_TARS_STRING_LITERAL_in_const_initializer627); 
					TARS_STRING_LITERAL65_tree = (Object)adaptor.create(TARS_STRING_LITERAL65);
					adaptor.addChild(root_0, TARS_STRING_LITERAL65_tree);

					 retval.result = (TARS_STRING_LITERAL65!=null?TARS_STRING_LITERAL65.getText():null);
					}
					break;
				case 4 :
					// TarsParser.g:91:4: TARS_FALSE
					{
					root_0 = (Object)adaptor.nil();


					TARS_FALSE66=(Token)match(input,TARS_FALSE,FOLLOW_TARS_FALSE_in_const_initializer634); 
					TARS_FALSE66_tree = (Object)adaptor.create(TARS_FALSE66);
					adaptor.addChild(root_0, TARS_FALSE66_tree);

					 retval.result = (TARS_FALSE66!=null?TARS_FALSE66.getText():null);
					}
					break;
				case 5 :
					// TarsParser.g:92:4: TARS_TRUE
					{
					root_0 = (Object)adaptor.nil();


					TARS_TRUE67=(Token)match(input,TARS_TRUE,FOLLOW_TARS_TRUE_in_const_initializer641); 
					TARS_TRUE67_tree = (Object)adaptor.create(TARS_TRUE67);
					adaptor.addChild(root_0, TARS_TRUE67_tree);

					 retval.result = (TARS_TRUE67!=null?TARS_TRUE67.getText():null);
					}
					break;

			}
			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "const_initializer"


	public static class type_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "type"
	// TarsParser.g:95:1: type : ( type_primitive | type_vector | type_map | type_custom );
	public final TarsParser.type_return type() throws RecognitionException {
		TarsParser.type_return retval = new TarsParser.type_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		ParserRuleReturnScope type_primitive68 =null;
		ParserRuleReturnScope type_vector69 =null;
		ParserRuleReturnScope type_map70 =null;
		ParserRuleReturnScope type_custom71 =null;


		try {
			// TarsParser.g:96:2: ( type_primitive | type_vector | type_map | type_custom )
			int alt20=4;
			switch ( input.LA(1) ) {
			case TARS_BOOL:
			case TARS_BYTE:
			case TARS_DOUBLE:
			case TARS_FLOAT:
			case TARS_INT:
			case TARS_LONG:
			case TARS_SHORT:
			case TARS_STRING:
			case TARS_UNSIGNED:
			case TARS_VOID:
				{
				alt20=1;
				}
				break;
			case TARS_VECTOR:
				{
				alt20=2;
				}
				break;
			case TARS_MAP:
				{
				alt20=3;
				}
				break;
			case TARS_IDENTIFIER:
				{
				alt20=4;
				}
				break;
			default:
				NoViableAltException nvae =
					new NoViableAltException("", 20, 0, input);
				throw nvae;
			}
			switch (alt20) {
				case 1 :
					// TarsParser.g:96:4: type_primitive
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_type_primitive_in_type654);
					type_primitive68=type_primitive();
					state._fsp--;

					adaptor.addChild(root_0, type_primitive68.getTree());

					}
					break;
				case 2 :
					// TarsParser.g:97:4: type_vector
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_type_vector_in_type659);
					type_vector69=type_vector();
					state._fsp--;

					adaptor.addChild(root_0, type_vector69.getTree());

					}
					break;
				case 3 :
					// TarsParser.g:98:4: type_map
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_type_map_in_type664);
					type_map70=type_map();
					state._fsp--;

					adaptor.addChild(root_0, type_map70.getTree());

					}
					break;
				case 4 :
					// TarsParser.g:99:4: type_custom
					{
					root_0 = (Object)adaptor.nil();


					pushFollow(FOLLOW_type_custom_in_type669);
					type_custom71=type_custom();
					state._fsp--;

					adaptor.addChild(root_0, type_custom71.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "type"


	public static class type_primitive_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "type_primitive"
	// TarsParser.g:102:1: type_primitive : ( TARS_VOID -> ^( TARS_VOID ) | TARS_BOOL -> ^( TARS_BOOL ) | TARS_BYTE -> ^( TARS_BYTE ) | TARS_SHORT -> ^( TARS_SHORT ) | TARS_INT -> ^( TARS_INT ) | TARS_LONG -> ^( TARS_LONG ) | TARS_FLOAT -> ^( TARS_FLOAT ) | TARS_DOUBLE -> ^( TARS_DOUBLE ) | TARS_STRING -> ^( TARS_STRING ) | TARS_UNSIGNED TARS_INT -> ^( TARS_LONG ) );
	public final TarsParser.type_primitive_return type_primitive() throws RecognitionException {
		TarsParser.type_primitive_return retval = new TarsParser.type_primitive_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_VOID72=null;
		Token TARS_BOOL73=null;
		Token TARS_BYTE74=null;
		Token TARS_SHORT75=null;
		Token TARS_INT76=null;
		Token TARS_LONG77=null;
		Token TARS_FLOAT78=null;
		Token TARS_DOUBLE79=null;
		Token TARS_STRING80=null;
		Token TARS_UNSIGNED81=null;
		Token TARS_INT82=null;

		Object TARS_VOID72_tree=null;
		Object TARS_BOOL73_tree=null;
		Object TARS_BYTE74_tree=null;
		Object TARS_SHORT75_tree=null;
		Object TARS_INT76_tree=null;
		Object TARS_LONG77_tree=null;
		Object TARS_FLOAT78_tree=null;
		Object TARS_DOUBLE79_tree=null;
		Object TARS_STRING80_tree=null;
		Object TARS_UNSIGNED81_tree=null;
		Object TARS_INT82_tree=null;
		RewriteRuleTokenStream stream_TARS_DOUBLE=new RewriteRuleTokenStream(adaptor,"token TARS_DOUBLE");
		RewriteRuleTokenStream stream_TARS_BYTE=new RewriteRuleTokenStream(adaptor,"token TARS_BYTE");
		RewriteRuleTokenStream stream_TARS_UNSIGNED=new RewriteRuleTokenStream(adaptor,"token TARS_UNSIGNED");
		RewriteRuleTokenStream stream_TARS_STRING=new RewriteRuleTokenStream(adaptor,"token TARS_STRING");
		RewriteRuleTokenStream stream_TARS_SHORT=new RewriteRuleTokenStream(adaptor,"token TARS_SHORT");
		RewriteRuleTokenStream stream_TARS_FLOAT=new RewriteRuleTokenStream(adaptor,"token TARS_FLOAT");
		RewriteRuleTokenStream stream_TARS_BOOL=new RewriteRuleTokenStream(adaptor,"token TARS_BOOL");
		RewriteRuleTokenStream stream_TARS_LONG=new RewriteRuleTokenStream(adaptor,"token TARS_LONG");
		RewriteRuleTokenStream stream_TARS_INT=new RewriteRuleTokenStream(adaptor,"token TARS_INT");
		RewriteRuleTokenStream stream_TARS_VOID=new RewriteRuleTokenStream(adaptor,"token TARS_VOID");

		try {
			// TarsParser.g:103:2: ( TARS_VOID -> ^( TARS_VOID ) | TARS_BOOL -> ^( TARS_BOOL ) | TARS_BYTE -> ^( TARS_BYTE ) | TARS_SHORT -> ^( TARS_SHORT ) | TARS_INT -> ^( TARS_INT ) | TARS_LONG -> ^( TARS_LONG ) | TARS_FLOAT -> ^( TARS_FLOAT ) | TARS_DOUBLE -> ^( TARS_DOUBLE ) | TARS_STRING -> ^( TARS_STRING ) | TARS_UNSIGNED TARS_INT -> ^( TARS_LONG ) )
			int alt21=10;
			switch ( input.LA(1) ) {
			case TARS_VOID:
				{
				alt21=1;
				}
				break;
			case TARS_BOOL:
				{
				alt21=2;
				}
				break;
			case TARS_BYTE:
				{
				alt21=3;
				}
				break;
			case TARS_SHORT:
				{
				alt21=4;
				}
				break;
			case TARS_INT:
				{
				alt21=5;
				}
				break;
			case TARS_LONG:
				{
				alt21=6;
				}
				break;
			case TARS_FLOAT:
				{
				alt21=7;
				}
				break;
			case TARS_DOUBLE:
				{
				alt21=8;
				}
				break;
			case TARS_STRING:
				{
				alt21=9;
				}
				break;
			case TARS_UNSIGNED:
				{
				alt21=10;
				}
				break;
			default:
				NoViableAltException nvae =
					new NoViableAltException("", 21, 0, input);
				throw nvae;
			}
			switch (alt21) {
				case 1 :
					// TarsParser.g:103:4: TARS_VOID
					{
					TARS_VOID72=(Token)match(input,TARS_VOID,FOLLOW_TARS_VOID_in_type_primitive681);  
					stream_TARS_VOID.add(TARS_VOID72);

					// AST REWRITE
					// elements: TARS_VOID
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 104:3: -> ^( TARS_VOID )
					{
						// TarsParser.g:104:6: ^( TARS_VOID )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_VOID.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 2 :
					// TarsParser.g:105:4: TARS_BOOL
					{
					TARS_BOOL73=(Token)match(input,TARS_BOOL,FOLLOW_TARS_BOOL_in_type_primitive697);  
					stream_TARS_BOOL.add(TARS_BOOL73);

					// AST REWRITE
					// elements: TARS_BOOL
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 106:3: -> ^( TARS_BOOL )
					{
						// TarsParser.g:106:6: ^( TARS_BOOL )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_BOOL.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 3 :
					// TarsParser.g:107:4: TARS_BYTE
					{
					TARS_BYTE74=(Token)match(input,TARS_BYTE,FOLLOW_TARS_BYTE_in_type_primitive713);  
					stream_TARS_BYTE.add(TARS_BYTE74);

					// AST REWRITE
					// elements: TARS_BYTE
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 108:3: -> ^( TARS_BYTE )
					{
						// TarsParser.g:108:6: ^( TARS_BYTE )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_BYTE.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 4 :
					// TarsParser.g:109:4: TARS_SHORT
					{
					TARS_SHORT75=(Token)match(input,TARS_SHORT,FOLLOW_TARS_SHORT_in_type_primitive729);  
					stream_TARS_SHORT.add(TARS_SHORT75);

					// AST REWRITE
					// elements: TARS_SHORT
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 110:3: -> ^( TARS_SHORT )
					{
						// TarsParser.g:110:6: ^( TARS_SHORT )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_SHORT.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 5 :
					// TarsParser.g:111:4: TARS_INT
					{
					TARS_INT76=(Token)match(input,TARS_INT,FOLLOW_TARS_INT_in_type_primitive745);  
					stream_TARS_INT.add(TARS_INT76);

					// AST REWRITE
					// elements: TARS_INT
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 112:3: -> ^( TARS_INT )
					{
						// TarsParser.g:112:6: ^( TARS_INT )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_INT.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 6 :
					// TarsParser.g:113:4: TARS_LONG
					{
					TARS_LONG77=(Token)match(input,TARS_LONG,FOLLOW_TARS_LONG_in_type_primitive761);  
					stream_TARS_LONG.add(TARS_LONG77);

					// AST REWRITE
					// elements: TARS_LONG
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 114:3: -> ^( TARS_LONG )
					{
						// TarsParser.g:114:6: ^( TARS_LONG )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_LONG.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 7 :
					// TarsParser.g:115:4: TARS_FLOAT
					{
					TARS_FLOAT78=(Token)match(input,TARS_FLOAT,FOLLOW_TARS_FLOAT_in_type_primitive777);  
					stream_TARS_FLOAT.add(TARS_FLOAT78);

					// AST REWRITE
					// elements: TARS_FLOAT
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 116:3: -> ^( TARS_FLOAT )
					{
						// TarsParser.g:116:6: ^( TARS_FLOAT )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_FLOAT.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 8 :
					// TarsParser.g:117:4: TARS_DOUBLE
					{
					TARS_DOUBLE79=(Token)match(input,TARS_DOUBLE,FOLLOW_TARS_DOUBLE_in_type_primitive793);  
					stream_TARS_DOUBLE.add(TARS_DOUBLE79);

					// AST REWRITE
					// elements: TARS_DOUBLE
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 118:3: -> ^( TARS_DOUBLE )
					{
						// TarsParser.g:118:6: ^( TARS_DOUBLE )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_DOUBLE.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 9 :
					// TarsParser.g:119:4: TARS_STRING
					{
					TARS_STRING80=(Token)match(input,TARS_STRING,FOLLOW_TARS_STRING_in_type_primitive809);  
					stream_TARS_STRING.add(TARS_STRING80);

					// AST REWRITE
					// elements: TARS_STRING
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 120:3: -> ^( TARS_STRING )
					{
						// TarsParser.g:120:6: ^( TARS_STRING )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(stream_TARS_STRING.nextToken()), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 10 :
					// TarsParser.g:121:4: TARS_UNSIGNED TARS_INT
					{
					TARS_UNSIGNED81=(Token)match(input,TARS_UNSIGNED,FOLLOW_TARS_UNSIGNED_in_type_primitive825);  
					stream_TARS_UNSIGNED.add(TARS_UNSIGNED81);

					TARS_INT82=(Token)match(input,TARS_INT,FOLLOW_TARS_INT_in_type_primitive827);  
					stream_TARS_INT.add(TARS_INT82);

					// AST REWRITE
					// elements: 
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 122:3: -> ^( TARS_LONG )
					{
						// TarsParser.g:122:6: ^( TARS_LONG )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsPrimitiveType(TARS_LONG), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;

			}
			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "type_primitive"


	public static class type_vector_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "type_vector"
	// TarsParser.g:125:1: type_vector : TARS_VECTOR LT type GT -> ^( TARS_VECTOR type ) ;
	public final TarsParser.type_vector_return type_vector() throws RecognitionException {
		TarsParser.type_vector_return retval = new TarsParser.type_vector_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_VECTOR83=null;
		Token LT84=null;
		Token GT86=null;
		ParserRuleReturnScope type85 =null;

		Object TARS_VECTOR83_tree=null;
		Object LT84_tree=null;
		Object GT86_tree=null;
		RewriteRuleTokenStream stream_TARS_VECTOR=new RewriteRuleTokenStream(adaptor,"token TARS_VECTOR");
		RewriteRuleTokenStream stream_GT=new RewriteRuleTokenStream(adaptor,"token GT");
		RewriteRuleTokenStream stream_LT=new RewriteRuleTokenStream(adaptor,"token LT");
		RewriteRuleSubtreeStream stream_type=new RewriteRuleSubtreeStream(adaptor,"rule type");

		try {
			// TarsParser.g:126:2: ( TARS_VECTOR LT type GT -> ^( TARS_VECTOR type ) )
			// TarsParser.g:126:4: TARS_VECTOR LT type GT
			{
			TARS_VECTOR83=(Token)match(input,TARS_VECTOR,FOLLOW_TARS_VECTOR_in_type_vector849);  
			stream_TARS_VECTOR.add(TARS_VECTOR83);

			LT84=(Token)match(input,LT,FOLLOW_LT_in_type_vector851);  
			stream_LT.add(LT84);

			pushFollow(FOLLOW_type_in_type_vector853);
			type85=type();
			state._fsp--;

			stream_type.add(type85.getTree());
			GT86=(Token)match(input,GT,FOLLOW_GT_in_type_vector855);  
			stream_GT.add(GT86);

			// AST REWRITE
			// elements: type, TARS_VECTOR
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 127:3: -> ^( TARS_VECTOR type )
			{
				// TarsParser.g:127:6: ^( TARS_VECTOR type )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsVectorType(stream_TARS_VECTOR.nextToken()), root_1);
				adaptor.addChild(root_1, stream_type.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "type_vector"


	public static class type_map_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "type_map"
	// TarsParser.g:130:1: type_map : TARS_MAP LT type COMMA type GT -> ^( TARS_MAP type type ) ;
	public final TarsParser.type_map_return type_map() throws RecognitionException {
		TarsParser.type_map_return retval = new TarsParser.type_map_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token TARS_MAP87=null;
		Token LT88=null;
		Token COMMA90=null;
		Token GT92=null;
		ParserRuleReturnScope type89 =null;
		ParserRuleReturnScope type91 =null;

		Object TARS_MAP87_tree=null;
		Object LT88_tree=null;
		Object COMMA90_tree=null;
		Object GT92_tree=null;
		RewriteRuleTokenStream stream_GT=new RewriteRuleTokenStream(adaptor,"token GT");
		RewriteRuleTokenStream stream_LT=new RewriteRuleTokenStream(adaptor,"token LT");
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleTokenStream stream_TARS_MAP=new RewriteRuleTokenStream(adaptor,"token TARS_MAP");
		RewriteRuleSubtreeStream stream_type=new RewriteRuleSubtreeStream(adaptor,"rule type");

		try {
			// TarsParser.g:131:2: ( TARS_MAP LT type COMMA type GT -> ^( TARS_MAP type type ) )
			// TarsParser.g:131:4: TARS_MAP LT type COMMA type GT
			{
			TARS_MAP87=(Token)match(input,TARS_MAP,FOLLOW_TARS_MAP_in_type_map879);  
			stream_TARS_MAP.add(TARS_MAP87);

			LT88=(Token)match(input,LT,FOLLOW_LT_in_type_map881);  
			stream_LT.add(LT88);

			pushFollow(FOLLOW_type_in_type_map883);
			type89=type();
			state._fsp--;

			stream_type.add(type89.getTree());
			COMMA90=(Token)match(input,COMMA,FOLLOW_COMMA_in_type_map885);  
			stream_COMMA.add(COMMA90);

			pushFollow(FOLLOW_type_in_type_map887);
			type91=type();
			state._fsp--;

			stream_type.add(type91.getTree());
			GT92=(Token)match(input,GT,FOLLOW_GT_in_type_map889);  
			stream_GT.add(GT92);

			// AST REWRITE
			// elements: type, TARS_MAP, type
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (Object)adaptor.nil();
			// 132:3: -> ^( TARS_MAP type type )
			{
				// TarsParser.g:132:6: ^( TARS_MAP type type )
				{
				Object root_1 = (Object)adaptor.nil();
				root_1 = (Object)adaptor.becomeRoot(new TarsMapType(stream_TARS_MAP.nextToken()), root_1);
				adaptor.addChild(root_1, stream_type.nextTree());
				adaptor.addChild(root_1, stream_type.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;

			}

			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "type_map"


	public static class type_custom_return extends ParserRuleReturnScope {
		Object tree;
		@Override
		public Object getTree() { return tree; }
	};


	// $ANTLR start "type_custom"
	// TarsParser.g:135:1: type_custom : ( TARS_IDENTIFIER -> ^( TARS_MAP[$TARS_IDENTIFIER.text] ) |ns= TARS_IDENTIFIER COLON COLON id= TARS_IDENTIFIER -> ^( TARS_REF[$ns.text,$id.text] ) );
	public final TarsParser.type_custom_return type_custom() throws RecognitionException {
		TarsParser.type_custom_return retval = new TarsParser.type_custom_return();
		retval.start = input.LT(1);

		Object root_0 = null;

		Token ns=null;
		Token id=null;
		Token TARS_IDENTIFIER93=null;
		Token COLON94=null;
		Token COLON95=null;

		Object ns_tree=null;
		Object id_tree=null;
		Object TARS_IDENTIFIER93_tree=null;
		Object COLON94_tree=null;
		Object COLON95_tree=null;
		RewriteRuleTokenStream stream_COLON=new RewriteRuleTokenStream(adaptor,"token COLON");
		RewriteRuleTokenStream stream_TARS_IDENTIFIER=new RewriteRuleTokenStream(adaptor,"token TARS_IDENTIFIER");

		try {
			// TarsParser.g:136:2: ( TARS_IDENTIFIER -> ^( TARS_MAP[$TARS_IDENTIFIER.text] ) |ns= TARS_IDENTIFIER COLON COLON id= TARS_IDENTIFIER -> ^( TARS_REF[$ns.text,$id.text] ) )
			int alt22=2;
			int LA22_0 = input.LA(1);
			if ( (LA22_0==TARS_IDENTIFIER) ) {
				int LA22_1 = input.LA(2);
				if ( (LA22_1==COLON) ) {
					alt22=2;
				}
				else if ( (LA22_1==COMMA||LA22_1==GT||LA22_1==TARS_IDENTIFIER) ) {
					alt22=1;
				}

				else {
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 22, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				NoViableAltException nvae =
					new NoViableAltException("", 22, 0, input);
				throw nvae;
			}

			switch (alt22) {
				case 1 :
					// TarsParser.g:136:4: TARS_IDENTIFIER
					{
					TARS_IDENTIFIER93=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_type_custom916);  
					stream_TARS_IDENTIFIER.add(TARS_IDENTIFIER93);

					// AST REWRITE
					// elements: 
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 137:3: -> ^( TARS_MAP[$TARS_IDENTIFIER.text] )
					{
						// TarsParser.g:137:6: ^( TARS_MAP[$TARS_IDENTIFIER.text] )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsCustomType(TARS_MAP, (TARS_IDENTIFIER93!=null?TARS_IDENTIFIER93.getText():null)), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;
				case 2 :
					// TarsParser.g:138:4: ns= TARS_IDENTIFIER COLON COLON id= TARS_IDENTIFIER
					{
					ns=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_type_custom935);  
					stream_TARS_IDENTIFIER.add(ns);

					COLON94=(Token)match(input,COLON,FOLLOW_COLON_in_type_custom937);  
					stream_COLON.add(COLON94);

					COLON95=(Token)match(input,COLON,FOLLOW_COLON_in_type_custom939);  
					stream_COLON.add(COLON95);

					id=(Token)match(input,TARS_IDENTIFIER,FOLLOW_TARS_IDENTIFIER_in_type_custom943);  
					stream_TARS_IDENTIFIER.add(id);

					// AST REWRITE
					// elements: 
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (Object)adaptor.nil();
					// 139:3: -> ^( TARS_REF[$ns.text,$id.text] )
					{
						// TarsParser.g:139:6: ^( TARS_REF[$ns.text,$id.text] )
						{
						Object root_1 = (Object)adaptor.nil();
						root_1 = (Object)adaptor.becomeRoot(new TarsCustomType(TARS_REF, (ns!=null?ns.getText():null), (id!=null?id.getText():null)), root_1);
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;

					}
					break;

			}
			retval.stop = input.LT(-1);

			retval.tree = (Object)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (Object)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "type_custom"

	// Delegated rules



	public static final BitSet FOLLOW_include_def_in_start70 = new BitSet(new long[]{0x0000004080000000L});
	public static final BitSet FOLLOW_namespace_def_in_start73 = new BitSet(new long[]{0x0000004000000002L});
	public static final BitSet FOLLOW_TARS_INCLUDE_in_include_def102 = new BitSet(new long[]{0x0000200000000000L});
	public static final BitSet FOLLOW_TARS_STRING_LITERAL_in_include_def104 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_NAMESPACE_in_namespace_def127 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_namespace_def129 = new BitSet(new long[]{0x0000000000001000L});
	public static final BitSet FOLLOW_LBRACE_in_namespace_def131 = new BitSet(new long[]{0x0000400C05000000L});
	public static final BitSet FOLLOW_definition_in_namespace_def134 = new BitSet(new long[]{0x0000000000200000L});
	public static final BitSet FOLLOW_SEMI_in_namespace_def136 = new BitSet(new long[]{0x0000400C05040000L});
	public static final BitSet FOLLOW_RBRACE_in_namespace_def140 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_const_def_in_definition167 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_enum_def_in_definition174 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_struct_def_in_definition179 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_key_def_in_definition184 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_interface_def_in_definition189 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_CONST_in_const_def202 = new BitSet(new long[]{0x0005181112C00000L});
	public static final BitSet FOLLOW_type_primitive_in_const_def204 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_const_def206 = new BitSet(new long[]{0x0000000000000100L});
	public static final BitSet FOLLOW_EQ_in_const_def208 = new BitSet(new long[]{0x0000A00228000000L});
	public static final BitSet FOLLOW_const_initializer_in_const_def212 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_ENUM_in_enum_def238 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_enum_def242 = new BitSet(new long[]{0x0000000000001000L});
	public static final BitSet FOLLOW_LBRACE_in_enum_def244 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_enum_def248 = new BitSet(new long[]{0x0000000000040020L});
	public static final BitSet FOLLOW_COMMA_in_enum_def251 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_enum_def255 = new BitSet(new long[]{0x0000000000040020L});
	public static final BitSet FOLLOW_COMMA_in_enum_def259 = new BitSet(new long[]{0x0000000000040000L});
	public static final BitSet FOLLOW_RBRACE_in_enum_def262 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_ENUM_in_enum_def284 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_enum_def288 = new BitSet(new long[]{0x0000000000001000L});
	public static final BitSet FOLLOW_LBRACE_in_enum_def290 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_enum_def294 = new BitSet(new long[]{0x0000000000000100L});
	public static final BitSet FOLLOW_EQ_in_enum_def296 = new BitSet(new long[]{0x0000000200000000L});
	public static final BitSet FOLLOW_TARS_INTEGER_LITERAL_in_enum_def300 = new BitSet(new long[]{0x0000000000040020L});
	public static final BitSet FOLLOW_COMMA_in_enum_def303 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_enum_def307 = new BitSet(new long[]{0x0000000000000100L});
	public static final BitSet FOLLOW_EQ_in_enum_def309 = new BitSet(new long[]{0x0000000200000000L});
	public static final BitSet FOLLOW_TARS_INTEGER_LITERAL_in_enum_def313 = new BitSet(new long[]{0x0000000000040020L});
	public static final BitSet FOLLOW_COMMA_in_enum_def317 = new BitSet(new long[]{0x0000000000040000L});
	public static final BitSet FOLLOW_RBRACE_in_enum_def320 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_STRUCT_in_struct_def351 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_struct_def353 = new BitSet(new long[]{0x0000000000001000L});
	public static final BitSet FOLLOW_LBRACE_in_struct_def355 = new BitSet(new long[]{0x0000000200000000L});
	public static final BitSet FOLLOW_struct_member_in_struct_def358 = new BitSet(new long[]{0x0000000000200000L});
	public static final BitSet FOLLOW_SEMI_in_struct_def360 = new BitSet(new long[]{0x0000000200040000L});
	public static final BitSet FOLLOW_RBRACE_in_struct_def364 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_INTEGER_LITERAL_in_struct_member391 = new BitSet(new long[]{0x0000028000000000L});
	public static final BitSet FOLLOW_TARS_REQUIRE_in_struct_member396 = new BitSet(new long[]{0x0007183152C00000L});
	public static final BitSet FOLLOW_TARS_OPTIONAL_in_struct_member402 = new BitSet(new long[]{0x0007183152C00000L});
	public static final BitSet FOLLOW_type_in_struct_member405 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_struct_member407 = new BitSet(new long[]{0x0000000000000102L});
	public static final BitSet FOLLOW_EQ_in_struct_member410 = new BitSet(new long[]{0x0000A00228000000L});
	public static final BitSet FOLLOW_const_initializer_in_struct_member414 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_KEY_in_key_def443 = new BitSet(new long[]{0x0000000000002000L});
	public static final BitSet FOLLOW_LBRACKET_in_key_def445 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_key_def449 = new BitSet(new long[]{0x0000000000000020L});
	public static final BitSet FOLLOW_COMMA_in_key_def452 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_key_def456 = new BitSet(new long[]{0x0000000000080020L});
	public static final BitSet FOLLOW_RBRACKET_in_key_def460 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_INTERFACE_in_interface_def488 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_interface_def490 = new BitSet(new long[]{0x0000000000001000L});
	public static final BitSet FOLLOW_LBRACE_in_interface_def492 = new BitSet(new long[]{0x0007183152C00000L});
	public static final BitSet FOLLOW_operation_in_interface_def495 = new BitSet(new long[]{0x0000000000200000L});
	public static final BitSet FOLLOW_SEMI_in_interface_def497 = new BitSet(new long[]{0x0007183152C40000L});
	public static final BitSet FOLLOW_RBRACE_in_interface_def501 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_type_in_operation528 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_operation530 = new BitSet(new long[]{0x0000000000004000L});
	public static final BitSet FOLLOW_LPAREN_in_operation532 = new BitSet(new long[]{0x00071D3152D00000L});
	public static final BitSet FOLLOW_param_in_operation535 = new BitSet(new long[]{0x0000000000100020L});
	public static final BitSet FOLLOW_COMMA_in_operation538 = new BitSet(new long[]{0x00071D3152C00000L});
	public static final BitSet FOLLOW_param_in_operation540 = new BitSet(new long[]{0x0000000000100020L});
	public static final BitSet FOLLOW_RPAREN_in_operation547 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_ROUTE_KEY_in_param576 = new BitSet(new long[]{0x0007193152C00000L});
	public static final BitSet FOLLOW_TARS_OUT_in_param579 = new BitSet(new long[]{0x0007183152C00000L});
	public static final BitSet FOLLOW_type_in_param582 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_param584 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_INTEGER_LITERAL_in_const_initializer613 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_FLOATING_POINT_LITERAL_in_const_initializer620 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_STRING_LITERAL_in_const_initializer627 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_FALSE_in_const_initializer634 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_TRUE_in_const_initializer641 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_type_primitive_in_type654 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_type_vector_in_type659 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_type_map_in_type664 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_type_custom_in_type669 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_VOID_in_type_primitive681 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_BOOL_in_type_primitive697 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_BYTE_in_type_primitive713 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_SHORT_in_type_primitive729 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_INT_in_type_primitive745 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_LONG_in_type_primitive761 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_FLOAT_in_type_primitive777 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_DOUBLE_in_type_primitive793 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_STRING_in_type_primitive809 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_UNSIGNED_in_type_primitive825 = new BitSet(new long[]{0x0000000100000000L});
	public static final BitSet FOLLOW_TARS_INT_in_type_primitive827 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_VECTOR_in_type_vector849 = new BitSet(new long[]{0x0000000000008000L});
	public static final BitSet FOLLOW_LT_in_type_vector851 = new BitSet(new long[]{0x0007183152C00000L});
	public static final BitSet FOLLOW_type_in_type_vector853 = new BitSet(new long[]{0x0000000000000400L});
	public static final BitSet FOLLOW_GT_in_type_vector855 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_MAP_in_type_map879 = new BitSet(new long[]{0x0000000000008000L});
	public static final BitSet FOLLOW_LT_in_type_map881 = new BitSet(new long[]{0x0007183152C00000L});
	public static final BitSet FOLLOW_type_in_type_map883 = new BitSet(new long[]{0x0000000000000020L});
	public static final BitSet FOLLOW_COMMA_in_type_map885 = new BitSet(new long[]{0x0007183152C00000L});
	public static final BitSet FOLLOW_type_in_type_map887 = new BitSet(new long[]{0x0000000000000400L});
	public static final BitSet FOLLOW_GT_in_type_map889 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_type_custom916 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_type_custom935 = new BitSet(new long[]{0x0000000000000010L});
	public static final BitSet FOLLOW_COLON_in_type_custom937 = new BitSet(new long[]{0x0000000000000010L});
	public static final BitSet FOLLOW_COLON_in_type_custom939 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_TARS_IDENTIFIER_in_type_custom943 = new BitSet(new long[]{0x0000000000000002L});
}
