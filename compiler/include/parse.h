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

struct TreeNode;

void PrintTreeNode(const TreeNode* node, int depth);

enum NodeKindEnum {
  ProK,
  PheadK,
  TypeK,
  VarK,
  ProcDecK,
  StmLK,
  DecK,
  StmtK,
  ExpK
};

enum DeckEnum { // �����ڵ�����ö��
  ArrayK,       // ��������
  CharK,        // �ַ�����
  IntegerK,     // ��������
  RecordK,      // ��¼����
  IdK           // �����ͱ�־����Ϊ����
};

enum StmtKEnum { // ���ڵ�����ö��
  IfK,           // �ж��������
  WhileK,        // ѭ���������
  AssignK,       // ��ֵ�������
  ReadK,         // ���������
  WriteK,        // д�������
  CallK,         // ���������������
  ReturnK        // �����������
};

enum ExpKEnum { //	���ʽ�ڵ�����ö��
  OpK,          //	����������
  ConstK,       // ����������
  IdEK          // ��ʶ������
};

enum ParamtEnum { // ������ֵ�λ��Ǳ��
  Valparamtype,
  Varparamtype
};

enum VarKindEnum { // ���������,�ֱ��ʾ�����Ǳ�־�������������Ա�����������Ա����
  IdV,
  ArrayMembV,
  FieldMembV
};

enum ExpType { Void, Integer, Boolean };

union Kind {
  DeckEnum
      dec; // ��¼�﷨���ڵ����������,��nodekind=DecKʱ��Ч
  StmtKEnum
      stmt; // ��¼�﷨���ڵ���������,��nodekind=StmtKʱ��Ч
  ExpKEnum
      exp; // ��¼�﷨���ڵ�ı��ʽ����,��nodekind=ExpKʱ��Ч
};

struct ArrayAttr { // ��¼�������͵����ԡ�
  int low;
  int up;
  DeckEnum childType;
};

struct ProcAttr { // ��¼���̵����ԡ�
  ParamtEnum paramt;
};

struct ExpAttr { // ��¼���ʽ�����ԡ�
  scanner::LexType
      op; // ��¼�﷨���ڵ����������ʣ�Ϊ��������
  int val;             // ��¼�﷨���ڵ����ֵ,
  VarKindEnum varkind; // ��¼���������
  ExpType type; // ��¼�﷨���ڵ�ļ�����ͣ�ȡֵVoid,
                // Integer,Boolean,Ϊ���ͼ��ExpType����
};

struct Attr { // ��¼�﷨���ڵ���������
  ArrayAttr arrayAttr;
  ProcAttr procAttr;
  ExpAttr expAttr;
};

struct symbtable;
struct typeIR;

enum IdKind { typeKind, varKind, procKind };

enum AccessKind { dir, indir };

struct ParamTable {
  symbtable* entry = nullptr;
  ParamTable* next = nullptr;
};

typedef struct {
  typeIR* idtype;
  IdKind kind;
  union {
    struct {
      AccessKind access;
      int level;
      int off;
    } VarAttr;
    struct {
      int level;
      ParamTable* param;
      int code;
      int size;
    } ProcAttr;
  } More;
} AttributeIR;

struct symbtable {
  std::string idname;
  AttributeIR attrIR;
  symbtable* next = nullptr;
};

struct typeIR;

enum TypeKind {
  intTy,
  charTy,
  arrayTy,
  recordTy,
  boolTy
}; // intTy, charTy, arrayTy, recordTy, boolTy

struct fieldChain {
  std::string idname;
  typeIR* unitType;
  int offset;
  fieldChain* next;
};

struct typeIR {
  int size;
  TypeKind kind; // intTy, charTy, arrayTy, recordTy, boolTy
  union {
    struct {
      typeIR* indexTy;
      typeIR* elemTy;
      int low;
      int up;
    } ArrayAttr;
    fieldChain* body;
  } More;
};

typeIR* TypeProcess(parse::TreeNode* t);

void statement(parse::TreeNode* t);

void bianli(parse::TreeNode* t);

void Body(parse::TreeNode* t);

void Analyze(parse::TreeNode* currentP);
typeIR* recordVar(parse::TreeNode* t);
typeIR* arrayVar(parse::TreeNode* t);

struct TreeNode {
  TreeNode* child[3];
  TreeNode* sibling;
  int lineno;
  NodeKindEnum nodekind;
  Kind kind;
  int idnum = 0;
  std::vector<std::string> name;
  std::vector<symbtable*> table;
  std::string type_name;
  Attr attr;

  TreeNode() = default;
  ~TreeNode() = default;
};

void Input(TreeNode* root, const std::string& path);

TreeNode* DesParse();

TreeNode* Program();

TreeNode* ProgramHead();

TreeNode* DeclarePart();

TreeNode* TypeDec();

TreeNode* TypeDeclaration();

TreeNode* TypeDecList();

TreeNode* TypeDecMore();

void TypeId(TreeNode* t);

void TypeDef(TreeNode* t);

void BaseType(TreeNode* t);

void StructureType(TreeNode* t);

void ArrayType(TreeNode* t);

void RecType(TreeNode* t);

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

TreeNode* ProcDec();

TreeNode* ProcDeclaration();

TreeNode* ProcDecMore();

void ParamList(TreeNode* t);

TreeNode* ParamDecList();

TreeNode* ParamMore();

TreeNode* Param();

void FormList(TreeNode* t);

void FidMore(TreeNode* t);

TreeNode* ProcDecPart();

TreeNode* ProcBody();

TreeNode* ProgramBody();

TreeNode* StmList();

TreeNode* StmMore();

TreeNode* Stm();

void AssCall(TreeNode* t);

void AssignmentRest(TreeNode* t);

TreeNode* ConditionalStm();

TreeNode* LoopStm();

TreeNode* InputStm();

TreeNode* OutputStm();

TreeNode* ReturnStm();

void CallStmRest(TreeNode* t);

TreeNode* ActParamList();

TreeNode* ActParamMore();

TreeNode* Exp();

TreeNode* Simple_exp();

TreeNode* Term();

TreeNode* Factor();

TreeNode* Variable();

void VariMore(TreeNode* t);

TreeNode* FieldVar();

void FieldVarMore(TreeNode* t);

void Match(scanner::LexType expexted);

void ReadToken();

} // namespace parse
#endif // PARSE_H
