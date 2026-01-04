#include "Lexer.h"
#include <QDebug>
//using namespace LatexLexer;
LatexLexer::Lexer::Lexer(const std::string& input_text) : text(input_text), pos(0) {
    for (unsigned char c : input_text)
    {
        qDebug() << (int)c << "\n";
        if (c >= 128)
            throw std::invalid_argument("Only ASCII input allowed");
    }
}

unsigned char LatexLexer::Lexer::look()
{
    return pos < text.size() ? (unsigned char)text[pos] : '\0';
}

unsigned char LatexLexer::Lexer::get()
{
    return pos < text.size() ? (unsigned char)text[pos++] : '\0';
}

void LatexLexer::Lexer::skip_spaces()
{
    while (true)
    {
        unsigned char c = look();
        if (c == 0) return;

        if (c >= 128)
            throw std::invalid_argument("Lexer Error: Only ASCII symbols allowed");

        if (!isspace(c))
            return;

        get();
    }
}

LatexLexer::Token LatexLexer::Lexer::read_num()
{
    std::string number;
    bool float_type = false;

    while (isdigit((unsigned char)look()) || (unsigned char)look() == '.')
    {
        if (look() == '.')
        {
            if (!float_type) float_type = true;
            else
            {
                throw(std::invalid_argument(std::string("Lexer Error: Two float points in pos: ") + std::to_string(pos) + ")"));
            }
        }
        number += get();
    }
    return {LatexLexer::TokenType::Number, number};
}

std::string LatexLexer::Lexer::read_block(char open, char close)
{
    std::string block;

    if (look() != open)
        throw std::invalid_argument(std::string("Lexer Error: Expected '") + open + "' in pos " + std::to_string(pos));
    int open_pos = pos;
    block += get();
    int depth = 1; // уровень вложенности

    while (pos < text.size() && depth > 0)
    {
        char symb = get();
        block += symb;

        if (symb == open) depth++;
        else if (symb == close) depth--;
    }

    if(depth != 0)
        throw std::invalid_argument(std::string("Lexer Error: Unclosed block '") + open + "' in pos" + std::to_string(pos));
    return block;
}

LatexLexer::Token LatexLexer::Lexer::read_word()
{
    std::string word;

    word += get();
    while (isalpha((unsigned char)look())) word += get();
    // Для log_{}{}
    if (word == "\\log")
    {
        if (look() != '_') throw(std::invalid_argument("Lexer Error: No index '_' in log (e.g:\n\\log_2{x}\n\\ln(x)\\lg(x)\\log_{3}^{2}{x})"));
        else word += get();
    }
    // Для arcctg, argsinh, argcosh, argtgh
    else if (word == "\\textrm")
    {
        word += read_block('{', '}');
    }
    // Константы
    else if (word == "\\pi" || word == "\\e")
    {
        return {LatexLexer::TokenType::Const, word};
    }
    return {LatexLexer::TokenType::Function, word};
}

LatexLexer::Token LatexLexer::Lexer::read_variable()
{
    std::string word;
    // Переменная
    while (isalnum((unsigned char)look())) word += get();
    return {LatexLexer::TokenType::Variable, word};
}

std::vector<LatexLexer::Token> LatexLexer::Lexer::get_tokens()
{
    std::vector<LatexLexer::Token> tokens;

    while (pos < text.size())
    {
        skip_spaces();
        unsigned char symb = (unsigned char)look();

        if (symb >= 128) {
            throw(std::invalid_argument("Lexer Error: Only ASCII symbols allowed"));
        }

        if (isalpha((unsigned char)symb)) tokens.emplace_back(read_variable());
        else if(symb == '\\') tokens.emplace_back(read_word());
        else if (isdigit((unsigned char)symb)) tokens.emplace_back(read_num());
        else
        {
            switch (symb)
            {
            case '+' : case '-' : case '*' : case '^':
            {
                tokens.push_back({LatexLexer::TokenType::Operator, std::string(1, get())});
                break;
            }
            case '(':
                tokens.push_back({LatexLexer::TokenType::Left_Par, std::string(1, get())});
                break;
            case ')':
                tokens.push_back({LatexLexer::TokenType::Right_Par, std::string(1, get())});
                break;
            case '{':
                tokens.push_back({LatexLexer::TokenType::Left_Brace, std::string(1, get())});
                break;
            case '}':
                tokens.push_back({LatexLexer::TokenType::Right_Brace, std::string(1, get())});
                break;
            case '[':
                tokens.push_back({LatexLexer::TokenType::Left_Sqr_Par, std::string(1, get())});
                break;
            case ']':
                tokens.push_back({LatexLexer::TokenType::Right_Sqr_Par, std::string(1, get())});
                break;
            case '|':
                tokens.push_back({LatexLexer::TokenType::Abs, std::string(1, get())});
                break;
            default:
                throw(std::invalid_argument(std::string("Lexer Error: Unknown symbol: ") + std::string(1, symb) + "in pos " + std::to_string(pos)));
                break;
            }
        }
    }
    tokens.push_back({LatexLexer::TokenType::End, ""});
    return tokens;
}
