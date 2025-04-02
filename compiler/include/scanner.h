//
// Created by jmxyyy.
//

#ifndef SCANNER_H
#define SCANNER_H

#include <fstream>
#include <string>

namespace scanner {

enum LexType {
  ID,
  IF,
  BEGIN,
  INTC,
  END,
  PLUS,
  MINUS,
  TIMES,
  OVER,
  EQ,
  LT,
  LMIDPAREN,
  RMIDPAREN,
  DOT,
  TYPE,
  VAR,
  PROCEDURE,
  PROGRAM,
  SEMI,
  INTEGER,
  CHAR,
  ARRAY,
  RECORD,
  UNDERANGE,
  OF,
  COMMA,
  LPAREN,
  RPAREN,
  ENDWH,
  WHILE,
  RETURN,
  READ,
  WRITE,
  ASSIGN,
  THEN,
  FI,
  ELSE,
  DO
};

enum state {
  START,
  INASSIGN,
  INCOMMENT,
  INNUM,
  INID,
  INCHAR,
  INRANGE,
  DONE,
  FINISH
};

inline std::string arrayLexType[] = {
    "id",    "if",     "begin",     "intc",    "end",   "+",       "-",
    "*",     "/",      "=",         "<",       "[",     "]",       ".",
    "type",  "var",    "procedure", "program", ";",     "integer", "char",
    "array", "record", "..",        "of",      ",",     "(",       ")",
    "endwh", "while",  "return",    "read",    "write", ":=",      "then",
    "fi",    "else",   "do"

};

class Token {
public:
  int lineShow;    // 行号
  LexType lex;     // 词法信息
  std::string sem; // 语义信息

  Token() : lineShow(-1), lex(LexType::END) {};
  Token(const int lineShow_, const LexType lex_, std::string sem_)
      : lineShow(lineShow_), lex(lex_), sem(std::move(sem_)) {}
};

void Scanner();

inline void InputError(const std::string& errorInfo, const std::string& path) {
  std::ofstream output;
  output.open(path);
  output << errorInfo;
  output.close();
}

} // namespace scanner

#endif // SCANNER_H
