#include "../include/midcode.h"
// #include "../include/parse.h"

#include <iostream>
#include <unordered_map>

namespace midcode {

std::unordered_map<int, std::string> codeKind_output = {
    {ADD, "ADD"},           {SUB, "SUB"},
    {MULT, "MULT"},         {DIV, "DIV"},
    {EQC, "EQC"},           {LTC, "LTC"},
    {READC, "READC"},       {WRITEC, "WRITEC"},
    {RETURNC, "RETURNC"},   {ASSIG, "ASSIG"},
    {AADD, "AADD"},         {LABEL, "LABEL"},
    {JUMP, "JUMP"},         {JUMP0, "JUMP0"},
    {CALL, "CALL"},         {VARACT, "VARACT"},
    {VALACT, "VALACT"},     {PENTRY, "PENTRY"},
    {ENDPROC, "ENDPROC"},   {MENTRY, "MENTRY"},
    {ENDWHILE, "ENDWHILE"}, {WHILESTART, "WHILESTART"},
};

// 临时变量先存储着，再处理
ArgRecord* NewTemp(parse::AccessKind access) {
  auto* tmp_arg = new ArgRecord{
      AddrForm,
      {-1,
       -1,
       {("tmp" + std::to_string(tmp_var_order)), -1, tmp_var_order++, access}}};
  return tmp_arg;
}

ArgRecord* ARGValue(int value) {
  auto* tmp_arg = new ArgRecord{ValueForm, {value, -1, {}}};
  return tmp_arg;
}

int NewLabel() { return ++globalLabel; }

ArgRecord* ARGLabel(int label) {
  auto* tmp_arg = new ArgRecord{LabelForm, {-1, label, {}}};
  return tmp_arg;
}

ArgRecord* ARGAddr(const std::string& id, const int level, const int off1,
                   const parse::AccessKind access) {
  auto* tmp_arg = new ArgRecord{AddrForm, {-1, -1, {id, level, off1, access}}};
  return tmp_arg;
}

void PrintMidCode(const MidcodeList* dummy) {
  midNum = 0;
  while (dummy) {
    std::cout << midNum << ": ";
    std::cout << codeKind_output[dummy->Midcode->codeKind] << "   ";
    if (dummy->Midcode->op1 == nullptr) {
      std::cout << "NULL" << "   ";
    } else {
      if (dummy->Midcode->op1->form == ValueForm) {
        std::cout << dummy->Midcode->op1->Attr.value << "   ";
      } else if (dummy->Midcode->op1->form == LabelForm) {
        std::cout << "LABEL" << dummy->Midcode->op1->Attr.label << "     ";
      } else {
        std::cout << dummy->Midcode->op1->Attr.Addr.name << "     ";
      }
    }
    if (dummy->Midcode->op2 == nullptr) {
      std::cout << "NULL" << "   ";
    } else {
      if (dummy->Midcode->op2->form == ValueForm) {
        std::cout << dummy->Midcode->op2->Attr.value << "   ";
      } else if (dummy->Midcode->op2->form == LabelForm) {
        std::cout << "LABEL" << dummy->Midcode->op2->Attr.label << "     ";
      } else {
        std::cout << dummy->Midcode->op2->Attr.Addr.name << "     ";
      }
    }
    if (dummy->Midcode->op3 == nullptr) {
      std::cout << "NULL" << "   ";
    } else {
      if (dummy->Midcode->op3->form == ValueForm) {
        std::cout << dummy->Midcode->op3->Attr.value << "   ";
      } else if (dummy->Midcode->op3->form == LabelForm) {
        std::cout << "LABEL" << dummy->Midcode->op3->Attr.label << "     ";
      } else {
        std::cout << dummy->Midcode->op3->Attr.Addr.name << "     ";
      }
    }
    dummy = dummy->next;
    std::cout << std::endl;
    midNum++;
  }
};

MidcodeList* GenCode(const codeKindType codeKind, ArgRecord* first,
                     ArgRecord* second, ArgRecord* third) {

  auto* content = new MidcodeStruct{codeKind, first, second, third};
  MidcodeList* result;
  if (firstDummy == nullptr) {
    auto* genMid = new MidcodeList{nullptr, content, nullptr};
    firstDummy = genMid;
    lastDummy = genMid;
    result = genMid;
  } else {
    auto* genMid = new MidcodeList{lastDummy, content, nullptr};
    lastDummy->next = genMid;
    lastDummy = genMid;
    result = genMid;
  }
  return result;
}

MidcodeList* GenMidCode(const parse::TreeNode* t) {
  const parse::TreeNode* son1 = t->child[1];
  while (son1) {
    if (son1->nodeKind == parse::ProcDecK) {
      GenProDec(son1);
    }
    son1 = son1->sibling;
  }
  ArgRecord* ARG3 = ARGValue(off);
  ArgRecord* ARG2 = ARGValue(0); // 回填

  // ARG2过程活动记录大小，ARG3是display表的偏移量
  // PENTRY则是标号，过活大小，层数
  const MidcodeList* main = GenCode(MENTRY, nullptr, ARG2, ARG3);

  tmp_var_order = off + 1; // 初始化临时变量编号

  GenBody(t->child[2]);
  main->Midcode->op2->Attr.value = tmp_var_order; // 回填完
  return firstDummy;
}

void GenProDec(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  const int ProcEny = NewLabel();
  parse::symbolTable* Entry = t->table[0];
  Entry->attrIR.More.ProcAttr.code = ProcEny;
  const int level = Entry->attrIR.More.ProcAttr.level;
  const int off_tmp = Entry->attrIR.More.ProcAttr.size;
  auto* ArgOff = new ArgRecord{LabelForm, {-1, ProcEny, {}}};
  auto* ArgLevel = new ArgRecord{ValueForm, {level, -1, {}}};

  parse::TreeNode* allDeclare = t->child[1];
  const parse::TreeNode* DeclareAgain = allDeclare;
  while (DeclareAgain) {
    if (DeclareAgain->nodeKind == parse::ProcDecK) {
      GenProDec(DeclareAgain);
    }
    DeclareAgain = DeclareAgain->sibling;
  }

  // 主程序level为0，但display得存个自身
  // 分程序level为1,但display得存俩，以此类推
  tmp_var_order = off_tmp + level + 1;
  auto* ArgSizeToFill = new ArgRecord{ValueForm, {-1, -1, {}}};
  const MidcodeList* Pentry = GenCode(PENTRY, ArgOff, ArgSizeToFill, ArgLevel);
  GenBody(t->child[2]);
  Pentry->Midcode->op2->Attr.value = tmp_var_order;
  MidcodeList* End = GenCode(ENDPROC, nullptr, nullptr, nullptr);
}

void GenBody(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  parse::TreeNode* Statement = t->child[0];
  while (Statement) {
    GenStatement(Statement);
    Statement = Statement->sibling;
  }
}

void GenStatement(parse::TreeNode* t) {
  if (t == nullptr)
    return;
  switch (t->kind.stmt) {
  case parse::AssignK:
    GenAssignS(t);
    break;
  case parse::CallK:
    GenCallS(t);
    break;
  case parse::ReadK:
    GenReadS(t);
    break;
  case parse::WriteK:
    GenWriteS(t);
    break;
  case parse::IfK:
    GenIfS(t);
    break;
  case parse::WhileK:
    GenWhileS(t);
    break;
  case parse::ReturnK:
    GenCode(RETURNC, nullptr, nullptr, nullptr);
    break;
  default:
    std::cout << "Without such statements, the syntax tree is wrong and "
                 "intermediate code generation fails";
    break;
  }
}

void GenAssignS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  ArgRecord* LArg = GenVar(t->child[0]);
  ArgRecord* RArg = GenExpr(t->child[1]);
  GenCode(ASSIG, LArg, RArg, nullptr);
}

