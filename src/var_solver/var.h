#ifndef RTFSS_DEF_VAR_PROPERTY
#define RTFSS_DEF_VAR_PROPERTY

#include "../ast/ast.h"
#include <vector>
#include <memory>

struct var_property{ //FIXME Make a class out of this?

	enum var_kind{
		UNKNOWN,
		IN,
		OUT,
		CONST,
		FOR_VAR,
		REGULAR
		//RESERVED //TODO
	};

	//Mandatory 
	std::string name;
	std::string cblock_name;
	const unsigned int unique_id;
	//TODO add binding unique_id? cblocks var connections, maybe

	var_kind tk;
	datatype type;

	unsigned int pid; 
	bool found_pid; 

	bool decl; //already declared

	var_property(unsigned int unique_id,std::string& name):
		name(name),
		unique_id(unique_id),
		tk(UNKNOWN),
		pid(0),
		found_pid(false),
		decl(false){}

	std::string to_string();
};

struct pulse_property{

	enum pulse_kind{
		UNKNOWN,
		REGULAR,
		CBLOCK,
	};

	const unsigned int unique_id;
	//TODO add binding unique_id? cblocks var connections, maybe
	
	pulse_kind pk;
	pulse_id pid; //Contains name
	time_scale ts;
	double pulse_val;

	bool decl;

	pulse_property(unsigned int unique_id,std::string& name):
		unique_id(unique_id),
		pk(UNKNOWN),
		pid(name), //TODO horrible. should already recive pulse_id, and not name. maybe.
		decl(false){}

	std::string to_string();

};

class var_complete_scope{
	private:
		std::vector<std::shared_ptr<var_property>> program_vars;

	public:
		std::vector<std::shared_ptr<var_property>>& get();
};

class pulse_complete_scope{
	private:
		std::vector<std::shared_ptr<pulse_property>> program_pulses;

	public:
		std::vector<std::shared_ptr<pulse_property>>& get();
};

class var_scope{

	private:
		
		std::map<unsigned int,std::shared_ptr<var_property>> scope_vars;

		std::shared_ptr<var_complete_scope> complete_scope;

	public:

		var_scope(bool is_global);
		
		std::shared_ptr<var_property> get_in_scope(std::string& name);
		std::shared_ptr<var_property> get_in_scope(unsigned int unique_id);
		std::map<unsigned int,std::shared_ptr<var_property>>& get_scope();
		std::shared_ptr<var_complete_scope> get_complete();

		unsigned int create(std::string& name); //Return largest int if already existent FIXME
};

class pulse_scope{

	private:
		
		std::shared_ptr<pulse_complete_scope> complete_scope;

		std::map<unsigned int,std::shared_ptr<pulse_property>> scope_pulses;
		void add_global();

	public:

		pulse_scope(bool is_global);
		
		std::shared_ptr<pulse_property> get_in_scope(std::string& name);
		std::shared_ptr<pulse_property> get_in_scope(unsigned int unique_id);
		std::map<unsigned int,std::shared_ptr<pulse_property>>& get_scope();
		std::shared_ptr<pulse_complete_scope> get_complete();

		unsigned int create(std::string& name); //Return largest int if already existent FIXME
};

#endif
