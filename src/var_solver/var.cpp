#include "var.h"
#include "../error_handler/error_handler.h"

//var_property
std::string var_property::to_string(){
	std::map<var_kind,std::string> mapping{
		{UNKNOWN,"UNKNOWN"},	
		{IN,"IN"},	
		{OUT,"OUT"},	
		{CONST,"CONST"},	
		{FOR_VAR,"FOR_VAR"},	
		{REGULAR,"REGULAR"}	
	};
	std::stringstream ret;
	
	ret<<"{"<<"unique_id:"<<unique_id<<","
		<<"name:"<<name<<","
		<<"var_kind:"<<mapping[tk]<<",";
	if(tk!=UNKNOWN&&tk!=FOR_VAR){
		ret<<"datatype:"<<type.to_string()<<",";
	}
	if(tk!=UNKNOWN){	
		ret<<"pid:"<<pid<<",";
	}
	ret<<"decl:"<<decl<<"}";

	return ret.str(); 
}

//pulse_property
std::string pulse_property::to_string(){
	std::map<pulse_kind,std::string> mapping{
		{UNKNOWN,"UNKNOWN"},	
		{REGULAR,"REGULAR"},	
		{CBLOCK,"CBLOCK"}
	};
	std::stringstream ret;
	
	ret<<"{"<<"unique_id:"<<unique_id<<","
		<<"pid:"<<pid.to_string()<<","
		<<"pulse_kind:"<<mapping[pk]<<",";
	if(pk==REGULAR){
		ret<<"time_scale:"<<ts.to_string()<<","
			<<"pulse_val:"<<pulse_val<<",";
	}
	ret<<"decl:"<<decl<<"}";

	return ret.str(); 
}

//var_complete_scope
std::vector<std::shared_ptr<var_property>>& var_complete_scope::get(){
	return program_vars;
}

//pulse_complete_scope
std::vector<std::shared_ptr<pulse_property>>& pulse_complete_scope::get(){
	return program_pulses;
}

//var_scope
var_scope::var_scope(bool is_global){
	if(is_global){
		complete_scope=std::make_shared<var_complete_scope>();
	}
}

std::shared_ptr<var_property> var_scope::get_in_scope(std::string& name){
	
	for(auto it=scope_vars.begin();it!=scope_vars.end();it++){ 
		if(it->second->name==name){
			return it->second;
		}
	}

	return std::shared_ptr<var_property>();
}

std::shared_ptr<var_property> var_scope::get_in_scope(unsigned int unique_id){
	auto pos=scope_vars.find(unique_id);

	if(pos==scope_vars.cend()){
		return nullptr;
	}

	return pos->second;
}

std::map<unsigned int,std::shared_ptr<var_property>>& var_scope::get_scope(){
	return scope_vars;
}

std::shared_ptr<var_complete_scope> var_scope::get_complete(){
	return complete_scope;
}

unsigned int var_scope::create(std::string& name){
	
	if(get_in_scope(name)!=nullptr){
		error_handler::stream_redecl(name);
		return -1; //Largest integer, (twos compliment)
	}

	unsigned int unique_id=complete_scope->get().size();
	std::shared_ptr<var_property> tmp(std::make_shared<var_property>(unique_id,name));

	scope_vars.insert(std::make_pair(unique_id,tmp));
	complete_scope->get().push_back(tmp);

	return unique_id;
}

//pulse_scope
pulse_scope::pulse_scope(bool is_global){
	if(is_global){
		complete_scope=std::make_shared<pulse_complete_scope>();
		add_global();	
	}
}

void pulse_scope::add_global(){

	std::vector<std::string> globals({	"const",
										"max",
										//"masterpulse"
										});

	for(auto it=globals.begin();it!=globals.end();it++){
		std::shared_ptr<pulse_property> pulsep(get_in_scope(create(*it)));
		pulsep->decl=true;
	}
	//TODO create global pulses?
}

std::shared_ptr<pulse_property> pulse_scope::get_in_scope(std::string& name){
	
	for(auto it=scope_pulses.begin();it!=scope_pulses.end();it++){
		if(it->second->pid.to_string()==name){
			return it->second;
		}
	}

	return std::shared_ptr<pulse_property>();
}

std::shared_ptr<pulse_property> pulse_scope::get_in_scope(unsigned int unique_id){
	auto pos=scope_pulses.find(unique_id);

	if(pos==scope_pulses.cend()){
		return nullptr;
	}

	return pos->second;
}

std::map<unsigned int,std::shared_ptr<pulse_property>>& pulse_scope::get_scope(){

	return scope_pulses;
}

std::shared_ptr<pulse_complete_scope> pulse_scope::get_complete(){

	return complete_scope;
}

unsigned int pulse_scope::create(std::string& name){
	
	std::shared_ptr<pulse_property> tmp(get_in_scope(name));
	if(tmp!=nullptr){
		if(tmp->pid.get_pulse_type()==pulse_id::NORMAL){ //Check if it is not a reserved pulse
			error_handler::pulse_redecl(name);
			return -1; //Largest integer, (twos compliment)
		}
		else{
			return tmp->unique_id;
		}
	}

	unsigned int unique_id=complete_scope->get().size();
	tmp=std::make_shared<pulse_property>(unique_id,name);

	scope_pulses.insert(std::make_pair(unique_id,tmp));

	complete_scope->get().push_back(tmp);

	return unique_id;
}