ArgRecord* GenVar(const parse::TreeNode* t) {
  // 敢直接取第0位嘛，不敢
  // 特殊判断这个变量是不是自定义类型,其实我可以判断它name有几个
  if (t == nullptr)
    return nullptr;
  const parse::symbolTable* Entry = t->table[0];
  const parse::AccessKind access = Entry->attrIR.More.VarAttr.access;
  const int level = Entry->attrIR.More.VarAttr.level;
  const int off1 = Entry->attrIR.More.VarAttr.off;
  ArgRecord* V1arg =
      ARGAddr(t->name[0], level, off1, access); // level off1 access
  ArgRecord* Varg;                              // result
  switch (t->attr.expAttr.varKind) {
  case parse::IdV: {
    Varg = V1arg;
    break;
  }
  case parse::ArrayMemV: {
    const int low = Entry->attrIR.idtype->More.ArrayAttr.low;

    const int size = Entry->attrIR.idtype->More.ArrayAttr.elemTy->size;
    Varg = GenArray(V1arg, t, low, size);
    break;
  }
  case parse::FieldMemV: {
    Varg = GenField(V1arg, t, Entry->attrIR.idtype->More.body);
    break;
  }
  default:
    break;
  }
  return Varg;
}

ArgRecord* GenArray(ArgRecord* V1arg, const parse::TreeNode* t, const int low,
                    const int size) {
  if (t == nullptr)
    return nullptr;
  ArgRecord* Earg = GenExpr(t->child[0]);
  ArgRecord* lowArg = ARGValue(low);
  ArgRecord* sizeArg = ARGValue(size);
  ArgRecord* tmp1 = NewTemp(parse::dir);
  ArgRecord* tmp2 = NewTemp(parse::dir);
  ArgRecord* tmp3 = NewTemp(parse::indir);
  GenCode(SUB, Earg, lowArg, tmp1);
  GenCode(MULT, tmp1, sizeArg, tmp2);
  GenCode(AADD, V1arg, tmp2, tmp3);
  return tmp3;
}

