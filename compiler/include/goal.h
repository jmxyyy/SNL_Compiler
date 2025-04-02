//
// Created by jmxyyy.
//

#ifndef GOAL_H
#define GOAL_H

#include "../include/midcode.h"

#include <string>

namespace goal {

inline FILE* goal_code;

void outputInstruct(const std::string& errorInfo, const std::string& path);
void operandGen(const midcode::ArgRecord* arg);
void FindAddr(const midcode::ArgRecord* arg);
void FindSp(int varLevel);
void codeGen(midcode::MidcodeList* mid_code);
void arithGen(const midcode::MidcodeList* mid_code);
void aaddGen(const midcode::MidcodeList* mid_code);
void readGen(const midcode::MidcodeList* mid_code);
void writeGen(const midcode::MidcodeList* mid_code);
void returnGen(midcode::MidcodeList* mid_code);
void assignGen(const midcode::MidcodeList* mid_code);
void labelGen(const midcode::MidcodeList* mid_code);
void jumpGen(const midcode::MidcodeList* mid_code, int i);
void jump0Gen(const midcode::MidcodeList* mid_code);
void valactGen(const midcode::MidcodeList* mid_code);
void varactGen(const midcode::MidcodeList* mid_code);
void callGen(const midcode::MidcodeList* mid_code);
void pentryGen(const midcode::MidcodeList* mid_code);
void endProcGen(midcode::MidcodeList* mid_code);
void mentryGen(const midcode::MidcodeList* mid_code, int save);

} // namespace goal

#endif // GOAL_H
