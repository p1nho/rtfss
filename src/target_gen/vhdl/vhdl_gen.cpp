#include "vhdl_gen.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

std::string vhdl_gen::vprefix("n");
std::string vhdl_gen::pprefix("p_");
std::string vhdl_gen::master_clock("clk");
std::string vhdl_gen::master_reset("rst");

void vhdl_gen::execute(){

	//Start off file by adding the pulser block
	vhdl_templates::vhdl_pulser(ss_out);	

	//Add ffd template block 
	vhdl_templates::vhdl_ffd(ss_out);	

	//Add CLK_FREQ generic
	vs_generics.push_back(vhdl_templates::vhdl_entity_generic("CLK_FREQ","real"));

	//Add clock to ins
	vs_ports.push_back(master_clock+" : in std_logic");  //TODO vhdl_templates::vhdl_entity_port

	//Add reset to ins
	vs_ports.push_back(master_reset+" : in std_logic");  //TODO vhdl_templates::vhdl_entity_port

	frepr_rev_breath_traverse::execute();

	//Build final entity
	vhdl_templates::vhdl_entity(ss_out,"rtfss",vs_generics,vs_ports,"impl",ss_var_dcl.str(),ss_statm.str());

}

std::string vhdl_gen::solve_type(datatype dt){
	std::string type;
	switch(dt.get_datatype()){
		case datatype::SIGNED:
			type="signed";
			break;
		case datatype::UNSIGNED:
			type="unsigned";
			break;
		case datatype::FLOAT:
		case datatype::MIDI:
		case datatype::VUNDEF:
			std::cerr<<"VHDL_GEN CANT SOLVE TYPE"<<std::endl;
			break;
	}
	return type;
}

void vhdl_gen::spawn_ffd(std::string name,int dim,std::string en,boost::dynamic_bitset<> rst_val,std::string in){
	//Create generic maps
	std::vector<std::string> generics;
	generics.push_back(vhdl_templates::vhdl_mapping("SIZE",std::to_string(dim)));

	std::string srst_val;

	//Fix inputs
	if(en.empty()){
		en="\"1\"";
	}	

	if(rst_val.empty()){
		srst_val="(others=>'0')";
	}	
	else{
		boost::to_string(rst_val,srst_val);
		srst_val="\""+srst_val+"\"";
	}

	//Create port maps
	std::vector<std::string> ports;
	ports.push_back(vhdl_templates::vhdl_mapping("clk",master_clock));
	ports.push_back(vhdl_templates::vhdl_mapping("en",en));
	ports.push_back(vhdl_templates::vhdl_mapping("rst",master_reset));
	ports.push_back(vhdl_templates::vhdl_mapping("rst_val",srst_val));
	ports.push_back(vhdl_templates::vhdl_mapping("din",in));
	ports.push_back(vhdl_templates::vhdl_mapping("dout",name));

	//Instantiate ffd
	std::string inst_name("inst"+std::to_string(inst_cnt++));
	vhdl_templates::vhdl_entity_inst(ss_statm,inst_name,"ffd",generics,ports);
}

void vhdl_gen::solve_clock(std::shared_ptr<frepr_clock> node){
	//Assumes only one clock source
	clock_id=node->get_node_id();
}

