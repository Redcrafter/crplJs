#include <stdexcept>
#include <functional>

#include "logger.h"
#include "parser.h"
#include "converter.h"

void ReturnStatement::visit(Converter& converter, bool discard) const { converter.visit(*this); }
void LoopStatement::visit(Converter& converter, bool discard) const { converter.visit(*this); }
void DoLoopStatement::visit(Converter& converter, bool discard) const { converter.visit(*this); }
void IfStatement::visit(Converter& converter, bool discard) const { converter.visit(*this); }
void Scope::visit(Converter& converter, bool discard) const { converter.visit(*this); }
void LetStatement::visit(Converter& converter, bool discard) const { converter.visit(*this); }
void FunctionDecl::visit(Converter& converter, bool discard) const { converter.visit(*this); }
void SelectExpression::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void PreExpression::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void PostExpression::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void Operation::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void FunctionCall::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void IntLit::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void FloatLit::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void StringLit::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void BoolLit::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }
void Variable::visit(Converter& converter, bool discard) const { converter.visit(*this, discard); }

Parser::Parser(std::deque<Token> tokens): Tokens(std::move(tokens)), CurrentToken(Tokens.front()) {
	Tokens.pop_front();
}

std::string Parser::accept(Tokentype type) {
	Token t = CurrentToken;
    if(CurrentToken.type != type) {
		LogError(Error, CurrentToken.location, "Unexpected symbol \"" + CurrentToken.spelling + '"');
    }
    acceptIt();
    return t.spelling;
}

void Parser::acceptIt() {
	CurrentToken = Tokens.front();
	Tokens.pop_front();
}

std::vector<AstNode*> Parser::Parse() {
    std::vector<AstNode*> statements;

    while (CurrentToken.type != Tokentype::Eof) {
        auto st = ParseStatement();
        statements.push_back(st);
    }
    
    return statements;
}

Statement* Parser::ParseStatement() {
    const SourceLocation location = CurrentToken.location;

    switch (CurrentToken.type) {
        case Tokentype::Return: {
            acceptIt();
            Expression* expr = nullptr;

            if (CurrentToken.type != Tokentype::Semicolon) {
                expr = ParseExpression();
            }
            if (CurrentToken.type == Tokentype::Semicolon) {
                acceptIt();
            }
            return new ReturnStatement(location, expr);
        }
        case Tokentype::For: {
            Expression* a = nullptr;
            Expression* b = nullptr;
            Expression* c = nullptr;
            acceptIt();
            accept(Tokentype::LParen);
            if (CurrentToken.type != Tokentype::Semicolon) {
                a = ParseExpression();
            }
            accept(Tokentype::Semicolon);
            if (CurrentToken.type != Tokentype::Semicolon) {
                b = ParseExpression();
            }
            accept(Tokentype::Semicolon);
            if (CurrentToken.type != Tokentype::Semicolon) {
                a = ParseExpression();
            }
            accept(Tokentype::RParen);

            return new LoopStatement(location, a, b, c, ParseStatement());
        }
        case Tokentype::While: {
            acceptIt();
            accept(Tokentype::LParen);
            auto condition = ParseExpression();
            accept(Tokentype::RParen);
            return new LoopStatement(location, nullptr, condition, nullptr, ParseStatement());
        }
        case Tokentype::Do: {
            acceptIt();
            auto body = ParseStatement();
            accept(Tokentype::While);
            accept(Tokentype::LParen);
            auto condition = ParseExpression();
            accept(Tokentype::RParen);

            return new DoLoopStatement(location, condition, body);
        }
        case Tokentype::If: {
            acceptIt();
            accept(Tokentype::LParen);
            auto condition = ParseExpression();
            accept(Tokentype::RParen);
            auto ifS = ParseStatement();
            Statement* elseS = nullptr;
            if (CurrentToken.type == Tokentype::Else) {
                acceptIt();
                elseS = ParseStatement();
            }

            return new IfStatement(location, condition, ifS, elseS);
        }
        case Tokentype::LBrace: {
            acceptIt();
            std::vector<Statement*> body;
            while (CurrentToken.type != Tokentype::RBrace) {
                body.push_back(ParseStatement());
            }
            acceptIt();
            return new Scope(location, body);
        }
        case Tokentype::Const:
        case Tokentype::Var:
        case Tokentype::Let: {
            auto type = CurrentToken.type;
            acceptIt();
            auto name = accept(Tokentype::ID);
            Expression* value = nullptr;
            if (CurrentToken.type == Tokentype::Assign) {
                acceptIt();
                value = ParseExpression();
            }
            accept(Tokentype::Semicolon);

            return new LetStatement(location, type, name, value);
        }
        case Tokentype::Function: {
            acceptIt();
            auto name = accept(Tokentype::ID);
            accept(Tokentype::LParen);

            // TODO: params
            std::vector<Param> params;
            while (CurrentToken.type != Tokentype::RParen) {
                auto name = accept(Tokentype::ID);
                Expression* def = nullptr;
                if (CurrentToken.type == Tokentype::Assign) {
                    acceptIt();
                    def = ParseExpression();
                }
                params.push_back(Param { name, def });
                if (CurrentToken.type != Tokentype::Comma) {
                    break;
                }
                acceptIt();
            }
            accept(Tokentype::RParen);
            auto body = ParseStatement();

            return new FunctionDecl(location, name, params, body);
        }
    }

    auto expr = ParseExpression();
    accept(Tokentype::Semicolon);
    return expr;
}

