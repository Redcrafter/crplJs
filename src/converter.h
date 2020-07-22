#pragma once
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "parser.h"

class AstNode;

struct ScopeItem {
    std::string Codename;
    // Types Typename;
    Tokentype Toketype;
};

struct StackItem {
    std::map<std::string, ScopeItem> variables;
    std::map<std::string, FunctionDecl*> functions;
};

class Converter {
    std::ostringstream functionBuffer;

    std::ostringstream* buffer = nullptr;
    int depth = 0;
    int forOfDepth = 0;
    std::stack<Types> stack;

    std::vector<StackItem> scopes;

public:
    Converter() {
        scopes.emplace_back();
    }

    std::string Convert(const std::vector<Statement*>& ast, const std::string& fileName);

private:
    const ScopeItem* findVar(const std::string& name);
    const FunctionDecl* findFunc(const std::string& name);
    void newLine();

    void declareFunctions(const std::vector<Statement*> body);
    void visit(AstNode* node, bool discard);

public:
    void visit(const ReturnStatement& node);
    void visit(const LoopStatement& node);
    void visit(const DoLoopStatement& node);
    void visit(const ForOfStatement& node);
    void visit(const IfStatement& node);
    void visit(const Scope& node);
    void visit(const LetStatement& node);
    void visit(FunctionDecl& node);

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
};
