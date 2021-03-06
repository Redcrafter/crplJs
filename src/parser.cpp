#include <cassert>
#include <stdexcept>
#include <functional>

#include "logger.h"
#include "parser.h"
#include "converter.h"

static const char* TokenNames[] = {
    "<EOF>",

    "ID",
    "Int Literal",
    "Float Literal",
    "String Literal",
    "Bool Literal",

    "let",
    "var",
    "const",
    "return",
    "for",
    "do",
    "while",
    "of",
    "in",
    "if",
    "else",
    "switch",
    "case",
    "default",
    "function",

    "break",
    "continue",

    "`",
    "$",
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",

    "<",
    "<=",
    "==",
    "!=",
    ">=",
    ">",

    ",",
    ":",
    ";",

    "+",
    "-",
    "*",
    "**",
    "/",
    "%",
    ">>",
    "<<",

    "++",
    "--",

    "=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "&=",
    "|=",
    "^=",
    "<<=",
    ">>=",

    "~",
    "^",
    "&",
    "|",

    "!",
    "&&",
    "||",

    ".",
    "?",
    "?.",
    "??"
};

void ReturnStatement::visit(Converter& converter, bool discard) { converter.visit(*this); }
void LoopStatement::visit(Converter& converter, bool discard) { converter.visit(*this); }
void DoLoopStatement::visit(Converter& converter, bool discard) { converter.visit(*this); }
void ForOfStatement::visit(Converter& converter, bool discard) { converter.visit(*this); }
void IfStatement::visit(Converter& converter, bool discard) { converter.visit(*this); }
void Scope::visit(Converter& converter, bool discard) { converter.visit(*this); }
void LetStatement::visit(Converter& converter, bool discard) { converter.visit(*this); }
void FunctionDecl::visit(Converter& converter, bool discard) { converter.visit(*this); }

void SelectExpression::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void PreExpression::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void PostExpression::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void Operation::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void FunctionCall::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void IntLit::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void FloatLit::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void StringLit::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void BoolLit::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void ArrayLit::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }
void Variable::visit(Converter& converter, bool discard) { converter.visit(*this, discard); }

Parser::Parser(std::deque<Token> tokens): Tokens(std::move(tokens)), CurrentToken(Tokens.front()) {
	Tokens.pop_front();
}

std::string Parser::accept(Tokentype type) {
	Token t = CurrentToken;
    if(CurrentToken.type != type) {
		Logger::Log(Error, CurrentToken.location, "Unexpected symbol \"" + CurrentToken.spelling + "\" expected \"" + TokenNames[(int)type + 1] + '"');
    }
    acceptIt();
    return t.spelling;
}

void Parser::acceptIt() {
	if(!Tokens.empty()) {
		CurrentToken = Tokens.front();
		Tokens.pop_front();
	}
}

std::vector<Statement*> Parser::Parse() {
    std::vector<Statement*> statements;

    while (CurrentToken.type != Tokentype::Eof) {
		if(CurrentToken.type == Tokentype::Semicolon) {
			acceptIt();
			continue;;
		}
		auto statement = ParseStatement();
    	if(statement == nullptr) {
			break;
    	}
    	statements.push_back(statement);
    }
    
    return statements;
}

