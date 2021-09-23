#ifndef RTFSS_DEF_AST
#define RTFSS_DEF_AST

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iostream>  //TODO remove

//TODO create a namespace?

	
class pulse_id{ 
	public:
		enum pulse_type{
			PUNDEF, //undefined
			NORMAL,
			MAX,
			CONST
		};
		pulse_id():
			pt(PUNDEF){} 
		pulse_id(std::string &name):
			pulse_name(name),
			pt(str2pulse(name)){} 
		pulse_id(std::string &name,pulse_type &pt):
			pulse_name(name),
			pt(pt){} 
		static pulse_type str2pulse(std::string &pulse);

		std::string to_string();
		pulse_type get_pulse_type();

	private:
		std::string pulse_name;
		pulse_type pt;
};

class datatype{
	public:
		enum dt_types{
			VUNDEF, //undefined
			SIGNED,
			UNSIGNED,
			MIDI,
			FLOAT
		};
		datatype():
			dt(VUNDEF){}
		datatype(dt_types dt,unsigned int integ,unsigned int frac):
			dt(dt),
			integ(integ),
			frac(frac){}
		datatype(dt_types dt,std::string &idsize):
			dt(dt),
			integ(0),
			frac(0),
			idsize(idsize){}
		datatype(std::string dt);
		std::string to_string();

		dt_types get_datatype();
		unsigned int get_integ();
		unsigned int get_frac();
		unsigned int get_dim(); //integ+frac
		bool equal(datatype &dt);

	private:
		dt_types dt;
		unsigned int integ,frac;
		std::string idsize; //used when the size is bound to a const
};

class time_scale{
	public:
		enum ts_types{
			SECOND,
			MILLISECOND,
			HERTZ,
			KILOHERTZ
		};
		time_scale()=default; //TODO keep? 
		time_scale(std::string &ts):
			ts(str2time_scale(ts)){}
		time_scale(ts_types ts):
			ts(ts){}
		static ts_types str2time_scale(std::string &ts);
		std::string to_string();
		ts_types get_ts();
	private:
		ts_types ts;
};

class assign_op{
	public:
		enum op_types{
			EQUAL,
			ADDEQ,
			SUBEQ,
			DIVEQ,
			MULEQ,
			MODEQ
		};
		assign_op(std::string &op):
			op(str2assign_op(op)){}
		assign_op(op_types op):
			op(op){}
		static op_types str2assign_op(std::string &op);
		op_types get_op();
		std::string to_string();
	private:
		op_types op;
};

class logic_op{
	public:
		enum op_types{
			EQUAL,
			MORE,
			LESS,
			MOREQ,
			LESEQ,
			DIFF
		};
		logic_op(std::string &op):
			op(str2logic_op(op)){}
		logic_op(op_types op):
			op(op){}
		static op_types str2logic_op(std::string &op);
		op_types get_op();
		std::string to_string();
	private:
		op_types op;
};

class ast{
	//Abstract Syntax Tree
	//Base class to all the possible nodes

	public:
		enum ast_op{
			START,			//Start Node	
			INCLUDE,		//Include/Use Statement
			CBLK_DECL,		//Cblock declaration
			CBLK_INST,		//Cblock instantiation
			S_CBLK_INST,	//Short Cblock instantiation (inline on expr)
			PULSE,			//Pulse declaration
			IF,				//IF statement
			FOR,			//FOR statement
			DECL,			//Const/Stream declaration
			ASSIGN,			//Const/Stream assignment
			ADD,			//Addition
			SUB,			//Subtraction
			SIM,			//Simetric
			MULT,			//Multiplication
			DIV,			//Division
			SHL,			//Shift left
			SHR,			//Shift right
			RTL,			//Rotate left
			RTR,			//Rotate right
			AND,			//AND gate
			OR,				//OR gate
			XOR,			//XOR gate
			NOT,			//NOT gate
			GAP,			//Gap operation
			COMP,			//Logic Comparison
			PROP,			//"Property of" operation
			MOD,			//Modulo operation
			CAST,			//Casting
			ARRINIT,		//Const array initialization
			NUM,			//Numerical
			ID				//Const/Stream identifier
		};