ArgRecord* GenField(ArgRecord* V1arg, const parse::TreeNode* t,
                    const parse::fieldChain* head) {
  if (t == nullptr)
    return nullptr;
  auto** Entry1 = new parse::fieldChain*;
  const int off1 = (*Entry1)->offset;
  ArgRecord* offArg = ARGValue(off1);
  ArgRecord* tmp1 = NewTemp(parse::indir);
  GenCode(AADD, V1arg, offArg, tmp1);
  ArgRecord* FieldV;
  if (head->unitType->kind == parse::arrayTy) {
    const int low = t->child[0]->attr.arrayAttr.low;
    const int size = head->unitType->size;
    FieldV = GenArray(tmp1, t->child[0], low, size);
  } else {
    FieldV = tmp1;
  }
  return FieldV;
}

ArgRecord* GenExpr(const parse::TreeNode* t) {
  if (t == nullptr)
    return nullptr;
  ArgRecord* arg;
  switch (t->kind.exp) {
  case parse::IdEK: {
    arg = GenVar(t);
    break;
  }
  case parse::ConstK: {
    arg = ARGValue(t->attr.expAttr.val);
    break;
  }
  case parse::OpK: {
    ArgRecord* LArg = GenExpr(t->child[0]);
    codeKindType OP;
    switch (t->attr.expAttr.op) {
    case scanner::PLUS: {
      OP = ADD;
      break;
    }
    case scanner::MINUS: {
      OP = SUB;
      break;
    }
    case scanner::TIMES: {
      OP = MULT;
      break;
    }
    case scanner::OVER: {
      OP = DIV;
      break;
    }
    case scanner::ASSIGN: {
      OP = ASSIG;
      break;
    }
    case scanner::LT: {
      OP = LTC;
      break;
    }
    case scanner::READ: {
      OP = READC;
      break;
    }
    case scanner::WRITE: {
      OP = WRITEC;
      break;
    } // TODO
    case scanner::EQ: {
      OP = EQC;
      break;
    }
    default: {
      break;
    }
    }
    ArgRecord* RArg = GenExpr(t->child[1]);
    ArgRecord* tmp1 = NewTemp(parse::dir);
    GenCode(OP, LArg, RArg, tmp1);
    arg = tmp1;
  }
  default:
    break;
  }
  return arg;
}