Statement* Parser::ParseStatement() {
    const SourceLocation location = CurrentToken.location;

    switch (CurrentToken.type) {
        case Tokentype::Return: {
            acceptIt();
            std::vector<Expression*> expressions;

            while (CurrentToken.type != Tokentype::Semicolon) {
                expressions.push_back(ParseExpression());
                if (CurrentToken.type == Tokentype::Comma) {
                    acceptIt();
                } else {
                    break;
                }
			}
            accept(Tokentype::Semicolon);
            return new ReturnStatement(location, expressions);
        }
        case Tokentype::For: {
            Statement* a = nullptr;
            Expression* b = nullptr;
            Expression* c = nullptr;
            acceptIt();
            accept(Tokentype::LParen);
            if (CurrentToken.type != Tokentype::Semicolon) {
                if(CurrentToken.type == Tokentype::Let || CurrentToken.type == Tokentype::Const || CurrentToken.type == Tokentype::Var) {
                    a = ParseStatement();
                    assert(dynamic_cast<LetStatement*>(a) != nullptr);

                    if(CurrentToken.type == Tokentype::Of) {
                        acceptIt();
                        Expression* iterable = ParseExpression();
						accept(Tokentype::RParen);

                        return new ForOfStatement(location, (LetStatement*)a, iterable, ParseStatement());
                    }
                } else {
                    a = ParseExpression();
                }
            }
            accept(Tokentype::Semicolon);
            if (CurrentToken.type != Tokentype::Semicolon) {
                b = ParseExpression();
            }
            accept(Tokentype::Semicolon);
            if (CurrentToken.type != Tokentype::Semicolon) {
                c = ParseExpression();
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
        case Tokentype::LBrace: return ParseScope();
        case Tokentype::Const:
        case Tokentype::Var:
        case Tokentype::Let: {
            auto type = CurrentToken.type;
            acceptIt();

            std::vector<LetItem> elements;
            while(true) {
                std::vector<std::string> names;
                auto location = CurrentToken.location;

                if(CurrentToken.type == Tokentype::LBracket) {
                    acceptIt();
                    while(true) {
                        names.push_back(accept(Tokentype::ID));
                        if(CurrentToken.type == Tokentype::Comma) {
                            acceptIt();
                        } else {
                            break;
                        }
                    }
                    accept(Tokentype::RBracket);
                } else {
                    names.push_back(accept(Tokentype::ID));
                }

                Expression* value = nullptr;
                if (CurrentToken.type == Tokentype::Assign) {
                    acceptIt();
                    value = ParseExpression();
                }

                elements.emplace_back(location, names, value);

                if(CurrentToken.type == Tokentype::Comma) {
                    acceptIt();
                } else {
                    break;
                }
            }

            return new LetStatement(location, type, elements);
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
            auto body = ParseScope();

            return new FunctionDecl(location, name, params, body);
        }
    }

    auto expr = ParseExpression();
    return expr;
}

Scope* Parser::ParseScope() {
    const SourceLocation location = CurrentToken.location;

    accept(Tokentype::LBrace);
    std::vector<Statement*> body;
    while (CurrentToken.type != Tokentype::RBrace && CurrentToken.type != Tokentype::Eof) {
		if(CurrentToken.type == Tokentype::Semicolon) {
			acceptIt();
			continue;;
		}
    	auto statement = ParseStatement();
		if(statement == nullptr) {
			break;
		}
		body.push_back(statement);
    }
	accept(Tokentype::RBrace);
    return new Scope(location, body);
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
    auto current = ParseLand();
    while (CurrentToken.type == Tokentype::LOr) {
        acceptIt();
        current = new Operation(location, Tokentype::LOr, current, ParseLand());
    }
    return current;
}

Expression* Parser::ParseLand() {
    auto location = CurrentToken.location;
    auto current = ParseNullCoal();
    while (CurrentToken.type == Tokentype::LAnd) {
        acceptIt();
        current = new Operation(location, Tokentype::LAnd, current, ParseNullCoal());
    }
    return current;
}

Expression* Parser::ParseNullCoal() {
    auto location = CurrentToken.location;
    auto current = ParseOr();
    while (CurrentToken.type == Tokentype::NullCoal) {
        acceptIt();
        current = new Operation(location, Tokentype::NullCoal, current, ParseOr());
    }
    return current;
}

Expression* Parser::ParseOr() {
    auto location = CurrentToken.location;
    auto current = ParseXor();
    while (CurrentToken.type == Tokentype::Or) {
        acceptIt();
        return new Operation(location, Tokentype::Or, current, ParseXor());
    }
    return current;
}

Expression* Parser::ParseXor() {
    auto location = CurrentToken.location;
    auto current = ParseAnd();
    while (CurrentToken.type == Tokentype::Xor) {
        acceptIt();
        current = new Operation(location, Tokentype::Xor, current, ParseAnd());
    }
    return current;
}

Expression* Parser::ParseAnd() {
    auto location = CurrentToken.location;
    auto current = ParseEq();
    while (CurrentToken.type == Tokentype::And) {
        acceptIt();
        current = new Operation(location, Tokentype::And, current, ParseEq());
    }
    return current;
}

Expression* Parser::ParseEq() {
    auto location = CurrentToken.location;
    auto current = ParseComp();
    while (CurrentToken.type == Tokentype::Equals ||
        CurrentToken.type == Tokentype::NEquals) {
        auto t = CurrentToken.type;
        acceptIt();
        current = new Operation(location, t, current, ParseComp());
    }
    return current;
}

Expression* Parser::ParseComp() {
    auto location = CurrentToken.location;
    auto current = ParseShift();
    while (CurrentToken.type == Tokentype::Less ||
        CurrentToken.type == Tokentype::LessEq ||
        CurrentToken.type == Tokentype::Greater ||
        CurrentToken.type == Tokentype::GreaterEq) {
        auto t = CurrentToken.type;
        acceptIt();
        current = new Operation(location, t, current, ParseShift());
    }
    return current;
}

Expression* Parser::ParseShift() {
    auto location = CurrentToken.location;
    auto current = ParseAdd();
    while (CurrentToken.type == Tokentype::LShift ||
        CurrentToken.type == Tokentype::RShift) {
        auto t = CurrentToken.type;
        acceptIt();
        current = new Operation(location, t, current, ParseAdd());
    }
    return current;
}

Expression* Parser::ParseAdd() {
    auto location = CurrentToken.location;
    auto current = ParseMul();
    while (CurrentToken.type == Tokentype::Add ||
        CurrentToken.type == Tokentype::Sub) {
        auto t = CurrentToken.type;
        acceptIt();

        current = new Operation(location, t, current, ParseMul());
    }
    return current;
}

Expression* Parser::ParseMul() {
    auto location = CurrentToken.location;
    auto current = ParsePrefix();
    while (CurrentToken.type == Tokentype::Mul ||
        CurrentToken.type == Tokentype::Div ||
        CurrentToken.type == Tokentype::Mod) {
        auto t = CurrentToken.type;
        acceptIt();

        current = new Operation(location, t, current, ParsePrefix());
    }
    return current;
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
            case Tokentype::LParen: {
                acceptIt();
                std::vector<Expression*> params;
                if(CurrentToken.type != Tokentype::RParen) {
                    while (true) {
                        params.push_back(ParseExpression());
                        if (CurrentToken.type != Tokentype::Comma) {
                            break;
                        }
                        acceptIt();
                    }
                }
                current = new FunctionCall(location, current, params);
                accept(Tokentype::RParen);
                continue;
            }
            case Tokentype::LBracket: {
                acceptIt();
                auto select = ParseExpression();
                accept(Tokentype::RBracket);
                return new Operation(location, Tokentype::LBracket, current, select);
            }
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
        case Tokentype::LParen: {
            acceptIt();
            auto ret = ParseExpression();
            accept(Tokentype::RParen);
            return ret;
        }
        case Tokentype::LBracket: {
            acceptIt();
            std::vector<Expression*> elements;
            if(CurrentToken.type != Tokentype::RBracket) {
                while (true) {
                    elements.push_back(ParseExpression());
                    if(CurrentToken.type == Tokentype::Comma) {
                        acceptIt();
                    } else {
                        break;
                    }
                }
            }
            accept(Tokentype::RBracket);
            return new ArrayLit(location, elements);
        }
        case Tokentype::Backtick: {
            acceptIt();

			Expression* ret = nullptr;
    	
            while (true) {
				Expression* ptr;
            	if(CurrentToken.type == Tokentype::StringLit) {
					ptr = new StringLit(CurrentToken.location, accept(Tokentype::StringLit));
            	} else if(CurrentToken.type == Tokentype::Dollar) {
					acceptIt();
					ptr = ParseExpression();
            	} else {
                    break;
                }

				if(ret == nullptr) {
					ret = ptr;
				} else {
					ret = new FunctionCall(CurrentToken.location, new Variable(CurrentToken.location, "Concat"), std::vector<Expression*>({ ret, ptr }));
				}
            }
            accept(Tokentype::Backtick);
    		// TODO: maybe add custom class?
			return ret;
        }
    }

	Logger::Log(Error, CurrentToken.location, "Unexpected symbol \"" + CurrentToken.spelling + '"');
	return nullptr;
}