void vhdl_gen::solve_ffd(std::shared_ptr<frepr_ffd> node){
	std::string vname=vprefix+std::to_string(node->get_node_id());
	boost::dynamic_bitset<> def_val(node->get_default_val());

	//Handle type
	std::string p;
	switch(node->get_vk()){
		case var_property::IN:
			{
				//Add port
				std::string pvname(pprefix);
				if(node->get_cblock_name()=="main"){
					pvname+=node->get_name();
				}
				else{
					assert(false); //only main is allowed to access the outside
				}
				
				unsigned int dim=node->get_datatype().get_dim();
				std::string port(vhdl_templates::vhdl_entity_port(pvname,"in",dim));
				vs_ports.push_back(port);

				//Create signal
				vhdl_templates::vhdl_signal(ss_var_dcl,vname,dim); //Cant have default value

				//Control port with ffd
				std::string enname(vprefix+std::to_string(node->get_en()->get_node_id()));
				spawn_ffd(vname,dim,enname,def_val,pvname);
				//vhdl_templates::vhdl_ffd_en(ss_statm,pvname,vname,master_clock,enname);

				return;
			}
			break;	
		case var_property::OUT:
			{
				//Add port
				std::string pvname(pprefix);
				if(node->get_cblock_name()=="main"){
					pvname+=node->get_name();
				}
				else{
					assert(false); //only main is allowed to access the outside
				}

				unsigned int dim=node->get_datatype().get_dim();
				std::string port(vhdl_templates::vhdl_entity_port(pvname,"out",dim));
				vs_ports.push_back(port);

				//Create signal
				vhdl_templates::vhdl_signal(ss_var_dcl,vname,dim);

				//Control port with ffd
				std::string inname=vprefix+std::to_string(node->get_src()->get_node_id());
				std::string enname(vprefix+std::to_string(node->get_en()->get_node_id()));
				spawn_ffd(vname,dim,enname,def_val,inname);
				//vhdl_templates::vhdl_ffd_en(ss_statm,inname,vname,master_clock,enname);

				//Bind vname to pvname
				vhdl_templates::vhdl_assign(ss_statm,pvname,vname);

				return;
			}
			break;	
		case var_property::REGULAR:
			{
				unsigned int dim=node->get_datatype().get_dim();

				//Add node var to ss_var_dcl
				vhdl_templates::vhdl_signal(ss_var_dcl,vname,dim);

				//Add statement
				std::string inname=vprefix+std::to_string(node->get_src()->get_node_id());
				if(node->get_en()==nullptr){ //Flip flop without enable
					spawn_ffd(vname,dim,"",def_val,inname);
					//vhdl_templates::vhdl_ffd(ss_statm,inname,vname,master_clock);
				}
				else{ //Flip flop with enable
					std::string enname=vprefix+std::to_string(node->get_en()->get_node_id());
					spawn_ffd(vname,dim,enname,def_val,inname);
					//vhdl_templates::vhdl_ffd_en(ss_statm,inname,vname,master_clock,enname);
				}

				return;
			}
			break;
		case var_property::FOR_VAR:
		case var_property::UNKNOWN:
		case var_property::CONST:
			assert(false);
			break;
	}
}

void vhdl_gen::solve_pulser(std::shared_ptr<frepr_pulser> node){
	//Create generic maps
	std::vector<std::string> generics;
	generics.push_back(vhdl_templates::vhdl_mapping("CLK_FREQ","CLK_FREQ"));

	float freq;
	switch(node->get_ts().get_ts()){
		case time_scale::SECOND:
			freq=1.0/node->get_val();
			break;
		case time_scale::MILLISECOND:
			freq=1000.0/node->get_val();
			break;
		case time_scale::HERTZ:
			freq=node->get_val();
			break;
		case time_scale::KILOHERTZ:
			freq=1000*node->get_val();
			break;
	}
	generics.push_back(vhdl_templates::vhdl_mapping("TICK_FREQ",std::to_string(freq)));

	//Create port maps
	std::vector<std::string> ports;
	ports.push_back(vhdl_templates::vhdl_mapping("clk",master_clock));

	std::string vname=vprefix+std::to_string(node->get_node_id());
	ports.push_back(vhdl_templates::vhdl_mapping("tick",vname));

	//Create tick signal
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,1);

	//Instantiate pulser
	std::string inst_name("inst"+std::to_string(inst_cnt++));
	vhdl_templates::vhdl_entity_inst(ss_statm,inst_name,"pulser",generics,ports);
}

