#pragma once
#include <string>
#include <vector>

// Name should be TokenType but that is defined somewhere in Windows.h
enum class Tokentype {
    Eof = -1,

    ID,

    IntLit,     // [0-9]+
    FloatLit,   // [0-9]+.[0-9]+
    StringLit,  // "\w*"

    BoolLit,   // true|false
    Let,       // let
    Var,       // var
    Const,     // const
    Return,    // return
    For,       // for
    Do,        // do
    While,     // while
    If,        // if
    Else,      // else
    Switch,    // switch
    Case,      // case
    Default,   // default
    Function,  // function

    Break,     // break
    Continue,  // continue

    LParen,  // (
    RParen,  // )
    LBrace,  // {
    RBrace,  // }

    Less,       // <
    LessEq,     // <=
    Equals,     // ==
    NEquals,    // !=
    GreaterEq,  // >=
    Greater,    // >

    Comma,      // ,
    Colon,      // :
    Semicolon,  // ;

    Add,     // +
    Sub,     // -
    Mul,     // *
    Pow,     // **
    Div,     // /
    Mod,     // %
    RShift,  // >>
    LShift,  // <<

    Inc,  // ++
    Dec,  // --

    Assign,     // =
    AddAssign,  // +=
    SubAssign,  // -=
    MulAssign,  // *=
    DivAssign,  // /=
    ModAssign,  // %=
    AndAssign,  // &=
    OrAssign,   // |=
    XorAssign,  // ^=
    LshiftAssign, // <<=
    RshiftAssign, // >>=

    Not,  // ~
    Xor,  // ^
    And,  // &
    Or,   // |

    LNot,  // !
    LAnd,  // &&
    LOr,   // ||

    Dot,       // .
    QMark,     // ?
    OptChain,  // ?.
    NullCoal,  // ??
};

struct SourceLocation {
	std::string fileName = "";
    int line;
    int column;
};

struct Token {
    Tokentype type;
    std::string spelling;
    SourceLocation location;
};

std::vector<Token> tokenize(const std::string& fileName);