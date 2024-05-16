#include <sstream>
#include "Lexer.h"



string Lexer::preprocess(const string &input) {
    string result;
    char prev = '\0'; // Previous character

    for (size_t i = 0; i < input.size() ; ++i) {
        char curr = input[i];
        string token;
        token.push_back(curr);

        if (i+1 <= input.size() && preprocessSet.count(token + input[i+1]) > 0){
            if (!isspace(prev) && prev != '\0'){
                result.push_back(' ');
            }
            result += token + input[i+1];
            if (!isspace(input[i+2]) && input[i+2] != '\0'){
                result.push_back(' ');
            }
            prev = input[i+2];
            ++i;
        } else if (preprocessSet.count(token) > 0){
            if (!isspace(prev) && prev != '\0'){
                result.push_back(' ');
            }
            result += token;
            if (!isspace(input[i+1]) && input[i+1] != '\0'){
                result.push_back(' ');
            }
            prev = curr;
        } else {
            result += token;
            prev = curr;
        }

    }

    return result;
}

vector<Token> Lexer::tokenSize() {
    vector<Token> tokens;
    stringstream ss(input);
    string statement;

    while(getline(ss, statement, ';')){
        if (!statement.empty()){
            string processStatement = preprocess(statement);
            vector<Token> statementTokens = getStatementTokens(processStatement);
            tokens.insert(tokens.end(), statementTokens.begin(), statementTokens.end());
            tokens.push_back(Token(TokenType::SEMICOLON, ";"));
        }
    }

    if (!tokens.empty()) {
        tokens.pop_back();
    }

    return tokens;
}

vector<Token> Lexer::getStatementTokens(const string &statement) {
    vector<Token> tokens;
    stringstream ss(statement);
    string tokenValue;

    while (ss>>tokenValue) {
        tokens.push_back(getToken(tokenValue));
    }

    return tokens;
}

Token Lexer::getToken(const string &tokenValue) {

    if (isKeyword(tokenValue)){
        return Token(TokenType::KEYWORD, tokenValue);
    } else if (isOperator(tokenValue)){
        return Token(TokenType::OPERATOR, tokenValue);
    } else if (isNumber(tokenValue)){
        return Token(TokenType::NUMBER, tokenValue);
    } else if(isSemicolon(tokenValue)){
        return Token(TokenType::SEMICOLON, tokenValue);
    } else if (isLeftParen(tokenValue)){
        return Token(TokenType::LEFT_PAREN, tokenValue);
    } else if (isRightParen(tokenValue)) {
        return Token(TokenType::RIGHT_PAREN, tokenValue);
    } else if (isIdentifier(tokenValue)) {
        return Token(TokenType::IDENTIFIER, tokenValue);
    } else if (isString(tokenValue)) {
        return Token(TokenType::STRING, tokenValue);
    } else if (isAssignment(tokenValue)) {
        return Token(TokenType::ASSIGNMENT, tokenValue);
    } else if (isLeftBrace(tokenValue)){
        return Token(TokenType::LEFT_BRACE, tokenValue);
    } else if (isRightBrace(tokenValue)){
        return Token(TokenType::RIGHT_BRACE, tokenValue);
    } else {
        return Token(TokenType::UNKNOWN, tokenValue);
    }

}

bool Lexer::isString(const string &str) {
    if (str.empty()) return false;
    return (str.front() == '"' && str.back() == '"');
}

bool Lexer::isAssignment(const string &str) {
    if (str.empty()) return false;
    if (str != "=") return false;
    return true;
}

bool Lexer::isKeyword(const string &str) {
    if (str.empty()) return false;
    if (keywordSet.find(str) == keywordSet.end()) return false;
    return true;
}

bool Lexer::isRightParen(const string &str) {
    if (str.empty()) return false;
    if (str != ")") return false;
    return true;
}

bool Lexer::isLeftParen(const string &str) {
    if (str.empty()) return false;
    if (str != "(") return false;
    return true;
}

bool Lexer::isSemicolon(const string &str) {
    if (str.empty()) return false;
    if (str != ";") return false;
    return true;
}

bool Lexer::isOperator_char(const char &c) {
    if (c == ' ') return false;

}

bool Lexer::isOperator(const string &str) {
    if (str.empty()) return false;
    if (operatorSet.find(str) == operatorSet.end()) return false;
    return true;
}

bool Lexer::isNumber(const string &str) {
    if(str.empty()) return false;
    for (char c: str) {
        if (!isdigit(c))
            return false;
    }
    return true;
}

bool Lexer::isIdentifier(const string &str) {
    if(str.empty() || !(isalpha(str[0]) || str[0] == '_')) return false;
    for (size_t i = 1; i < str.length(); ++i) {
        if (!(isalnum(str[i]) || str[i] == '_'))
            return false;
    }
    return true;
}

bool Lexer::isLeftBrace(const string &str) {
    if (str.empty()) return false;
    if (str != "{") return false;
    return true;
}

bool Lexer::isRightBrace(const string &str) {
    if (str.empty()) return false;
    if (str != "}") return false;
    return true;}

string Lexer::tokenTypeToString(TokenType type) {
        switch(type) {
            case TokenType::IDENTIFIER:
                return "IDENTIFIER";
            case TokenType::NUMBER:
                return "NUMBER";
            case TokenType::OPERATOR:
                return "OPERATOR";
            case TokenType::SEMICOLON:
                return "SEMICOLON";
            case TokenType::LEFT_PAREN:
                return "LEFT_PAREN";
            case TokenType::RIGHT_PAREN:
                return "RIGHT_PAREN";
            case TokenType::LEFT_BRACE:
                return "LEFT_BRACE";
            case TokenType::RIGHT_BRACE:
                return "RIGHT_BRACE";
            case TokenType::KEYWORD:
                return "KEYWORD";
            case TokenType::STRING:
                return "STRING";
            case TokenType::ASSIGNMENT:
                return "ASSIGNMENT";
            case TokenType::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
        }
}









