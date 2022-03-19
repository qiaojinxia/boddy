//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cctype>
#include "Lexer.h"
#include "Diag.h"
#include <string>

using namespace BDD;
void BDD::Lexer::GetNextChar() {
    if (Cursor == SourceCode.size()){
        CurChar = '\0';
        return;
    }
    CurChar = SourceCode[Cursor++];
}

void BDD::Lexer::GetNextToken() {
    while(isspace(CurChar)){
        if (CurChar == '\n'){
            Line ++;
            LineHead = Cursor;
        }
        GetNextChar();
    }
    SourceLocation Location;
    TokenKind kind;
    Location.Line = Line;
    Location.Col = Cursor - 1 -LineHead;
    int value = 0;
    int startPos = Cursor -1;
    if (CurChar == '\0'){
        kind = TokenKind::Eof;
    }else if(CurChar == '+'){
        kind = TokenKind::Add;
        GetNextChar();
    }else if(CurChar == '-'){
        kind = TokenKind::Sub;
        GetNextChar();
    }else if (CurChar == ','){
        kind = TokenKind::Comma;
        GetNextChar();
    }else if(CurChar == '*'){
        kind = TokenKind::Mul;
        GetNextChar();
    }else if(CurChar == '/'){
        kind = TokenKind::Div;
        GetNextChar();
    }else if(CurChar == '%'){
        kind = TokenKind::Mod;
        GetNextChar();
    }else if(CurChar == '('){
        kind = TokenKind::LParent;
        GetNextChar();
    }else if(CurChar == ')'){
        kind = TokenKind::RParent;
        GetNextChar();
    }else if(CurChar == '{'){
        kind = TokenKind::LBrace;
        GetNextChar();
    }else if(CurChar == '}'){
        kind = TokenKind::RBrace;
        GetNextChar();
    }else if(CurChar == ';'){
        kind = TokenKind::Semicolon;
        GetNextChar();
    }else if(CurChar == '='){
        if (PeekChar(1)=='='){
            GetNextChar();
            kind = TokenKind::Equal;
        }else{
            kind = TokenKind::Assign;
        }
        GetNextChar();
    }else if (CurChar == '!'){
        if (PeekChar(1) == '='){
            GetNextChar();
            kind = TokenKind::NotEqual;
        }else{
            DiagE(SourceCode,CurrentToken->Location.Line,CurrentToken->Location.Col+1,"token '%c' is illegal",CurChar);
        }
        GetNextChar();
    }else if(isdigit(CurChar)){
        kind = TokenKind::Num;
        value = 0;
        do {
            value = value * 10 + CurChar - '0';
            GetNextChar();
        }while (isdigit(CurChar));
    }else if (CurChar == '>'){
        if (PeekChar(1) == '='){
            GetNextChar();
            kind = TokenKind::GreaterEqual;
        }else{
            kind = TokenKind::Greater;
        }
        GetNextChar();
    }else if (CurChar == '<'){
        if (PeekChar(1) == '='){
            GetNextChar();
            kind = TokenKind::LesserEqual;
        }else{
            kind = TokenKind::Lesser;
        }
        GetNextChar();
    }else{
        if (IsLetter()){
            GetNextChar();
            while(IsLetterOrDigit()){
                GetNextChar();
            }
            kind = TokenKind::Identifier;
            std::string_view content = SourceCode.substr(startPos,Cursor - 1 - startPos);
            if (content  == "if"){
                kind = TokenKind::If;
            }else if (content == "else"){
                kind = TokenKind::Else;
            }else if (content == "while"){
                kind = TokenKind::While;
            }else if (content == "do"){
                kind = TokenKind::Do;
            }else if (content == "for"){
                kind = TokenKind::For;
            }else if (content == "func"){
                kind = TokenKind::FunctionDefine;
            }else if (content == "return"){
                kind = TokenKind::Return;
            }
        }else{
            DiagE(SourceCode,CurrentToken->Location.Line,CurrentToken->Location.Col+1,"token '%c' is illegal",CurChar);
        }
    }
    CurrentToken = std::make_shared<Token>();
    CurrentToken->Kind = kind;
    CurrentToken->Value = value;
    CurrentToken -> Location = Location;
    CurrentToken->Content = SourceCode.substr(startPos,Cursor - 1 -startPos);
}

bool BDD::Lexer::IsLetter() {
    return (CurChar >= 'a' && CurChar <= 'z') || (CurChar >= 'A' && CurChar <= 'Z') || CurChar == '_';
}

bool BDD::Lexer::IsDigit() {
    return CurChar >= '0' && CurChar <= '9';
}

bool BDD::Lexer::IsLetterOrDigit() {
    return IsLetter() || IsDigit();
}

void Lexer::ExceptToken(TokenKind kind) {
    if (CurrentToken->Kind == kind){
        GetNextToken();
    }else{
        DiagE(SourceCode,Line,CurrentToken->Location.Col,"'%s' excepted",GetTokenName(kind));
    }
}

const char *Lexer::GetTokenName(TokenKind kind) {
    switch (kind) {
        case TokenKind::Add:
            return "+";
        case TokenKind::Sub:
            return "-";
        case TokenKind::Mul:
            return "*";
        case TokenKind::Div:
            return "/";
        case TokenKind::Semicolon:
            return ";";
        case TokenKind::LParent:
            return "(";
        case TokenKind::RParent:
            return ")";
        case TokenKind::Assign:
            return "=";

        case TokenKind::FunctionDefine:
            return "func ";
        case TokenKind::Eof:
            return "eof ";
        default:
            break;
    }
    return "";
}

char Lexer::PeekChar(int n) {
    assert(n >0);
    if (Cursor - 1 + n < SourceCode.size()){
        return SourceCode[Cursor - 1 + n];
    }
    return '\0';
}

void Lexer::EndPeekToken() {
    CurChar = PeekPointCurChar;
    Cursor = PeekPointCursor;
    Line = PeekPointLine;
    LineHead = PeekPointLineHead;
    CurrentToken = PeekPointCurrentToken;
}

void Lexer::BeginPeekToken() {
    PeekPointCurChar = CurChar;
    PeekPointCursor = Cursor;
    PeekPointLine = Line;
    PeekPointLineHead = LineHead;
    PeekPointCurrentToken = CurrentToken;
}