void vhdl_gen::solve_add(std::shared_ptr<frepr_add> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Get types
	std::string lefttype(solve_type(node->get_left()->get_datatype()));
	std::string righttype(solve_type(node->get_right()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_add(ss_statm,vname,lefttype,righttype,leftname,rightname);
}

void vhdl_gen::solve_sub(std::shared_ptr<frepr_sub> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Get types
	std::string lefttype(solve_type(node->get_left()->get_datatype()));
	std::string righttype(solve_type(node->get_right()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_sub(ss_statm,vname,lefttype,righttype,leftname,rightname);
}

void vhdl_gen::solve_sim(std::shared_ptr<frepr_sim> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string targetname=vprefix+std::to_string(node->get_target()->get_node_id());

	//Solve type
	std::string type(solve_type(node->get_target()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_sim(ss_statm,vname,type,targetname);
}

void vhdl_gen::solve_mult(std::shared_ptr<frepr_mult> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Get types
	std::string lefttype(solve_type(node->get_left()->get_datatype()));
	std::string righttype(solve_type(node->get_right()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_mult(ss_statm,vname,lefttype,righttype,leftname,rightname,node->get_datatype().get_dim());
}

void vhdl_gen::solve_div(std::shared_ptr<frepr_div> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Get types
	std::string lefttype(solve_type(node->get_left()->get_datatype()));
	std::string righttype(solve_type(node->get_right()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_div(ss_statm,vname,lefttype,righttype,leftname,rightname);
}

void vhdl_gen::solve_shl(std::shared_ptr<frepr_shl> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string toshiftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string amntname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Solve type
	std::string type(solve_type(node->get_left()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_shl(ss_statm,vname,type,toshiftname,amntname);
}

void vhdl_gen::solve_shr(std::shared_ptr<frepr_shr> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string toshiftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string amntname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Solve type
	std::string type(solve_type(node->get_left()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_shr(ss_statm,vname,type,toshiftname,amntname);
}

void vhdl_gen::solve_rtl(std::shared_ptr<frepr_rtl> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string torotatename=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string amntname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Solve type
	std::string type(solve_type(node->get_left()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_rtl(ss_statm,vname,type,torotatename,amntname);
}

void vhdl_gen::solve_rtr(std::shared_ptr<frepr_rtr> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string torotatename=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string amntname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Solve type
	std::string type(solve_type(node->get_left()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_rtr(ss_statm,vname,type,torotatename,amntname);
}

void vhdl_gen::solve_and(std::shared_ptr<frepr_and> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Instantiate template
	vhdl_templates::vhdl_and(ss_statm,vname,leftname,rightname);
}

void vhdl_gen::solve_or(std::shared_ptr<frepr_or> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Instantiate template
	vhdl_templates::vhdl_or(ss_statm,vname,leftname,rightname);
}

void vhdl_gen::solve_xor(std::shared_ptr<frepr_xor> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Instantiate template
	vhdl_templates::vhdl_xor(ss_statm,vname,leftname,rightname);
}

void vhdl_gen::solve_not(std::shared_ptr<frepr_not> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get target name
	std::string targetname=vprefix+std::to_string(node->get_target()->get_node_id());

	//Instantiate template
	vhdl_templates::vhdl_not(ss_statm,vname,targetname);
}

void vhdl_gen::solve_comp(std::shared_ptr<frepr_comp> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Solve types
	std::string type(solve_type(node->get_datatype()));
	std::shared_ptr<frepr_arith> aleft(std::static_pointer_cast<frepr_arith>(node->get_left()));
	std::shared_ptr<frepr_arith> aright(std::static_pointer_cast<frepr_arith>(node->get_right()));
	std::string lefttype(solve_type(aleft->get_datatype()));
	std::string righttype(solve_type(aright->get_datatype()));

	//Find the right template and instantiate it.
	switch(node->get_logic_op().get_op()){
		case logic_op::EQUAL:
			vhdl_templates::vhdl_equal(ss_statm,vname,leftname,rightname);
			break;
		case logic_op::DIFF:
			vhdl_templates::vhdl_diff(ss_statm,vname,leftname,rightname);
			break;
		case logic_op::MORE:
			vhdl_templates::vhdl_more(ss_statm,vname,lefttype,leftname,righttype,rightname);
			break;
		case logic_op::LESS:
			vhdl_templates::vhdl_less(ss_statm,vname,lefttype,leftname,righttype,rightname);
			break;
		case logic_op::MOREQ:
			vhdl_templates::vhdl_moreq(ss_statm,vname,lefttype,leftname,righttype,rightname);
			break;
		case logic_op::LESEQ:
			vhdl_templates::vhdl_leseq(ss_statm,vname,lefttype,leftname,righttype,rightname);
			break;
	}
}

void vhdl_gen::solve_mod(std::shared_ptr<frepr_mod> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string leftname=vprefix+std::to_string(node->get_left()->get_node_id());
	std::string rightname=vprefix+std::to_string(node->get_right()->get_node_id());

	//Get types
	std::string lefttype(solve_type(node->get_left()->get_datatype()));
	std::string righttype(solve_type(node->get_right()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_mod(ss_statm,vname,lefttype,righttype,leftname,rightname);
}

void vhdl_gen::solve_resize(std::shared_ptr<frepr_resize> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());
	vhdl_templates::vhdl_signal(ss_var_dcl,vname,node->get_datatype().get_dim());

	//Get names
	std::string targetname=vprefix+std::to_string(node->get_target()->get_node_id());

	//Solve type
	std::string type(solve_type(node->get_target()->get_datatype()));

	//Instantiate template
	vhdl_templates::vhdl_resize(ss_statm,vname,targetname,type,node->get_datatype().get_dim());
}

void vhdl_gen::solve_const(std::shared_ptr<frepr_const> node){
	//Add node var to ss_var_dcl
	std::string vname=vprefix+std::to_string(node->get_node_id());

	datatype dt(node->get_datatype());
	unsigned int dim(dt.get_dim()); //Fixed point representation
	switch(dt.get_datatype()){
		case datatype::SIGNED:
		case datatype::UNSIGNED:
			vhdl_templates::vhdl_const(ss_var_dcl,vname,dim,node->get_const());
			break;
		case datatype::FLOAT:
		case datatype::MIDI:
		case datatype::VUNDEF:
			std::cerr<<"VHDL_GEN CONST CANT SOLVE TYPE"<<std::endl;
			break;
	}
}
