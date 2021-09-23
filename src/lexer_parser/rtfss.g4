//lexer grammar rtfss_lexer;
grammar rtfss;

//statm=statement
//decl=declaration
//assign=assignment

entry_point: use_stat* cblock_decl* EOF;

use_stat: USE std_id=(IDENTIFIER | STRING_LITERAL); 

cblock_decl: CBLOCK PULSE_SEP pulse_freq IDENTIFIER cblock_args code_block; 

cblock_args: PAR_OPEN (const_args=cblock_arg_list ARG_GROUP_SEP)? in_args=cblock_arg_list ARG_GROUP_SEP out_args=cblock_arg_list PAR_CLOSE; 
cblock_arg_list: ((data_type var_name COMMA)* data_type var_name)?;

code_block: CURLY_OPEN statm* CURLY_CLOSE;

statm: nonterminated_statm | terminated_statm TERMINATOR;

terminated_statm: var_assign | data_decl | pulse_decl | cblock_inst;
nonterminated_statm: if_statm | for_statm;

if_statm: IF PAR_OPEN expr PAR_CLOSE code_block 
			(ELSEIF PAR_OPEN expr PAR_CLOSE code_block)*
			(ELSE code_block)?;

for_statm: FOR PAR_OPEN IDENTIFIER IN expr TO expr (INC expr)? PAR_CLOSE code_block;

cblock_inst: BRAC_OPEN (oargs+=var_name COMMA)* oargs+=var_name BRAC_CLOSE EQUAL cblock_inst_short;
cblock_inst_short: IDENTIFIER PULSE_SEP pulse_freq PAR_OPEN ((cargs+=expr COMMA)* cargs+=expr)? (ARG_GROUP_SEP ((iargs+=expr COMMA)* iargs+=expr)?)? PAR_CLOSE;

pulse_decl: PULSE IDENTIFIER PULSE_LITERAL;  
data_decl: data_type PULSE_SEP pulse_freq stream_id (EQUAL expr)?;
var_assign: stream_id assign_op expr; 
assign_op: EQUAL | COMBOP;

expr: 	
		SUB expr									#negExpr
	|	PAR_OPEN data_type PAR_CLOSE expr			#castAExpr
	|	data_type PAR_OPEN expr	PAR_CLOSE 			#castBExpr
	|	cblock_inst_short							#cblkExpr
	|	PAR_OPEN expr PAR_CLOSE						#parExpr
	|	CURLY_OPEN (expr COMMA)* expr CURLY_CLOSE	#arrExpr
	|	PROP expr									#propExpr
	|	expr GAP expr								#gapExpr
	|	expr (SL|SR) expr							#shiftExpr
	|	expr (RL|RR) expr							#rotExpr
	|	expr AND expr								#andExpr 
	|	expr OR expr								#orExpr 
	|	expr XOR expr								#xorExpr 
	|	NOT expr									#notExpr 
	|	expr MULT expr								#multExpr 
	|	expr DIV expr								#divExpr
	|	expr MOD expr								#modExpr
	|	expr ADD expr								#addExpr
	|	expr SUB expr								#subExpr
	|	expr LOGIC_OP expr							#evalExpr

	//Midi operators
	|	NOTEOF PAR_OPEN expr COMMA expr ARG_GROUP_SEP expr COMMA expr PAR_CLOSE	#noteofExpr
	|	FREQOF PAR_OPEN expr COMMA expr ARG_GROUP_SEP expr COMMA expr PAR_CLOSE	#freqofExpr 
	|	VELOF PAR_OPEN expr COMMA expr ARG_GROUP_SEP expr COMMA expr PAR_CLOSE	#velofExpr 
	|	PATOF PAR_OPEN expr COMMA expr ARG_GROUP_SEP expr COMMA expr PAR_CLOSE	#patofExpr 
	|	NNTOF PAR_OPEN expr COMMA expr ARG_GROUP_SEP expr COMMA expr PAR_CLOSE	#nntofExpr 
	|	CCOF  PAR_OPEN expr COMMA expr COMMA expr PAR_CLOSE						#ccofExpr 
	|	POF PAR_OPEN expr PAR_CLOSE												#pofExpr
	|	CPOF  PAR_OPEN expr PAR_CLOSE											#cpofExpr
	|	PBEND PAR_OPEN expr PAR_CLOSE											#pbendExpr

	//End nodes
	|	stream_id									#sidExpr
	|	NUM_LITERAL									#numExpr
	;


