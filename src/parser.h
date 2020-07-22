#pragma once
#include <deque>

#include "tokenizer.h"

class Converter;
class TypeChecker;

enum Types {
    Number = 1,
    Int = Number,
    Boolean = Number,
    Float = Number,

    String = 2,
    List = 4,  // TODO: somehow list of n
    Reference = 8,

    StringNum = String | Number,

    unknown = Number | String | List,
};

struct AstNode {
    SourceLocation Location;

    AstNode(const SourceLocation& location) : Location(location) {}
    AstNode(const AstNode&) = delete;
    AstNode(AstNode&& o) noexcept : Location(o.Location) {}
    virtual ~AstNode(){};

    virtual void visit(Converter& converter, bool discard) = 0;
};

struct Statement : AstNode {
    Statement(const SourceLocation location) : AstNode(location) {}
};

struct Expression : Statement {
    Expression(const SourceLocation location) : Statement(location) {}
};

#pragma region Statements
struct LetItem {
    SourceLocation Location;

    std::vector<std::string> Names;
    Expression* Value = nullptr;

    LetItem(LetItem&& other) {
        this->Location = other.Location;
        this->Names = std::move(other.Names);
        this->Value = other.Value;
        other.Value = nullptr;
    }
    LetItem() {}
    LetItem(const SourceLocation location, const std::vector<std::string>& names, Expression* value) : Location(location), Names(names), Value(value) {}
    LetItem(const LetItem&) = delete;
    ~LetItem() { delete Value; }
};
struct LetStatement : Statement {
    Tokentype TokenType;
    std::vector<LetItem> Items;

    LetStatement(const SourceLocation location, Tokentype type, std::vector<LetItem>& items) : Statement(location), TokenType(type), Items(std::move(items)) {}

    void visit(Converter& converter, bool discard);
};

struct ReturnStatement : Statement {
    std::vector<Expression*> ReturnValues;

    ReturnStatement(const SourceLocation location, const std::vector<Expression*>& returnValues) : Statement(location), ReturnValues(returnValues) {}
    ~ReturnStatement() {
        for (auto item : ReturnValues) {
            delete item;
        }
    }

    void visit(Converter& converter, bool discard);
};

struct LoopStatement : Statement {
    Statement* Init = nullptr;
    Expression *Condition = nullptr, *Repeat = nullptr;
    Statement* Body = nullptr;

    LoopStatement(const SourceLocation location, Statement* init, Expression* condition, Expression* repeat, Statement* body)
        : Statement(location), Init(init), Condition(condition), Repeat(repeat), Body(body) {}

    ~LoopStatement() {
        delete Init;
        delete Condition;
        delete Repeat;
        delete Body;
    }

    void visit(Converter& converter, bool discard);
};

struct DoLoopStatement : Statement {
    Expression* Condition = nullptr;
    Statement* Body = nullptr;

    DoLoopStatement(const SourceLocation location, Expression* condition, Statement* body)
        : Statement(location), Condition(condition), Body(body) {}

    ~DoLoopStatement() {
        delete Condition;
        delete Body;
    }

    void visit(Converter& converter, bool discard);
};

struct ForOfStatement : Statement {
    LetStatement* Name = nullptr;
    Expression* Iterable = nullptr;
    Statement* Body = nullptr;

    ForOfStatement(const SourceLocation location, LetStatement* name, Expression* iterable, Statement* body)
        : Statement(location), Name(name), Iterable(iterable), Body(body) {}

    ~ForOfStatement() {
        delete Name;
        delete Iterable;
        delete Body;
    }

    void visit(Converter& converter, bool discard);
};

struct IfStatement : Statement {
    Expression* Condition = nullptr;
    Statement *If = nullptr, *Else = nullptr;

    IfStatement(const SourceLocation location, Expression* condition, Statement* if_, Statement* else_)
        : Statement(location), Condition(condition), If(if_), Else(else_) {}

    ~IfStatement() {
        delete Condition;
        delete If;
        delete Else;
    }

    void visit(Converter& converter, bool discard);
};
struct Scope : Statement {
    std::vector<Statement*> Body;

