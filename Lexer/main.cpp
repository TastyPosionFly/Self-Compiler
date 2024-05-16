#include <iostream>
#include "Lexer.h"

using namespace std;

int main(){
    // 输入源代码
    string input = "x=1+2;";

    // 创建词法分析器对象
    Lexer lexer(input);

    // 执行词法分析
    vector<Token> tokens = lexer.tokenSize();

    // 输出词法分析结果
    cout << "The result of lex:" << std::endl;
    for (const Token& token : tokens) {
        cout << "Token Type: " <<  lexer.tokenTypeToString(token.tokenType) << ", value: " << token.value << std::endl;
    }

    return 0;

//    int main(){
//        int x = 2;
//        x = x+3;
//        return x;
//    }
}