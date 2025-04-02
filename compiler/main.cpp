#include "./include/scanner.h"
#include "./include/parse.h"
#include "./include/midcode.h"

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

  return 0;
}
