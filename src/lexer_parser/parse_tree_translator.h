#ifndef RTFSS_DEF_PARSE_TREE_TRANSLATOR
#define RTFSS_DEF_PARSE_TREE_TRANSLATOR

#include "antlr4-runtime.h"
#include "rtfssBaseVisitor.h"
#include "../ast/ast.h"
#include "tree/ParseTreeProperty.h"
#include "tree/ParseTree.h"
#include <memory>
#include <cassert>
#include "../error_handler/error_handler.h"

class parse_tree_translator : public rtfssVisitor{
	private:
		std::shared_ptr<ast_start> &start;
		antlr4::tree::ParseTreeProperty<std::shared_ptr<ast>> ptp;
	public:
		parse_tree_translator(std::shared_ptr<ast_start> &start):start(start){};

		virtual antlrcpp::Any visitEntry_point(rtfssParser::Entry_pointContext *ctx) override;
		virtual antlrcpp::Any visitUse_stat(rtfssParser::Use_statContext *ctx) override;
		virtual antlrcpp::Any visitCblock_decl(rtfssParser::Cblock_declContext *ctx) override;
		virtual antlrcpp::Any visitCblock_args(rtfssParser::Cblock_argsContext *ctx) override;
		virtual antlrcpp::Any visitCblock_arg_list(rtfssParser::Cblock_arg_listContext *ctx) override;
		virtual antlrcpp::Any visitCode_block(rtfssParser::Code_blockContext *ctx) override;
		virtual antlrcpp::Any visitStatm(rtfssParser::StatmContext *ctx) override;
		virtual antlrcpp::Any visitTerminated_statm(rtfssParser::Terminated_statmContext *ctx) override;
		virtual antlrcpp::Any visitNonterminated_statm(rtfssParser::Nonterminated_statmContext *ctx) override;
		virtual antlrcpp::Any visitIf_statm(rtfssParser::If_statmContext *ctx) override;
		virtual antlrcpp::Any visitFor_statm(rtfssParser::For_statmContext *ctx) override;
		virtual antlrcpp::Any visitCblock_inst(rtfssParser::Cblock_instContext *ctx) override;
		virtual antlrcpp::Any visitCblock_inst_short(rtfssParser::Cblock_inst_shortContext *ctx) override;
		virtual antlrcpp::Any visitPulse_decl(rtfssParser::Pulse_declContext *ctx) override;
		virtual antlrcpp::Any visitData_decl(rtfssParser::Data_declContext *ctx) override;
		virtual antlrcpp::Any visitVar_assign(rtfssParser::Var_assignContext *ctx) override;
		virtual antlrcpp::Any visitAssign_op(rtfssParser::Assign_opContext *ctx) override;
		virtual antlrcpp::Any visitNntofExpr(rtfssParser::NntofExprContext *ctx) override;
		virtual antlrcpp::Any visitModExpr(rtfssParser::ModExprContext *ctx) override;
		virtual antlrcpp::Any visitPropExpr(rtfssParser::PropExprContext *ctx) override;
		virtual antlrcpp::Any visitCcofExpr(rtfssParser::CcofExprContext *ctx) override;
		virtual antlrcpp::Any visitSubExpr(rtfssParser::SubExprContext *ctx) override;
		virtual antlrcpp::Any visitXorExpr(rtfssParser::XorExprContext *ctx) override;
		virtual antlrcpp::Any visitParExpr(rtfssParser::ParExprContext *ctx) override;
		virtual antlrcpp::Any visitCastBExpr(rtfssParser::CastBExprContext *ctx) override;
		virtual antlrcpp::Any visitSidExpr(rtfssParser::SidExprContext *ctx) override;
		virtual antlrcpp::Any visitGapExpr(rtfssParser::GapExprContext *ctx) override;
		virtual antlrcpp::Any visitRotExpr(rtfssParser::RotExprContext *ctx) override;
		virtual antlrcpp::Any visitArrExpr(rtfssParser::ArrExprContext *ctx) override;
		virtual antlrcpp::Any visitDivExpr(rtfssParser::DivExprContext *ctx) override;
		virtual antlrcpp::Any visitNoteofExpr(rtfssParser::NoteofExprContext *ctx) override;
		virtual antlrcpp::Any visitVelofExpr(rtfssParser::VelofExprContext *ctx) override;
		virtual antlrcpp::Any visitOrExpr(rtfssParser::OrExprContext *ctx) override;
		virtual antlrcpp::Any visitCastAExpr(rtfssParser::CastAExprContext *ctx) override;
		virtual antlrcpp::Any visitMultExpr(rtfssParser::MultExprContext *ctx) override;
		virtual antlrcpp::Any visitPbendExpr(rtfssParser::PbendExprContext *ctx) override;
		virtual antlrcpp::Any visitNumExpr(rtfssParser::NumExprContext *ctx) override;
		virtual antlrcpp::Any visitCpofExpr(rtfssParser::CpofExprContext *ctx) override;
		virtual antlrcpp::Any visitShiftExpr(rtfssParser::ShiftExprContext *ctx) override;
		virtual antlrcpp::Any visitPatofExpr(rtfssParser::PatofExprContext *ctx) override;
		virtual antlrcpp::Any visitNotExpr(rtfssParser::NotExprContext *ctx) override;
		virtual antlrcpp::Any visitCblkExpr(rtfssParser::CblkExprContext *ctx) override;
		virtual antlrcpp::Any visitAddExpr(rtfssParser::AddExprContext *ctx) override;
		virtual antlrcpp::Any visitNegExpr(rtfssParser::NegExprContext *ctx) override;
		virtual antlrcpp::Any visitPofExpr(rtfssParser::PofExprContext *ctx) override;
		virtual antlrcpp::Any visitEvalExpr(rtfssParser::EvalExprContext *ctx) override;
		virtual antlrcpp::Any visitFreqofExpr(rtfssParser::FreqofExprContext *ctx) override;
		virtual antlrcpp::Any visitAndExpr(rtfssParser::AndExprContext *ctx) override;
		virtual antlrcpp::Any visitVar_name(rtfssParser::Var_nameContext *ctx) override;
		virtual antlrcpp::Any visitStream_id(rtfssParser::Stream_idContext *ctx) override;
		virtual antlrcpp::Any visitData_type(rtfssParser::Data_typeContext *ctx) override;
		virtual antlrcpp::Any visitFixedSizeType(rtfssParser::FixedSizeTypeContext *ctx) override;
		virtual antlrcpp::Any visitVarSizeType(rtfssParser::VarSizeTypeContext *ctx) override;
		virtual antlrcpp::Any visitPulse_freq(rtfssParser::Pulse_freqContext *ctx) override;
};

#endif
