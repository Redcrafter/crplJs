#include "logger.h"
#include "tokenizer.h"

#include <map>
#include <stdexcept>
#include <fstream>

/*struct test {
    const char* str;
    Tokentype type;
};*/

static bool isLetter(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool isDigit(int c) {
    return c >= '0' && c <= '9';
}

std::map<std::string, Tokentype> keywords = {
    {"true", Tokentype::BoolLit},
	{"false", Tokentype::BoolLit},
    {"let", Tokentype::Let},
    {"var", Tokentype::Var},
    {"const", Tokentype::Const},
    {"return", Tokentype::Return},
    {"for", Tokentype::Return},
    {"do", Tokentype::Do},
    {"while", Tokentype::While},
    {"if", Tokentype::If},
    {"else", Tokentype::Else},
    {"switch", Tokentype::Switch},
    {"case", Tokentype::Case},
    {"default", Tokentype::Default},
    {"function", Tokentype::Function},
    {"break", Tokentype::Break},
    {"continue", Tokentype::Continue},
};

// TODO: use tree matching
/* test all[] = {
    {"(", Tokentype::LParen},
    {")", Tokentype::LParen},
    {"{", Tokentype::LParen},
    {"}", Tokentype::LParen},

    {"<", Tokentype::LParen},
    {"<=", Tokentype::LParen},
    {"==", Tokentype::LParen},
    {"!=", Tokentype::LParen},
    {">=", Tokentype::LParen},
    {">", Tokentype::LParen},

    {",", Tokentype::LParen},
    {":", Tokentype::LParen},
    {";", Tokentype::LParen},

    {"+", Tokentype::LParen},
    {"-", Tokentype::LParen},
    {"*", Tokentype::LParen},
    {"/", Tokentype::LParen},
    {"%", Tokentype::LParen},
    {">>", Tokentype::LParen},
    {"<<", Tokentype::LParen},

    {"++", Tokentype::LParen},
    {"--", Tokentype::LParen},

    {"=", Tokentype::LParen},
    {"+=", Tokentype::LParen},
    {"-=", Tokentype::LParen},
    {"*=", Tokentype::LParen},
    {"/=", Tokentype::LParen},
    {"%=", Tokentype::LParen},
    {"&=", Tokentype::LParen},
    {"|=", Tokentype::LParen},
    {"^=", Tokentype::LParen},
    {"<<=", Tokentype::LParen},
    {">>=", Tokentype::LParen},

    {"~", Tokentype::LParen},
    {"^", Tokentype::LParen},
    {"&", Tokentype::LParen},
    {"|", Tokentype::LParen},

    {"!", Tokentype::LParen},
    {"&&", Tokentype::LParen},
    {"||", Tokentype::LParen},

    {"?", Tokentype::LParen},
    {"??", Tokentype::LParen},

    {"//", Tokentype::Comment},
    {"/*", Tokentype::MultiComment}
};*/

static char escapeChar(SourceLocation loc, char c) {
    switch (c) {
        case '\'': return '\'';
        case '"': return '"';
        case '?': return '?';
        case '\\': return '\\';
        case 'a': return '\a';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
        default: LogError(Error, loc, std::string("Unknown escape char \"") + static_cast<char>(c) + "\"");
    }
}

std::vector<Token> tokenize(const std::string& fileName) {
    SourceLocation loc{fileName,  1, 1};
	std::ifstream file(fileName, std::ios::in | std::ios::binary);

	std::vector<Token> tokens;
    std::string spelling;
    auto currentChar = file.get();
	int isInTemp = 0;
	
    auto skip = [&] {
        if (currentChar == '\n') {
            loc.line++;
            loc.column = 1;
        } else {
            loc.column++;
        }

        currentChar = file.get();
    };

    auto take = [&] {
        spelling += static_cast<char>(currentChar);
        skip();
    };

	auto jesus = [&] {
		auto start = loc;
		
		while(true) {
			if(currentChar == '$') {
				skip();
				if(currentChar == '{') {
					skip();

					if(!spelling.empty()) {
						tokens.push_back(Token{ Tokentype::StringLit, spelling, start });
					}
					tokens.push_back(Token{ Tokentype::Dollar, "$", start });

					isInTemp++;
					break;
				} else {
					spelling += '$';
				}
			} else if(currentChar == '\\') {
				skip();
				spelling += escapeChar(loc, currentChar);
				skip();
			} else if(currentChar == '`') {
				if(!spelling.empty()) {
					tokens.push_back(Token{ Tokentype::StringLit, spelling, start });
				}
				tokens.push_back(Token{ Tokentype::Backtick, "`", loc });
				skip();
				break;
			} else {
				take();
			}
		}
	};
	
    while (currentChar != EOF) {
        SourceLocation start = loc;
        spelling = "";
        Tokentype type;

        /*
            std::list<test> possible(all, all + sizeof(all) / sizeof(test));
            int pos = 0;

            auto asdf = [=](const test& val) { 
                return val.str[pos] != currentChar; 
            };

            while (true) {
                possible.remove_if(asdf);
                pos++;

                if (possible.size() == 1) {
                    type = possible.front().type;
                    break;
                } else if (possible.size() == 0) {
                    throw std::runtime_error("Unexpected token " + spelling);
                } else {
                    take();
                }
            }

            if (type == Tokentype::Comment) {
                continue;
            } else if (type == Tokentype::MultiComment) {
                continue;
            }
        */

        switch (currentChar) {
            case ' ':
            case '\t':
            case '\r':
            case '\n': skip(); continue;
            case '(': type = Tokentype::LParen; take(); break;
            case ')': type = Tokentype::RParen; take(); break;
            case '{': type = Tokentype::LBrace; take(); break;
            case '}':
        		if(isInTemp) {
					skip();
					isInTemp--;
					jesus();
        			continue;
        		} else {
					take();
					type = Tokentype::RBrace; take();
        		}
				break;
            case ',': type = Tokentype::Comma; take(); break;
            case ':': type = Tokentype::Colon; take(); break;
            case ';': type = Tokentype::Semicolon; take(); break;
            case '~': type = Tokentype::Not; take(); break;
            case '[': type = Tokentype::LBracket; take(); break;
            case ']': type = Tokentype::RBracket; take(); break;
            case '`':
        		skip();
				tokens.push_back(Token{ Tokentype::Backtick, "`", start });
				jesus();
                continue;
            case '.':
                take();
                if(isDigit(currentChar)) {
                    do {
                        take();
                    } while (isDigit(currentChar));

                    type = Tokentype::FloatLit;
                } else {
                    type = Tokentype::Dot; take(); 
                }
                break;
            case '+': 
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::AddAssign;
                } else if(currentChar == '+') {
                    take();
                    type = Tokentype::Inc;
                } else {
                    type = Tokentype::Add; 
                }
                break;
            case '-': 
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::SubAssign;
                } else if(currentChar == '+') {
                    take();
                    type = Tokentype::Dec;
                } else {
                    type = Tokentype::Sub; 
                }
                break;
            case '*': 
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::MulAssign;
                } else if(currentChar == '*') {
                    take();
                    type = Tokentype::Pow;
                } else {
                    type = Tokentype::Mul; 
                }
                break;
            case '%': 
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::ModAssign;
                } else {
                    type = Tokentype::Mod; 
                }
                break;
            case '?':
                take();
                if(currentChar == '?') {
                    take();
                    type = Tokentype::NullCoal;
                } else if(currentChar == '.') {
                    take();
                    type = Tokentype::OptChain;
                } else {
                    type = Tokentype::QMark;
                }
                break;
            
            case '!':
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::NEquals;
                } else {
                    type = Tokentype::LNot;
                }
                break;
            case '/':
                take();
                if(currentChar == '/') { // Line comment
                    do {
                        skip();
                    } while (currentChar != '\n');
                    skip();
                    continue;
                } else if(currentChar == '*') { // Multi line comment
                    skip();
                    while (true) {
                        if(currentChar == '*') {
                            skip();
                            if(currentChar == '/') {
                                skip();
                                break;
                            }
                        } else {
                            skip();
                        }
                    }
                    continue;
                } else if(currentChar == '=') {
                    take();
                    type = Tokentype::DivAssign;
                } else {
                    type = Tokentype::Div;
                }
                break;
            case '<':
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::LessEq;
                } else if(currentChar == '<') {
                    take();

                    if(currentChar == '=') {
                        take();
                        type = Tokentype::LshiftAssign;
                    } else {
                        type = Tokentype::LShift;
                    }
                } else {
                    type = Tokentype::Less;
                }
                break;
            case '>':
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::GreaterEq;
                } else if(currentChar == '>') {
                    take();

                    if(currentChar == '=') {
                        take();
                        type = Tokentype::RshiftAssign;
                    } else {
                        type = Tokentype::RShift;
                    }
                } else {
                    type = Tokentype::Greater;
                }
                break;
            case '=':
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::Equals;
                } else {
                    type = Tokentype::Assign;
                }
                break;
            case '&':
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::AndAssign;
                } else if(currentChar == '&') {
                    take();
                    type = Tokentype::LAnd;
                } else {
                    type = Tokentype::And;
                }
                break;
            case '|':
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::OrAssign;
                } else if(currentChar == '|') {
                    take();
                    type = Tokentype::LOr;
                } else {
                    type = Tokentype::Or;
                }
                break;
            case '^':
                take();
                if(currentChar == '=') {
                    take();
                    type = Tokentype::XorAssign;
                } else {
                    type = Tokentype::Xor;
                }
                break;
            case '"':
                skip();
                while (true) {
                    if(currentChar == '"') {
                        skip();
                        break;
                    } else if(currentChar == '\\') {
                        skip();
                        spelling += escapeChar(loc, currentChar);
                        skip();
                    } else {
                        take();
                    }
                }
                type = Tokentype::StringLit;
                break;
            default:
                if(isLetter(currentChar)) {
                    do {
                        take();
                    } while (isLetter(currentChar) || isDigit(currentChar) || currentChar == '_');

                    if(keywords.count(spelling)) {
                        type = keywords[spelling];
                    } else {
                        type = Tokentype::ID;
                    }
                } else if(isDigit(currentChar)) {
                    do {
                        take();
                    } while (isDigit(currentChar));

                    if(currentChar == '.') {
                        do {
                            take();
                        } while (isDigit(currentChar));

                        type = Tokentype::FloatLit;
                    } else {
                        type = Tokentype::IntLit;
                    }
                } else {
                	LogError(Error, loc, "Unexpected character \"" + std::string(1, currentChar) + "\" at (" + std::to_string(loc.line) + "," + std::to_string(loc.column) + ")");
                }
                break;
        }

        tokens.push_back(Token{type, spelling, start});
    }

    return tokens;
}