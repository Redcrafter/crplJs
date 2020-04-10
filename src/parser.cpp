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

std::string Parser::accept(TokenType type) {
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

    while (CurrentToken.type != TokenType::Eof) {
        auto st = ParseStatement();
        statements.push_back(st);
    }
    
    return statements;
}

Statement* Parser::ParseStatement() {
    const SourceLocation location = CurrentToken.location;

    switch (CurrentToken.type) {
        case TokenType::Return: {
            acceptIt();
            Expression* expr = nullptr;

            if (CurrentToken.type != TokenType::Semicolon) {
                expr = ParseExpression();
            }
            if (CurrentToken.type == TokenType::Semicolon) {
                acceptIt();
            }
            return new ReturnStatement(location, expr);
        }
        case TokenType::For: {
            Expression* a = nullptr;
            Expression* b = nullptr;
            Expression* c = nullptr;
            acceptIt();
            accept(TokenType::LParen);
            if (CurrentToken.type != TokenType::Semicolon) {
                a = ParseExpression();
            }
            accept(TokenType::Semicolon);
            if (CurrentToken.type != TokenType::Semicolon) {
                b = ParseExpression();
            }
            accept(TokenType::Semicolon);
            if (CurrentToken.type != TokenType::Semicolon) {
                a = ParseExpression();
            }
            accept(TokenType::RParen);

            return new LoopStatement(location, a, b, c, ParseStatement());
        }
        case TokenType::While: {
            acceptIt();
            accept(TokenType::LParen);
            auto condition = ParseExpression();
            accept(TokenType::RParen);
            return new LoopStatement(location, nullptr, condition, nullptr, ParseStatement());
        }
        case TokenType::Do: {
            acceptIt();
            auto body = ParseStatement();
            accept(TokenType::While);
            accept(TokenType::LParen);
            auto condition = ParseExpression();
            accept(TokenType::RParen);

            return new DoLoopStatement(location, condition, body);
        }
        case TokenType::If: {
            acceptIt();
            accept(TokenType::LParen);
            auto condition = ParseExpression();
            accept(TokenType::RParen);
            auto ifS = ParseStatement();
            Statement* elseS = nullptr;
            if (CurrentToken.type == TokenType::Else) {
                acceptIt();
                elseS = ParseStatement();
            }

            return new IfStatement(location, condition, ifS, elseS);
        }
        case TokenType::LBrace: {
            acceptIt();
            std::vector<Statement*> body;
            while (CurrentToken.type != TokenType::RBrace) {
                body.push_back(ParseStatement());
            }
            acceptIt();
            return new Scope(location, body);
        }
        case TokenType::Const:
        case TokenType::Var:
        case TokenType::Let: {
            auto type = CurrentToken.type;
            acceptIt();
            auto name = accept(TokenType::ID);
            Expression* value = nullptr;
            if (CurrentToken.type == TokenType::Assign) {
                acceptIt();
                value = ParseExpression();
            }
            accept(TokenType::Semicolon);

            return new LetStatement(location, type, name, value);
        }
        case TokenType::Function: {
            acceptIt();
            auto name = accept(TokenType::ID);
            accept(TokenType::LParen);

            // TODO: params
            std::vector<Param> params;
            while (CurrentToken.type != TokenType::RParen) {
                auto name = accept(TokenType::ID);
                Expression* def = nullptr;
                if (CurrentToken.type == TokenType::Assign) {
                    acceptIt();
                    def = ParseExpression();
                }
                params.push_back(Param { name, def });
                if (CurrentToken.type != TokenType::Comma) {
                    break;
                }
                acceptIt();
            }
            accept(TokenType::RParen);
            auto body = ParseStatement();

            return new FunctionDecl(location, name, params, body);
        }
    }

    auto expr = ParseExpression();
    accept(TokenType::Semicolon);
    return expr;
}

Expression* Parser::ParseExpression() {
    auto location = CurrentToken.location;
    auto left = ParseSelect();

    switch (CurrentToken.type) {
        case TokenType::Assign:
        case TokenType::AddAssign:
        case TokenType::SubAssign:
        case TokenType::MulAssign:
        case TokenType::DivAssign:
        case TokenType::ModAssign:
        case TokenType::AndAssign:
        case TokenType::OrAssign:
        case TokenType::XorAssign:
        case TokenType::LshiftAssign:
        case TokenType::RshiftAssign:
            TokenType type = CurrentToken.type;
            acceptIt();
            return new Operation(location, type, left, ParseExpression());
    }
    return left;
}

Expression* Parser::ParseSelect() {
    auto location = CurrentToken.location;

    auto select = ParseLor();
    if (CurrentToken.type == TokenType::QMark) {
        acceptIt();
        auto trueVal = ParseSelect();
        accept(TokenType::Colon);
        auto falseVal = ParseSelect();

        return new SelectExpression(location, select, trueVal, falseVal);
    }
    return select;
}

