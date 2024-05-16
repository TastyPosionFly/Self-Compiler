#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <string>
#include <vector>
#include "DataBase.h"

using namespace std;

struct Token{
    TokenType tokenType;
    string value;

    Token(TokenType t, const string& v): tokenType(t), value(v){}
    string toString(){
        string type;
        switch(tokenType) {
            case TokenType::IDENTIFIER:
                type = "IDENTIFIER";
                break;
            case TokenType::NUMBER:
                type = "NUMBER";
                break;
            case TokenType::OPERATOR:
                type = "OPERATOR";
                break;
            case TokenType::SEMICOLON:
                type = "SEMICOLON";
                break;
            case TokenType::LEFT_PAREN:
                type = "LEFT_PAREN";
                break;
            case TokenType::RIGHT_PAREN:
                type = "RIGHT_PAREN";
                break;
            case TokenType::LEFT_BRACE:
                type = "LEFT_BRACE";
                break;
            case TokenType::RIGHT_BRACE:
                type = "RIGHT_BRACE";
                break;
            case TokenType::KEYWORD:
                type = "KEYWORD";
                break;
            case TokenType::STRING:
                type = "STRING";
                break;
            case TokenType::ASSIGNMENT:
                type = "ASSIGNMENT";
                break;
            case TokenType::UNKNOWN:
                type = "UNKNOWN";
                break;
            default:
                type = "UNKNOWN";
                break;
        }
        return "Token Type: " + type + ", value: " + value;
    }
};

class Lexer {
public:
    explicit Lexer(const string& input): input(input){
        DataBase dataBase;
        operatorSet = dataBase.getOperatorSet();
        keywordSet = dataBase.getKeywordSet();
        preprocessSet = dataBase.getPreprocessSet();
    };
    vector<Token> tokenSize();
    static string tokenTypeToString(TokenType type);

private:
    string preprocess(const string& input);
    vector<Token> getStatementTokens(const string& statement);
    Token getToken(const string& tokenValue);
    bool isIdentifier(const string& str);
    bool isNumber(const string& str);
    bool isOperator(const string& str);
    bool isOperator_char(const char& c);
    bool isSemicolon(const string& str);
    bool isLeftParen(const string& str);
    bool isRightParen(const string& str);
    bool isKeyword(const string& str);
    bool isString(const string& str);
    bool isAssignment(const string& str);
    bool isLeftBrace(const string& str);
    bool isRightBrace(const string& str);


    string input;
    unordered_set<string> operatorSet;
    unordered_set<string> keywordSet;
    unordered_set<string> preprocessSet;
};


#endif //COMPILER_LEXER_H