var_name: IDENTIFIER (BRAC_OPEN expr BRAC_CLOSE)?;
stream_id: IDENTIFIER (BRAC_OPEN idx=expr BRAC_CLOSE)? (INS ins=expr)?;
data_type: fixedSizeType | varSizeType;

fixedSizeType: MIDI_TYPE; //TODO FIX
varSizeType: SIGNED_TYPE | UNSIGNED_TYPE | FLOATING_TYPE;

pulse_freq: MAX | CONST | IDENTIFIER;

USE: 'use';
CBLOCK: 'cblock';
PULSE: 'pulse';
IF: 'if';
ELSEIF: 'elseif';
ELSE: 'else';
FOR: 'for';
IN: 'in';
TO: 'to';
INC: 'inc';

CURLY_OPEN: '{';
CURLY_CLOSE: '}';

BRAC_OPEN: '[';
BRAC_CLOSE: ']';

PAR_OPEN: '(';
PAR_CLOSE: ')';

ARG_GROUP_SEP: ':';
COMMA: ',';

PULSE_SEP: '@';

EQUAL: '=';
ADD: '+';
SUB: '-';
MULT: '*';
DIV: '/';
MOD: '%';
SL: '<<';
SR: '>>';
RL: '<<<';
RR: '>>>';
AND:'and';
OR:'or';
XOR:'xor';
NOT:'not';
GAP: 'gap';
PROP: '&';
INS: '\'';

//MIDI OPs
NOTEOF: 'noteof';
FREQOF: 'freqof';
VELOF: 'velof';
PATOF: 'patof';
NNTOF: 'nntof';
CCOF: 'ccof';
POF: 'pof';
CPOF: 'cpof';
PBEND: 'pbend';

COMBOP: [+\-*/%] EQUAL; //Combined and equal

TERMINATOR: ';';
LOGIC_OP: ([<>]'='?) | ([!=] '=');

fragment SIGNED_TYPE_PREFIX: 'I';
fragment UNSIGNED_TYPE_PREFIX: 'U';
fragment FLOATING_TYPE_PREFIX: 'F';

MAX: 'max';
CONST:'const';

MIDI_TYPE: 'midi';
SIGNED_TYPE: SIGNED_TYPE_PREFIX (DEC_LITERAL|IDENTIFIER);
UNSIGNED_TYPE: UNSIGNED_TYPE_PREFIX (DEC_LITERAL|IDENTIFIER); 
FLOATING_TYPE: FLOATING_TYPE_PREFIX (DEC_INT|IDENTIFIER); 

NUM_LITERAL: OCT_LITERAL|DEC_LITERAL|HEX_LITERAL;

OCT_LITERAL: OCT_INT|OCT_FRAC;
DEC_LITERAL: DEC_INT|DEC_FRAC;
HEX_LITERAL: HEX_INT|HEX_FRAC;

PULSE_LITERAL: NUM_LITERAL PULSE_LITERAL_SUFFIX;
fragment PULSE_LITERAL_SUFFIX: SECOND | MILLISECOND | HERTZ | KILOHERTZ; 
fragment SECOND: 's';
fragment MILLISECOND: 'ms';
fragment HERTZ: 'Hz';
fragment KILOHERTZ: 'kHz';

fragment OCT_PREFIX: '0o';
fragment DEC_PREFIX: '0d';
fragment HEX_PREFIX: '0x' | '0h';
fragment FRAC_SEPARATOR: '.';

fragment OCT_DIGIT: [0-7];
fragment DEC_DIGIT: [8-9]|OCT_DIGIT;
fragment HEX_DIGIT: [A-Fa-f]|DEC_DIGIT;

fragment OCT_INT: OCT_PREFIX OCT_DIGIT+;
fragment DEC_INT: DEC_PREFIX? DEC_DIGIT+;
fragment HEX_INT: HEX_PREFIX HEX_DIGIT+;

fragment OCT_FRAC: OCT_PREFIX (OCT_DIGIT+ FRAC_SEPARATOR OCT_DIGIT* | OCT_DIGIT* FRAC_SEPARATOR OCT_DIGIT+);
fragment DEC_FRAC: DEC_PREFIX? (DEC_DIGIT+ FRAC_SEPARATOR DEC_DIGIT* | DEC_DIGIT* FRAC_SEPARATOR DEC_DIGIT+);
fragment HEX_FRAC: HEX_PREFIX (HEX_DIGIT+ FRAC_SEPARATOR HEX_DIGIT* | HEX_DIGIT* FRAC_SEPARATOR HEX_DIGIT+);

IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*;
STRING_LITERAL: '"' .*? '"';

BLOCK_COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;
WS: [ \t\n\r] -> skip;