Expression* Parser::ParseLor() {
    auto location = CurrentToken.location;
    auto left = ParseLand();
    if (CurrentToken.type == TokenType::LOr) {
        acceptIt();
        return new Operation(location, TokenType::LOr, left, ParseLor());
    }
    return left;
}

Expression* Parser::ParseLand() {
    auto location = CurrentToken.location;
    auto left = ParseNullCoal();
    if (CurrentToken.type == TokenType::LAnd) {
        acceptIt();
        return new Operation(location, TokenType::LAnd, left, ParseLand());
    }
    return left;
}

Expression* Parser::ParseNullCoal() {
    auto location = CurrentToken.location;
    auto left = ParseOr();
    if (CurrentToken.type == TokenType::NullCoal) {
        acceptIt();
        return new Operation(location, TokenType::NullCoal, left, ParseNullCoal());
    }
    return left;
}

Expression* Parser::ParseOr() {
    auto location = CurrentToken.location;
    auto left = ParseXor();
    if (CurrentToken.type == TokenType::Or) {
        acceptIt();
        return new Operation(location, TokenType::Or, left, ParseOr());
    }
    return left;
}

Expression* Parser::ParseXor() {
    auto location = CurrentToken.location;
    auto left = ParseAnd();
    if (CurrentToken.type == TokenType::Xor) {
        acceptIt();
        return new Operation(location, TokenType::Xor, left, ParseXor());
    }
    return left;
}

Expression* Parser::ParseAnd() {
    auto location = CurrentToken.location;
    auto left = ParseEq();
    if (CurrentToken.type == TokenType::And) {
        acceptIt();
        return new Operation(location, TokenType::And, left, ParseAnd());
    }
    return left;
}

Expression* Parser::ParseEq() {
    auto location = CurrentToken.location;
    auto left = ParseComp();
    if (CurrentToken.type == TokenType::Equals ||
        CurrentToken.type == TokenType::NEquals) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseEq());
    }
    return left;
}

Expression* Parser::ParseComp() {
    auto location = CurrentToken.location;
    auto left = ParseShift();
    if (CurrentToken.type == TokenType::Less ||
        CurrentToken.type == TokenType::LessEq ||
        CurrentToken.type == TokenType::Greater ||
        CurrentToken.type == TokenType::GreaterEq) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseComp());
    }
    return left;
}

Expression* Parser::ParseShift() {
    auto location = CurrentToken.location;
    auto left = ParseAdd();
    if (CurrentToken.type == TokenType::LShift ||
        CurrentToken.type == TokenType::RShift) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseShift());
    }
    return left;
}

Expression* Parser::ParseAdd() {
    auto location = CurrentToken.location;
    auto left = ParseMul();
    if (CurrentToken.type == TokenType::Add ||
        CurrentToken.type == TokenType::Sub) {
        auto t = CurrentToken.type;
        acceptIt();
        return new Operation(location, t, left, ParseAdd());
    }
    return left;
}

Expression* Parser::ParseMul() {
    auto location = CurrentToken.location;
    auto left = ParsePrefix();
    if (CurrentToken.type == TokenType::Mul ||
        CurrentToken.type == TokenType::Div ||
        CurrentToken.type == TokenType::Mod) {
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
        case TokenType::Inc:
        case TokenType::Dec:
        case TokenType::Add:
        case TokenType::Sub:
        case TokenType::LNot:
        case TokenType::Not:
            acceptIt();
            return new PreExpression(location, type, ParsePrefix());
        // case TokenType::LParen: {
        //     acceptIt();
        //     auto type = ParseType();
        //     accept(TokenType::RParen);

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
            case TokenType::Inc:
            case TokenType::Dec:
                acceptIt();
                current = new PostExpression(location, type, current);
                continue;
            case TokenType::LParen:
                acceptIt();
                std::vector<Expression*> params;
                while (CurrentToken.type != TokenType::RParen) {
                    params.push_back(ParseExpression());
                    if (CurrentToken.type == TokenType::Comma) {
                        acceptIt();
                    }
                }
                current = new FunctionCall(location, current, params);
                accept(TokenType::RParen);
                continue;
        }
        break;
    }

    return current;
}

Expression* Parser::ParseAtom() {
    auto location = CurrentToken.location;

    switch (CurrentToken.type) {
        case TokenType::IntLit: return new IntLit(location, std::stol(accept(TokenType::IntLit)));
        case TokenType::FloatLit: return new FloatLit(location, std::stod(accept(TokenType::FloatLit)));
        case TokenType::StringLit: return new StringLit(location, accept(TokenType::StringLit));
        case TokenType::BoolLit: return new BoolLit(location, accept(TokenType::BoolLit) == "true");
        case TokenType::ID: return new Variable(location, accept(TokenType::ID));
        case TokenType::LParen:
            acceptIt();
            auto ret = ParseExpression();
            accept(TokenType::RParen);
            return ret;
    }

	LogError(Error, CurrentToken.location, "Unexpected symbol \"" + CurrentToken.spelling + '"');
}