#ifndef RTFSS_DEF_ARCH_NODE
#define RTFSS_DEF_ARCH_NODE

#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <boost/dynamic_bitset.hpp>

#include "../ast/ast.h"
#include "../var_solver/var.h"

//TODO create a namespace?

class arch_node{
	//Architecture node for the architecture graph
	//Base class to all the possible nodes

	public:
		enum arch_op{
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
			//maybe? GAP,	//Gap operation
			COMP,			//Logic Comparison
			//PROP,			//"Property of" operation TODO remove for good
			MOD,			//Modulo operation
			CAST,			//Cast operation
			NUM,			//Numerical
			ID,				//Const/Stream identifier
			INS				//Stream instant delay
							//TODO BITWISE LOGIC OPS 
		};

	protected:
		arch_node(arch_op oper,unsigned int node_id,unsigned int pulse_id,datatype& dt):
			op(oper),
			node_id(node_id),
			pulse_id(pulse_id),
			dt(dt){} //Force "interface" class
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base)=0;
		arch_op op;

		unsigned int node_id;
		unsigned int pulse_id;
		datatype dt;

	public:
		//virtual std::shared_ptr<arch_node> duplicate()=0;

		arch_op get_op();
		unsigned int get_node_id();
		unsigned int get_pulse_id();
		datatype get_datatype();
		void set_datatype(datatype dt);

		static std::string gv_label;
		virtual void gz_extra_conn(std::stringstream &ss);
		virtual std::string to_string()=0;

};

class arch_arith : public arch_node{ //Arch arithmetic operations  //TODO REMOVE, its useless, every op is arith...
	protected:
		arch_arith(arch_op oper,unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_node(oper,node_id,pulse_id,dt){} //Force "interface" class
	public:
		//virtual void build_vz(std::stringstream &ss,unsigned int base)=0;

		//virtual std::shared_ptr<arch_node> duplicate()=0;

		virtual std::string to_string()=0;
};

class arch_id : public arch_arith{ 
	private:
		std::string name;
		std::string cblock_name;
		var_property::var_kind vk;
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_id(unsigned int node_id,unsigned int pulse_id,datatype& dt,std::string& name,std::string& cblock_name,var_property::var_kind& vk):
			arch_arith(ID,node_id,pulse_id,dt),
			name(name),
			cblock_name(cblock_name),
			vk(vk){}

		//virtual std::shared_ptr<arch_node> duplicate();
		std::string get_name();
		std::string get_cblock_name();
		var_property::var_kind get_vk();

		virtual std::string to_string();
};

class arch_unary_arith : public arch_arith{ //Arch unary operators arithmetic
	//Class can't be instantiated alone
	private:
		std::shared_ptr<arch_node> target; //TODO remove shorcut pointer because its useless, trust me!
		//virtual void build_vz(std::stringstream &ss,unsigned int base)=0;
	protected:
		//std::shared_ptr<arch_node> duplicate_unary(std::shared_ptr<arch_unary_arith> &dup_self);

		arch_unary_arith(arch_op oper,unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_arith(oper,node_id,pulse_id,dt){} 
	public:
		void set_target(std::shared_ptr<arch_node> &target);
		std::shared_ptr<arch_node> get_target();

		//virtual std::shared_ptr<arch_node> duplicate()=0;
		
		virtual void gz_extra_conn(std::stringstream &ss);
		virtual std::string to_string()=0;
};

class arch_bin_arith : public arch_arith{ //Arch binary operators arithmetic
	//Class can't be instantiated alone
	private:
		std::shared_ptr<arch_node> left;  //FIXME should be weak_ptr
		std::shared_ptr<arch_node> right;  //FIXME should be weak_ptr
		//virtual void build_vz(std::stringstream &ss,unsigned int base)=0;
	protected:
		//std::shared_ptr<arch_node> duplicate_binary(std::shared_ptr<arch_bin_arith> &dup_self);

		arch_bin_arith(arch_op oper,unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_arith(oper,node_id,pulse_id,dt){} 
	public:
		void set_left(std::shared_ptr<arch_node> &left);
		void set_right(std::shared_ptr<arch_node> &right);
		std::shared_ptr<arch_node> get_left();
		std::shared_ptr<arch_node> get_right();

		//virtual std::shared_ptr<arch_node> duplicate()=0;
		
		virtual void gz_extra_conn(std::stringstream &ss);
		virtual std::string to_string()=0;
};

class arch_ins : public arch_unary_arith{ 
	private:
		std::string name;
		var_property::var_kind vk;
		unsigned int inst;
		boost::dynamic_bitset<> default_value;
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_ins(unsigned int node_id,unsigned int pulse_id,datatype& dt,std::string& name,var_property::var_kind& vk,unsigned int inst):
			arch_unary_arith(INS,node_id,pulse_id,dt),
			name(name),
			vk(vk),
			inst(inst){} 

		void set_default_value(boost::dynamic_bitset<> dv);

		//virtual std::shared_ptr<arch_node> duplicate();
		std::string get_name();
		unsigned int get_inst();
		boost::dynamic_bitset<> get_default_value();
		std::string get_df_in_hex();
		var_property::var_kind get_vk();

		virtual void gz_extra_conn(std::stringstream &ss);
		virtual std::string to_string();
};

class arch_add : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_add(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(ADD,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();
};

class arch_sub : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_sub(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(SUB,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_sim : public arch_unary_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_sim(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_unary_arith(SIM,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_mult : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_mult(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(MULT,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_div : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_div(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(DIV,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_shl : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_shl(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(SHL,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_shr : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_shr(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(SHR,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_rtl : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_rtl(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(RTL,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_rtr : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_rtr(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(RTR,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_and : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_and(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(AND,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_or : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_or(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(OR,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_xor : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_xor(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(XOR,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_not : public arch_unary_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_not(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_unary_arith(NOT,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

/*
class arch_gap : public arch_bin_arith{
	private:
		virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_gap(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(GAP,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};
*/

class arch_comp : public arch_bin_arith{
	private:
		logic_op op;
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_comp(unsigned int node_id,logic_op op,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(COMP,node_id,pulse_id,dt),
			op(op){}

		//virtual std::shared_ptr<arch_node> duplicate();
		logic_op get_logic_op();
		
		virtual std::string to_string();
};

/*
class arch_prop : public arch_unary_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_prop(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_unary_arith(PROP,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};
*/

class arch_mod : public arch_bin_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_mod(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_bin_arith(MOD,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();

};

class arch_cast : public arch_unary_arith{
	private:
		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_cast(unsigned int node_id,unsigned int pulse_id,datatype& dt):
			arch_unary_arith(CAST,node_id,pulse_id,dt){}

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();
};

class arch_num : public arch_arith{ 
	private:
		//std::shared_ptr<ast_num> num;
		boost::dynamic_bitset<> fixed_num;

		//virtual void build_vz(std::stringstream &ss,unsigned int base);
	public:
		arch_num(unsigned int node_id,unsigned int pulse_id,datatype& dt,boost::dynamic_bitset<> fixed_num):
			arch_arith(NUM,node_id,pulse_id,dt),
			fixed_num(fixed_num){}

		boost::dynamic_bitset<> get_num();
		std::string get_in_hex();

		//virtual std::shared_ptr<arch_node> duplicate();
		
		virtual std::string to_string();
};

#endif
