#include "../include/midcode.h"
// #include "../include/parse.h"

#include <iostream>
#include <unordered_map>

namespace midcode {
std::unordered_map<int, std::string> codekind_output = {
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
ArgRecord* NewTemp(parse::AccessKind access) {
  auto* tmp_arg = new ArgRecord{
      AddrForm,
      {-1,
       -1,
       {("tmp" + std::to_string(tmp_var_order)), -1, tmp_var_order++, access}}};
  return tmp_arg;
} // ��ʱ�����ȴ洢�ţ��ٴ���

ArgRecord* ARGvalue(int value) {
  auto* tmp_arg = new ArgRecord{ValueForm, {value, -1, {}}};
  return tmp_arg;
} // ��������

int NewLabel() { return ++globalLabel; } // �ȷ���

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
  midnum = 0;
  while (dummy) {
    std::cout << midnum << ": ";
    std::cout << codekind_output[dummy->Midcode->codekind] << "   ";
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
    midnum++;
  }
};

MidcodeList* GenCode(codekindtype codekind, ArgRecord* first,
                            ArgRecord* second, ArgRecord* third) {

  auto* content =
      new MidcodeStruct{codekind, first, second, third};
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
  parse::TreeNode* son1 = t->child[1];
  while (son1) {
    if (son1->nodekind == parse::ProcDecK) {
      // parse::TreeNode* iterate = son1->child[0];
      // while (iterate != NULL) {
      GenProDec(son1);
      // iterate = iterate->sibling;
      //}
    }
    son1 = son1->sibling;
  }
  ArgRecord* ARG3 = ARGvalue(off);
  ArgRecord* ARG2 = ARGvalue(0); // ����?
  MidcodeList* main = GenCode(
      MENTRY, nullptr, ARG2,
      ARG3); // ARG2���̻��¼��С��ARG3��display����ƫ����
  // //PENTRY���Ǳ�ţ������С������

  tmp_var_order = off + 1; // ��ʼ����ʱ�������
  //
  Genbody(t->child[2]);
  main->Midcode->op2->Attr.value = tmp_var_order; // �����꿩
  return firstDummy;
}

void GenProDec(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  int ProcEny = NewLabel();
  // symbtable** Entry = new symbtable*;
  // bool flag = FindEntry(t->name[0], true,Entry);
  parse::symbtable* Entry = t->table[0];
  Entry->attrIR.More.ProcAttr.code = ProcEny;
  int level = Entry->attrIR.More.ProcAttr.level;
  int off_tmp = Entry->attrIR.More.ProcAttr.size;
  auto* ArgOff = new ArgRecord{LabelForm, {-1, ProcEny, {}}};
  auto* ArgLevel = new ArgRecord{ValueForm, {level, -1, {}}};

  parse::TreeNode* alldeclare = t->child[1];
  // �˴�4.19 23:13�¼����޸�
  // if (alldeclare == NULL) {
  //	guoChengDeclareAgain = NULL;
  // }
  // else {
  //	guoChengDeclareAgain = alldeclare->child[0];
  // }
  parse::TreeNode* guoChengDeclareAgain = alldeclare;
  while (guoChengDeclareAgain) {
    if (guoChengDeclareAgain->nodekind == parse::ProcDecK) {
      // parse::TreeNode* iterate = guoChengDeclareAgain->child[0];
      // while (iterate != NULL) {
      GenProDec(guoChengDeclareAgain);
      // iterate = iterate->sibling;
      //}
    }
    guoChengDeclareAgain = guoChengDeclareAgain->sibling;
  }
  ///////////////////////////////////////////////�Ҹ�����
  tmp_var_order = off_tmp + level + 1; // ������levelΪ0����display�ô��������
  // //�ֳ���levelΪ1,��display�ô������Դ�����
  auto* ArgSizeToFill = new ArgRecord{ValueForm, {-1, -1, {}}};
  MidcodeList* Pentry = GenCode(PENTRY, ArgOff, ArgSizeToFill, ArgLevel);
  Genbody(t->child[2]);
  Pentry->Midcode->op2->Attr.value = tmp_var_order;
  MidcodeList* End = GenCode(ENDPROC, nullptr, nullptr, nullptr);
}

void Genbody(const parse::TreeNode* t) {
  if (t == nullptr)
    return; // �޹�����
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
    // return;
    GenCode(RETURNC, nullptr, nullptr, nullptr);
    break;
  default:
    std::cout << "û��������䣬�﷨������,"
                 "�м��������ʧ��";
    break;
  }
  }

void GenAssignS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  ArgRecord* Larg = GenVar(t->child[0]);
  ArgRecord* Rarg = GenExpr(t->child[1]);
  GenCode(ASSIG, Larg, Rarg, nullptr);
}