    Scope(const SourceLocation location, const std::vector<Statement*>& body) : Statement(location), Body(body) {}

    ~Scope() {
        for (auto& i : Body) {
            delete i;
        }
    }

    void visit(Converter& converter, bool discard);
};

struct Param {
    std::string Name;
    Expression* Default = nullptr;

    ~Param() { delete Default; }
};
struct FunctionDecl : Statement {
    std::string Name;
    std::vector<Param> Params;
    std::vector<Types> ReturnTypes;
    Scope* Body = nullptr;

    FunctionDecl(const SourceLocation location, std::string name, const std::vector<Param>& params, Scope* body) : Statement(location), Name(name), Params(params), Body(body) {}
    ~FunctionDecl() { delete Body; }

    void visit(Converter& converter, bool discard);
};
#pragma endregion

#pragma region
struct SelectExpression : Expression {
    Expression* Condition = nullptr;
    Statement *True = nullptr, *False = nullptr;

    SelectExpression(const SourceLocation location, Expression* condition, Statement* trueE, Statement* falseE) : Expression(location), Condition(condition), True(trueE), False(falseE) {}

    ~SelectExpression() {
        delete Condition;
        delete True;
        delete False;
    }

    void visit(Converter& converter, bool discard);
};
struct PreExpression : Expression {
    Tokentype Type;
    Expression* Right = nullptr;

    PreExpression(const SourceLocation location, Tokentype type, Expression* left) : Expression(location), Type(type), Right(left) {}
    ~PreExpression() { delete Right; }

    void visit(Converter& converter, bool discard);
};
struct PostExpression : Expression {
    Tokentype Type;
    Expression* Left = nullptr;

    PostExpression(const SourceLocation location, Tokentype type, Expression* right) : Expression(location), Type(type), Left(right) {}
    ~PostExpression() { delete Left; }

    void visit(Converter& converter, bool discard);
};
struct Operation : Expression {
    Tokentype Type;
    Expression* Left = nullptr;
    Expression* Right = nullptr;

    Operation(const SourceLocation location, Tokentype type, Expression* left, Expression* right) : Expression(location), Type(type), Left(left), Right(right) {}
    ~Operation() {
        delete Left;
        delete Right;
    }

    void visit(Converter& converter, bool discard);
};
struct FunctionCall : Expression {
    Expression* Left = nullptr;
    std::vector<Expression*> Params;

    FunctionCall(const SourceLocation location, Expression* left, const std::vector<Expression*>& params) : Expression(location), Left(left), Params(params) {}
    ~FunctionCall() {
        delete Left;
        for (auto el : Params) {
            delete el;
        }
    }

    void visit(Converter& converter, bool discard);
};

struct IntLit : Expression {
    int64_t Value;
    IntLit(const SourceLocation location, int64_t value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard);
};
struct FloatLit : Expression {
    double Value;
    FloatLit(const SourceLocation location, double value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard);
};
struct StringLit : Expression {
    std::string Value;
    StringLit(const SourceLocation location, std::string value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard);
};
struct BoolLit : Expression {
    bool Value;
    BoolLit(const SourceLocation location, bool value) : Expression(location), Value(value) {}

    void visit(Converter& converter, bool discard);
};
struct ArrayLit : Expression {
    std::vector<Expression*> Elements;

    ArrayLit(const SourceLocation location, const std::vector<Expression*>& elements) : Expression(location), Elements(elements) {}
    ~ArrayLit() {
        for (auto i : Elements) {
            delete i;
        }
    }
    void visit(Converter& converter, bool discard);
};
struct Variable : Expression {
    std::string Name;
    Variable(const SourceLocation location, std::string name) : Expression(location), Name(name) {}

    void visit(Converter& converter, bool discard);
};
#pragma endregion

class Parser {
   private:
    std::deque<Token> Tokens;
    Token CurrentToken;

   public:
    Parser(std::deque<Token> tokens);

    std::vector<Statement*> Parse();

   private:
    void acceptIt();
    std::string accept(Tokentype type);

    Statement* ParseStatement();
    Scope* ParseScope();

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
    Expression* ParsePrefix();
    Expression* ParsePostfix();
    Expression* ParseAtom();
};
