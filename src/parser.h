#pragma once
#include <deque>

#include "tokenizer.h"
// #include "converter.h"

class Converter;

struct AstNode {
    SourceLocation Location;

    AstNode(const SourceLocation& location) : Location(location) {}
    AstNode(const AstNode&) = delete;
    AstNode(AstNode&& o) noexcept : Location(o.Location) {}
	virtual ~AstNode() { };

    virtual void visit(Converter& converter, bool discard) const = 0;
};

struct Statement : AstNode {
    Statement(const SourceLocation location) : AstNode(location) {}
};

struct Expression : Statement {
    Expression(const SourceLocation location) : Statement(location) {}
};

#pragma region Statements
struct ReturnStatement : Statement {
    Expression* ReturnValue;

    ReturnStatement(const SourceLocation location, Expression* returnValue) : Statement(location), ReturnValue(returnValue) {}
    ~ReturnStatement() { delete ReturnValue; }

    void visit(Converter& converter, bool discard) const;
};

struct LoopStatement : Statement {
    Expression *Init, *Condition, *Repeat;
    Statement* Body;

    LoopStatement(const SourceLocation location, Expression* init, Expression* condition, Expression* repeat, Statement* body)
        : Statement(location), Init(init), Condition(condition), Repeat(repeat), Body(body) {}

    ~LoopStatement() {
        delete Init;
        delete Condition;
        delete Repeat;
        delete Body;
    }

    void visit(Converter& converter, bool discard) const;
};

struct DoLoopStatement : Statement {
    Expression* Condition;
    Statement* Body;

    DoLoopStatement(const SourceLocation location, Expression* condition, Statement* body)
        : Statement(location), Condition(condition), Body(body) {}

    ~DoLoopStatement() {
        delete Condition;
        delete Body;
    }

    void visit(Converter& converter, bool discard) const;
};
struct IfStatement : Statement {
    Expression* Condition;
    Statement *If, *Else;

    IfStatement(const SourceLocation location, Expression* condition, Statement* if_, Statement* else_)
        : Statement(location), Condition(condition), If(if_), Else(else_) {}

    ~IfStatement() {
        delete Condition;
        delete If;
        delete Else;
    }

    void visit(Converter& converter, bool discard) const;
};
struct Scope : Statement {
    std::vector<Statement*> Body;

    Scope(const SourceLocation location, std::vector<Statement*> body) : Statement(location), Body(body) {}

    ~Scope() {
        for (auto i : Body) {
            delete i;
        }
    }

    void visit(Converter& converter, bool discard) const;
};
struct LetStatement : Statement {
    Tokentype Type;
    std::string Name;
    Expression* Value = nullptr;

    LetStatement(const SourceLocation location, Tokentype type, std::string name, Expression* value) : Statement(location), Type(type), Name(name), Value(value) {}
    ~LetStatement() { delete Value; }

    std::string getName() const {
        if(Type == Tokentype::Const) {
            return Name;
        }
        return Name + "_" + std::to_string(Location.line) + "_" + std::to_string(Location.column);
    }
    void visit(Converter& converter, bool discard) const;
};
struct Param {
    std::string Name;
    Expression* Default;

    ~Param() { delete Default; }
};
struct FunctionDecl : Statement {
    std::string Name;
    std::vector<Param> Params;
    Statement* Body;

    FunctionDecl(const SourceLocation location, std::string name, std::vector<Param> params, Statement* body) : Statement(location), Name(name), Params(params), Body(body) {}
    ~FunctionDecl() { delete Body; }

    void visit(Converter& converter, bool discard) const;
};
#pragma endregion

#pragma region
struct SelectExpression : Expression {
    Expression* Condition;
    Statement *True, *False;

    SelectExpression(const SourceLocation location, Expression* condition, Statement* trueE, Statement* falseE) : Expression(location), Condition(condition), True(trueE), False(falseE) {}

    ~SelectExpression() {
        delete Condition;
        delete True;
        delete False;
    }

    void visit(Converter& converter, bool discard) const;
};
struct PreExpression : Expression {
    Tokentype Type;
    Expression* Left;

    PreExpression(const SourceLocation location, Tokentype type, Expression* left) : Expression(location), Type(type), Left(left) {}
    ~PreExpression() { delete Left; }

    void visit(Converter& converter, bool discard) const;
};
struct PostExpression : Expression {
    Tokentype Type;
    Expression* Right;

    PostExpression(const SourceLocation location, Tokentype type, Expression* right) : Expression(location), Type(type), Right(right) {}
    ~PostExpression() { delete Right; }

    void visit(Converter& converter, bool discard) const;
};
struct Operation : Expression {
    Tokentype Type;
    Expression* Left;
    Expression* Right;

    Operation(const SourceLocation location, Tokentype type, Expression* left, Expression* right) : Expression(location), Type(type), Left(left), Right(right) {}
    ~Operation() {
        delete Left;
        delete Right;
    }

    void visit(Converter& converter, bool discard) const;
};
struct FunctionCall : Expression {
    Expression* Left;
    std::vector<Expression*> Params;

    FunctionCall(const SourceLocation location, Expression* left, std::vector<Expression*> params) : Expression(location), Left(left), Params(std::move(params)) {}
    ~FunctionCall() {
        delete Left;
        for (auto el : Params) {
            delete el;
        }
    }

    void visit(Converter& converter, bool discard) const;
};

struct IntLit : Expression {
    int64_t Value;
    IntLit(const SourceLocation location, int64_t value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard) const;
};
struct FloatLit : Expression {
    double Value;
    FloatLit(const SourceLocation location, double value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard) const;
};
struct StringLit : Expression {
    std::string Value;
    StringLit(const SourceLocation location, std::string value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard) const;
};
struct BoolLit : Expression {
    bool Value;
    BoolLit(const SourceLocation location, bool value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard) const;
};
struct ArrayLit: Expression {
    std::vector<Expression*> Elements;

    ArrayLit(const SourceLocation location, std::vector<Expression*> elements): Expression(location), Elements(elements) {}
    ~ArrayLit() {
        for (auto i : Elements) {
            delete i;
        }
    }
    void visit(Converter& converter, bool discard) const;
};
struct Variable : Expression {
    std::string Name;
    Variable(const SourceLocation location, std::string name) : Expression(location), Name(name) {}

    void visit(Converter& converter, bool discard) const;
};
#pragma endregion

class Parser {
   private:
    std::deque<Token> Tokens;
    Token CurrentToken;

   public:
    Parser(std::deque<Token> tokens);

    std::vector<AstNode*> Parse();

   private:
    void acceptIt();
    std::string accept(Tokentype type);

    Statement* ParseStatement();

    Expression* ParseExpression();
    Expression* ParseSelect();
    Expression* ParseLor();
    Expression* ParseLand();
    Expression* ParseNullCoal();
    Expression* ParseOr();
    Expression* ParseXor();
    Expression* ParseAnd();
    Expression* ParseEq();
    Expression* ParseComp();
    Expression* ParseShift();
    Expression* ParseAdd();
    Expression* ParseMul();
    Expression* ParsePow();
    Expression* ParsePrefix();
    Expression* ParsePostfix();
    Expression* ParseAtom();
};