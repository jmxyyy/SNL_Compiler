#include <fstream>

#include "../include/config.h"
#include "../include/parse.h"
#include "../include/scanner.h"

#include <iostream>
#include <sstream>

namespace parse {

std::string temp_name;
scanner::Token token;         // 存储当前token
std::ifstream in(TOKEN_PATH); // 读取token.txt

TreeNode* Parse() {
  ReadToken();
  TreeNode* t = Program(); // 语法分析
  if (in.eof()) {          // 解析完成
    Input(t, AST_PATH);    // 写入语法树
  } else {
    const std::string a = "file END ahead";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* Program() {
  auto* root = new TreeNode();
  root->nodeKind = ProK;
  root->lineno = token.lineShow;
  root->child[0] = ProgramHead(); // 语法分析程序头
  TreeNode* q = DeclarePart();    // 语法分析声明部分
  root->child[2] = ProgramBody(); // 语法分析程序体
  if (q == nullptr) {
    const std::string a = "no declare";
    scanner::InputError(a, ERROR_PATH);
  } else {
    root->child[1] = q;
  }
  Match(scanner::DOT);
  return root;
}

TreeNode* ProgramHead() {
  auto* t = new TreeNode();
  t->nodeKind = PheadK;
  t->lineno = token.lineShow;
  Match(scanner::PROGRAM);
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
  }
  Match(scanner::ID);
  return t;
}

TreeNode* DeclarePart() {
  // 声明部分可能有三种情况：类型声明，变量声明，过程声明
  // sibling的作用是将这三种情况串联起来，形成一个链表
  // 这样就可以在语法树中以兄弟节点的形式存储，而不是以子节点的形式存储
  // 三者之间的关系是并列的
  TreeNode *typeP = nullptr, *varP = nullptr;
  int line = token.lineShow;
  if (TreeNode* tp1 = TypeDec(); tp1 != nullptr) {
    typeP = new TreeNode();
    typeP->nodeKind = TypeK;
    typeP->child[0] = tp1;
    typeP->lineno = line;
  }
  line = token.lineShow;
  if (TreeNode* tp2 = VarDec(); tp2 != nullptr) {
    varP = new TreeNode();
    varP->nodeKind = VarK;
    varP->child[0] = tp2;
    varP->lineno = line;
  }
  TreeNode* s = ProcDec();
  if (varP == nullptr) {
    if (typeP == nullptr) {
      typeP = s;
    } else {
      typeP->sibling = s;
    }
  } else {
    varP->sibling = s;
    if (typeP == nullptr) {
      typeP = varP;
    } else {
      typeP->sibling = varP;
    }
  }
  return typeP;
}

TreeNode* ProgramBody() {
  auto* t = new TreeNode();
  t->nodeKind = StmLK;
  t->lineno = token.lineShow;
  Match(scanner::BEGIN);
  t->child[0] = StmList();
  Match(scanner::END);
  return t;
}

TreeNode* TypeDec() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::TYPE) {
    t = TypeDeclaration();
  } else if (token.lex == scanner::VAR || token.lex == scanner::PROCEDURE ||
             token.lex == scanner::BEGIN) {
  } else {
    ReadToken();
    const std::string a = std::to_string(token.lineShow) + ": word " +
                          arrayLexTypeD[static_cast<int>(token.lex)] +
                          "wrong, skip";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* TypeDeclaration() {
  Match(scanner::TYPE);
  TreeNode* t = TypeDecList();
  return t;
}

TreeNode* TypeDecList() {
  auto* t = new TreeNode();
  t->nodeKind = DecK;
  t->lineno = token.lineShow;
  TypeId(t);
  Match(scanner::EQ);
  TypeDef(t);
  Match(scanner::SEMI);
  TreeNode* p = TypeDecMore();
  if (p != nullptr) {
    t->sibling = p;
  }
  return t;
}

TreeNode* TypeDecMore() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::ID) {
    t = TypeDecList();
  } else if (token.lex == scanner::VAR || token.lex == scanner::PROCEDURE ||
             token.lex == scanner::BEGIN) {
  } else {
    const std::string a = std::to_string(token.lineShow) + " line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

void TypeId(TreeNode* t) {
  if (token.lex == scanner::ID && t != nullptr) {
    t->idNum++;
    t->name.push_back(token.sem);
  }
  Match(scanner::ID);
}

void TypeDef(TreeNode* t) {
  if (t != nullptr) {
    if (token.lex == scanner::INTEGER || token.lex == scanner::CHAR) {
      BaseType(t);
    } else if (token.lex == scanner::ARRAY || token.lex == scanner::RECORD) {
      StructureType(t);
    } else if (token.lex == scanner::ID) {
      t->kind.dec = IdK;
      t->idNum++;
      t->name.push_back(token.sem);
      t->type_name = token.sem;
      Match(scanner::ID);
    } else {
      std::string a = std::to_string(token.lineShow) + "line error";
      scanner::InputError(a, ERROR_PATH);
      exit(0);
    }
  }
}

void BaseType(TreeNode* t) {
  if (token.lex == scanner::INTEGER) {
    Match(scanner::INTEGER);
    t->kind.dec = IntegerK;
  } else if (token.lex == scanner::CHAR) {
    Match(scanner::CHAR);
    t->kind.dec = CharK;
  }
}

void StructureType(TreeNode* t) {
  if (token.lex == scanner::ARRAY) {
    ArrayType(t);
  } else if (token.lex == scanner::RECORD) {
    t->kind.dec = RecordK;
    RecType(t);
  }
}

void ArrayType(TreeNode* t) {
  Match(scanner::ARRAY);
  Match(scanner::LMIDPAREN);
  if (token.lex == scanner::INTC) {
    t->attr.arrayAttr.low = stoi(token.sem);
  }
  Match(scanner::INTC);
  Match(scanner::UNDERANGE);
  if (token.lex == scanner::INTC) {
    t->attr.arrayAttr.up = stoi(token.sem);
  }
  Match(scanner::INTC);
  Match(scanner::RMIDPAREN);
  Match(scanner::OF);
  BaseType(t);
  t->attr.arrayAttr.childType = t->kind.dec;
  t->kind.dec = ArrayK;
}

void RecType(TreeNode* t) {
  Match(scanner::RECORD);
  TreeNode* p = FieldDecList();
  t->child[0] = p;
  Match(scanner::END);
}

TreeNode* FieldDecList() {
  auto* t = new TreeNode();
  t->nodeKind = DecK;
  t->lineno = token.lineShow;
  TreeNode* p = nullptr;
  if (token.lex == scanner::INTEGER || token.lex == scanner::CHAR) {
    BaseType(t);
    IdList(t);
    Match(scanner::SEMI);
    p = FieldDecMore();
  } else if (token.lex == scanner::ARRAY) {
    ArrayType(t);
    IdList(t);
    Match(scanner::SEMI);
    p = FieldDecMore();
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  t->sibling = p;
  return t;
}

TreeNode* FieldDecMore() {
  TreeNode* t = nullptr;
  if (token.lex != scanner::END) {
    if (token.lex == scanner::INTEGER || token.lex == scanner::CHAR ||
        token.lex == scanner::ARRAY) {
      t = FieldDecList();
    } else {
      std::string a = std::to_string(token.lineShow) + "line error";
      scanner::InputError(a, ERROR_PATH);
      exit(0);
    }
  }
  return t;
}

void IdList(TreeNode* t) {
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
  }
  Match(scanner::ID);
  IdMore(t);
}

void IdMore(TreeNode* t) {
  if (token.lex == scanner::SEMI) {
  } else if (token.lex == scanner::COMMA) {
    Match(scanner::COMMA);
    IdList(t);
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

TreeNode* VarDec() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::PROCEDURE || token.lex == scanner::BEGIN) {
  } else if (token.lex == scanner::VAR) {
    t = VarDeclaration();
  } else {
    std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* VarDeclaration() {
  Match(scanner::VAR);
  TreeNode* t = VarDecList();
  return t;
}

TreeNode* VarDecList() {
  auto* t = new TreeNode();
  t->nodeKind = DecK;
  t->lineno = token.lineShow;
  TreeNode* p = nullptr;
  TypeDef(t);
  VarIdList(t);
  Match(scanner::SEMI);
  p = VarDecMore();
  t->sibling = p;
  return t;
}

TreeNode* VarDecMore() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::PROCEDURE || token.lex == scanner::BEGIN) {
  } else if (token.lex == scanner::INTEGER || token.lex == scanner::CHAR ||
             token.lex == scanner::ARRAY || token.lex == scanner::RECORD ||
             token.lex == scanner::ID) {
    t = VarDecList();
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

void VarIdList(TreeNode* t) {
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
    Match(scanner::ID);
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  VarIdMore(t);
}

void VarIdMore(TreeNode* t) {
  if (token.lex == scanner::SEMI) {
  } else if (token.lex == scanner::COMMA) {
    Match(scanner::COMMA);
    VarIdList(t);
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

TreeNode* ProcDec() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::BEGIN) {
  } else if (token.lex == scanner::PROCEDURE) {
    t = ProcDeclaration();
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* ProcDeclaration() {
  auto* t = new TreeNode();
  t->nodeKind = ProcDecK;
  t->lineno = token.lineShow;
  Match(scanner::PROCEDURE);
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
    Match(scanner::ID);
  }
  Match(scanner::LPAREN);
  ParamList(t);
  Match(scanner::RPAREN);
  Match(scanner::SEMI);
  t->child[1] = ProcDecPart();
  t->child[2] = ProcBody();
  t->sibling = ProcDecMore();

  return t;
}

TreeNode* ProcDecMore() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::BEGIN) {
    return nullptr;
  } else if (token.lex == scanner::PROCEDURE) {
    t = ProcDeclaration();
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

void ParamList(TreeNode* t) {
  if (token.lex == scanner::RPAREN) {
  } else if (token.lex == scanner::INTEGER || token.lex == scanner::CHAR ||
             token.lex == scanner::ARRAY || token.lex == scanner::RECORD ||
             token.lex == scanner::ID || token.lex == scanner::VAR) {
    TreeNode* p = nullptr;
    p = ParamDecList();
    t->child[0] = p;
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

TreeNode* ParamDecList() {
  TreeNode* t = Param();
  TreeNode* p = ParamMore();
  if (p != nullptr) {
    t->sibling = p;
  }
  return t;
}

TreeNode* ParamMore() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::RPAREN) {
  } else if (token.lex == scanner::SEMI) {
    Match(scanner::SEMI);
    t = ParamDecList();
    if (t == nullptr) {
      const std::string a = std::to_string(token.lineShow) + "line error";
      scanner::InputError(a, ERROR_PATH);
      exit(0);
    }
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* Param() {
  auto* t = new TreeNode();
  t->nodeKind = DecK;
  t->lineno = token.lineShow;
  if (token.lex == scanner::INTEGER || token.lex == scanner::CHAR ||
      token.lex == scanner::RECORD || token.lex == scanner::ARRAY ||
      token.lex == scanner::ID) {
    t->attr.procAttr.paramType = ValParamType;
    TypeDef(t);
    FormList(t);
  } else if (token.lex == scanner::VAR) {
    Match(scanner::VAR);
    t->attr.procAttr.paramType = VarParamType;
    TypeDef(t);
    FormList(t);
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

void FormList(TreeNode* t) {
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
    Match(scanner::ID);
  }
  FidMore(t);
}

void FidMore(TreeNode* t) {
  if (token.lex == scanner::SEMI || token.lex == scanner::RPAREN) {
  } else if (token.lex == scanner::COMMA) {
    Match(scanner::COMMA);
    FormList(t);
  } else {
    std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

TreeNode* ProcDecPart() { return DeclarePart(); }

TreeNode* ProcBody() { return ProgramBody(); }

TreeNode* StmList() {
  TreeNode* t = Stm();
  TreeNode* p = StmMore();
  t->sibling = p;
  return t;
}

TreeNode* StmMore() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::END || token.lex == scanner::ENDWH ||
      token.lex == scanner::ELSE || token.lex == scanner::FI) {
  } else if (token.lex == scanner::SEMI) {
    Match(scanner::SEMI);
    t = StmList();
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* Stm() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::IF) {
    t = ConditionalStm();
  } else if (token.lex == scanner::WHILE) {
    t = LoopStm();
  } else if (token.lex == scanner::RETURN) {
    t = ReturnStm();
  } else if (token.lex == scanner::READ) {
    t = InputStm();
  } else if (token.lex == scanner::WRITE) {
    t = OutputStm();
  } else if (token.lex == scanner::ID) {
    auto* f = new TreeNode();
    f->nodeKind = StmtK;
    f->lineno = token.lineShow;
    auto* t1 = new TreeNode();
    t1->nodeKind = ExpK;
    t1->lineno = token.lineShow;
    t1->kind.exp = IdEK;
    t1->attr.expAttr.varKind = IdV;
    t1->idNum++;
    t1->name.push_back(token.sem);
    f->child[0] = t1;
    temp_name = token.sem;
    AssCall(f);
    t = f;
  } else {
    const std::string a =
        std::to_string(token.lineShow) + "line error" + " maybe = left not var";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

void AssCall(TreeNode* t) {
  Match(scanner::ID);
  // ASSIGN: x := 1
  // scanner::LMIDPAREN: x[1] := 1
  // DOT: x.a := 1
  if (token.lex == scanner::ASSIGN || token.lex == scanner::LMIDPAREN ||
      token.lex == scanner::DOT) {
    AssignmentRest(t);
    t->kind.stmt = AssignK;
  } else if (token.lex == scanner::LPAREN) {
    CallStmRest(t);
    t->kind.stmt = CallK;
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

void AssignmentRest(TreeNode* t) {
  VariMore(t->child[0]);
  Match(scanner::ASSIGN);
  t->child[1] = Exp();
}

TreeNode* ConditionalStm() {
  auto* t = new TreeNode();
  t->nodeKind = StmtK;
  t->kind.stmt = IfK;
  t->lineno = token.lineShow;
  Match(scanner::IF);
  t->child[0] = Exp();
  Match(scanner::THEN);
  t->child[1] = StmList();
  if (token.lex == scanner::ELSE) {
    Match(scanner::ELSE);
    t->child[2] = StmList();
  }
  Match(scanner::FI);
  return t;
}

TreeNode* LoopStm() {
  auto* t = new TreeNode();
  t->nodeKind = StmtK;
  t->kind.stmt = WhileK;
  t->lineno = token.lineShow;
  Match(scanner::WHILE);
  t->child[0] = Exp();
  Match(scanner::DO);
  t->child[1] = StmList();
  Match(scanner::ENDWH);
  return t;
}

TreeNode* InputStm() {
  auto* t = new TreeNode();
  t->nodeKind = StmtK;
  t->kind.stmt = ReadK;
  t->lineno = token.lineShow;
  Match(scanner::READ);
  Match(scanner::LPAREN);
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
  }
  Match(scanner::ID);
  Match(scanner::RPAREN);
  return t;
}

TreeNode* OutputStm() {
  auto* t = new TreeNode();
  t->nodeKind = StmtK;
  t->kind.stmt = WriteK;
  t->lineno = token.lineShow;
  Match(scanner::WRITE);
  Match(scanner::LPAREN);
  t->child[0] = Exp();
  Match(scanner::RPAREN);
  return t;
}

TreeNode* ReturnStm() {
  auto* t = new TreeNode();
  t->nodeKind = StmtK;
  t->kind.stmt = ReturnK;
  t->lineno = token.lineShow;
  Match(scanner::RETURN);
  return t;
}

void CallStmRest(TreeNode* t) {
  Match(scanner::LPAREN);
  t->child[1] = ActParamList();
  Match(scanner::RPAREN);
}

TreeNode* ActParamList() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::RPAREN) {
  } else if (token.lex == scanner::ID || token.lex == scanner::INTC) {
    t = Exp();
    if (t != nullptr) {
      t->sibling = ActParamMore();
    }
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* ActParamMore() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::RPAREN) {
  } else if (token.lex == scanner::COMMA) {
    Match(scanner::COMMA);
    t = ActParamList();
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* Exp() {
  const int line = token.lineShow;
  TreeNode* t = Simple_exp();
  if (token.lex == scanner::LT || token.lex == scanner::EQ) {
    auto* p = new TreeNode();
    p->lineno = line;
    p->nodeKind = ExpK;
    p->kind.exp = OpK;
    p->child[0] = t;
    p->attr.expAttr.op = token.lex;
    t = p;
    Match(token.lex);
    // TODO
    // if (t != nullptr) {
    //   t->child[1] = Simple_exp();
    // }
    t->child[1] = Simple_exp();
  }
  return t;
}

TreeNode* Simple_exp() {
  int line = token.lineShow;
  TreeNode* t = Term();
  while (token.lex == scanner::PLUS || token.lex == scanner::MINUS) {
    auto* p = new TreeNode();
    p->lineno = line;
    p->nodeKind = ExpK;
    p->kind.exp = OpK;
    p->child[0] = t;
    p->attr.expAttr.op = token.lex;
    t = p;
    Match(token.lex);
    t->child[1] = Term();
  }
  return t;
}

TreeNode* Term() {
  int line = token.lineShow;
  TreeNode* t = Factor();
  while (token.lex == scanner::TIMES || token.lex == scanner::OVER) {
    auto* p = new TreeNode();
    p->lineno = line;
    p->nodeKind = ExpK;
    p->kind.exp = OpK;
    p->child[0] = t;
    p->attr.expAttr.op = token.lex;
    t = p;
    Match(token.lex);
    t->child[1] = Factor();
  }
  return t;
}

TreeNode* Factor() {
  TreeNode* t = nullptr;
  if (token.lex == scanner::INTC) {
    t = new TreeNode();
    t->lineno = token.lineShow;
    t->nodeKind = ExpK;
    t->kind.exp = ConstK;
    t->attr.expAttr.val = stoi(token.sem);
    Match(scanner::INTC);
  } else if (token.lex == scanner::ID) {
    t = Variable();
  } else if (token.lex == scanner::LPAREN) {
    Match(scanner::LPAREN);
    t = Exp();
    Match(scanner::RPAREN);
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
  return t;
}

TreeNode* Variable() {
  auto* t = new TreeNode();
  t->nodeKind = ExpK;
  t->kind.exp = IdEK;
  t->lineno = token.lineShow;
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
    Match(scanner::ID);
    VariMore(t);
  }
  return t;
}

void VariMore(TreeNode* t) {
  if (token.lex == scanner::ASSIGN || token.lex == scanner::TIMES ||
      token.lex == scanner::EQ || token.lex == scanner::LT ||
      token.lex == scanner::PLUS || token.lex == scanner::MINUS ||
      token.lex == scanner::OVER || token.lex == scanner::RPAREN ||
      token.lex == scanner::SEMI || token.lex == scanner::COMMA ||
      token.lex == scanner::THEN || token.lex == scanner::RMIDPAREN ||
      token.lex == scanner::ELSE || token.lex == scanner::FI ||
      token.lex == scanner::DO || token.lex == scanner::ENDWH ||
      token.lex == scanner::END) {
  } else if (token.lex == scanner::LMIDPAREN) {
    Match(scanner::LMIDPAREN);
    t->child[0] = Exp();
    t->attr.expAttr.varKind = ArrayMemV;
    t->child[0]->attr.expAttr.varKind = IdV;
    Match(scanner::RMIDPAREN);
  } else if (token.lex == scanner::DOT) {
    Match(scanner::DOT);
    t->child[0] = FieldVar();
    t->attr.expAttr.varKind = FieldMemV;
    t->child[0]->attr.expAttr.varKind = IdV;
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

TreeNode* FieldVar() {
  auto* t = new TreeNode();
  t->nodeKind = ExpK;
  t->kind.exp = IdEK;
  t->lineno = token.lineShow;
  if (token.lex == scanner::ID) {
    t->idNum++;
    t->name.push_back(token.sem);
    Match(scanner::ID);
    FieldVarMore(t);
  }
  return t;
}

void FieldVarMore(TreeNode* t) {
  if (token.lex == scanner::ASSIGN || token.lex == scanner::TIMES ||
      token.lex == scanner::EQ || token.lex == scanner::LT ||
      token.lex == scanner::PLUS || token.lex == scanner::MINUS ||
      token.lex == scanner::OVER || token.lex == scanner::RPAREN ||
      token.lex == scanner::SEMI || token.lex == scanner::COMMA ||
      token.lex == scanner::THEN || token.lex == scanner::RMIDPAREN ||
      token.lex == scanner::ELSE || token.lex == scanner::FI ||
      token.lex == scanner::DO || token.lex == scanner::ENDWH ||
      token.lex == scanner::END) {
  } else if (token.lex == scanner::LMIDPAREN) {
    Match(scanner::LMIDPAREN);
    t->child[0] = Exp();
    t->child[0]->attr.expAttr.varKind = ArrayMemV;
    Match(scanner::RMIDPAREN);
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

void Match(const scanner::LexType expected) {
  if (token.lex == expected) {
    ReadToken();
  } else {
    const std::string a = std::to_string(token.lineShow) + "line error";
    scanner::InputError(a, ERROR_PATH);
    exit(0);
  }
}

void ReadToken() {
  std::string line;
  getline(in, line);
  std::istringstream in_word(line);

  in_word >> token.lineShow;
  int lex;
  in_word >> lex;
  token.lex = static_cast<scanner::LexType>(lex);
  in_word >> token.sem;
}

} // namespace parse