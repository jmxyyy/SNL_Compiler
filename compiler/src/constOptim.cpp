//
// created by jmxyyy
//

#include "../include/midcode.h"
#include "../include/optim.h"

#include <iostream>

namespace optim {

bool FindConsT(const midcode::ArgRecord* x, ConstDefT** Entry) {
  ConstDefT* now = firstConstDefT;
  while (now != nullptr) {
    if (now->variable->Attr.Addr.dataLevel == x->Attr.Addr.dataLevel &&
        now->variable->Attr.Addr.dataOff == x->Attr.Addr.dataOff) {
      (*Entry) = now;
      return true;
    }
    now = now->next;
  }
  return false;
}

// 划分基本块
void DivBaseBlock() {
  midcode::MidcodeList* now = midcode::firstDummy;
  BaseBlockType tmp;
  tmp.begin = tmp.end = nullptr;
  if (now != nullptr) {
    tmp.begin = now;
  }
  while (now != nullptr) {
    // 碰到跳转，函数，读，则结束
    if (now->Midcode->codeKind == midcode::ENDWHILE ||
        now->Midcode->codeKind == midcode::RETURNC ||
        now->Midcode->codeKind == midcode::JUMP ||
        now->Midcode->codeKind == midcode::JUMP0 ||
        now->Midcode->codeKind == midcode::CALL ||
        now->Midcode->codeKind == midcode::ENDPROC ||
        (now->next != nullptr &&
         now->next->Midcode->codeKind == midcode::LABEL) ||
        (now->next != nullptr &&
         now->next->Midcode->codeKind == midcode::WHILESTART) ||
        (now->next != nullptr &&
         now->next->Midcode->codeKind == midcode::PENTRY) ||
        (now->next != nullptr &&
         now->next->Midcode->codeKind == midcode::MENTRY)) {
      tmp.end = now;
      BaseBlock.push_back(tmp);
      tmp.begin = nullptr;
      tmp.end = nullptr;
      if (now->next != nullptr) {
        tmp.begin = now->next;
      }
    }

    if (now->next == nullptr) {
      // 还有基本块没结束
      if (tmp.begin != nullptr && tmp.end == nullptr) {
        tmp.end = now;
        BaseBlock.push_back(tmp);
      }
    }
    now = now->next;
  }
}

void AppendTable(midcode::ArgRecord* x, const int result) {
  // 表中已存在
  if (auto** Entry = new ConstDefT*; FindConsT(x, Entry)) {
    (*Entry)->constValue = result;
  } else {
    auto* newConstDefT = new ConstDefT;
    newConstDefT->constValue = result;
    newConstDefT->variable = x;
    newConstDefT->next = nullptr;
    newConstDefT->former = lastConstDefT;
    if (lastConstDefT != nullptr)
      lastConstDefT->next = newConstDefT;
    lastConstDefT = newConstDefT;
    if (firstConstDefT == nullptr)
      firstConstDefT = newConstDefT;
  }
}

void DelConst(const midcode::ArgRecord* x) {
  // 若找到则删除
  if (auto** Entry = new ConstDefT*; FindConsT(x, Entry)) {
    if ((*Entry) == firstConstDefT) {
      firstConstDefT = (*Entry)->next;
      firstConstDefT->former = nullptr;
      delete (*Entry);
    } else if ((*Entry) == lastConstDefT) {
      lastConstDefT = (*Entry)->former;
      lastConstDefT->next = nullptr;
      delete (*Entry);
    } else {
      (*Entry)->former->next = (*Entry)->next;
      (*Entry)->next->former = (*Entry)->former;
      delete (*Entry);
    }
  }
}
void SubstiArg(const midcode::MidcodeList* x, const int i) {
  // 选择要处理的运算分量
  midcode::ArgRecord** y = nullptr;
  if (i == 1) {
    y = &x->Midcode->op1;
  } else if (i == 2) {
    y = &x->Midcode->op2;
  } else if (i == 3) {
    y = &x->Midcode->op3;
  }
  if ((*y)->form == midcode::AddrForm) {
    auto** Entry = new ConstDefT*;
    if (bool constflag = FindConsT((*y), Entry); constflag) {
      const int value = (*Entry)->constValue;
      if (i == 1)
        x->Midcode->op1 = midcode::ARGValue(value); // ��op1,op2�ĳ�ָ��
      if (i == 2)
        x->Midcode->op2 = midcode::ARGValue(value);
      if (i == 3)
        x->Midcode->op3 = midcode::ARGValue(value);
    }
  }
}
bool ArithC(const midcode::MidcodeList* x) {
  bool Delflag = false;
  int result = 0;
  SubstiArg(x, 1);
  SubstiArg(x, 2);
  if (x->Midcode->op2->form == midcode::ValueForm &&
      x->Midcode->op1->form == midcode::ValueForm) {
    if (x->Midcode->codeKind == midcode::ADD)
      result = x->Midcode->op1->Attr.value + x->Midcode->op2->Attr.value;
    else if (x->Midcode->codeKind == midcode::SUB)
      result = x->Midcode->op1->Attr.value - x->Midcode->op2->Attr.value;
    else if (x->Midcode->codeKind == midcode::MULT)
      result = x->Midcode->op1->Attr.value * x->Midcode->op2->Attr.value;
    else if (x->Midcode->codeKind == midcode::DIV)
      result = x->Midcode->op1->Attr.value / x->Midcode->op2->Attr.value;
    else if (x->Midcode->codeKind == midcode::LTC)
      result = x->Midcode->op1->Attr.value < x->Midcode->op2->Attr.value;
    else if (x->Midcode->codeKind == midcode::EQC)
      result = (x->Midcode->op1->Attr.value == x->Midcode->op2->Attr.value);
    AppendTable(x->Midcode->op3, result);
    Delflag = true;
  }
  return Delflag;
}

void OptiBlock(int i) {
  const midcode::MidcodeList* now = BaseBlock[i].begin;
  while (true) {
    if (now->Midcode->codeKind == midcode::ADD ||
        now->Midcode->codeKind == midcode::SUB ||
        now->Midcode->codeKind == midcode::MULT ||
        now->Midcode->codeKind == midcode::DIV ||
        now->Midcode->codeKind == midcode::LTC ||
        now->Midcode->codeKind == midcode::EQC) {
      if (ArithC(now)) {
        // 删除中间代码
        if (now->pre != nullptr) {
          now->pre->next = now->next;
        } else {
          midcode::firstDummy = midcode::firstDummy->next;
          midcode::firstDummy->pre = nullptr;
        }
        if (now->next != nullptr) {
          now->next->pre = now->pre;
        } else {
          midcode::lastDummy = midcode::lastDummy->pre;
          midcode::lastDummy->next = nullptr;
        }
      }
    } else if (now->Midcode->codeKind == midcode::ASSIG) {
      SubstiArg(now, 2);
      if (now->Midcode->op2->form == midcode::ValueForm) {
        AppendTable(now->Midcode->op1, now->Midcode->op2->Attr.value);
      } else {
        DelConst(now->Midcode->op1);
      }
    } else if (now->Midcode->codeKind == midcode::JUMP0 ||
               now->Midcode->codeKind == midcode::WRITEC) {
      SubstiArg(now, 1);
    } else if (now->Midcode->codeKind == midcode::AADD) {
      SubstiArg(now, 2);
    }
    if (now == BaseBlock[i].end)
      break;
    now = now->next;
  }
}

midcode::MidcodeList* ConstOptimize() {
  DivBaseBlock();
  firstConstDefT = nullptr;
  for (int i = 0; i < BaseBlock.size(); i++) {
    OptiBlock(i);
    firstConstDefT = nullptr;
    lastConstDefT = firstConstDefT;
  }
  return midcode::firstDummy;
}
void OutBaseBlock() {
  midcode::midNum = 0;
  for (int i = 0; i < BaseBlock.size(); i++) {
    printf("\n");
    printf("The %d base block begins\n", i);
    const midcode::MidcodeList* now = BaseBlock[i].begin;
    while (true) {
      std::cout << midcode::midNum << ": ";
      std::cout << codeKind_output[now->Midcode->codeKind] << "   ";
      if (now->Midcode->op1 == nullptr) {
        std::cout << "nullptr" << "   ";
      } else {
        if (now->Midcode->op1->form == midcode::ValueForm) {
          std::cout << now->Midcode->op1->Attr.value << "   ";
        } else if (now->Midcode->op1->form == midcode::LabelForm) {
          std::cout << "LABEL" << now->Midcode->op1->Attr.label << "     ";
        } else {
          std::cout << now->Midcode->op1->Attr.Addr.name << "     ";
        }
      }
      if (now->Midcode->op2 == nullptr) {
        std::cout << "nullptr" << "   ";
      } else {
        if (now->Midcode->op2->form == midcode::ValueForm) {
          std::cout << now->Midcode->op2->Attr.value << "   ";
        } else if (now->Midcode->op2->form == midcode::LabelForm) {
          std::cout << "LABEL" << now->Midcode->op2->Attr.label << "     ";
        } else {
          std::cout << now->Midcode->op2->Attr.Addr.name << "     ";
        }
      }
      if (now->Midcode->op3 == nullptr) {
        std::cout << "nullptr" << "   ";
      } else {
        if (now->Midcode->op3->form == midcode::ValueForm) {
          std::cout << now->Midcode->op3->Attr.value << "   ";
        } else if (now->Midcode->op3->form == midcode::LabelForm) {
          std::cout << "LABEL" << now->Midcode->op3->Attr.label << "     ";
        } else {
          std::cout << now->Midcode->op3->Attr.Addr.name << "     ";
        }
      }
      printf("\n");
      midcode::midNum++;
      if (now == BaseBlock[i].end)
        break;
      now = now->next;
    }

    printf("The %d base block end\n", i);
    printf("\n");
  }
}

} // namespace optim