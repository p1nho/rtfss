#include "ast.h"
#include <cctype>

//pulse_id
pulse_id::pulse_type pulse_id::str2pulse(std::string &pulse){
	//Possible types = NORMAL,MAX,CONST,PUNDEF
	
	if(pulse=="const")
		return CONST;
	else if(pulse=="max")
		return MAX;
	else if(pulse.length()!=0)
		return NORMAL;
	else{
		std::cout<<"UNDEFINED PULSE"<<std::endl;

		return PUNDEF;
	}
}

std::string pulse_id::to_string(){
	return pulse_name;
}

pulse_id::pulse_type pulse_id::get_pulse_type(){
	return pt;
}

//datatype
datatype::datatype(std::string dt){
	//TODO optimize
	//Possible types = SIGNED,UNSIGNED,MIDI,FLOAT
	if(dt=="midi"){
		this->dt=MIDI;
	}
	else{
		char &t(dt[0]); //indicates the type
		std::string n(dt.substr(1)); //indicates the dimensions

		switch(t){ //Solve type
			case 'I': //Signed
				this->dt=SIGNED;
				break;
			case 'U': //Unsigned
				this->dt=UNSIGNED;
				break;
			case 'F': //Float
				this->dt=FLOAT;
				break;
			default:
				this->dt=SIGNED;
				std::cout<<"UNDEFINED DATATYPE"<<std::endl;
				break;
		}

		//Solve dimension
		size_t del_pos=n.find('.');

		bool bounded = std::find_if_not(n.begin(),n.end(),
			[](char c) {return std::isdigit(c)||c=='.';})!=
			n.end();	//true if bounded to a const var

		if(bounded){ //Bounded type, not solved on first pass
			integ=0;
			frac=0;
			idsize=n;
		}
		else if(del_pos==std::string::npos){ //"Fully" Integer type
			//ex I13
			integ=std::stoi(n);		
			frac=0;
		}
		else if(del_pos==0){ //only frac type
			//ex I.3
			integ=0;
			frac=std::stoi(n.substr(1));
		}
		else if(del_pos==n.length()-1){ //only integ type
			//ex I13.
			integ=std::stoi(n.substr(0,del_pos));
			frac=0;
		}
		else{ //mixed
			//ex I13.3
			integ=std::stoi(n.substr(0,del_pos));
			frac=std::stoi(n.substr(del_pos+1,std::string::npos));
		}
	}
}

std::string datatype::to_string(){
	std::stringstream ss;
	switch(dt){
		case SIGNED:
			ss<<"I";
			break;
		case UNSIGNED:
			ss<<"U";
			break;
		case FLOAT:
			ss<<"F";
			break;
		case MIDI:
			return "midi";;
		default:
			return "";
	}
	if(idsize.length()!=0){
		ss<<idsize;
	}
	else{
		ss<<integ<<"."<<frac;
	}
	return ss.str();
}

datatype::dt_types datatype::get_datatype(){
	return dt;
}

unsigned int datatype::get_integ(){
	return integ;
}

unsigned int datatype::get_frac(){
	return frac;
}

unsigned int datatype::get_dim(){
	return integ+frac;
}

bool datatype::equal(datatype &dt){
	return this->dt==dt.get_datatype() &&
			integ==dt.get_integ() &&
			frac==dt.get_frac();
}

//time_scale
time_scale::ts_types time_scale::str2time_scale(std::string &ts){
	//Possible types = SECOND,MILLISECOND,HERTZ,KILOHERTZ

	if(ts=="Hz")
		return HERTZ;
	else if(ts=="kHz")
		return HERTZ;
	else if(ts=="s")
		return SECOND;
	else if(ts=="ms")
		return MILLISECOND;
	else{
		std::cout<<"UNDEFINED TIME SCALE"<<std::endl;
		return SECOND;
	}
}

std::string time_scale::to_string(){
	switch(ts){
		default:
		case SECOND:
			return "s";
		case MILLISECOND:
			return "ms";
		case HERTZ:
			return "Hz";
		case KILOHERTZ:
			return "kHz";
	}
}

time_scale::ts_types time_scale::get_ts(){
	return ts;
}

