#include "antlr4_main.h"

std::shared_ptr<ast_start> antlr4_main::create_ast(std::string &filename) {

	antlr4::ANTLRFileStream infile;
	infile.loadFromFile(filename);

	rtfssLexer rl(&infile);
	antlr4::CommonTokenStream tokens(&rl);
	tokens.fill();
	rtfssParser rp(&tokens);

	antlr4::tree::ParseTree *tree=rp.entry_point();

	if(rl.getNumberOfSyntaxErrors()!=0||rp.getNumberOfSyntaxErrors()!=0){
		error_handler::syntax_error();	
	}

	std::shared_ptr<ast_start> start(ast_start::build_start(filename));

	parse_tree_translator ptt(start);
	ptt.visit(tree);	


	return start;
}
