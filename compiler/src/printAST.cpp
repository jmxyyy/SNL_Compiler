#include "../include/parse.h"
#include "../include/scanner.h"

#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace parse {

// 符号栈，用于进行SNL的LL(1)语法分析
// 一开始只压栈当前节点的儿子或者兄弟节点，并不是直接压入节点
// 因为节点的生成是在语法分析过程中完成的
std::stack<std::string> signStack;

// 操作符栈，生成声明部分和语句部分的语法树
std::stack<TreeNode*> opSignStack;

// 操作数栈，用于生成表达式部分的语法树
std::stack<TreeNode*> opNumStack;

// 语法树栈，用于生成表达式部分的语法树
std::stack<TreeNode**> syntaxTreeStack;

// 存储所有的产生式，每个产生式是一个vector<string>
// vector<string>中的每个元素是一个终极符或者非终极符
std::vector<std::vector<std::string>> productSet;

// 存储所有产生式的predict集，predict集中每个元素是一个终极符
std::vector<std::vector<std::string>> predictSet;

// LL1矩阵，作用是对于当前非终极符和当前输入符确定应该选择的语法规则
// 其值对应产生式的编号或者错误编号
// 产生式编号从1开始，错误编号为0，产生式是行号
// predict集中每个元素是列号
std::vector<std::vector<int>> LL1Table;

// 存储所有终极符
std::set<std::string> VT;

// 存储所有非终极符
std::set<std::string> VN;

// 开始符
std::string S;

// 存储当前节点
TreeNode* currentP;

// 保存当前指向记录类型声明节点的指针
TreeNode* saveP;

// 保存指向某一节点的指针
TreeNode* saveFuncP;
DeckEnum* temp;
bool getExpResult = true;

// 用于数组
bool getExpResult2;

// 用于计算括号
int expflag = 0;

extern scanner::Token token;
int flag_tag = 0;

std::map<NodeKindEnum, std::string> nodeKindMapForOutput = {
    {ProK, "ProK"}, {PheadK, "PheadK"},     {TypeK, "TypeK"},
    {VarK, "VarK"}, {ProcDecK, "ProcDecK"}, {StmLK, "StmLK"},
    {DecK, "DecK"}, {StmtK, "StmtK"},       {ExpK, "ExpK"}};

void PrintTreeNode(const TreeNode* node, int depth) {
  for (int i = 0; i < depth; i++) {
    std::string str = "��   ";
    std::cout << str;
  }

  if (node->idNum > 0) {
    if (node->nodeKind == ProcDecK)
      flag_tag = 1;
    std::cout << "������ " << nodeKindMapForOutput[node->nodeKind] << "  ";
    if (nodeKindMapForOutput[node->nodeKind] == "DecK") {
      switch (node->kind.dec) {
      case 0:
        std::cout << "ArrayK  ";
        break;
      case 1:
        std::cout << "CharK  ";
        break;
      case 2:
        if (flag_tag == 1 && node->attr.procAttr.paramType == ValParamType)
          std::cout << " value param: IntegerK  ";
        else if (flag_tag == 1 && node->attr.procAttr.paramType == VarParamType)
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
    } else if (nodeKindMapForOutput[node->nodeKind] == "StmtK") {
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
    } else if (nodeKindMapForOutput[node->nodeKind] == "ExpK") {
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
    for (int i = 0; i < node->idNum; i++) {
      std::cout << node->name[i];
      if (i < node->idNum - 1) {
        std::cout << ", ";
      }
    }
    std::cout << ")" << std::endl;
  } else if (nodeKindMapForOutput[node->nodeKind] == "StmtK") {
    std::cout << "������ " << nodeKindMapForOutput[node->nodeKind] << "  ";
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

  } else if (nodeKindMapForOutput[node->nodeKind] == "ExpK") {
    std::cout << "|--- " << nodeKindMapForOutput[node->nodeKind] << "  ";
    switch (node->kind.exp) {
    case 0:
      std::cout << "OpK" << "  " << arrayLexTypeD[(node->attr).expAttr.op]
                << std::endl;
      break;
    case 1:
      std::cout << "ConstK" << "  " << (node->attr).expAttr.val << std::endl;
      break;
    default:
      break;
    }
  } else {
    if (node->nodeKind == VarK)
      flag_tag = 0;
    if (node->nodeKind == StmLK)
      flag_tag = 0;
    std::cout << "|--- " << nodeKindMapForOutput[node->nodeKind] << std::endl;
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
    ouput << t->nodeKind << " ";
    ouput << t->kind.dec << " ";
    ouput << t->idNum << " ";
    for (int i = 0; i < t->idNum; i++) {
      ouput << t->name[i] << " ";
    }
    ouput << t->type_name << " ";
    ouput << t->attr.arrayAttr.childType << " " << t->attr.arrayAttr.low << " "
          << t->attr.arrayAttr.up << " ";
    ouput << t->attr.expAttr.op << " " << t->attr.expAttr.type << " "
          << t->attr.expAttr.val << " " << t->attr.expAttr.varKind << " ";
    ouput << t->attr.procAttr.paramType << std::endl;

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

} // namespace parse
