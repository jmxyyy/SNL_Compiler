#include "../include/parse.h"
#include "../include/scanner.h"

#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace parse {

std::stack<std::string> signStack;
std::stack<TreeNode*> opSignStack;
std::stack<TreeNode*> opNumStack;
std::stack<TreeNode**> syntaxTreeStack;
std::vector<std::vector<std::string>> productSet;
std::vector<std::vector<std::string>> predictSet;
std::vector<std::vector<int>> LL1Table;
std::set<std::string> VT;
std::set<std::string> VN;
std::string S;
TreeNode* currentP;
TreeNode* saveP;
TreeNode* saveFuncP;
DeckEnum* temp;
bool getExpResult = true;
bool getExpResult2;
int expflag = 0;
extern scanner::Token token;
int flag_tag = 0;
// std::string arrayLexType_hyf[] = {
//     "ID",        "IF",      "BEGIN",  "INTC",   "END",       "PLUS",
//     "MINUS",     "TIMES",   "OVER",   "EQ",     "LT",        "LMIDPAREN",
//     "RMIDPAREN", "DOT",     "TYPE",   "VAR",    "PROCEDURE", "PROGRAM",
//     "SEMI",      "INTEGER", "CHAR",   "ARRAY",  "RECORD",    "UNDERANGE",
//     "OF",        "COMMA",   "LPAREN", "RPAREN", "ENDWH",     "WHILE",
//     "RETURN",    "READ",    "WRITE",  "ASSIGN", "THEN",      "FI",
//     "ELSE",      "DO"};

std::map<NodeKindEnum, std::string> nodeKindMapForOutput = {
    {ProK, "ProK"}, {PheadK, "PheadK"},     {TypeK, "TypeK"},
    {VarK, "VarK"}, {ProcDecK, "ProcDecK"}, {StmLK, "StmLK"},
    {DecK, "DecK"}, {StmtK, "StmtK"},       {ExpK, "ExpK"}};

void PrintTreeNode(const TreeNode* node, int depth) {
  for (int i = 0; i < depth; i++) {
    std::string str = "��   ";
    std::cout << str;
  }

  if (node->idnum > 0) {
    if (node->nodekind == ProcDecK)
      flag_tag = 1;
    std::cout << "������ " << nodeKindMapForOutput[node->nodekind] << "  ";
    if (nodeKindMapForOutput[node->nodekind] == "DecK") {
      switch (node->kind.dec) {
      case 0:
        std::cout << "ArrayK  ";
        break;
      case 1:
        std::cout << "CharK  ";
        break;
      case 2:
        if (flag_tag == 1 && node->attr.procAttr.paramt == Valparamtype)
          std::cout << " value param: IntegerK  ";
        else if (flag_tag == 1 && node->attr.procAttr.paramt == Varparamtype)
          std::cout << " var param: IntegerK  ";
        else
          std::cout << "IntegerK  ";
        break;
      case 3:
        std::cout << "RecordK  ";
        break;
      case 4:
        std::cout << "IdK  ";
        break;
      }
    } else if (nodeKindMapForOutput[node->nodekind] == "StmtK") {
      switch (node->kind.stmt) {
      case 0:
        std::cout << "IfK  ";
        break;
      case 1:
        std::cout << "WhileK  ";
        break;
      case 2:
        std::cout << "AssignK";
        break;
      case 3:
        std::cout << "ReadK";
        break;
      case 4:
        std::cout << "WriteK";
        break;
      case 5:
        std::cout << "CallK";
        break;
      case 6:
        std::cout << "ReturnK";
        break;
      }

      std::cout << "  ";
    } else if (nodeKindMapForOutput[node->nodekind] == "ExpK") {
      switch (node->kind.exp) {
      case 0:
        std::cout << "OpK";
        break;
      case 1:
        std::cout << "ConstK";
        break;
      case 2:
        std::cout << "IdEK";
        break;
      }
      std::cout << "  ";
    }
    std::cout << "ID(";
    for (int i = 0; i < node->idnum; i++) {
      std::cout << node->name[i];
      if (i < node->idnum - 1) {
        std::cout << ", ";
      }
    }
    std::cout << ")" << std::endl;
  } else if (nodeKindMapForOutput[node->nodekind] == "StmtK") {
    std::cout << "������ " << nodeKindMapForOutput[node->nodekind] << "  ";
    switch (node->kind.stmt) {
    case 0:
      std::cout << "IfK" << std::endl;
      break;
    case 1:
      std::cout << "WhileK" << std::endl;
      break;
    case 2:
      std::cout << "AssignK" << std::endl;
      break;
    case 3:
      std::cout << "ReadK" << std::endl;
      break;
    case 4:
      std::cout << "WriteK" << std::endl;
      break;
    case 5:
      std::cout << "CallK" << std::endl;
      break;
    case 6:
      std::cout << "ReturnK" << std::endl;
      break;
    }

  } else if (nodeKindMapForOutput[node->nodekind] == "ExpK") {
    std::cout << "������ " << nodeKindMapForOutput[node->nodekind] << "  ";
    switch (node->kind.exp) {
    case 0:
      std::cout << "OpK" << "  " << arrayLexTypeD[(node->attr).expAttr.op]
                << std::endl;
      break;
    case 1:
      std::cout << "ConstK" << "  " << (node->attr).expAttr.val << std::endl;
      break;
    }

  } else {
    if (node->nodekind == VarK)
      flag_tag = 0;
    if (node->nodekind == StmLK)
      flag_tag = 0;
    std::cout << "������ " << nodeKindMapForOutput[node->nodekind] << std::endl;
  }

  for (int i = 0; i <= 2; i++) {
    if (node->child[i] != nullptr) {
      PrintTreeNode(node->child[i], depth + 1);
    }
  }

  if (node->sibling != nullptr) {
    PrintTreeNode(node->sibling, depth);
  }
}

void Input(TreeNode* root, const std::string& path) {
  PrintTreeNode(root, 0);
  std::ofstream ouput;
  ouput.open(path);
  std::stack<TreeNode*> inputSort;
  inputSort.push(root);
  while (!inputSort.empty()) {
    TreeNode* t = inputSort.top();
    inputSort.pop();
    ouput << t->child[0] << " " << t->child[1] << " " << t->child[2] << " ";
    ouput << t->sibling << " ";
    ouput << t->lineno << " ";
    ouput << t->nodekind << " ";
    ouput << t->kind.dec << " ";
    ouput << t->idnum << " ";
    for (int i = 0; i < t->idnum; i++) {
      ouput << t->name[i] << " ";
    }
    ouput << t->type_name << " ";
    ouput << t->attr.arrayAttr.childType << " " << t->attr.arrayAttr.low << " "
          << t->attr.arrayAttr.up << " ";
    ouput << t->attr.expAttr.op << " " << t->attr.expAttr.type << " "
          << t->attr.expAttr.val << " " << t->attr.expAttr.varkind << " ";
    ouput << t->attr.procAttr.paramt << std::endl;

    if (t->sibling != nullptr) {
      inputSort.push(t->sibling);
    }
    for (int i = 2; i >= 0; i--) {
      if (t->child[i] != nullptr) {
        inputSort.push(t->child[i]);
      }
    }
  }
  ouput.close();
}
//
// void InputError(std::string errorInfo, std::string path) {
//   std::ofstream ouput;
//   ouput.open(path);
//   ouput << errorInfo;
//   ouput.close();
// }
} // namespace parse