Expression* Parser::ParseExpression() {
    auto location = CurrentToken.location;
    auto left = ParseSelect();

    switch (CurrentToken.type) {
        case Tokentype::Assign:
        case Tokentype::AddAssign:
        case Tokentype::SubAssign:
        case Tokentype::MulAssign:
        case Tokentype::DivAssign:
        case Tokentype::ModAssign:
        case Tokentype::AndAssign:
        case Tokentype::OrAssign:
        case Tokentype::XorAssign:
        case Tokentype::LshiftAssign:
        case Tokentype::RshiftAssign:
            Tokentype type = CurrentToken.type;
            acceptIt();
            return new Operation(location, type, left, ParseExpression());
    }
    return left;
}

Expression* Parser::ParseSelect() {
    auto location = CurrentToken.location;

    auto select = ParseLor();
    if (CurrentToken.type == Tokentype::QMark) {
        acceptIt();
        auto trueVal = ParseSelect();
        accept(Tokentype::Colon);
        auto falseVal = ParseSelect();

        return new SelectExpression(location, select, trueVal, falseVal);
    }
    return select;
}

Expression* Parser::ParseLor() {
    auto location = CurrentToken.location;
    auto left = ParseLand();
    if (CurrentToken.type == Tokentype::LOr) {
        acceptIt();
        return new Operation(location, Tokentype::LOr, left, ParseLor());
    }
    return left;
}

Expression* Parser::ParseLand() {
    auto location = CurrentToken.location;
    auto left = ParseNullCoal();
    if (CurrentToken.type == Tokentype::LAnd) {
        acceptIt();
        return new Operation(location, Tokentype::LAnd, left, ParseLand());
    }
    return left;
}

Expression* Parser::ParseNullCoal() {
    auto location = CurrentToken.location;
    auto left = ParseOr();
    if (CurrentToken.type == Tokentype::NullCoal) {
        acceptIt();
        return new Operation(location, Tokentype::NullCoal, left, ParseNullCoal());
    }
    return left;
}

Expression* Parser::ParseOr() {
    auto location = CurrentToken.location;
    auto left = ParseXor();
    if (CurrentToken.type == Tokentype::Or) {
        acceptIt();
        return new Operation(location, Tokentype::Or, left, ParseOr());
    }
    return left;
}

Expression* Parser::ParseXor() {
    auto location = CurrentToken.location;
    auto left = ParseAnd();
    if (CurrentToken.type == Tokentype::Xor) {
        acceptIt();
        return new Operation(location, Tokentype::Xor, left, ParseXor());
    }
    return left;
}