	protected:
		static unsigned int vz_num;
		static std::string vz_label;
		//static std::map<std::weak_ptr<ast>,unsigned int,std::owner_less<std::weak_ptr<ast>>> vz_parent; understand better
		static std::map<ast*,unsigned int> vz_parent; 
		static bool vz_show_parent;
		static bool vz_orfan;

		ast(std::weak_ptr<ast> &parent,ast_op oper):
			parent(parent),
			op(oper){} //Force "interface" class
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base)=0;
		std::weak_ptr<ast> parent;
		ast_op op;
	public:
		unsigned int gen_vz(std::stringstream &ss,bool top_level=true);

		virtual std::shared_ptr<ast> duplicate()=0;

		ast_op get_op();
		std::weak_ptr<ast>& get_parent();
		virtual std::vector<std::shared_ptr<ast>> get_children()=0;
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child)=0;
		void set_parent(std::weak_ptr<ast>& parent);
};

class ast_arith : public ast{ //AST arithmetic operations
	protected:
		ast_arith(std::weak_ptr<ast> parent,ast_op oper):
			ast(parent,oper){} //Force "interface" class
	public:
		virtual void build_vz(std::stringstream &ss,unsigned int base)=0;

		virtual std::shared_ptr<ast> duplicate()=0;
		virtual std::vector<std::shared_ptr<ast>> get_children()=0;
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child)=0;
};

class ast_id : public ast_arith{ 
	private:
		std::string name;
		datatype dt;
		pulse_id pid;
		std::shared_ptr<ast_arith> index;
		std::shared_ptr<ast_arith> instant;

		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		//only name
		ast_id(std::weak_ptr<ast> &parent,std::string &name):
			ast_arith(parent,ID),
			name(name),
			dt(){}
		//name and type
		ast_id(std::weak_ptr<ast> &parent,std::string &name,std::string &dat_type):
			ast_arith(parent,ID),
			name(name),
			dt(dat_type){} 
		//name, type and pulse
		ast_id(std::weak_ptr<ast> &parent,std::string &name,std::string &dat_type,std::string &pulse):
			ast_arith(parent,ID),
			name(name),
			dt(dat_type), 
			pid(pulse){}
		void set_index(std::shared_ptr<ast_arith> &index);
		void set_instant(std::shared_ptr<ast_arith> &instant);
		void set_datatype(std::string &dt);
		void set_pulse(std::string &pid);