void GenCallS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  const parse::symbolTable* Entry = t->child[0]->table[0];
  const parse::ParamTable* head = Entry->attrIR.More.ProcAttr.param;
  // 找到形参
  const parse::TreeNode* head1 = t->child[1];
  while (head1) {
    const int temp13 = head->entry->attrIR.More.VarAttr.off;
    ArgRecord* op23 = ARGValue(temp13);
    ArgRecord* EArg = GenExpr(head1); // 处理实际参数
    if (head->entry->attrIR.More.VarAttr.access == parse::dir) {
      GenCode(VALACT, EArg, op23, nullptr);
    } else {
      GenCode(VARACT, EArg, op23, nullptr);
    }
    head1 = head1->sibling;
    head = head->next;
  }
  ArgRecord* labelArg = ARGLabel(Entry->attrIR.More.ProcAttr.code);
  ArgRecord* arg_off = ARGValue(Entry->attrIR.More.ProcAttr.size);
  GenCode(CALL, labelArg, nullptr, arg_off);
}

void GenReadS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  parse::symbolTable* Entry = t->table[0];
  const int level = Entry->attrIR.More.VarAttr.level;
  const int off1 = Entry->attrIR.More.VarAttr.off;
  const parse::AccessKind access = Entry->attrIR.More.VarAttr.access;
  ArgRecord* EArg = ARGAddr(t->name[0], level, off1, access);
  GenCode(READC, EArg, nullptr, nullptr);
}

void GenIfS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  const int else_label = NewLabel();
  const int if_label = NewLabel();
  ArgRecord* ElseLArg = ARGLabel(else_label);
  ArgRecord* OutLArg = ARGLabel(if_label);
  ArgRecord* EArg = GenExpr(t->child[0]);
  GenCode(JUMP0, EArg, ElseLArg, nullptr);
  parse::TreeNode* Statement = t->child[1];
  while (Statement) {
    GenStatement(Statement);
    Statement = Statement->sibling;
  }
  GenCode(JUMP, OutLArg, nullptr, nullptr);
  GenCode(LABEL, ElseLArg, nullptr, nullptr);
  if (t->child[2] != nullptr) {
    Statement = t->child[2];
    while (Statement) {
      GenStatement(Statement);
      Statement = Statement->sibling;
    }
  }
  GenCode(LABEL, OutLArg, nullptr, nullptr);
}

void GenWriteS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  ArgRecord* EArg = GenExpr(t->child[0]);
  GenCode(WRITEC, EArg, nullptr, nullptr);
}

void GenWhileS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  const int Label_in = NewLabel();
  const int Label_out = NewLabel();
  ArgRecord* InLArg = ARGLabel(Label_in);
  ArgRecord* OutLArg = ARGLabel(Label_out);
  GenCode(WHILESTART, InLArg, nullptr, nullptr); // WHILESTART
  ArgRecord* EArg = GenExpr(t->child[0]);
  GenCode(JUMP0, EArg, OutLArg, nullptr);
  parse::TreeNode* tmp = t->child[1];
  while (tmp) {
    GenStatement(tmp);
    tmp = tmp->sibling;
  }
  GenCode(JUMP, InLArg, nullptr, nullptr);
  GenCode(ENDWHILE, OutLArg, nullptr, nullptr); // ENDWHILE
}

} // namespace midcode