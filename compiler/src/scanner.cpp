//
// Created by jmxyyy.
//

#include "../include/scanner.h"
#include "../include/config.h"

#include <fstream>

namespace scanner {

void Scanner() {
  std::string line;  // 当前读取行
  int line_num = 0;  // 行号
  state sta = START; // 状态

  std::ifstream fin(INPUT_PATH);  // 输入流
  std::ofstream fout(TOKEN_PATH); // 输出流

  if (!fin && !fout) {
    std::string a = "file read fail";
    InputError(a, ERROR_PATH);
    exit(0);
  }

  while ((getline(fin, line))) {
    line_num++;
    int i = 0; // 行下标
    std::string buffer;
    auto query_length = line.length();

    if (sta == FINISH) {
      break;
    }
    while (i < query_length) {
      if (sta == FINISH) { // 读到句号, token写入文件退出循环
        Token tok(line_num, LexType::DOT, "Finish");
        fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem << std::endl;
        buffer.clear();
        break;
      }
      while ((sta == INCOMMENT || sta == START) &&
             i < query_length) { // 开始或注释
        if (line[i] == ' ' || line[i] == '\t') {
          while (line[i] == ' ' || line[i] == '\t') { // 跳过空格
            i++;
          }
        }
        // 处理字母
        if (line[i] >= 'a' && line[i] <= 'z') {
          if (sta == START) {
            sta = INID;
          }
          while ((line[i] >= 'a' && line[i] <= 'z') ||
                 (line[i] >= '0' && line[i] <= '9')) {
            buffer += line[i];
            i++;
            if (i == query_length) { // 行末
              break;
            }
          }
          if (sta == INID) {
            Token tok(line_num, LexType::ID, buffer);
            for (int a = 0; a <= 37; a++) {
              if (arrayLexType[a] == buffer) { // 保留字
                tok.lex = static_cast<LexType>(a);
                tok.sem = "ResaverWord";
                break;
              }
            }
            fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem
                 << std::endl;
            buffer.clear();
            sta = START;
            break;
          }
        }
        // 处理数字
        if (line[i] >= '0' && line[i] <= '9') {
          if (sta == START) {
            sta = INNUM;
          }
          while (line[i] >= '0' && line[i] <= '9') {
            buffer += line[i++];
            if (i == query_length) {
              break;
            }
          }
          // 数字读取结束
          while (line[i] >= 'a' && line[i] <= 'z') {
            sta = START;
            i++;
            if (i == query_length) {
              break;
            }
          }
          if (sta == INNUM) { // 整数
            Token tok(line_num, INTC, buffer);
            fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem
                 << std::endl;
            buffer.clear();
            sta = START;
            break;
          }
          if (sta == START) {
            // 非法
            std::string a = std::to_string(line_num) + " line has num error";
            InputError(a, ERROR_PATH);
            exit(0);
          }
        }
        // 分界符
        if (line[i] == '+' || line[i] == '-' || line[i] == '*' ||
            line[i] == '/' || line[i] == '(' || line[i] == ')' ||
            line[i] == ';' || line[i] == '[' || line[i] == ']' ||
            line[i] == '=' || line[i] == '<') {
          if (sta == START) {
            sta = DONE;
          }
          if (sta == DONE) {
            Token tok;
            tok.lineShow = line_num;
            buffer = line[i];
            for (int a = 0; a <= 37; a++) {
              if (arrayLexType[a] == buffer) {
                tok.lex = static_cast<LexType>(a);
                tok.sem = "SingleSeparator";
                break;
              }
            }
            i++;
            fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem
                 << std::endl;
            buffer.clear();
            sta = START;
            break;
          }
        }
        if (line[i] == ':') {
          if (i < query_length - 1 && line[i + 1] == '=') {
            if (sta == START) {
              sta = INASSIGN;
            }
          }
          if (sta == INASSIGN) {
            Token tok(line_num, ASSIGN, "DobleSeparator");
            i += 2;
            fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem
                 << std::endl;
            buffer.clear();
            sta = START;
            break;
          }
        }
        if (line[i] == ',') {
          if (sta == START) {
            sta = INCHAR;
          }
          if (sta == INCHAR) {
            Token tok(line_num, COMMA, "SingleSeparator");
            i += 1;
            fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem
                 << std::endl;
            buffer.clear();
            sta = START;
            while (line[i] == ' ') {
              i++;
            }
            // 判断逗号之后是否有有效字符
            if (i < query_length && ((line[i] >= 'a' && line[i] <= 'z') ||
                                     (line[i] >= '0' && line[i] <= '9'))) {
            } else {
              std::string a = std::to_string(line_num) + " line error";
              InputError(a, ERROR_PATH);
              exit(0);
            }
            break;
          }
        }
        if (line[i] == '.') {
          if (i < query_length - 1 && line[i + 1] == '.') { // ..
            if (sta == START) {
              sta = INRANGE;
            }
          } else {
            // TODO
            if (sta == START) { // 结束符或数组下标的.
              sta = FINISH;
            }
            // else if (sta == START) {
            //   i++;
            //   Token tok(line_num, DOT, "Dot");
            //   fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem
            //        << std::endl;
            //   buffer.clear();
            //   break;
            // }
          }
          if (sta == INRANGE) {
            Token tok(line_num, UNDERANGE, "ArrayIndex");
            i += 2;
            fout << tok.lineShow << ' ' << tok.lex << ' ' << tok.sem
                 << std::endl;
            buffer.clear();
            sta = START;
            break;
          }
        }
        if (line[i] == '{') {
          if (sta == START) {
            sta = INCOMMENT;
          }
        }
        if (sta == INCOMMENT) { // 注释
          while (i < query_length && line[i] != '}') {
            i++;
          }
          if (line[i] == '}') {
            i++;
            sta = START;
            break;
          }
        }
        if (sta != INCOMMENT && sta != FINISH && i < query_length) {
          std::string a = std::to_string(line_num) + " line error";
          InputError(a, ERROR_PATH);
          exit(0);
        }
      }
    }
  }
  fin.close();
  fout.close();
}

} // namespace scanner