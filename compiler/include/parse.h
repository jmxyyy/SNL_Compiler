//
// Created by jmxyyy.
//

#ifndef PARSE_H
#define PARSE_H

#include "../include/scanner.h"

#include <fstream>
#include <string>
#include <vector>

namespace parse {

inline std::string arrayLexTypeD[] = {
    "ID",        "IF",      "BEGIN",  "INTC",   "END",       "PLUS",
    "MINUS",     "TIMES",   "OVER",   "EQ",     "LT",        "LMIDPAREN",
    "RMIDPAREN", "DOT",     "TYPE",   "VAR",    "PROCEDURE", "PROGRAM",
    "SEMI",      "INTEGER", "CHAR",   "ARRAY",  "RECORD",    "UNDERANGE",
    "OF",        "COMMA",   "LPAREN", "RPAREN", "ENDWH",     "WHILE",
    "RETURN",    "READ",    "WRITE",  "ASSIGN", "THEN",      "FI",
    "ELSE",      "DO"};

// 语法树节点类型
enum NodeKindEnum {
  ProK,     // 根
  PheadK,   // 程序头
  TypeK,    // 类型声明
  VarK,     // 变量声明
  ProcDecK, // 函数声明
  StmLK,    // 语句序列
  DecK,     // 声明节点
  StmtK,    // 语句节点
  ExpK      // 表达式节点
};

// 声明节点类型
enum DeckEnum {
  ArrayK,   // 数组
  CharK,    // 字符
  IntegerK, // 整数
  RecordK,  // 记录
  IdK       // 类类型标识符
};

// 语句节点类型
enum StmtKEnum { IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK };

// 表达式节点类型
enum ExpKEnum {
  OpK,    // 操作符
  ConstK, // 常整数
  IdEK    // 标识符
};

// 参数类型
enum ParamTypeEnum { ValParamType, VarParamType };

// 变量类型
enum VarKindEnum {
  IdV,       // 标识符变量
  ArrayMemV, // 数组变量
  FieldMemV  // 域成员变量
};

// 语法树节点检查类型
enum ExpType { Void, Integer, Boolean };

union Kind {
  DeckEnum dec;   // 记录语法树节点的声明类型,当nodeKind=DecK时有效
  StmtKEnum stmt; // 记录语法树节点的语句类型,当nodeKind=StmtK时有效
  ExpKEnum exp;   // 记录语法树节点的表达式类型,当nodeKind=ExpK时有效
};

// 记录数组属性
struct ArrayAttr {
  int low;
  int up;
  DeckEnum childType;
};

// 记录过程属性
struct ProcAttr {
  ParamTypeEnum paramType;
};

// 记录表达式属性
struct ExpAttr {
  scanner::LexType op; // 操作符
  int val;             // 语法树节点数值
  VarKindEnum varKind; // 变量类别
  ExpType type;        // 语法树节点检查类型
};

// 记录语法树节点其他属性
struct Attr {
  ArrayAttr arrayAttr;
  ProcAttr procAttr;
  ExpAttr expAttr;
};

enum IdKind { typeKind, varKind, procKind };

enum AccessKind { dir, indir };

enum TypeKind {
  intTy,
  charTy,
  arrayTy,
  recordTy,
  boolTy
}; // intTy, charTy, arrayTy, recordTy, boolTy

struct symbolTable;
struct typeIR;

// 形参信息表
struct ParamTable {
  symbolTable* entry = nullptr; // 形参标识符在符号表中位置
  ParamTable* next = nullptr;
};

// 标识符信息项
struct AttributeIR {
  typeIR* idtype; // 指向标识符的类型内部表示
  IdKind kind;    // 标识符类型
  // 标识符不同类型对应不同属性
  union {
    // 变量标识符属性
    struct {
      // 表示是直接变量还是间接变量
      // dir表示直接变量
      // indir表示是间接变量（变参属于间接变量），
      AccessKind access;
      int level;
      int off;
    } VarAttr;
    // 过程名标识符属性
    struct {
      int level;
      ParamTable* param; // 参数表
      int code;
      int size;
    } ProcAttr;
  } More;
};

// 符号表
struct symbolTable {
  std::string idName;
  AttributeIR attrIR;
  symbolTable* next = nullptr;
};

struct TreeNode {
  TreeNode* child[3];    // 指向子语法树节点指针,0 :程序头,1:声明，2:程序体
  TreeNode* sibling;     // 指向兄弟语法树节点指针
  int lineno;            // 记录源程序行号
  NodeKindEnum nodeKind; // 记录语法树节点类型
  Kind kind;             // 记录语法树节点的具体类型
  int idNum = 0;         // 记录一个节点中标志符个数
  std::vector<std::string> name; // 容器成员是节点中的标志符的名字
  // 容器成员是节点中的各个标志符在符号表中的入口
  std::vector<symbolTable*> table;
  // 记录类型名，当节点为声明类型且类型是由类型标志符表示是有效
  std::string type_name;
  Attr attr; // 记录语法树节点其他属性

