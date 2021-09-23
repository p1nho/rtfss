#ifndef RTFSS_DEF_ANTLR_MAIN
#define RTFSS_DEF_ANTLR_MAIN

#include <iostream>
#include <memory>

#include "antlr4-runtime.h"
#include "rtfssLexer.h"
#include "rtfssParser.h"

#include "../ast/ast.h"
#include "parse_tree_translator.h"
#include "../error_handler/error_handler.h"

class antlr4_main{
	public:
		static std::shared_ptr<ast_start> create_ast(std::string &filename);		
};

#endif
