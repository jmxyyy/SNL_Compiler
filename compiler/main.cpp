#include "./include/scanner.h"
#include "./include/parse.h"
#include "./include/midcode.h"
#include "./include/optim.h"
#include "./include/goal.h"
#include "./include/config.h"
#include "./include/config.h"

#include <iostream>

int main() {
  std::cout << "Hello, World!" << std::endl;

  // 词法
  scanner::Scanner();
  std::cout << "Token END" << std::endl;

  // 语法
  auto* root = parse::DesParse();
  std::cout << "Parse END" << std::endl;

  parse::Analyze(root);
  std::cout << "Analyse END" << std::endl;

  // 中间代码生成
  midcode::GenMidCode(root);
  midcode::PrintMidCode(midcode::firstDummy);
  std::cout << "GenMidCode END" << std::endl;

  // 优化
  optim::ConstOptimize();
  optim::LoopOpti();
  optim::OutBaseBlock();
  midcode::PrintMidCode(midcode::firstDummy);
  std::cout << "MidCode Optim END" << std::endl;

  // 目标代码
  goal::codeGen(midcode::firstDummy);
  std::cout << "GenGoalCode END" << std::endl;

  return 0;
}
