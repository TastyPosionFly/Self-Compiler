#ifndef COMPILER_DATABASE_H
#define COMPILER_DATABASE_H

#include <unordered_set>

using namespace std;

enum class TokenType{
    IDENTIFIER,//标识符
    NUMBER,//数字
    OPERATOR,//运算符
    SEMICOLON,//分号
    LEFT_PAREN,//左括号
    RIGHT_PAREN,//右括号
    LEFT_BRACE,//左花括号
    RIGHT_BRACE,//右花括号
    KEYWORD,//关键字
    STRING,//字符串
    ASSIGNMENT,//赋值操作
    UNKNOWN//未知符号
};

class DataBase{
private:
    unordered_set<string> operatorSet;
    unordered_set<string> keywordSet;
    unordered_set<string> preprocessSet;
public:
    DataBase(){
        operatorSet = {
                "+", "-", "*", "/", ">", ">=", "<", "<=", "==", "!", "&&", "||"
        };
        preprocessSet = {
                "+", "-", "*", "/", ">", ">=", "<", "<=", "==", "!", "&&", "||", "=",
                "(", ")", "{", "}"
        };
        keywordSet = {
                "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
                "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
                "class", "compl", "concept", "const", "consteval", "constexpr", "const_cast",
                "continue", "co_await", "co_return", "co_yield", "decltype", "default", "delete",
                "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern",
                "false", "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable",
                "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq",
                "private", "protected", "public", "register", "reinterpret_cast", "requires", "return",
                "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch",
                "synchronized", "template", "this", "thread_local", "throw", "true", "try", "typedef",
                "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile",
                "wchar_t", "while", "xor", "xor_eq"
        };
    }

    // 获取运算符集合
    const unordered_set<string>& getOperatorSet() const {
        return operatorSet;
    }

    // 获取关键字集合
    const unordered_set<string>& getKeywordSet() const {
        return keywordSet;
    }

    //获取可能没有被空格分开的字符
    const unordered_set<string>& getPreprocessSet() const{
        return preprocessSet;
    }
};

#endif //COMPILER_DATABASE_H