  ~TreeNode() {
    for (const auto& i : child) {
      delete i;
    }
    for (const auto i: table) {
      delete i;
    }
  }
};

// 记录的域表
struct fieldChain {
  std::string idName; // 记录域中的标识符
  typeIR* unitType;   // 指向域类型的内部表示
  int offset;         // 表示当前标识符在记录中的偏移 TODO:error here
  fieldChain* next;   // 指向下一个域
};

// 标识符内部表示
struct typeIR {
  int size;      // 类型所占空间大小
  TypeKind kind; // intTy, charTy, arrayTy, recordTy, boolTy
  union {
    struct {
      typeIR* indexTy;
      typeIR* elemTy;
      int low;
      int up;
    } ArrayAttr;
    fieldChain* body; // 记录类型中的域链
  } More;
};

typeIR* TypeProcess(const TreeNode* t);

void statement(TreeNode* t);

void traverse(TreeNode* t);

void Body(TreeNode* t);

void Analyze(TreeNode* currentP);
typeIR* recordVar(TreeNode* t);
typeIR* arrayVar(TreeNode* t);

// 语法解析函数,生成语法树
TreeNode* Parse();

TreeNode* Program();

// 读入程序头
TreeNode* ProgramHead();

// 读入声明
TreeNode* DeclarePart();

// 读入程序体
TreeNode* ProgramBody();

// 解析类型声明部分
TreeNode* TypeDec();

// 类型声明部分
TreeNode* TypeDeclaration();

// 解析类型定义列表
TreeNode* TypeDecList();

// 解析可能存在的其他类型定义
TreeNode* TypeDecMore();

// 解析基本的数据类型，然后将具体的类型保存到语法树节点中，即保存标识符
void TypeId(TreeNode* t);

// 解析具体到底是什么类型
void TypeDef(TreeNode* t);

void BaseType(TreeNode* t);

void StructureType(TreeNode* t);

void ArrayType(TreeNode* t);

void RecType(TreeNode* t);

// 解析结构体声明
TreeNode* FieldDecList();

TreeNode* FieldDecMore();

void IdList(TreeNode* t);

void IdMore(TreeNode* t);

TreeNode* VarDec();

TreeNode* VarDeclaration();

TreeNode* VarDecList();

TreeNode* VarDecMore();

void VarIdList(TreeNode* t);

void VarIdMore(TreeNode* t);

// 解析过程声明
TreeNode* ProcDec();

// 解析过程声明部分
TreeNode* ProcDeclaration();

// 解析其他过程声明
TreeNode* ProcDecMore();

// 解析参数列表
void ParamList(TreeNode* t);

// 解析参数声明列表
TreeNode* ParamDecList();

// 解析其他参数声明
TreeNode* ParamMore();

// 解析参数
TreeNode* Param();

// 解析形式参数列表
void FormList(TreeNode* t);

void FidMore(TreeNode* t);

TreeNode* ProcDecPart();

TreeNode* ProcBody();

TreeNode* StmList();

TreeNode* StmMore();

// 语句
TreeNode* Stm();

void AssCall(TreeNode* t);

// 函数调用
void AssignmentRest(TreeNode* t);

// 处理条件语句
TreeNode* ConditionalStm();

// 处理循环语句
TreeNode* LoopStm();

// 处理输入语句
TreeNode* InputStm();

// 处理输出语句
TreeNode* OutputStm();

// 处理返回语句
TreeNode* ReturnStm();

// 处理调用语句剩余部分
void CallStmRest(TreeNode* t);

// 解析实参列表
TreeNode* ActParamList();

// 实参列表后面可能还有实参
TreeNode* ActParamMore();

// 处理表达式
TreeNode* Exp();

// 处理简单表达式
TreeNode* Simple_exp();

TreeNode* Term();

TreeNode* Factor();

// 处理标识符
TreeNode* Variable();

void VariMore(TreeNode* t);

TreeNode* FieldVar();

void FieldVarMore(TreeNode* t);

// 检查LexType与预期匹配则读入token
void Match(scanner::LexType expected);

void ReadToken();

void PrintTreeNode(const TreeNode* node, int depth);

void Input(TreeNode* root, const std::string& path);

} // namespace parse
#endif // PARSE_H