//assign_op
assign_op::op_types assign_op::str2assign_op(std::string &op){
	//Possible types = EQUAL,ADDEQ,SUBEQ,DIVEQ,MULEQ,MODEQ,INSEQ

	if(op=="=")
		return EQUAL;
	else if(op=="+=")
		return ADDEQ;
	else if(op=="-=")
		return SUBEQ;
	else if(op=="/=")
		return DIVEQ;
	else if(op=="*=")
		return MULEQ;
	else if(op=="%=")
		return MODEQ;
	else{
		std::cout<<"UNDEFINED ASSIGN OPERAND"<<std::endl;
		return EQUAL;
	}
}

assign_op::op_types assign_op::get_op(){
	return op;
}

std::string assign_op::to_string(){
	switch(op){
		default:
		case EQUAL:
			return "=";
		case ADDEQ:
			return "+=";
		case SUBEQ:
			return "-=";
		case DIVEQ:
			return "/=";
		case MULEQ:
			return "*=";
		case MODEQ:
			return "%=";
	}
}

//logic_op
logic_op::op_types logic_op::str2logic_op(std::string &op){
	//Possible types = EQUAL,MORE,LESS,MOREQ,LESEQ,DIFF

	if(op=="==")
		return EQUAL;
	else if(op=="!=")
		return DIFF;
	else if(op==">")
		return MORE;
	else if(op=="<")
		return LESS;
	else if(op==">=")
		return MOREQ;
	else if(op=="<=")
		return LESEQ;
	else{
		std::cout<<"UNDEFINED LOGIC OPERAND"<<std::endl;
		return EQUAL;
	}
}

logic_op::op_types logic_op::get_op(){
	return op;
}

std::string logic_op::to_string(){
	switch(op){
		default:
		case EQUAL:
			return "==";
		case MORE:
			return ">";
		case LESS:
			return "<";
		case MOREQ:
			return ">=";
		case LESEQ:
			return "<=";
		case DIFF:
			return "!=";
	}
}

//ast
ast::ast_op ast::get_op(){
	return op;
}

std::weak_ptr<ast>& ast::get_parent(){
	return parent;
}

void ast::set_parent(std::weak_ptr<ast>& parent){
	this->parent=parent;
}

//ast_id
void ast_id::set_index(std::shared_ptr<ast_arith> &index){
	this->index=index;
}

void ast_id::set_instant(std::shared_ptr<ast_arith> &instant){
	this->instant=instant;
}

void ast_id::set_datatype(std::string &dt){
	this->dt=datatype(dt);
}

void ast_id::set_pulse(std::string &pid){
	this->pid=pulse_id(pid);
}

std::string& ast_id::get_name(){
	return name;
}

datatype& ast_id::get_datatype(){
	return dt;
}

pulse_id& ast_id::get_pulse_id(){
	return pid;
}

std::shared_ptr<ast_arith>& ast_id::get_index(){
	return index;
}

std::shared_ptr<ast_arith>& ast_id::get_instant(){
	return instant;
}

//ast_start
std::shared_ptr<ast_start> ast_start::build_start(std::string &file_name){
	std::weak_ptr<ast> dummy; //nullptr
	return std::make_shared<ast_start>(dummy,file_name);
}

std::vector<std::shared_ptr<ast>>& ast_start::get_code(){
	return code;
}

void ast_start::add_code(std::shared_ptr<ast> &c){
	code.push_back(c);
}

//ast_include
ast_include::ast_include(std::weak_ptr<ast> &parent,std::string &file_name):
	ast(parent,INCLUDE){

	if(file_name[0]==file_name[file_name.length()-1]&&
	   file_name[0]=='"'){ //remove trailing double quotes
		this->file_name=file_name.substr(1,file_name.length()-2);
	}
	else{
		this->file_name=file_name;
	}
}

//ast_cblk_decl
void ast_cblk_decl::add_const(std::shared_ptr<ast_id> &c){
	const_list.push_back(c);
}

void ast_cblk_decl::add_in(std::shared_ptr<ast_id> &i){
	in_list.push_back(i);
}

void ast_cblk_decl::add_out(std::shared_ptr<ast_id> &o){
	out_list.push_back(o);
}

void ast_cblk_decl::add_code(std::shared_ptr<ast> &c){
	code_block.push_back(c);
}

void ast_cblk_decl::set_code(std::vector<std::shared_ptr<ast>> &c){
	code_block=c;
}

