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
private:
    QTextEdit *codeInput;
    QPushButton *analyzeButton;
    QSplitter *splitter;
    QTextEdit *resultOutput;

    void setupUI() {
        codeInput = new QTextEdit(this);
        codeInput->setPlaceholderText("输入代码");

        analyzeButton = new QPushButton("进行词法分析", this);
        connect(analyzeButton, &QPushButton::clicked, this, &GUI::analyzeCode);

        resultOutput = new QTextEdit(this);
        resultOutput->setReadOnly(true);

        splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(codeInput);
        splitter->addWidget(resultOutput);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(splitter);
        layout->addWidget(analyzeButton);

        setLayout(layout);
    }

};

#endif //COMPILER_GUI_H
