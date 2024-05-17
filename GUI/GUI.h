#ifndef COMPILER_GUI_H
#define COMPILER_GUI_H

#include <QApplication>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSplitter>

#include "../Lexer/Lexer.h"
#include "../Parser/LRParserGenerator.h"
#include "../Parser/parser.h"

class GUI: public QWidget{
    Q_OBJECT
public:
    GUI(QWidget *parent = nullptr) : QWidget(parent){
        setupUI();
    }

private slots:
    void analyzeCode() {
        QString code = codeInput->toPlainText();
        // 调用词法分析器进行分析
        Lexer lexer(code.toStdString());
        std::vector<Token> tokens = lexer.tokenSize();
        // 将词法分析结果显示在消息框中
        QString result;
        for (Token token : tokens) {
            result += QString::fromStdString(token.toString()) + "\n";
        }
        resultOutput->setText(result);;
    }

    void analyzeSyntax() {
        QString code = codeInput->toPlainText();
        // 调用词法分析器进行分析
        Lexer lexer(code.toStdString());
        std::vector<Token> tokens = lexer.tokenSize();

        // 将标记序列分割成语句
        vector<vector<Token>> statements = splitIntoStatements(tokens);

        // 初始化语法分析器
        vector<LRParserGenerator::Production> productions = lrParserGenerator.getProduction();
        map<pair<int, LRParserGenerator::Symbol>, pair<char, int>> actionTable = lrParserGenerator.getActionTable();
        map<pair<int, LRParserGenerator::Symbol>, int> gotoTable = lrParserGenerator.gotoTable;
        parser parser(productions, actionTable, gotoTable);

        // 解析每个语句
        QString result;
        for (const auto& statement : statements) {
            vector<LRParserGenerator::Symbol> inputSymbols;
            for (const auto& token : statement) {
                inputSymbols.push_back({token.value, tokenTypeToSymbolType(token.tokenType)});
            }

            inputSymbols.push_back({"$", LRParserGenerator::SymbolType::TERMINAL});

            cout << "Input Symbols: ";
            for (const auto& symbol : inputSymbols) {
                cout << symbol.name << " ";
            }
            cout << endl;

            bool parseResult = parser.parse(inputSymbols);
            if (parseResult) {
                result += "Statement parsed successfully.\n";
            } else {
                result += "Syntax error in statement: ";
                for (const auto& token : statement) {
                    result += QString::fromStdString(token.value) + " ";
                }
                result += "\n";
            }
        }
        resultOutput->setText(result);
    }

    void analyzeBoth() {
        analyzeCode();
        analyzeSyntax();
    }

private:
    QTextEdit *codeInput;
    QPushButton *analyzeButton;
    QPushButton *syntaxButton;
    QPushButton *bothButton;
    QSplitter *splitter;
    QTextEdit *resultOutput;

    LRParserGenerator lrParserGenerator;

    string filename = "D:/Work/Self-Compiler/grammar.txt";

    void setupUI() {
        lrParserGenerator.generateParser(filename);

        codeInput = new QTextEdit(this);
        codeInput->setPlaceholderText("输入代码");

        analyzeButton = new QPushButton("进行词法分析", this);
        connect(analyzeButton, &QPushButton::clicked, this, &GUI::analyzeCode);

        syntaxButton = new QPushButton("进行语法分析", this);
        connect(syntaxButton, &QPushButton::clicked, this, &GUI::analyzeSyntax);

        bothButton = new QPushButton("进行词法和语法分析", this);
        connect(bothButton, &QPushButton::clicked, this, &GUI::analyzeBoth);

        resultOutput = new QTextEdit(this);
        resultOutput->setReadOnly(true);

        splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(codeInput);
        splitter->addWidget(resultOutput);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(splitter);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(analyzeButton);
        buttonLayout->addWidget(syntaxButton);
        buttonLayout->addWidget(bothButton);

        layout->addLayout(buttonLayout);

        setLayout(layout);
    }


    vector<vector<Token>> splitIntoStatements(const vector<Token>& tokens) {
        vector<vector<Token>> statements;
        vector<Token> currentStatement;

        for (const auto& token : tokens) {
            currentStatement.push_back(token);
            if (token.tokenType == TokenType::SEMICOLON) {
                currentStatement.pop_back();
                statements.push_back(currentStatement);
                currentStatement.clear();
            }
        }

        // Handle the case where the last statement does not end with a semicolon
        if (!currentStatement.empty()) {
            statements.push_back(currentStatement);
        }

        return statements;
    }

    LRParserGenerator::SymbolType tokenTypeToSymbolType(TokenType tokenType) {
        switch (tokenType) {
            case TokenType::IDENTIFIER:
            case TokenType::NUMBER:
            case TokenType::OPERATOR:
            case TokenType::SEMICOLON:
            case TokenType::LEFT_PAREN:
            case TokenType::RIGHT_PAREN:
            case TokenType::LEFT_BRACE:
            case TokenType::RIGHT_BRACE:
            case TokenType::KEYWORD:
            case TokenType::STRING:
            case TokenType::ASSIGNMENT:
                return LRParserGenerator::SymbolType::TERMINAL;
            default:
                return LRParserGenerator::SymbolType::TERMINAL; // or handle UNKNOWN type appropriately
        }
    }
};

#endif //COMPILER_GUI_H