ArgRecord* GenVar(const parse::TreeNode* t) {
  // symbtable** Entry = new symbtable*;
  // bool flag = FindEntry(t->name[0], true, Entry);
  // ��ֱ��ȡ��0λ����ң������ж���������ǲ����Զ�������,��ʵ�ҿ����ж���name�м���
  if (t == nullptr)
    return nullptr;
  parse::symbtable* Entry = t->table[0];
  parse::AccessKind access = Entry->attrIR.More.VarAttr.access;
  const int level = Entry->attrIR.More.VarAttr.level;
  const int off1 = Entry->attrIR.More.VarAttr.off;
  ArgRecord* V1arg =
      ARGAddr(t->name[0], level, off1, access); // level off1 access
  ArgRecord* Varg;                              // result
  switch (t->attr.expAttr.varkind) {
  case parse::IdV: {
    Varg = V1arg;
    break;
  }
  case parse::ArrayMembV: {
    const int low = Entry->attrIR.idtype->More.ArrayAttr.low;

    const int size = Entry->attrIR.idtype->More.ArrayAttr.elemTy->size;
    Varg = GenArray(V1arg, t, low, size);
    break;
  }
  case parse::FieldMembV: {
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
  ArgRecord* lowArg = ARGvalue(low);
  ArgRecord* sizeArg = ARGvalue(size);
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
  // bool flag1 = FindField(t->child[0]->name[0], head, Entry1);
  const int off1 = (*Entry1)->offset;
  ArgRecord* offArg = ARGvalue(off1);
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
    arg = ARGvalue(t->attr.expAttr.val);
    break;
  }
  case parse::OpK: {
    ArgRecord* Larg = GenExpr(t->child[0]);
    codekindtype OP;
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
    } // �ƺ���ȱ�㶫��
    case scanner::EQ: {
      OP = EQC;
      break;
    }
    default: {
      break;
    }
    }
    ArgRecord* Rarg = GenExpr(t->child[1]);
    ArgRecord* tmp1 = NewTemp(parse::dir);
    GenCode(OP, Larg, Rarg, tmp1);
    arg = tmp1;
  }
  default:
    break;
  }
  return arg;
}

void GenCallS(const parse::TreeNode* t) {
  // symbtable** Entry = new symbtable*;
  // bool flag = FindEntry(t->name[0], true, Entry);
  if (t == nullptr)
    return;
  const parse::symbtable* Entry = t->child[0]->table[0];
  const parse::ParamTable* head = Entry->attrIR.More.ProcAttr.param;
  // �ҵ��β�
  parse::TreeNode* head1 = t->child[1];
  while (head1) {
    const int temp13 = head->entry->attrIR.More.VarAttr.off;
    ArgRecord* op23 = ARGvalue(temp13);
    ArgRecord* Earg = GenExpr(head1); // ����ʵ�ʲ���
    if (head->entry->attrIR.More.VarAttr.access == parse::dir) {
      GenCode(VALACT, Earg, op23, nullptr);
    } else {
      GenCode(VARACT, Earg, op23, nullptr);
    }
    head1 = head1->sibling;
    head = head->next;
  }
  ArgRecord* labelarg = ARGLabel(Entry->attrIR.More.ProcAttr.code);
  ArgRecord* arg_off = ARGvalue(Entry->attrIR.More.ProcAttr.size);
  GenCode(CALL, labelarg, nullptr, arg_off);
}

void GenReadS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  // symbtable** Entry = new symbtable*;
  // bool flag = FindEntry(t->child[0]->name[0], true, Entry);
  parse::symbtable* Entry = t->table[0];
  int level = Entry->attrIR.More.VarAttr.level;
  int off1 = Entry->attrIR.More.VarAttr.off;
  parse::AccessKind access = Entry->attrIR.More.VarAttr.access;
  ArgRecord* Earg = ARGAddr(t->name[0], level, off1, access);
  GenCode(READC, Earg, nullptr, nullptr);
}

void GenIfS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  int else_label = NewLabel();
  int if_label = NewLabel();
  ArgRecord* ElseLarg = ARGLabel(else_label);
  ArgRecord* OutLarg = ARGLabel(if_label);
  ArgRecord* Earg = GenExpr(t->child[0]);
  GenCode(JUMP0, Earg, ElseLarg, nullptr);
  // Genbody(t->child[0]->sibling);
  parse::TreeNode* Statement = t->child[1];
  while (Statement) {
    GenStatement(Statement);
    Statement = Statement->sibling;
  }
  GenCode(JUMP, OutLarg, nullptr, nullptr);
  GenCode(LABEL, ElseLarg, nullptr, nullptr);
  // Genbody(t->child[0]->sibling->sibling);
  if (t->child[2] != nullptr) {
    Statement = t->child[2];
    while (Statement) {
      GenStatement(Statement);
      Statement = Statement->sibling;
    }
  }
  GenCode(LABEL, OutLarg, nullptr, nullptr);
}

void GenWriteS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  ArgRecord* Earg = GenExpr(t->child[0]);
  GenCode(WRITEC, Earg, nullptr, nullptr);
}

void GenWhileS(const parse::TreeNode* t) {
  if (t == nullptr)
    return;
  const int Label_in = NewLabel();
  const int Label_out = NewLabel();
  ArgRecord* InLarg = ARGLabel(Label_in);
  ArgRecord* OutLarg = ARGLabel(Label_out);
  GenCode(WHILESTART, InLarg, nullptr, nullptr); // WHILESTART
  ArgRecord* Earg = GenExpr(t->child[0]);
  GenCode(JUMP0, Earg, OutLarg, nullptr);
  parse::TreeNode* xunhuanti = t->child[1];
  while (xunhuanti) {
    GenStatement(xunhuanti);
    xunhuanti = xunhuanti->sibling;
  }

  GenCode(JUMP, InLarg, nullptr, nullptr);
  GenCode(ENDWHILE, OutLarg, nullptr, nullptr); // ENDWHILE
}

} // namespace midcode