std::string ast_cblk_decl::get_cblock_name(){
	return cblock_name;
}

pulse_id& ast_cblk_decl::get_pid(){
	return pid;
}

std::vector<std::shared_ptr<ast_id>>& ast_cblk_decl::get_const_list(){
	return const_list;
}

std::vector<std::shared_ptr<ast_id>>& ast_cblk_decl::get_in_list(){
	return in_list;
}

std::vector<std::shared_ptr<ast_id>>& ast_cblk_decl::get_out_list(){
	return out_list;
}

std::vector<std::shared_ptr<ast>>& ast_cblk_decl::get_code(){
	return code_block;
}

//ast_s_cblk_inst
void ast_s_cblk_inst::add_const(std::shared_ptr<ast_arith> &c){
	const_list.push_back(c);
}

void ast_s_cblk_inst::add_in(std::shared_ptr<ast_arith> &i){
	in_list.push_back(i);
}

pulse_id ast_s_cblk_inst::get_pulse_id(){
	return pid;
}

std::string ast_s_cblk_inst::get_cblock_name(){
	return cblock_name;
}

std::vector<std::shared_ptr<ast_arith>>& ast_s_cblk_inst::get_const_list(){
	return const_list;
}

std::vector<std::shared_ptr<ast_arith>>& ast_s_cblk_inst::get_in_list(){
	return in_list;
}

//ast_cblk_inst
/*
ast_cblk_inst::ast_cblk_inst(std::weak_ptr<ast> &parent,std::shared_ptr<ast_s_cblk_inst> &asc):
	ast(parent,CBLK_INST){
		cblock_name=asc->get_cblock_name();
		pid=asc->get_pulse_id();
		const_list=asc->get_const_list();
		in_list=asc->get_in_list();

		//Fix parent connections on const_list
		//Fix parent connections on in_list
};
*/
void ast_cblk_inst::add_const(std::shared_ptr<ast_arith> &c){
	const_list.push_back(c);
}

void ast_cblk_inst::add_in(std::shared_ptr<ast_arith> &i){
	in_list.push_back(i);
}

void ast_cblk_inst::add_out(std::shared_ptr<ast_id> &o){
	out_list.push_back(o);
}

void ast_cblk_inst::set_const(std::vector<std::shared_ptr<ast_arith>> &vc){
	const_list=vc;
}

void ast_cblk_inst::set_in(std::vector<std::shared_ptr<ast_arith>> &vi){
	in_list=vi;
}

void ast_cblk_inst::set_out(std::vector<std::shared_ptr<ast_id>> &vo){
	out_list=vo;
}

std::vector<std::shared_ptr<ast_arith>>& ast_cblk_inst::get_const_list(){
	return const_list;
}

std::vector<std::shared_ptr<ast_arith>>& ast_cblk_inst::get_in_list(){
	return in_list;
}

std::vector<std::shared_ptr<ast_id>>& ast_cblk_inst::get_out_list(){
	return out_list;
}

pulse_id& ast_cblk_inst::get_pulse_id(){
	return pid;
}

//ast_pulse
std::pair<std::string,std::string> ast_pulse::separate_literal(std::string &literal){

	//returns the subtraction between the begin iterator and iterator to the first letter
	unsigned int first_letter_idx =
		std::find_if_not(literal.begin(),literal.end(),
		[](char c) {return std::isdigit(c)||c=='.';})-
		literal.begin(); 

	std::string num(literal.substr(0,first_letter_idx));
	std::string scale(literal.substr(first_letter_idx));

	return std::make_pair(scale,num);
}

pulse_id& ast_pulse::get_pid(){
	return pid;
}

time_scale& ast_pulse::get_ts(){
	return ts;
}

double ast_pulse::get_val(){
	return val;
}

//ast_if
void ast_if::set_if_logic(std::shared_ptr<ast_arith> &c){
	logic_expr=c;
}

void ast_if::set_if_code(std::vector<std::shared_ptr<ast>> &c){
	code_block=c;
}

void ast_if::add_elseif(std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>> &p){
	elseif.push_back(p);
}

void ast_if::set_else_code(std::vector<std::shared_ptr<ast>> &c){
	else_code_block=c;
}

std::shared_ptr<ast_arith>& ast_if::get_if_logic(){
	return logic_expr;
}	

