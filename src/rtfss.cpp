#include <fstream>
#include <iostream>
#include <sstream>
#include "ast/ast.h"
#include "ast/ast_property.h"
#include "ast/ast_integrity.h"
#include "const_solver/const_solver.h"
#include "var_solver/var_solver.h"
#include "lexer_parser/antlr4_main.h"
#include "assign_handler/assign_trimmer.h"
#include "assign_handler/assign_mover.h"
#include "arch_gen/arch_graph.h"
#include "arch_gen/arch_gen.h"
#include "frepr_gen/frepr_node.h"
#include "frepr_gen/frepr_gen.h"
#include "frepr_gen/frepr_gv.h"
#include "target_gen/vhdl/vhdl_gen.h"

#include <cstdio>
FILE* draw_gv(std::stringstream &ss){
	FILE *gv=popen("dot -Txlib","w");
	if(gv==NULL)
		return NULL;

	std::string str(ss.str());
	const char *cstr(str.c_str());
	fwrite(cstr,sizeof(char),str.size(),gv);
	//fclose(gv);
	return gv;
}
void save_ss(std::stringstream &ss,std::string filename){
	std::ofstream f(filename,std::ios_base::out|std::ios_base::trunc);
	f<<ss.str();
	f.close();
}

int main(int argc,char** argv) {
	(void) argc; //Temporary
	std::vector<FILE*> draw_windows;

	std::string filename(argv[1]);
	bool save(argc==3);
	std::string save_folder;
	if(save){
		save_folder=argv[2];
		save_folder+='/';
	}

	try{
	//Tokenize, Parse, and generate AST
	std::cerr<<"antlr4_main"<<std::endl;
	std::shared_ptr<ast_start> start(antlr4_main::create_ast(filename));
	ast_integrity(start).check();
	std::stringstream ss_ast;
	start->gen_vz(ss_ast);
	if(save) save_ss(ss_ast,save_folder+"create_ast.dot");
	draw_windows.push_back(draw_gv(ss_ast));

	std::cerr<<"const_solver"<<std::endl;
	const_solver cs(start);
	cs.execute();
	ast_integrity(start).check();
	std::stringstream ss_cs;
	start->gen_vz(ss_cs);
	if(save) save_ss(ss_cs,save_folder+"const_solver.dot");
	draw_windows.push_back(draw_gv(ss_cs));

	//TODO tree balancer

	ast_property<unsigned int> var_id;
	ast_property<std::shared_ptr<var_scope>> vscopes;
	std::shared_ptr<var_complete_scope> vcscopes;

	ast_property<unsigned int> pulse_id;
	ast_property<std::shared_ptr<pulse_scope>> pscopes;
	std::shared_ptr<pulse_complete_scope> pcscopes;

	std::cerr<<"var_solver"<<std::endl;
	var_solver vs(start,var_id,vscopes,pulse_id,pscopes);
	vs.execute();
	vcscopes=(*vscopes.get(start))->get_complete();
	pcscopes=(*pscopes.get(start))->get_complete();

	std::cerr<<"assign_trimmer"<<std::endl;
	assign_trimmer at(start,var_id,vcscopes);
	at.execute();
	ast_integrity(start).check();
	std::stringstream ss_at;
	start->gen_vz(ss_at);
	if(save) save_ss(ss_at,save_folder+"assign_trimmer.dot");
	draw_windows.push_back(draw_gv(ss_at));

	std::cerr<<"assign_mover"<<std::endl;
	assign_mover am(start,var_id,vcscopes,pulse_id);
	am.execute();
	ast_integrity(start).check();
	std::stringstream ss_am;
	start->gen_vz(ss_am);
	if(save) save_ss(ss_am,save_folder+"assign_mover.dot");
	draw_windows.push_back(draw_gv(ss_am));

	std::cerr<<"arch_gen"<<std::endl;
	arch_graph graph(pcscopes->get().size());
	arch_gen ag(start,var_id,vcscopes,pcscopes,graph);
	ag.execute();
	std::stringstream ss_arch;
	graph.gen_gv(ss_arch);
	if(save) save_ss(ss_arch,save_folder+"arch_gen.dot");
	draw_windows.push_back(draw_gv(ss_arch));

	std::cerr<<"frepr_gen"<<std::endl;
	frepr_anchor anchor;
	frepr_gen fg(graph,anchor,pcscopes);
	fg.execute();
	std::stringstream ss_frepr;
	frepr_gv(anchor,ss_frepr).execute();
	if(save) save_ss(ss_frepr,save_folder+"frepr_gen.dot");
	draw_windows.push_back(draw_gv(ss_frepr));

	std::cerr<<"vhdl_gen"<<std::endl;
	std::stringstream ss_code;
	vhdl_gen(anchor,ss_code).execute();
	if(save) save_ss(ss_code,save_folder+"vhdl_gen.vhd");
	//std::cout<<ss_code.str()<<std::endl;

	//Free popen mem
	/*for(auto f=draw_windows.begin();f!=draw_windows.end();f++){
		fclose(*f);
	}
	*/
	}
	catch(rtfss_error e){
		return 0;
	}

	return 0;
}
