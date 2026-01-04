#pragma once
#include <iostream>
#include <cctype>
#include <string>
#include <vector>

// Превращение входной строки (LaTeX) в вектор токенов (мат.единиц)
// \sin ( x + 2 ) ^ 3 -> ["\\sin", "(", "x", "+", "2", ")"]
namespace LatexLexer
{
enum class TokenType
{
    Number,         // 1, 3.5 ...
    Variable,       // x
    Operator,       // +, -, *, ^
    Function,       // \\sin, \\frac, \\ln
    Left_Par,       // (
    Right_Par,      // )
    Left_Brace,     // {
    Right_Brace,    // }
    Left_Sqr_Par,   // [
    Right_Sqr_Par,  // ]
    Abs,            // |
    Const,          // \pi, \e
    End             // конец строки
};

struct Token
{
    TokenType type; // тип токена
    std::string value; // сам токен
};

class Lexer
{
public:
    explicit Lexer(const std::string& input_text);

    std::vector<Token> get_tokens();

private:
    std::string text;
    int pos;

    unsigned char look(); // посмотреть текущий символ
    unsigned char get(); // получить текущий символ и сдвинуться на 1 позицию вправо
    void skip_spaces(); // пропустить пробелы

    Token read_num(); // cчитать число
    Token read_variable(); // считатть переменную
    std::string read_block(char open, char close); // вспомогательная для read_word
    Token read_word(); // считать иной enum

};
}
