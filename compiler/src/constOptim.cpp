//
// created by jmxyyy
//

#include "../include/optim.h"
#include "../include/midcode.h"

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

void DivBaseBlock() {
  midcode::MidcodeList* now = midcode::firstDummy;
  BaseBlocktype tmp;
  tmp.begin = tmp.end = nullptr;
  if (now != nullptr) {
    tmp.begin = now;
  }
  while (now != nullptr) {
    if (now->Midcode->codekind == midcode::ENDWHILE ||
        now->Midcode->codekind == midcode::RETURNC ||
        now->Midcode->codekind == midcode::JUMP ||
        now->Midcode->codekind == midcode::JUMP0 ||
        now->Midcode->codekind == midcode::CALL ||
        now->Midcode->codekind == midcode::ENDPROC ||
        (now->next != nullptr &&
         now->next->Midcode->codekind == midcode::LABEL) ||
        (now->next != nullptr &&
         now->next->Midcode->codekind == midcode::WHILESTART) ||
        (now->next != nullptr &&
         now->next->Midcode->codekind == midcode::PENTRY) ||
        (now->next != nullptr &&
         now->next->Midcode->codekind == midcode::MENTRY)) {
      tmp.end = now;
      BaseBlock.push_back(tmp);
      tmp.begin = nullptr;
      tmp.end = nullptr;
      if (now->next != nullptr) {
        tmp.begin = now->next;
        // now = now->next;
      }
    }

    if (now->next == nullptr) {
      if (tmp.begin != nullptr && tmp.end == nullptr) {
        tmp.end = now;
        BaseBlock.push_back(tmp);
      }
    }
    now = now->next;
  }
}
void AppendTable(midcode::ArgRecord* x, const int result) {
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
void SubstiArg(const midcode::MidcodeList* x, int i) {
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
        x->Midcode->op1 = midcode::ARGvalue(value); // ��op1,op2�ĳ�ָ��
      if (i == 2)
        x->Midcode->op2 = midcode::ARGvalue(value);
      if (i == 3)
        x->Midcode->op3 = midcode::ARGvalue(value);
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
    if (x->Midcode->codekind == midcode::ADD)
      result = x->Midcode->op1->Attr.value + x->Midcode->op2->Attr.value;
    else if (x->Midcode->codekind == midcode::SUB)
      result = x->Midcode->op1->Attr.value - x->Midcode->op2->Attr.value;
    else if (x->Midcode->codekind == midcode::MULT)
      result = x->Midcode->op1->Attr.value * x->Midcode->op2->Attr.value;
    else if (x->Midcode->codekind == midcode::DIV)
      result = x->Midcode->op1->Attr.value / x->Midcode->op2->Attr.value;
    else if (x->Midcode->codekind == midcode::LTC)
      result = x->Midcode->op1->Attr.value < x->Midcode->op2->Attr.value;
    else if (x->Midcode->codekind == midcode::EQC)
      result = (x->Midcode->op1->Attr.value == x->Midcode->op2->Attr.value);
    AppendTable(x->Midcode->op3, result);
    Delflag = true;
  }
  return Delflag;
}

void OptiBlock(int i) {
  midcode::MidcodeList* now = BaseBlock[i].begin;
  while (true) {
    if (now->Midcode->codekind == midcode::ADD ||
        now->Midcode->codekind == midcode::SUB ||
        now->Midcode->codekind == midcode::MULT ||
        now->Midcode->codekind == midcode::DIV ||
        now->Midcode->codekind == midcode::LTC ||
        now->Midcode->codekind == midcode::EQC) {
      if (ArithC(now)) {
        // ɾ���м����
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
    } else if (now->Midcode->codekind == midcode::ASSIG) {
      SubstiArg(now, 2);
      if (now->Midcode->op2->form == midcode::ValueForm) {
        AppendTable(now->Midcode->op1, now->Midcode->op2->Attr.value);
      } else {
        DelConst(now->Midcode->op1);
      }
    } else if (now->Midcode->codekind == midcode::JUMP0 ||
               now->Midcode->codekind == midcode::WRITEC) {
      SubstiArg(now, 1);
    } else if (now->Midcode->codekind == midcode::AADD) {
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
  midcode::midnum = 0;
  for (int i = 0; i < BaseBlock.size(); i++) {
    printf("\n");
    printf("--------��%d�������鿪ʼ-------\n", i);
    midcode::MidcodeList* now = BaseBlock[i].begin;
    while (true) {
      std::cout << midcode::midnum << ": ";
      std::cout << codekind_output[now->Midcode->codekind] << "   ";
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
      midcode::midnum++;
      if (now == BaseBlock[i].end)
        break;
      now = now->next;
    }

    printf("--------��%d�����������---------\n", i);
    printf("\n");
  }
}
} // namespace optim