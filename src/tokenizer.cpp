#include "logger.h"
#include "tokenizer.h"

#include <map>
#include <stdexcept>
#include <fstream>

/*struct test {
    const char* str;
    TokenType type;
};*/

static bool isLetter(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool isDigit(int c) {
    return c >= '0' && c <= '9';
}

std::map<std::string, TokenType> keywords = {
    {"true", TokenType::BoolLit},
	{"false", TokenType::BoolLit},
    {"let", TokenType::Let},
    {"var", TokenType::Var},
    {"const", TokenType::Const},
    {"return", TokenType::Return},
    {"for", TokenType::Return},
    {"do", TokenType::Do},
    {"while", TokenType::While},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"default", TokenType::Default},
    {"function", TokenType::Function},
    {"break", TokenType::Break},
    {"continue", TokenType::Continue},
};

// TODO: use tree matching
/* test all[] = {
    {"(", TokenType::LParen},
    {")", TokenType::LParen},
    {"{", TokenType::LParen},
    {"}", TokenType::LParen},

    {"<", TokenType::LParen},
    {"<=", TokenType::LParen},
    {"==", TokenType::LParen},
    {"!=", TokenType::LParen},
    {">=", TokenType::LParen},
    {">", TokenType::LParen},

    {",", TokenType::LParen},
    {":", TokenType::LParen},
    {";", TokenType::LParen},

    {"+", TokenType::LParen},
    {"-", TokenType::LParen},
    {"*", TokenType::LParen},
    {"/", TokenType::LParen},
    {"%", TokenType::LParen},
    {">>", TokenType::LParen},
    {"<<", TokenType::LParen},

    {"++", TokenType::LParen},
    {"--", TokenType::LParen},

    {"=", TokenType::LParen},
    {"+=", TokenType::LParen},
    {"-=", TokenType::LParen},
    {"*=", TokenType::LParen},
    {"/=", TokenType::LParen},
    {"%=", TokenType::LParen},
    {"&=", TokenType::LParen},
    {"|=", TokenType::LParen},
    {"^=", TokenType::LParen},
    {"<<=", TokenType::LParen},
    {">>=", TokenType::LParen},

    {"~", TokenType::LParen},
    {"^", TokenType::LParen},
    {"&", TokenType::LParen},
    {"|", TokenType::LParen},

    {"!", TokenType::LParen},
    {"&&", TokenType::LParen},
    {"||", TokenType::LParen},

    {"?", TokenType::LParen},
    {"??", TokenType::LParen},

    {"//", TokenType::Comment},
    {"/*", TokenType::MultiComment}
};*/

std::vector<Token> tokenize(const std::string& fileName) {
    SourceLocation loc{fileName,  1, 1};
	std::ifstream file(fileName);

	std::vector<Token> tokens;
    std::string spelling;
    auto currentChar = file.get();
	
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

    while (currentChar != EOF) {
        SourceLocation start = loc;
        spelling = "";
        TokenType type;

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

            if (type == TokenType::Comment) {
                continue;
            } else if (type == TokenType::MultiComment) {
                continue;
            }
        */

        switch (currentChar) {
            case ' ':
            case '\t':
            case '\r':
            case '\n': skip(); continue;
            case '(': type = TokenType::LParen; take(); break;
            case ')': type = TokenType::RParen; take(); break;
            case '{': type = TokenType::LBrace; take(); break;
            case '}': type = TokenType::RBrace; take(); break;
            case ',': type = TokenType::Comma; take(); break;
            case ':': type = TokenType::Colon; take(); break;
            case ';': type = TokenType::Semicolon; take(); break;
            case '~': type = TokenType::Not; take(); break;
            case '.': 
                take();
                if(isDigit(currentChar)) {
                    do {
                        take();
                    } while (isDigit(currentChar));

                    type = TokenType::FloatLit;
                } else {
                    type = TokenType::Dot; take(); 
                }
                break;
            case '+': 
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::AddAssign;
                } else if(currentChar == '+') {
                    take();
                    type = TokenType::Inc;
                } else {
                    type = TokenType::Add; 
                }
                break;
            case '-': 
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::SubAssign;
                } else if(currentChar == '+') {
                    take();
                    type = TokenType::Dec;
                } else {
                    type = TokenType::Sub; 
                }
                break;
            case '*': 
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::MulAssign;
                } else if(currentChar == '*') {
                    take();
                    type = TokenType::Pow;
                } else {
                    type = TokenType::Mul; 
                }
                break;
            case '%': 
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::ModAssign;
                } else {
                    type = TokenType::Mod; 
                }
                break;
            case '?':
                take();
                if(currentChar == '?') {
                    take();
                    type = TokenType::NullCoal;
                } else if(currentChar == '.') {
                    take();
                    type = TokenType::OptChain;
                } else {
                    type = TokenType::QMark;
                }
                break;
            
            case '!':
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::NEquals;
                } else {
                    type = TokenType::LNot;
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
                    type = TokenType::DivAssign;
                } else {
                    type = TokenType::Div;
                }
                break;
            case '<':
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::LessEq;
                } else if(currentChar == '<') {
                    take();

                    if(currentChar == '=') {
                        take();
                        type = TokenType::LshiftAssign;
                    } else {
                        type = TokenType::LShift;
                    }
                } else {
                    type = TokenType::Less;
                }
                break;
            case '>':
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::GreaterEq;
                } else if(currentChar == '>') {
                    take();

                    if(currentChar == '=') {
                        take();
                        type = TokenType::RshiftAssign;
                    } else {
                        type = TokenType::RShift;
                    }
                } else {
                    type = TokenType::Greater;
                }
                break;
            case '=':
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::Equals;
                } else {
                    type = TokenType::Assign;
                }
                break;
            case '&':
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::AndAssign;
                } else if(currentChar == '&') {
                    take();
                    type = TokenType::LAnd;
                } else {
                    type = TokenType::And;
                }
                break;
            case '|':
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::OrAssign;
                } else if(currentChar == '|') {
                    take();
                    type = TokenType::LOr;
                } else {
                    type = TokenType::Or;
                }
                break;
            case '^':
                take();
                if(currentChar == '=') {
                    take();
                    type = TokenType::XorAssign;
                } else {
                    type = TokenType::Xor;
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
                        switch (currentChar) {
                            case '"': take(); break;
                            case 'n': spelling += '\n'; skip(); break;
                            case 'r': spelling += '\r'; skip(); break;
                            case 't': spelling += '\t'; skip(); break;
							default: LogError(Error, loc, std::string("Unknown escape char \"") + static_cast<char>(currentChar) + "\"");
                        }
                    } else {
                        take();
                    }
                }
                type = TokenType::StringLit;
                break;
            default:
                if(isLetter(currentChar)) {
                    do {
                        take();
                    } while (isLetter(currentChar) || isDigit(currentChar) || currentChar == '_');

                    if(keywords.count(spelling)) {
                        type = keywords[spelling];
                    } else {
                        type = TokenType::ID;
                    }
                } else if(isDigit(currentChar)) {
                    do {
                        take();
                    } while (isDigit(currentChar));

                    if(currentChar == '.') {
                        do {
                            take();
                        } while (isDigit(currentChar));

                        type = TokenType::FloatLit;
                    } else {
                        type = TokenType::IntLit;
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