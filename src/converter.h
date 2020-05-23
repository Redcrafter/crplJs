#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stack>

#include "parser.h"

class AstNode;

struct ScopeItem {
	std::string Codename;
	Types Typename;
	Tokentype Toketype;
};

class Converter {
public:
    std::ostringstream buffer;
private:
    const ScopeItem* findVar(const std::string& name);
public:
    std::vector<std::map<std::string, ScopeItem>> scopes;
	std::map<std::string, FunctionDecl*> functions;
	int depth = 0;
    std::stack<Types> stack;
	
    Converter() {
        scopes.emplace_back();
    }
	
	void newLine();

	void visit(AstNode* node, bool discard);
	
    void visit(const ReturnStatement& node);
    void visit(const LoopStatement& node);
    void visit(const DoLoopStatement& node);
    void visit(const IfStatement& node);
    void visit(const Scope& node);
    void visit(LetStatement& node);
    void visit(const FunctionDecl& node);

    void visit(const SelectExpression& node, bool discard);
    void visit(const PreExpression& node, bool discard);
    void visit(const PostExpression& node, bool discard);
    void visit(const Operation& node, bool discard);
    void visit(const FunctionCall& node, bool discard);

    void visit(const IntLit& node, bool discard);
    void visit(const FloatLit& node, bool discard);
    void visit(const StringLit& node, bool discard);
    void visit(const BoolLit& node, bool discard);
    void visit(const ArrayLit& node, bool discard);
    void visit(const Variable& node, bool discard);

    void visitFunctionDecl(const FunctionDecl& node);
};

std::string crpl(const std::vector<AstNode*>& ast, const std::string& fileName);