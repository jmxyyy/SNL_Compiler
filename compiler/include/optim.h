//
// Created by jmxyyy.
//

#ifndef OPTIM_H
#define OPTIM_H

#include "../include/midcode.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace optim {

// 常表达式节省结构体
struct ConstDefT {
  ConstDefT* former = nullptr;
  midcode::ArgRecord* variable;
  int constValue = 0;
  ConstDefT* next = nullptr;
};

// 基本块结构体
struct BaseBlockType {
  midcode::MidcodeList* begin;
  midcode::MidcodeList* end;
};

struct LoopInfo {
  int state;                        // 外提状态，是否可外提
  midcode::MidcodeList* whileEntry; // 循环开始位置
  midcode::MidcodeList* whileEnd;   // 循环结束位置
  int varDef;                       // 该循环变量定值表数组的下标
};

inline ConstDefT* firstConstDefT;
inline ConstDefT* lastConstDefT;
// 循环信息栈
inline std::vector<LoopInfo> LoopInfoStack;
inline std::unordered_map<int, std::string> codeKind_output;
// 基本块vector
inline std::vector<BaseBlockType> BaseBlock;
// 循环不变式外提
inline std::vector<midcode::ArgRecord*> VarDefSet; // 变量定值表

midcode::MidcodeList* LoopOpti();
midcode::MidcodeList* ConstOptimize();
void OutBaseBlock();

} // namespace optim
#endif // OPTIM_H