std::vector<std::shared_ptr<ast>>& ast_if::get_true_code(){
	return code_block;
}	

std::vector<std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>>>& ast_if::get_elseif(){
	return elseif;
}	

std::vector<std::shared_ptr<ast>>& ast_if::get_else_code(){
	return else_code_block;	
}

//ast_for
void ast_for::set_upper(std::shared_ptr<ast_arith> &u){
	upper_bound=u;
}

void ast_for::set_lower(std::shared_ptr<ast_arith> &l){
	lower_bound=l;
}

void ast_for::set_increment(std::shared_ptr<ast_arith> &i){
	increment=i;
}

void ast_for::set_code(std::vector<std::shared_ptr<ast>> &c){
	for_code_block=c;
}

std::shared_ptr<ast_arith>& ast_for::get_upper_bound(){
	    return upper_bound;
}

std::shared_ptr<ast_arith>& ast_for::get_lower_bound(){
	    return lower_bound;
}

std::shared_ptr<ast_arith>& ast_for::get_increment(){
	    return increment;
}

std::vector<std::shared_ptr<ast>>& ast_for::get_code_block(){
	return for_code_block;	
}

std::string& ast_for::get_for_var(){
	return for_var;
}

//ast_decl
void ast_decl::set_var(std::shared_ptr<ast_id> &v){
	var=v;
}

void ast_decl::set_attrib(std::shared_ptr<ast_arith> &a){
	attrib=a;
}

std::shared_ptr<ast_id>& ast_decl::get_var(){
	return var;
}

std::shared_ptr<ast_arith>& ast_decl::get_attrib(){
	return attrib;
}

//ast_assign
void ast_assign::set_var(std::shared_ptr<ast_id> &v){
	var=v;
}

void ast_assign::set_attrib(std::shared_ptr<ast_arith> &a){
	attrib=a;
}

std::shared_ptr<ast_id>& ast_assign::get_var(){
	return var;
}

assign_op& ast_assign::get_assign_op(){
	return ao;
}

std::shared_ptr<ast_arith>& ast_assign::get_attrib(){
	return attrib;
}

//ast_unary_arith
void ast_unary_arith::set_target(std::shared_ptr<ast_arith> &target){
	this->target=target;
}

std::shared_ptr<ast_arith> ast_unary_arith::get_target(){
	return target;
}

//ast_bin_arith
void ast_bin_arith::set_left(std::shared_ptr<ast_arith> &left){
	this->left=left;
}

void ast_bin_arith::set_right(std::shared_ptr<ast_arith> &right){
	this->right=right;
}

std::shared_ptr<ast_arith> ast_bin_arith::get_left(){
	return left;
}

std::shared_ptr<ast_arith> ast_bin_arith::get_right(){
	return right;
}

//ast_comp
logic_op ast_comp::get_logic_op(){
	return op;
}

//ast_cast
void ast_cast::set_target(std::shared_ptr<ast_arith> &target){
	this->target=target;
}

std::shared_ptr<ast_arith> ast_cast::get_target(){
	return target;
}

datatype ast_cast::get_datatype(){
	return dt;
}

//ast_arrinit
void ast_arrinit::add_vals(std::shared_ptr<ast_arith> &val){
	vals.push_back(val);
}

std::vector<std::shared_ptr<ast_arith>>& ast_arrinit::get_vals(){
	return vals;
}

//ast_num
ast_num::ast_num(std::weak_ptr<ast> &parent,std::string &val): //FIXME not completely correct
	ast_arith(parent,NUM){
	//0x... = HEX
	//0... = OCT
	//DEC otherwise	
	
	if(val.find('.')==std::string::npos){ //no commma found, must be integer
		i=std::stoi(val,0,0); //Auto base detection
		is_int=true;
	}
	else{
		d=std::stod(val);
		is_int=false;
	}
}

void ast_num::set_is_int(bool is_int){
	this->is_int=is_int;
}

void ast_num::set_int(int iint){
	i=iint;
}

void ast_num::set_double(double ddouble){
	d=ddouble;
}

bool ast_num::get_is_int(){
	return is_int;
}

int ast_num::get_int(){
	return i;
}

double ast_num::get_double(){
	return d;
}

std::string ast_num::to_string(){
	if(is_int){
		return std::to_string(i);	
	}
	else{
		return std::to_string(d);
	}
}