		std::string& get_name();
		datatype& get_datatype();
		pulse_id& get_pulse_id();
		std::shared_ptr<ast_arith>& get_index();
		std::shared_ptr<ast_arith>& get_instant();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_start : public ast{ 
	private:
		std::string file_name;
		std::vector<std::shared_ptr<ast>> code;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_start(std::weak_ptr<ast> &parent,std::string &file_name):
			ast(parent,START),
			file_name(file_name){}
		static std::shared_ptr<ast_start> build_start(std::string &file_name); //FIXME workaround
		void add_code(std::shared_ptr<ast> &c);
		std::vector<std::shared_ptr<ast>>& get_code();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_include : public ast{ 
	private:
		std::string file_name;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_include(std::weak_ptr<ast> &parent,std::string &file_name);

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_cblk_decl : public ast{
	private:
		std::string cblock_name;
		pulse_id pid;
		std::vector<std::shared_ptr<ast_id>> const_list;
		std::vector<std::shared_ptr<ast_id>> in_list;
		std::vector<std::shared_ptr<ast_id>> out_list;
		std::vector<std::shared_ptr<ast>> code_block;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_cblk_decl(std::weak_ptr<ast> &parent,std::string &cblock_name,pulse_id &pulse):
			ast(parent,CBLK_DECL),
			cblock_name(cblock_name),
			pid(pulse){}
		void add_const(std::shared_ptr<ast_id> &c);
		void add_in(std::shared_ptr<ast_id> &i);
		void add_out(std::shared_ptr<ast_id> &o);
		void add_code(std::shared_ptr<ast> &c);
		void set_code(std::vector<std::shared_ptr<ast>> &c);

		pulse_id& get_pid();
		std::string get_cblock_name();
		std::vector<std::shared_ptr<ast_id>>& get_const_list();
		std::vector<std::shared_ptr<ast_id>>& get_in_list();
		std::vector<std::shared_ptr<ast_id>>& get_out_list();
		std::vector<std::shared_ptr<ast>>& get_code();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_s_cblk_inst : public ast_arith{
	private:
		std::string cblock_name;
		pulse_id pid;
		std::vector<std::shared_ptr<ast_arith>> const_list; //arguments can be expr
		std::vector<std::shared_ptr<ast_arith>> in_list;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_s_cblk_inst(std::weak_ptr<ast> &parent,std::string &cblock_name,std::string &pulse):
			ast_arith(parent,S_CBLK_INST),
			cblock_name(cblock_name),
			pid(pulse){}
		void add_const(std::shared_ptr<ast_arith> &c);
		void add_in(std::shared_ptr<ast_arith> &i);
		pulse_id get_pulse_id();
		std::string get_cblock_name();
		std::vector<std::shared_ptr<ast_arith>>& get_const_list(); 
		std::vector<std::shared_ptr<ast_arith>>& get_in_list();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_cblk_inst : public ast{
	private:
		std::string cblock_name;
		pulse_id pid;
		std::vector<std::shared_ptr<ast_arith>> const_list; //arguments can be expr
		std::vector<std::shared_ptr<ast_arith>> in_list;
		std::vector<std::shared_ptr<ast_id>> out_list;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		//ast_cblk_inst(std::weak_ptr<ast> &parent,std::shared_ptr<ast_s_cblk_inst> &asc);
		ast_cblk_inst(std::weak_ptr<ast> &parent,std::string &cblock_name,pulse_id &pulse):
			ast(parent,CBLK_INST),
			cblock_name(cblock_name),
			pid(pulse){}
		void add_const(std::shared_ptr<ast_arith> &c);
		void add_in(std::shared_ptr<ast_arith> &i);
		void add_out(std::shared_ptr<ast_id> &o);
		void set_const(std::vector<std::shared_ptr<ast_arith>> &vc);
		void set_in(std::vector<std::shared_ptr<ast_arith>> &vi);
		void set_out(std::vector<std::shared_ptr<ast_id>> &vo);
		std::vector<std::shared_ptr<ast_arith>>& get_const_list(); 
		std::vector<std::shared_ptr<ast_arith>>& get_in_list();
		std::vector<std::shared_ptr<ast_id>>& get_out_list();
		pulse_id& get_pulse_id();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_pulse : public ast{
	private:
		pulse_id pid;
		time_scale ts;
		double val; //FIXME Not the best?
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_pulse(std::weak_ptr<ast> &parent,std::string &pulse_name,std::string &time_scale,std::string &val):
			ast(parent,PULSE),
			pid(pulse_name),
			ts(time_scale),
			val(std::stod(val)){}
		//first=scale, second=number
		static std::pair<std::string,std::string> separate_literal(std::string &literal);
		
		pulse_id& get_pid();
		time_scale& get_ts();
		double get_val();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_if : public ast{
	private:
		std::shared_ptr<ast_arith> logic_expr;
		std::vector<std::shared_ptr<ast>> code_block;
		std::vector<std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>>> elseif; //FIXME ugly
		std::vector<std::shared_ptr<ast>> else_code_block;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_if(std::weak_ptr<ast> &parent):
			ast(parent,IF){}
		ast_if(std::weak_ptr<ast> &parent,std::shared_ptr<ast_arith> &logic_expr):
			ast(parent,IF),
			logic_expr(logic_expr){}
		void set_if_logic(std::shared_ptr<ast_arith> &c);
		void set_if_code(std::vector<std::shared_ptr<ast>> &c);
		void add_elseif(std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>> &p);
		void set_else_code(std::vector<std::shared_ptr<ast>> &c);
		std::shared_ptr<ast_arith>& get_if_logic(); 
		std::vector<std::shared_ptr<ast>>& get_true_code(); 
		std::vector<std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>>  >& get_elseif(); 
		std::vector<std::shared_ptr<ast>>& get_else_code();


		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_for : public ast{
	private:
		std::string for_var;
		std::shared_ptr<ast_arith> lower_bound;
		std::shared_ptr<ast_arith> upper_bound;
		std::shared_ptr<ast_arith> increment;
		std::vector<std::shared_ptr<ast>> for_code_block;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_for(std::weak_ptr<ast> &parent,std::string &for_var):
			ast(parent,FOR),
			for_var(for_var){}
		ast_for(std::weak_ptr<ast> &parent,std::string &for_var,std::shared_ptr<ast_arith> lower_bound,std::shared_ptr<ast_arith> upper_bound):
			ast(parent,FOR),
			for_var(for_var),
			lower_bound(lower_bound),
			upper_bound(upper_bound),
			increment(nullptr){}
		ast_for(std::weak_ptr<ast> &parent,std::string &for_var,std::shared_ptr<ast_arith> lower_bound,std::shared_ptr<ast_arith> upper_bound,std::shared_ptr<ast_arith> increment):
			ast(parent,FOR),
			for_var(for_var),
			lower_bound(lower_bound),
			upper_bound(upper_bound),
			increment(increment){}
		void set_upper(std::shared_ptr<ast_arith> &u);
		void set_lower(std::shared_ptr<ast_arith> &l);
		void set_increment(std::shared_ptr<ast_arith> &i);
		void set_code(std::vector<std::shared_ptr<ast>> &c);
		std::shared_ptr<ast_arith>& get_upper_bound(); 
		std::shared_ptr<ast_arith>& get_lower_bound(); 
		std::shared_ptr<ast_arith>& get_increment(); 
		std::vector<std::shared_ptr<ast>>& get_code_block(); 
		std::string& get_for_var();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_decl : public ast{
	private:
		std::shared_ptr<ast_id> var;	
		std::shared_ptr<ast_arith> attrib;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_decl(std::weak_ptr<ast> &parent):
			ast(parent,DECL){}
		void set_var(std::shared_ptr<ast_id> &v);
		void set_attrib(std::shared_ptr<ast_arith> &a);

		std::shared_ptr<ast_id>& get_var();
		std::shared_ptr<ast_arith>& get_attrib();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_assign : public ast{ //includes +=, -=, etc
	private:
		std::shared_ptr<ast_id> var;	
		assign_op ao;
		std::shared_ptr<ast_arith> attrib;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_assign(std::weak_ptr<ast> &parent,std::string &oper):
			ast(parent,ASSIGN),
			ao(assign_op::str2assign_op(oper)){}
		void set_var(std::shared_ptr<ast_id> &v);
		void set_attrib(std::shared_ptr<ast_arith> &a);

		std::shared_ptr<ast_id>& get_var();
		assign_op& get_assign_op();
		std::shared_ptr<ast_arith>& get_attrib();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_unary_arith : public ast_arith{ //AST unary operators arithmetic
	//Class can't be instantiated alone
	private:
		std::shared_ptr<ast_arith> target;
		virtual void build_vz(std::stringstream &ss,unsigned int base)=0;
	protected:
		void unary_build_vz(std::stringstream &ss,unsigned int base,std::string op);
		std::shared_ptr<ast> duplicate_unary(std::shared_ptr<ast_unary_arith> &dup_self);

		ast_unary_arith(std::weak_ptr<ast> &parent,ast_op oper):
			ast_arith(parent,oper){} 
	public:
		void set_target(std::shared_ptr<ast_arith> &target);
		std::shared_ptr<ast_arith> get_target();

		virtual std::shared_ptr<ast> duplicate()=0;
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_bin_arith : public ast_arith{ //AST binary operators arithmetic
	//Class can't be instantiated alone
	private:
		std::shared_ptr<ast_arith> left;
		std::shared_ptr<ast_arith> right;
		virtual void build_vz(std::stringstream &ss,unsigned int base)=0;
	protected:
		void bin_build_vz(std::stringstream &ss,unsigned int base,std::string op);
		std::shared_ptr<ast> duplicate_binary(std::shared_ptr<ast_bin_arith> &dup_self);

		ast_bin_arith(std::weak_ptr<ast> &parent,ast_op oper):
			ast_arith(parent,oper){} 
	public:
		void set_left(std::shared_ptr<ast_arith> &left);
		void set_right(std::shared_ptr<ast_arith> &right);
		std::shared_ptr<ast_arith> get_left();
		std::shared_ptr<ast_arith> get_right();

		virtual std::shared_ptr<ast> duplicate()=0;
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_add : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_add(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,ADD){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_sub : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_sub(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,SUB){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_sim : public ast_unary_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_sim(std::weak_ptr<ast> &parent):
			ast_unary_arith(parent,SIM){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_mult : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_mult(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,MULT){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_div : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_div(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,DIV){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_shl : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_shl(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,SHL){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_shr : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_shr(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,SHR){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_rtl : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_rtl(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,RTL){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_rtr : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_rtr(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,RTR){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_and : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_and(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,AND){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_or : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_or(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,OR){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_xor : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_xor(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,XOR){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_not : public ast_unary_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_not(std::weak_ptr<ast> &parent):
			ast_unary_arith(parent,NOT){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_gap : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_gap(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,GAP){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_comp : public ast_bin_arith{
	private:
		logic_op op;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_comp(std::weak_ptr<ast> &parent,std::string &operand):
			ast_bin_arith(parent,COMP),
			op(operand){}

		logic_op get_logic_op();
		virtual std::shared_ptr<ast> duplicate();
};

class ast_prop : public ast_unary_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_prop(std::weak_ptr<ast> &parent):
			ast_unary_arith(parent,PROP){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_mod : public ast_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_mod(std::weak_ptr<ast> &parent):
			ast_bin_arith(parent,MOD){}

		virtual std::shared_ptr<ast> duplicate();

};

class ast_cast : public ast_arith{
	private:
		datatype dt;
		std::shared_ptr<ast_arith> target;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_cast(std::weak_ptr<ast> &parent,std::string &data_t):
			ast_arith(parent,CAST),
			dt(data_t){}
		ast_cast(std::weak_ptr<ast> &parent,datatype dt):
			ast_arith(parent,CAST),
			dt(dt){}
		void set_target(std::shared_ptr<ast_arith> &target);
		std::shared_ptr<ast_arith> get_target();
		datatype get_datatype();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_arrinit : public ast_arith{
	private:
		std::vector<std::shared_ptr<ast_arith>> vals;
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_arrinit(std::weak_ptr<ast> &parent):
			ast_arith(parent,ARRINIT){}
		void add_vals(std::shared_ptr<ast_arith> &val);
		std::vector<std::shared_ptr<ast_arith>>& get_vals();

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);
};

class ast_num : public ast_arith{ //FIXME not completely correct to all bases
	private:
		bool is_int;
		union{
			int i;
			double d;
		};
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		ast_num(std::weak_ptr<ast> &parent):
			ast_arith(parent,NUM){}
		ast_num(std::weak_ptr<ast> &parent,std::string &val);

		virtual std::shared_ptr<ast> duplicate();
		virtual std::vector<std::shared_ptr<ast>> get_children();
		virtual std::shared_ptr<ast> cut_child(std::shared_ptr<ast> child);

		void set_is_int(bool is_int);
		void set_int(int iint);
		void set_double(double ddouble);

		bool get_is_int();
		int get_int();
		double get_double();

		std::string to_string();
};

#endif