Expression* Parser::ParseAnd() {
    auto location = CurrentToken.location;
    auto left = ParseEq();
    if (CurrentToken.type == Tokentype::And) {
        acceptIt();
        return new Operation(location, Tokentype::And, left, ParseAnd());
    }
    return left;
}

Expression* Parser::ParseEq() {
    auto location = CurrentToken.location;
    auto left = ParseComp();
    if (CurrentToken.type == Tokentype::Equals ||
        CurrentToken.type == Tokentype::NEquals) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseEq());
    }
    return left;
}

Expression* Parser::ParseComp() {
    auto location = CurrentToken.location;
    auto left = ParseShift();
    if (CurrentToken.type == Tokentype::Less ||
        CurrentToken.type == Tokentype::LessEq ||
        CurrentToken.type == Tokentype::Greater ||
        CurrentToken.type == Tokentype::GreaterEq) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseComp());
    }
    return left;
}

Expression* Parser::ParseShift() {
    auto location = CurrentToken.location;
    auto left = ParseAdd();
    if (CurrentToken.type == Tokentype::LShift ||
        CurrentToken.type == Tokentype::RShift) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseShift());
    }
    return left;
}

Expression* Parser::ParseAdd() {
    auto location = CurrentToken.location;
    auto left = ParseMul();
    if (CurrentToken.type == Tokentype::Add ||
        CurrentToken.type == Tokentype::Sub) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseAdd());
    }
    return left;
}

Expression* Parser::ParseMul() {
    auto location = CurrentToken.location;
    auto left = ParsePrefix();
    if (CurrentToken.type == Tokentype::Mul ||
        CurrentToken.type == Tokentype::Div ||
        CurrentToken.type == Tokentype::Mod) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseComp());
    }
    return left;
}

Expression* Parser::ParsePrefix() {
    auto location = CurrentToken.location;

    auto type = CurrentToken.type;
    switch (type) {
        case Tokentype::Inc:
        case Tokentype::Dec:
        case Tokentype::Add:
        case Tokentype::Sub:
        case Tokentype::LNot:
        case Tokentype::Not:
            acceptIt();
            return new PreExpression(location, type, ParsePrefix());
        // case Tokentype::LParen: {
        //     acceptIt();
        //     auto type = ParseType();
        //     accept(Tokentype::RParen);

        //     return new CastExpression(location, type, ParsePrefix());
        // }
    }

    return ParsePostfix();
}

Expression* Parser::ParsePostfix() {
    auto location = CurrentToken.location;
    auto current = ParseAtom();

    while (true) {
        auto type = CurrentToken.type;
        switch (type) {
            case Tokentype::Inc:
            case Tokentype::Dec:
                acceptIt();
                current = new PostExpression(location, type, current);
                continue;
            case Tokentype::LParen:
                acceptIt();
                std::vector<Expression*> params;
                while (CurrentToken.type != Tokentype::RParen) {
                    params.push_back(ParseExpression());
                    if (CurrentToken.type == Tokentype::Comma) {
                        acceptIt();
                    }
                }
                current = new FunctionCall(location, current, params);
                accept(Tokentype::RParen);
                continue;
        }
        break;
    }

    return current;
}

Expression* Parser::ParseAtom() {
    auto location = CurrentToken.location;

    switch (CurrentToken.type) {
        case Tokentype::IntLit: return new IntLit(location, std::stol(accept(Tokentype::IntLit)));
        case Tokentype::FloatLit: return new FloatLit(location, std::stod(accept(Tokentype::FloatLit)));
        case Tokentype::StringLit: return new StringLit(location, accept(Tokentype::StringLit));
        case Tokentype::BoolLit: return new BoolLit(location, accept(Tokentype::BoolLit) == "true");
        case Tokentype::ID: return new Variable(location, accept(Tokentype::ID));
        case Tokentype::LParen:
            acceptIt();
            auto ret = ParseExpression();
            accept(Tokentype::RParen);
            return ret;
    }

	LogError(Error, CurrentToken.location, "Unexpected symbol \"" + CurrentToken.spelling + '"');
}