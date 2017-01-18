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


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

@SuppressWarnings("all")
public class TarsLexer extends Lexer {
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

	// delegates
	// delegators
	public Lexer[] getDelegates() {
		return new Lexer[] {};
	}

	public TarsLexer() {} 
	public TarsLexer(CharStream input) {
		this(input, new RecognizerSharedState());
	}
	public TarsLexer(CharStream input, RecognizerSharedState state) {
		super(input,state);
	}
	@Override public String getGrammarFileName() { return "TarsLexer.g"; }

	// $ANTLR start "TARS_VOID"
	public final void mTARS_VOID() throws RecognitionException {
		try {
			int _type = TARS_VOID;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:8:2: ( 'void' )
			// TarsLexer.g:8:4: 'void'
			{
			match("void"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_VOID"

	// $ANTLR start "TARS_STRUCT"
	public final void mTARS_STRUCT() throws RecognitionException {
		try {
			int _type = TARS_STRUCT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:11:2: ( 'struct' )
			// TarsLexer.g:11:4: 'struct'
			{
			match("struct"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_STRUCT"

	// $ANTLR start "TARS_UNSIGNED"
	public final void mTARS_UNSIGNED() throws RecognitionException {
		try {
			int _type = TARS_UNSIGNED;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:14:2: ( 'unsigned' )
			// TarsLexer.g:14:4: 'unsigned'
			{
			match("unsigned"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_UNSIGNED"

	// $ANTLR start "TARS_BOOL"
	public final void mTARS_BOOL() throws RecognitionException {
		try {
			int _type = TARS_BOOL;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:17:2: ( 'bool' )
			// TarsLexer.g:17:4: 'bool'
			{
			match("bool"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_BOOL"

	// $ANTLR start "TARS_BYTE"
	public final void mTARS_BYTE() throws RecognitionException {
		try {
			int _type = TARS_BYTE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:20:2: ( 'byte' )
			// TarsLexer.g:20:4: 'byte'
			{
			match("byte"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_BYTE"

	// $ANTLR start "TARS_SHORT"
	public final void mTARS_SHORT() throws RecognitionException {
		try {
			int _type = TARS_SHORT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:23:2: ( 'short' )
			// TarsLexer.g:23:4: 'short'
			{
			match("short"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_SHORT"

	// $ANTLR start "TARS_INT"
	public final void mTARS_INT() throws RecognitionException {
		try {
			int _type = TARS_INT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:26:2: ( 'int' )
			// TarsLexer.g:26:4: 'int'
			{
			match("int"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_INT"

	// $ANTLR start "TARS_DOUBLE"
	public final void mTARS_DOUBLE() throws RecognitionException {
		try {
			int _type = TARS_DOUBLE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:29:2: ( 'double' )
			// TarsLexer.g:29:4: 'double'
			{
			match("double"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_DOUBLE"

	// $ANTLR start "TARS_FLOAT"
	public final void mTARS_FLOAT() throws RecognitionException {
		try {
			int _type = TARS_FLOAT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:32:2: ( 'float' )
			// TarsLexer.g:32:4: 'float'
			{
			match("float"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_FLOAT"

	// $ANTLR start "TARS_LONG"
	public final void mTARS_LONG() throws RecognitionException {
		try {
			int _type = TARS_LONG;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:35:2: ( 'long' )
			// TarsLexer.g:35:4: 'long'
			{
			match("long"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_LONG"

	// $ANTLR start "TARS_STRING"
	public final void mTARS_STRING() throws RecognitionException {
		try {
			int _type = TARS_STRING;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:38:2: ( 'string' )
			// TarsLexer.g:38:4: 'string'
			{
			match("string"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_STRING"

	// $ANTLR start "TARS_VECTOR"
	public final void mTARS_VECTOR() throws RecognitionException {
		try {
			int _type = TARS_VECTOR;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:41:2: ( 'vector' )
			// TarsLexer.g:41:4: 'vector'
			{
			match("vector"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_VECTOR"

	// $ANTLR start "TARS_MAP"
	public final void mTARS_MAP() throws RecognitionException {
		try {
			int _type = TARS_MAP;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:44:2: ( 'map' )
			// TarsLexer.g:44:4: 'map'
			{
			match("map"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_MAP"

	// $ANTLR start "TARS_KEY"
	public final void mTARS_KEY() throws RecognitionException {
		try {
			int _type = TARS_KEY;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:47:2: ( 'key' )
			// TarsLexer.g:47:4: 'key'
			{
			match("key"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_KEY"

	// $ANTLR start "TARS_ROUTE_KEY"
	public final void mTARS_ROUTE_KEY() throws RecognitionException {
		try {
			int _type = TARS_ROUTE_KEY;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:50:2: ( 'routekey' )
			// TarsLexer.g:50:4: 'routekey'
			{
			match("routekey"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_ROUTE_KEY"

	// $ANTLR start "TARS_INCLUDE"
	public final void mTARS_INCLUDE() throws RecognitionException {
		try {
			int _type = TARS_INCLUDE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:53:2: ( '#include' )
			// TarsLexer.g:53:4: '#include'
			{
			match("#include"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_INCLUDE"

	// $ANTLR start "TARS_NAMESPACE"
	public final void mTARS_NAMESPACE() throws RecognitionException {
		try {
			int _type = TARS_NAMESPACE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:56:2: ( 'module' )
			// TarsLexer.g:56:4: 'module'
			{
			match("module"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_NAMESPACE"

	// $ANTLR start "TARS_INTERFACE"
	public final void mTARS_INTERFACE() throws RecognitionException {
		try {
			int _type = TARS_INTERFACE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:59:2: ( 'interface' )
			// TarsLexer.g:59:4: 'interface'
			{
			match("interface"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_INTERFACE"

	// $ANTLR start "TARS_OUT"
	public final void mTARS_OUT() throws RecognitionException {
		try {
			int _type = TARS_OUT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:62:2: ( 'out' )
			// TarsLexer.g:62:4: 'out'
			{
			match("out"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_OUT"

	// $ANTLR start "TARS_REQUIRE"
	public final void mTARS_REQUIRE() throws RecognitionException {
		try {
			int _type = TARS_REQUIRE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:65:2: ( 'require' )
			// TarsLexer.g:65:4: 'require'
			{
			match("require"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_REQUIRE"

	// $ANTLR start "TARS_OPTIONAL"
	public final void mTARS_OPTIONAL() throws RecognitionException {
		try {
			int _type = TARS_OPTIONAL;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:68:2: ( 'optional' )
			// TarsLexer.g:68:4: 'optional'
			{
			match("optional"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_OPTIONAL"

	// $ANTLR start "TARS_FALSE"
	public final void mTARS_FALSE() throws RecognitionException {
		try {
			int _type = TARS_FALSE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:71:2: ( 'false' )
			// TarsLexer.g:71:4: 'false'
			{
			match("false"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_FALSE"

	// $ANTLR start "TARS_TRUE"
	public final void mTARS_TRUE() throws RecognitionException {
		try {
			int _type = TARS_TRUE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:74:2: ( 'true' )
			// TarsLexer.g:74:4: 'true'
			{
			match("true"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_TRUE"

	// $ANTLR start "TARS_ENUM"
	public final void mTARS_ENUM() throws RecognitionException {
		try {
			int _type = TARS_ENUM;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:77:2: ( 'enum' )
			// TarsLexer.g:77:4: 'enum'
			{
			match("enum"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_ENUM"

	// $ANTLR start "TARS_CONST"
	public final void mTARS_CONST() throws RecognitionException {
		try {
			int _type = TARS_CONST;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:80:2: ( 'const' )
			// TarsLexer.g:80:4: 'const'
			{
			match("const"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_CONST"

	// $ANTLR start "TARS_IDENTIFIER"
	public final void mTARS_IDENTIFIER() throws RecognitionException {
		try {
			int _type = TARS_IDENTIFIER;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:85:5: ( ( 'a' .. 'z' | 'A' .. 'Z' | '_' ) ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' )* )
			// TarsLexer.g:85:7: ( 'a' .. 'z' | 'A' .. 'Z' | '_' ) ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' )*
			{
			if ( (input.LA(1) >= 'A' && input.LA(1) <= 'Z')||input.LA(1)=='_'||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
				input.consume();
			}
			else {
				MismatchedSetException mse = new MismatchedSetException(null,input);
				recover(mse);
				throw mse;
			}
			// TarsLexer.g:85:31: ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' )*
			loop1:
			while (true) {
				int alt1=2;
				int LA1_0 = input.LA(1);
				if ( ((LA1_0 >= '0' && LA1_0 <= '9')||(LA1_0 >= 'A' && LA1_0 <= 'Z')||LA1_0=='_'||(LA1_0 >= 'a' && LA1_0 <= 'z')) ) {
					alt1=1;
				}

				switch (alt1) {
				case 1 :
					// TarsLexer.g:
					{
					if ( (input.LA(1) >= '0' && input.LA(1) <= '9')||(input.LA(1) >= 'A' && input.LA(1) <= 'Z')||input.LA(1)=='_'||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					break loop1;
				}
			}

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_IDENTIFIER"

	// $ANTLR start "TARS_INTEGER_LITERAL"
	public final void mTARS_INTEGER_LITERAL() throws RecognitionException {
		try {
			int _type = TARS_INTEGER_LITERAL;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:89:5: ( ( '0X' | '0x' ) ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' )+ | ( 'O' | 'o' ) ( '0' .. '7' )+ | ( '+' | '-' )? ( '0' .. '9' )+ )
			int alt7=3;
			switch ( input.LA(1) ) {
			case '0':
				{
				int LA7_1 = input.LA(2);
				if ( (LA7_1=='X'||LA7_1=='x') ) {
					alt7=1;
				}

				else {
					alt7=3;
				}

				}
				break;
			case 'O':
			case 'o':
				{
				alt7=2;
				}
				break;
			case '+':
			case '-':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
				alt7=3;
				}
				break;
			default:
				NoViableAltException nvae =
					new NoViableAltException("", 7, 0, input);
				throw nvae;
			}
			switch (alt7) {
				case 1 :
					// TarsLexer.g:89:7: ( '0X' | '0x' ) ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' )+
					{
					// TarsLexer.g:89:7: ( '0X' | '0x' )
					int alt2=2;
					int LA2_0 = input.LA(1);
					if ( (LA2_0=='0') ) {
						int LA2_1 = input.LA(2);
						if ( (LA2_1=='X') ) {
							alt2=1;
						}
						else if ( (LA2_1=='x') ) {
							alt2=2;
						}

						else {
							int nvaeMark = input.mark();
							try {
								input.consume();
								NoViableAltException nvae =
									new NoViableAltException("", 2, 1, input);
								throw nvae;
							} finally {
								input.rewind(nvaeMark);
							}
						}

					}

					else {
						NoViableAltException nvae =
							new NoViableAltException("", 2, 0, input);
						throw nvae;
					}

					switch (alt2) {
						case 1 :
							// TarsLexer.g:89:8: '0X'
							{
							match("0X"); 

							}
							break;
						case 2 :
							// TarsLexer.g:89:13: '0x'
							{
							match("0x"); 

							}
							break;

					}

					// TarsLexer.g:89:18: ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' )+
					int cnt3=0;
					loop3:
					while (true) {
						int alt3=2;
						int LA3_0 = input.LA(1);
						if ( ((LA3_0 >= '0' && LA3_0 <= '9')||(LA3_0 >= 'A' && LA3_0 <= 'F')||(LA3_0 >= 'a' && LA3_0 <= 'f')) ) {
							alt3=1;
						}

						switch (alt3) {
						case 1 :
							// TarsLexer.g:
							{
							if ( (input.LA(1) >= '0' && input.LA(1) <= '9')||(input.LA(1) >= 'A' && input.LA(1) <= 'F')||(input.LA(1) >= 'a' && input.LA(1) <= 'f') ) {
								input.consume();
							}
							else {
								MismatchedSetException mse = new MismatchedSetException(null,input);
								recover(mse);
								throw mse;
							}
							}
							break;

						default :
							if ( cnt3 >= 1 ) break loop3;
							EarlyExitException eee = new EarlyExitException(3, input);
							throw eee;
						}
						cnt3++;
					}

					}
					break;
				case 2 :
					// TarsLexer.g:90:7: ( 'O' | 'o' ) ( '0' .. '7' )+
					{
					if ( input.LA(1)=='O'||input.LA(1)=='o' ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					// TarsLexer.g:90:16: ( '0' .. '7' )+
					int cnt4=0;
					loop4:
					while (true) {
						int alt4=2;
						int LA4_0 = input.LA(1);
						if ( ((LA4_0 >= '0' && LA4_0 <= '7')) ) {
							alt4=1;
						}

						switch (alt4) {
						case 1 :
							// TarsLexer.g:
							{
							if ( (input.LA(1) >= '0' && input.LA(1) <= '7') ) {
								input.consume();
							}
							else {
								MismatchedSetException mse = new MismatchedSetException(null,input);
								recover(mse);
								throw mse;
							}
							}
							break;

						default :
							if ( cnt4 >= 1 ) break loop4;
							EarlyExitException eee = new EarlyExitException(4, input);
							throw eee;
						}
						cnt4++;
					}

					}
					break;
				case 3 :
					// TarsLexer.g:91:7: ( '+' | '-' )? ( '0' .. '9' )+
					{
					// TarsLexer.g:91:7: ( '+' | '-' )?
					int alt5=2;
					int LA5_0 = input.LA(1);
					if ( (LA5_0=='+'||LA5_0=='-') ) {
						alt5=1;
					}
					switch (alt5) {
						case 1 :
							// TarsLexer.g:
							{
							if ( input.LA(1)=='+'||input.LA(1)=='-' ) {
								input.consume();
							}
							else {
								MismatchedSetException mse = new MismatchedSetException(null,input);
								recover(mse);
								throw mse;
							}
							}
							break;

					}

					// TarsLexer.g:91:17: ( '0' .. '9' )+
					int cnt6=0;
					loop6:
					while (true) {
						int alt6=2;
						int LA6_0 = input.LA(1);
						if ( ((LA6_0 >= '0' && LA6_0 <= '9')) ) {
							alt6=1;
						}

						switch (alt6) {
						case 1 :
							// TarsLexer.g:
							{
							if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
								input.consume();
							}
							else {
								MismatchedSetException mse = new MismatchedSetException(null,input);
								recover(mse);
								throw mse;
							}
							}
							break;

						default :
							if ( cnt6 >= 1 ) break loop6;
							EarlyExitException eee = new EarlyExitException(6, input);
							throw eee;
						}
						cnt6++;
					}

					}
					break;

			}
			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_INTEGER_LITERAL"

	// $ANTLR start "TARS_FLOATING_POINT_LITERAL"
	public final void mTARS_FLOATING_POINT_LITERAL() throws RecognitionException {
		try {
			int _type = TARS_FLOATING_POINT_LITERAL;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:95:5: ( ( '+' | '-' )? ( '0' .. '9' )+ '.' ( '0' .. '9' )+ )
			// TarsLexer.g:95:9: ( '+' | '-' )? ( '0' .. '9' )+ '.' ( '0' .. '9' )+
			{
			// TarsLexer.g:95:9: ( '+' | '-' )?
			int alt8=2;
			int LA8_0 = input.LA(1);
			if ( (LA8_0=='+'||LA8_0=='-') ) {
				alt8=1;
			}
			switch (alt8) {
				case 1 :
					// TarsLexer.g:
					{
					if ( input.LA(1)=='+'||input.LA(1)=='-' ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

			}

			// TarsLexer.g:95:20: ( '0' .. '9' )+
			int cnt9=0;
			loop9:
			while (true) {
				int alt9=2;
				int LA9_0 = input.LA(1);
				if ( ((LA9_0 >= '0' && LA9_0 <= '9')) ) {
					alt9=1;
				}

				switch (alt9) {
				case 1 :
					// TarsLexer.g:
					{
					if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					if ( cnt9 >= 1 ) break loop9;
					EarlyExitException eee = new EarlyExitException(9, input);
					throw eee;
				}
				cnt9++;
			}

			match('.'); 
			// TarsLexer.g:95:36: ( '0' .. '9' )+
			int cnt10=0;
			loop10:
			while (true) {
				int alt10=2;
				int LA10_0 = input.LA(1);
				if ( ((LA10_0 >= '0' && LA10_0 <= '9')) ) {
					alt10=1;
				}

				switch (alt10) {
				case 1 :
					// TarsLexer.g:
					{
					if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					if ( cnt10 >= 1 ) break loop10;
					EarlyExitException eee = new EarlyExitException(10, input);
					throw eee;
				}
				cnt10++;
			}

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_FLOATING_POINT_LITERAL"

	// $ANTLR start "TARS_STRING_LITERAL"
	public final void mTARS_STRING_LITERAL() throws RecognitionException {
		try {
			int _type = TARS_STRING_LITERAL;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:99:5: ( '\"' ( ESC_SEQ |~ ( '\\\\' | '\"' ) )* '\"' )
			// TarsLexer.g:99:7: '\"' ( ESC_SEQ |~ ( '\\\\' | '\"' ) )* '\"'
			{
			match('\"'); 
			// TarsLexer.g:99:11: ( ESC_SEQ |~ ( '\\\\' | '\"' ) )*
			loop11:
			while (true) {
				int alt11=3;
				int LA11_0 = input.LA(1);
				if ( (LA11_0=='\\') ) {
					alt11=1;
				}
				else if ( ((LA11_0 >= '\u0000' && LA11_0 <= '!')||(LA11_0 >= '#' && LA11_0 <= '[')||(LA11_0 >= ']' && LA11_0 <= '\uFFFF')) ) {
					alt11=2;
				}

				switch (alt11) {
				case 1 :
					// TarsLexer.g:99:13: ESC_SEQ
					{
					mESC_SEQ(); 

					}
					break;
				case 2 :
					// TarsLexer.g:99:23: ~ ( '\\\\' | '\"' )
					{
					if ( (input.LA(1) >= '\u0000' && input.LA(1) <= '!')||(input.LA(1) >= '#' && input.LA(1) <= '[')||(input.LA(1) >= ']' && input.LA(1) <= '\uFFFF') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					break loop11;
				}
			}

			match('\"'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "TARS_STRING_LITERAL"

	// $ANTLR start "LPAREN"
	public final void mLPAREN() throws RecognitionException {
		try {
			int _type = LPAREN;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:103:5: ( '(' )
			// TarsLexer.g:103:9: '('
			{
			match('('); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "LPAREN"

	// $ANTLR start "RPAREN"
	public final void mRPAREN() throws RecognitionException {
		try {
			int _type = RPAREN;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:107:5: ( ')' )
			// TarsLexer.g:107:9: ')'
			{
			match(')'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "RPAREN"

	// $ANTLR start "LBRACE"
	public final void mLBRACE() throws RecognitionException {
		try {
			int _type = LBRACE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:111:5: ( '{' )
			// TarsLexer.g:111:9: '{'
			{
			match('{'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "LBRACE"

	// $ANTLR start "RBRACE"
	public final void mRBRACE() throws RecognitionException {
		try {
			int _type = RBRACE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:115:5: ( '}' )
			// TarsLexer.g:115:9: '}'
			{
			match('}'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "RBRACE"

	// $ANTLR start "LBRACKET"
	public final void mLBRACKET() throws RecognitionException {
		try {
			int _type = LBRACKET;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:119:5: ( '[' )
			// TarsLexer.g:119:9: '['
			{
			match('['); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "LBRACKET"

	// $ANTLR start "RBRACKET"
	public final void mRBRACKET() throws RecognitionException {
		try {
			int _type = RBRACKET;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:123:5: ( ']' )
			// TarsLexer.g:123:9: ']'
			{
			match(']'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "RBRACKET"

	// $ANTLR start "SEMI"
	public final void mSEMI() throws RecognitionException {
		try {
			int _type = SEMI;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:127:2: ( ';' )
			// TarsLexer.g:127:4: ';'
			{
			match(';'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "SEMI"

	// $ANTLR start "COMMA"
	public final void mCOMMA() throws RecognitionException {
		try {
			int _type = COMMA;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:131:2: ( ',' )
			// TarsLexer.g:131:4: ','
			{
			match(','); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "COMMA"

	// $ANTLR start "QUOTE"
	public final void mQUOTE() throws RecognitionException {
		try {
			int _type = QUOTE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:135:2: ( '\"' )
			// TarsLexer.g:135:4: '\"'
			{
			match('\"'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "QUOTE"

	// $ANTLR start "DOT"
	public final void mDOT() throws RecognitionException {
		try {
			int _type = DOT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:139:2: ( '.' )
			// TarsLexer.g:139:4: '.'
			{
			match('.'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "DOT"

	// $ANTLR start "COLON"
	public final void mCOLON() throws RecognitionException {
		try {
			int _type = COLON;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:143:5: ( ':' )
			// TarsLexer.g:143:9: ':'
			{
			match(':'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "COLON"

	// $ANTLR start "EQ"
	public final void mEQ() throws RecognitionException {
		try {
			int _type = EQ;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:147:2: ( '=' )
			// TarsLexer.g:147:4: '='
			{
			match('='); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "EQ"

	// $ANTLR start "GT"
	public final void mGT() throws RecognitionException {
		try {
			int _type = GT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:151:2: ( '>' )
			// TarsLexer.g:151:4: '>'
			{
			match('>'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "GT"

	// $ANTLR start "LT"
	public final void mLT() throws RecognitionException {
		try {
			int _type = LT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:155:2: ( '<' )
			// TarsLexer.g:155:4: '<'
			{
			match('<'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "LT"

	// $ANTLR start "COMMENT"
	public final void mCOMMENT() throws RecognitionException {
		try {
			int _type = COMMENT;
			int _channel = DEFAULT_TOKEN_CHANNEL;

					boolean isDoc = false;
				
			// TarsLexer.g:162:5: ( '//' (~ ( '\\n' | '\\r' ) )* ( '\\r' )? '\\n' | '/*' ( options {greedy=false; } : . )* '*/' )
			int alt15=2;
			int LA15_0 = input.LA(1);
			if ( (LA15_0=='/') ) {
				int LA15_1 = input.LA(2);
				if ( (LA15_1=='/') ) {
					alt15=1;
				}
				else if ( (LA15_1=='*') ) {
					alt15=2;
				}

				else {
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 15, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				NoViableAltException nvae =
					new NoViableAltException("", 15, 0, input);
				throw nvae;
			}

			switch (alt15) {
				case 1 :
					// TarsLexer.g:162:9: '//' (~ ( '\\n' | '\\r' ) )* ( '\\r' )? '\\n'
					{
					match("//"); 

					// TarsLexer.g:162:14: (~ ( '\\n' | '\\r' ) )*
					loop12:
					while (true) {
						int alt12=2;
						int LA12_0 = input.LA(1);
						if ( ((LA12_0 >= '\u0000' && LA12_0 <= '\t')||(LA12_0 >= '\u000B' && LA12_0 <= '\f')||(LA12_0 >= '\u000E' && LA12_0 <= '\uFFFF')) ) {
							alt12=1;
						}

						switch (alt12) {
						case 1 :
							// TarsLexer.g:
							{
							if ( (input.LA(1) >= '\u0000' && input.LA(1) <= '\t')||(input.LA(1) >= '\u000B' && input.LA(1) <= '\f')||(input.LA(1) >= '\u000E' && input.LA(1) <= '\uFFFF') ) {
								input.consume();
							}
							else {
								MismatchedSetException mse = new MismatchedSetException(null,input);
								recover(mse);
								throw mse;
							}
							}
							break;

						default :
							break loop12;
						}
					}

					// TarsLexer.g:162:28: ( '\\r' )?
					int alt13=2;
					int LA13_0 = input.LA(1);
					if ( (LA13_0=='\r') ) {
						alt13=1;
					}
					switch (alt13) {
						case 1 :
							// TarsLexer.g:162:28: '\\r'
							{
							match('\r'); 
							}
							break;

					}

					match('\n'); 
					skip();
					}
					break;
				case 2 :
					// TarsLexer.g:163:9: '/*' ( options {greedy=false; } : . )* '*/'
					{
					match("/*"); 


					                if((char)input.LA(1) == '*'){
					                    isDoc = true;
					                }
					            
					// TarsLexer.g:169:9: ( options {greedy=false; } : . )*
					loop14:
					while (true) {
						int alt14=2;
						int LA14_0 = input.LA(1);
						if ( (LA14_0=='*') ) {
							int LA14_1 = input.LA(2);
							if ( (LA14_1=='/') ) {
								alt14=2;
							}
							else if ( ((LA14_1 >= '\u0000' && LA14_1 <= '.')||(LA14_1 >= '0' && LA14_1 <= '\uFFFF')) ) {
								alt14=1;
							}

						}
						else if ( ((LA14_0 >= '\u0000' && LA14_0 <= ')')||(LA14_0 >= '+' && LA14_0 <= '\uFFFF')) ) {
							alt14=1;
						}

						switch (alt14) {
						case 1 :
							// TarsLexer.g:169:36: .
							{
							matchAny(); 
							}
							break;

						default :
							break loop14;
						}
					}

					match("*/"); 


					                if(isDoc==true){
					                    _channel=HIDDEN;
					                } else{
					                    skip();
					                }
					            
					}
					break;

			}
			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "COMMENT"

	// $ANTLR start "WS"
	public final void mWS() throws RecognitionException {
		try {
			int _type = WS;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// TarsLexer.g:180:5: ( ( ' ' | '\\t' | '\\r' | '\\n' ) )
			// TarsLexer.g:180:9: ( ' ' | '\\t' | '\\r' | '\\n' )
			{
			if ( (input.LA(1) >= '\t' && input.LA(1) <= '\n')||input.LA(1)=='\r'||input.LA(1)==' ' ) {
				input.consume();
			}
			else {
				MismatchedSetException mse = new MismatchedSetException(null,input);
				recover(mse);
				throw mse;
			}
			skip();
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "WS"

	// $ANTLR start "HEX_DIGIT"
	public final void mHEX_DIGIT() throws RecognitionException {
		try {
			// TarsLexer.g:188:11: ( ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' ) )
			// TarsLexer.g:
			{
			if ( (input.LA(1) >= '0' && input.LA(1) <= '9')||(input.LA(1) >= 'A' && input.LA(1) <= 'F')||(input.LA(1) >= 'a' && input.LA(1) <= 'f') ) {
				input.consume();
			}
			else {
				MismatchedSetException mse = new MismatchedSetException(null,input);
				recover(mse);
				throw mse;
			}
			}

		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "HEX_DIGIT"

	// $ANTLR start "ESC_SEQ"
	public final void mESC_SEQ() throws RecognitionException {
		try {
			// TarsLexer.g:192:5: ( '\\\\' ( 'b' | 't' | 'n' | 'f' | 'r' | '\\\"' | '\\'' | '\\\\' ) | UNICODE_ESC | OCTAL_ESC )
			int alt16=3;
			int LA16_0 = input.LA(1);
			if ( (LA16_0=='\\') ) {
				switch ( input.LA(2) ) {
				case '\"':
				case '\'':
				case '\\':
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
					{
					alt16=1;
					}
					break;
				case 'u':
					{
					alt16=2;
					}
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					{
					alt16=3;
					}
					break;
				default:
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 16, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}
			}

			else {
				NoViableAltException nvae =
					new NoViableAltException("", 16, 0, input);
				throw nvae;
			}

			switch (alt16) {
				case 1 :
					// TarsLexer.g:192:9: '\\\\' ( 'b' | 't' | 'n' | 'f' | 'r' | '\\\"' | '\\'' | '\\\\' )
					{
					match('\\'); 
					if ( input.LA(1)=='\"'||input.LA(1)=='\''||input.LA(1)=='\\'||input.LA(1)=='b'||input.LA(1)=='f'||input.LA(1)=='n'||input.LA(1)=='r'||input.LA(1)=='t' ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;
				case 2 :
					// TarsLexer.g:193:9: UNICODE_ESC
					{
					mUNICODE_ESC(); 

					}
					break;
				case 3 :
					// TarsLexer.g:194:9: OCTAL_ESC
					{
					mOCTAL_ESC(); 

					}
					break;

			}
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "ESC_SEQ"

	// $ANTLR start "OCTAL_ESC"
	public final void mOCTAL_ESC() throws RecognitionException {
		try {
			// TarsLexer.g:199:5: ( '\\\\' ( '0' .. '3' ) ( '0' .. '7' ) ( '0' .. '7' ) | '\\\\' ( '0' .. '7' ) ( '0' .. '7' ) | '\\\\' ( '0' .. '7' ) )
			int alt17=3;
			int LA17_0 = input.LA(1);
			if ( (LA17_0=='\\') ) {
				int LA17_1 = input.LA(2);
				if ( ((LA17_1 >= '0' && LA17_1 <= '3')) ) {
					int LA17_2 = input.LA(3);
					if ( ((LA17_2 >= '0' && LA17_2 <= '7')) ) {
						int LA17_4 = input.LA(4);
						if ( ((LA17_4 >= '0' && LA17_4 <= '7')) ) {
							alt17=1;
						}

						else {
							alt17=2;
						}

					}

					else {
						alt17=3;
					}

				}
				else if ( ((LA17_1 >= '4' && LA17_1 <= '7')) ) {
					int LA17_3 = input.LA(3);
					if ( ((LA17_3 >= '0' && LA17_3 <= '7')) ) {
						alt17=2;
					}

					else {
						alt17=3;
					}

				}

				else {
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 17, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				NoViableAltException nvae =
					new NoViableAltException("", 17, 0, input);
				throw nvae;
			}

			switch (alt17) {
				case 1 :
					// TarsLexer.g:199:9: '\\\\' ( '0' .. '3' ) ( '0' .. '7' ) ( '0' .. '7' )
					{
					match('\\'); 
					if ( (input.LA(1) >= '0' && input.LA(1) <= '3') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					if ( (input.LA(1) >= '0' && input.LA(1) <= '7') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					if ( (input.LA(1) >= '0' && input.LA(1) <= '7') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;
				case 2 :
					// TarsLexer.g:200:9: '\\\\' ( '0' .. '7' ) ( '0' .. '7' )
					{
					match('\\'); 
					if ( (input.LA(1) >= '0' && input.LA(1) <= '7') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					if ( (input.LA(1) >= '0' && input.LA(1) <= '7') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;
				case 3 :
					// TarsLexer.g:201:9: '\\\\' ( '0' .. '7' )
					{
					match('\\'); 
					if ( (input.LA(1) >= '0' && input.LA(1) <= '7') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

			}
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "OCTAL_ESC"

	// $ANTLR start "UNICODE_ESC"
	public final void mUNICODE_ESC() throws RecognitionException {
		try {
			// TarsLexer.g:206:5: ( '\\\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT )
			// TarsLexer.g:206:9: '\\\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
			{
			match('\\'); 
			match('u'); 
			mHEX_DIGIT(); 

			mHEX_DIGIT(); 

			mHEX_DIGIT(); 

			mHEX_DIGIT(); 

			}

		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "UNICODE_ESC"

	@Override
	public void mTokens() throws RecognitionException {
		// TarsLexer.g:1:8: ( TARS_VOID | TARS_STRUCT | TARS_UNSIGNED | TARS_BOOL | TARS_BYTE | TARS_SHORT | TARS_INT | TARS_DOUBLE | TARS_FLOAT | TARS_LONG | TARS_STRING | TARS_VECTOR | TARS_MAP | TARS_KEY | TARS_ROUTE_KEY | TARS_INCLUDE | TARS_NAMESPACE | TARS_INTERFACE | TARS_OUT | TARS_REQUIRE | TARS_OPTIONAL | TARS_FALSE | TARS_TRUE | TARS_ENUM | TARS_CONST | TARS_IDENTIFIER | TARS_INTEGER_LITERAL | TARS_FLOATING_POINT_LITERAL | TARS_STRING_LITERAL | LPAREN | RPAREN | LBRACE | RBRACE | LBRACKET | RBRACKET | SEMI | COMMA | QUOTE | DOT | COLON | EQ | GT | LT | COMMENT | WS )
		int alt18=45;
		alt18 = dfa18.predict(input);
		switch (alt18) {
			case 1 :
				// TarsLexer.g:1:10: TARS_VOID
				{
				mTARS_VOID(); 

				}
				break;
			case 2 :
				// TarsLexer.g:1:20: TARS_STRUCT
				{
				mTARS_STRUCT(); 

				}
				break;
			case 3 :
				// TarsLexer.g:1:32: TARS_UNSIGNED
				{
				mTARS_UNSIGNED(); 

				}
				break;
			case 4 :
				// TarsLexer.g:1:46: TARS_BOOL
				{
				mTARS_BOOL(); 

				}
				break;
			case 5 :
				// TarsLexer.g:1:56: TARS_BYTE
				{
				mTARS_BYTE(); 

				}
				break;
			case 6 :
				// TarsLexer.g:1:66: TARS_SHORT
				{
				mTARS_SHORT(); 

				}
				break;
			case 7 :
				// TarsLexer.g:1:77: TARS_INT
				{
				mTARS_INT(); 

				}
				break;
			case 8 :
				// TarsLexer.g:1:86: TARS_DOUBLE
				{
				mTARS_DOUBLE(); 

				}
				break;
			case 9 :
				// TarsLexer.g:1:98: TARS_FLOAT
				{
				mTARS_FLOAT(); 

				}
				break;
			case 10 :
				// TarsLexer.g:1:109: TARS_LONG
				{
				mTARS_LONG(); 

				}
				break;
			case 11 :
				// TarsLexer.g:1:119: TARS_STRING
				{
				mTARS_STRING(); 

				}
				break;
			case 12 :
				// TarsLexer.g:1:131: TARS_VECTOR
				{
				mTARS_VECTOR(); 

				}
				break;
			case 13 :
				// TarsLexer.g:1:143: TARS_MAP
				{
				mTARS_MAP(); 

				}
				break;
			case 14 :
				// TarsLexer.g:1:152: TARS_KEY
				{
				mTARS_KEY(); 

				}
				break;
			case 15 :
				// TarsLexer.g:1:161: TARS_ROUTE_KEY
				{
				mTARS_ROUTE_KEY(); 

				}
				break;
			case 16 :
				// TarsLexer.g:1:176: TARS_INCLUDE
				{
				mTARS_INCLUDE(); 

				}
				break;
			case 17 :
				// TarsLexer.g:1:189: TARS_NAMESPACE
				{
				mTARS_NAMESPACE(); 

				}
				break;
			case 18 :
				// TarsLexer.g:1:204: TARS_INTERFACE
				{
				mTARS_INTERFACE(); 

				}
				break;
			case 19 :
				// TarsLexer.g:1:219: TARS_OUT
				{
				mTARS_OUT(); 

				}
				break;
			case 20 :
				// TarsLexer.g:1:228: TARS_REQUIRE
				{
				mTARS_REQUIRE(); 

				}
				break;
			case 21 :
				// TarsLexer.g:1:241: TARS_OPTIONAL
				{
				mTARS_OPTIONAL(); 

				}
				break;
			case 22 :
				// TarsLexer.g:1:255: TARS_FALSE
				{
				mTARS_FALSE(); 

				}
				break;
			case 23 :
				// TarsLexer.g:1:266: TARS_TRUE
				{
				mTARS_TRUE(); 

				}
				break;
			case 24 :
				// TarsLexer.g:1:276: TARS_ENUM
				{
				mTARS_ENUM(); 

				}
				break;
			case 25 :
				// TarsLexer.g:1:286: TARS_CONST
				{
				mTARS_CONST(); 

				}
				break;
			case 26 :
				// TarsLexer.g:1:297: TARS_IDENTIFIER
				{
				mTARS_IDENTIFIER(); 

				}
				break;
			case 27 :
				// TarsLexer.g:1:313: TARS_INTEGER_LITERAL
				{
				mTARS_INTEGER_LITERAL(); 

				}
				break;
			case 28 :
				// TarsLexer.g:1:334: TARS_FLOATING_POINT_LITERAL
				{
				mTARS_FLOATING_POINT_LITERAL(); 

				}
				break;
			case 29 :
				// TarsLexer.g:1:362: TARS_STRING_LITERAL
				{
				mTARS_STRING_LITERAL(); 

				}
				break;
			case 30 :
				// TarsLexer.g:1:382: LPAREN
				{
				mLPAREN(); 

				}
				break;
			case 31 :
				// TarsLexer.g:1:389: RPAREN
				{
				mRPAREN(); 

				}
				break;
			case 32 :
				// TarsLexer.g:1:396: LBRACE
				{
				mLBRACE(); 

				}
				break;
			case 33 :
				// TarsLexer.g:1:403: RBRACE
				{
				mRBRACE(); 

				}
				break;
			case 34 :
				// TarsLexer.g:1:410: LBRACKET
				{
				mLBRACKET(); 

				}
				break;
			case 35 :
				// TarsLexer.g:1:419: RBRACKET
				{
				mRBRACKET(); 

				}
				break;
			case 36 :
				// TarsLexer.g:1:428: SEMI
				{
				mSEMI(); 

				}
				break;
			case 37 :
				// TarsLexer.g:1:433: COMMA
				{
				mCOMMA(); 

				}
				break;
			case 38 :
				// TarsLexer.g:1:439: QUOTE
				{
				mQUOTE(); 

				}
				break;
			case 39 :
				// TarsLexer.g:1:445: DOT
				{
				mDOT(); 

				}
				break;
			case 40 :
				// TarsLexer.g:1:449: COLON
				{
				mCOLON(); 

				}
				break;
			case 41 :
				// TarsLexer.g:1:455: EQ
				{
				mEQ(); 

				}
				break;
			case 42 :
				// TarsLexer.g:1:458: GT
				{
				mGT(); 

				}
				break;
			case 43 :
				// TarsLexer.g:1:461: LT
				{
				mLT(); 

				}
				break;
			case 44 :
				// TarsLexer.g:1:464: COMMENT
				{
				mCOMMENT(); 

				}
				break;
			case 45 :
				// TarsLexer.g:1:472: WS
				{
				mWS(); 

				}
				break;

		}
	}


	protected DFA18 dfa18 = new DFA18(this);
	static final String DFA18_eotS =
		"\1\uffff\13\23\1\uffff\5\23\1\75\2\uffff\1\75\1\100\17\uffff\27\23\4\uffff"+
		"\7\23\1\140\4\23\1\145\1\23\1\147\2\23\1\152\4\23\1\157\5\23\1\165\1\166"+
		"\1\23\1\uffff\3\23\1\173\1\uffff\1\23\1\uffff\2\23\1\uffff\1\23\1\u0080"+
		"\1\u0081\1\23\1\uffff\3\23\1\u0086\1\23\2\uffff\2\23\1\u008a\1\u008b\1"+
		"\uffff\4\23\2\uffff\1\u0090\1\u0091\1\u0092\1\u0093\1\uffff\2\23\1\u0096"+
		"\2\uffff\1\u0097\3\23\4\uffff\2\23\2\uffff\1\23\1\u009e\1\23\1\u00a0\1"+
		"\23\1\u00a2\1\uffff\1\u00a3\1\uffff\1\u00a4\3\uffff";
	static final String DFA18_eofS =
		"\u00a5\uffff";
	static final String DFA18_minS =
		"\1\11\1\145\1\150\1\156\1\157\1\156\1\157\1\141\1\157\1\141\2\145\1\uffff"+
		"\1\60\1\162\1\156\1\157\1\60\1\56\1\uffff\1\60\1\56\1\0\17\uffff\1\151"+
		"\1\143\1\162\1\157\1\163\1\157\2\164\1\165\1\157\1\154\1\156\1\160\1\144"+
		"\1\171\1\165\1\161\2\164\1\60\2\165\1\156\4\uffff\1\144\1\164\1\151\1"+
		"\162\1\151\1\154\1\145\1\60\1\142\1\141\1\163\1\147\1\60\1\165\1\60\1"+
		"\164\1\165\1\60\1\151\1\145\1\155\1\163\1\60\1\157\1\143\1\156\1\164\1"+
		"\147\2\60\1\162\1\uffff\1\154\1\164\1\145\1\60\1\uffff\1\154\1\uffff\1"+
		"\145\1\151\1\uffff\1\157\2\60\1\164\1\uffff\1\162\1\164\1\147\1\60\1\156"+
		"\2\uffff\1\146\1\145\2\60\1\uffff\1\145\1\153\1\162\1\156\2\uffff\4\60"+
		"\1\uffff\1\145\1\141\1\60\2\uffff\1\60\2\145\1\141\4\uffff\1\144\1\143"+
		"\2\uffff\1\171\1\60\1\154\1\60\1\145\1\60\1\uffff\1\60\1\uffff\1\60\3"+
		"\uffff";
	static final String DFA18_maxS =
		"\1\175\1\157\1\164\1\156\1\171\1\156\1\157\1\154\2\157\1\145\1\157\1\uffff"+
		"\1\165\1\162\1\156\1\157\1\67\1\71\1\uffff\2\71\1\uffff\17\uffff\1\151"+
		"\1\143\1\162\1\157\1\163\1\157\2\164\1\165\1\157\1\154\1\156\1\160\1\144"+
		"\1\171\1\165\1\161\2\164\1\67\2\165\1\156\4\uffff\1\144\1\164\1\165\1"+
		"\162\1\151\1\154\1\145\1\172\1\142\1\141\1\163\1\147\1\172\1\165\1\172"+
		"\1\164\1\165\1\172\1\151\1\145\1\155\1\163\1\172\1\157\1\143\1\156\1\164"+
		"\1\147\2\172\1\162\1\uffff\1\154\1\164\1\145\1\172\1\uffff\1\154\1\uffff"+
		"\1\145\1\151\1\uffff\1\157\2\172\1\164\1\uffff\1\162\1\164\1\147\1\172"+
		"\1\156\2\uffff\1\146\1\145\2\172\1\uffff\1\145\1\153\1\162\1\156\2\uffff"+
		"\4\172\1\uffff\1\145\1\141\1\172\2\uffff\1\172\2\145\1\141\4\uffff\1\144"+
		"\1\143\2\uffff\1\171\1\172\1\154\1\172\1\145\1\172\1\uffff\1\172\1\uffff"+
		"\1\172\3\uffff";
	static final String DFA18_acceptS =
		"\14\uffff\1\20\6\uffff\1\32\3\uffff\1\36\1\37\1\40\1\41\1\42\1\43\1\44"+
		"\1\45\1\47\1\50\1\51\1\52\1\53\1\54\1\55\27\uffff\1\33\1\34\1\35\1\46"+
		"\37\uffff\1\7\4\uffff\1\15\1\uffff\1\16\2\uffff\1\23\4\uffff\1\1\5\uffff"+
		"\1\4\1\5\4\uffff\1\12\4\uffff\1\27\1\30\4\uffff\1\6\3\uffff\1\11\1\26"+
		"\4\uffff\1\31\1\14\1\2\1\13\2\uffff\1\10\1\21\6\uffff\1\24\1\uffff\1\3"+
		"\1\uffff\1\17\1\25\1\22";
	static final String DFA18_specialS =
		"\26\uffff\1\0\u008e\uffff}>";
	static final String[] DFA18_transitionS = {
			"\2\45\2\uffff\1\45\22\uffff\1\45\1\uffff\1\26\1\14\4\uffff\1\27\1\30"+
			"\1\uffff\1\24\1\36\1\24\1\37\1\44\1\22\11\25\1\40\1\35\1\43\1\41\1\42"+
			"\2\uffff\16\23\1\21\13\23\1\33\1\uffff\1\34\1\uffff\1\23\1\uffff\1\23"+
			"\1\4\1\20\1\6\1\17\1\7\2\23\1\5\1\23\1\12\1\10\1\11\1\23\1\15\2\23\1"+
			"\13\1\2\1\16\1\3\1\1\4\23\1\31\1\uffff\1\32",
			"\1\47\11\uffff\1\46",
			"\1\51\13\uffff\1\50",
			"\1\52",
			"\1\53\11\uffff\1\54",
			"\1\55",
			"\1\56",
			"\1\60\12\uffff\1\57",
			"\1\61",
			"\1\62\15\uffff\1\63",
			"\1\64",
			"\1\66\11\uffff\1\65",
			"",
			"\10\71\70\uffff\1\70\4\uffff\1\67",
			"\1\72",
			"\1\73",
			"\1\74",
			"\10\71",
			"\1\76\1\uffff\12\25",
			"",
			"\12\25",
			"\1\76\1\uffff\12\25",
			"\0\77",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"\1\101",
			"\1\102",
			"\1\103",
			"\1\104",
			"\1\105",
			"\1\106",
			"\1\107",
			"\1\110",
			"\1\111",
			"\1\112",
			"\1\113",
			"\1\114",
			"\1\115",
			"\1\116",
			"\1\117",
			"\1\120",
			"\1\121",
			"\1\122",
			"\1\123",
			"\10\71",
			"\1\124",
			"\1\125",
			"\1\126",
			"",
			"",
			"",
			"",
			"\1\127",
			"\1\130",
			"\1\132\13\uffff\1\131",
			"\1\133",
			"\1\134",
			"\1\135",
			"\1\136",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\4\23\1\137\25\23",
			"\1\141",
			"\1\142",
			"\1\143",
			"\1\144",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\146",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\150",
			"\1\151",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\153",
			"\1\154",
			"\1\155",
			"\1\156",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\160",
			"\1\161",
			"\1\162",
			"\1\163",
			"\1\164",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\167",
			"",
			"\1\170",
			"\1\171",
			"\1\172",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"",
			"\1\174",
			"",
			"\1\175",
			"\1\176",
			"",
			"\1\177",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\u0082",
			"",
			"\1\u0083",
			"\1\u0084",
			"\1\u0085",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\u0087",
			"",
			"",
			"\1\u0088",
			"\1\u0089",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"",
			"\1\u008c",
			"\1\u008d",
			"\1\u008e",
			"\1\u008f",
			"",
			"",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"",
			"\1\u0094",
			"\1\u0095",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"",
			"",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\u0098",
			"\1\u0099",
			"\1\u009a",
			"",
			"",
			"",
			"",
			"\1\u009b",
			"\1\u009c",
			"",
			"",
			"\1\u009d",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\u009f",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"\1\u00a1",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"",
			"\12\23\7\uffff\32\23\4\uffff\1\23\1\uffff\32\23",
			"",
			"",
			""
	};

	static final short[] DFA18_eot = DFA.unpackEncodedString(DFA18_eotS);
	static final short[] DFA18_eof = DFA.unpackEncodedString(DFA18_eofS);
	static final char[] DFA18_min = DFA.unpackEncodedStringToUnsignedChars(DFA18_minS);
	static final char[] DFA18_max = DFA.unpackEncodedStringToUnsignedChars(DFA18_maxS);
	static final short[] DFA18_accept = DFA.unpackEncodedString(DFA18_acceptS);
	static final short[] DFA18_special = DFA.unpackEncodedString(DFA18_specialS);
	static final short[][] DFA18_transition;

	static {
		int numStates = DFA18_transitionS.length;
		DFA18_transition = new short[numStates][];
		for (int i=0; i<numStates; i++) {
			DFA18_transition[i] = DFA.unpackEncodedString(DFA18_transitionS[i]);
		}
	}

	protected class DFA18 extends DFA {

		public DFA18(BaseRecognizer recognizer) {
			this.recognizer = recognizer;
			this.decisionNumber = 18;
			this.eot = DFA18_eot;
			this.eof = DFA18_eof;
			this.min = DFA18_min;
			this.max = DFA18_max;
			this.accept = DFA18_accept;
			this.special = DFA18_special;
			this.transition = DFA18_transition;
		}
		@Override
		public String getDescription() {
			return "1:1: Tokens : ( TARS_VOID | TARS_STRUCT | TARS_UNSIGNED | TARS_BOOL | TARS_BYTE | TARS_SHORT | TARS_INT | TARS_DOUBLE | TARS_FLOAT | TARS_LONG | TARS_STRING | TARS_VECTOR | TARS_MAP | TARS_KEY | TARS_ROUTE_KEY | TARS_INCLUDE | TARS_NAMESPACE | TARS_INTERFACE | TARS_OUT | TARS_REQUIRE | TARS_OPTIONAL | TARS_FALSE | TARS_TRUE | TARS_ENUM | TARS_CONST | TARS_IDENTIFIER | TARS_INTEGER_LITERAL | TARS_FLOATING_POINT_LITERAL | TARS_STRING_LITERAL | LPAREN | RPAREN | LBRACE | RBRACE | LBRACKET | RBRACKET | SEMI | COMMA | QUOTE | DOT | COLON | EQ | GT | LT | COMMENT | WS );";
		}
		@Override
		public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
			IntStream input = _input;
			int _s = s;
			switch ( s ) {
					case 0 : 
						int LA18_22 = input.LA(1);
						s = -1;
						if ( ((LA18_22 >= '\u0000' && LA18_22 <= '\uFFFF')) ) {s = 63;}
						else s = 64;
						if ( s>=0 ) return s;
						break;
			}
			NoViableAltException nvae =
				new NoViableAltException(getDescription(), 18, _s, input);
			error(nvae);
			throw nvae;
		}
	}

}
