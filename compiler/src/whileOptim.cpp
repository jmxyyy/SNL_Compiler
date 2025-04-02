//
// Created by jmxyyy.
//

#include "../include/optim.h"

namespace optim {

int SearchTable(midcode::ArgRecord* arg, int head) {
  while (head < VarDefSet.size()) {
    // 临时变量与正常变量相同
    if (arg->Attr.Addr.dataLevel == VarDefSet[head]->Attr.Addr.dataLevel &&
        arg->Attr.Addr.dataOff == VarDefSet[head]->Attr.Addr.dataOff)
      return head;
    head++;
  }
  return -1;
}

void AddTable(midcode::ArgRecord* arg) {
  // 若变量在循环中
  int head = 0;
  if (!LoopInfoStack.empty()) {
    head = LoopInfoStack[LoopInfoStack.size() - 1].varDef;
  }
  // 若表中找不到
  if (SearchTable(arg, head) == -1) {
    VarDefSet.push_back(arg);
  }
}

void DelItem(midcode::ArgRecord* arg, const int head) {
  if (const int local = SearchTable(arg, head); local != -1) {
    // 挪位置
    for (int i = local + 1; i < VarDefSet.size(); i++) {
      VarDefSet[i - 1] = VarDefSet[i];
    }
    // 抛栈顶
    VarDefSet.pop_back();
  }
}

void LoopOutside(midcode::MidcodeList* code) {
  int level = 0;
  midcode::MidcodeList* now = code;
  now = now->next;
  const midcode::MidcodeList* end =
      LoopInfoStack[LoopInfoStack.size() - 1].whileEnd;
  midcode::MidcodeList* begin =
      LoopInfoStack[LoopInfoStack.size() - 1].whileEntry;
  while (now != end) {
    if (now->Midcode->codeKind == midcode::WHILESTART)
      level++;
    else if (now->Midcode->codeKind == midcode::ENDWHILE)
      level--;
    else if (now->Midcode->codeKind == midcode::AADD ||
             now->Midcode->codeKind == midcode::ADD ||
             now->Midcode->codeKind == midcode::SUB ||
             now->Midcode->codeKind == midcode::MULT) {
      const int head = LoopInfoStack[LoopInfoStack.size() - 1].varDef;
      const int loc1 = SearchTable(now->Midcode->op1, head);
      const int loc2 = SearchTable(now->Midcode->op2, head);
      // 两个操作分量均未定值
      if (loc1 == -1 && loc2 == -1) {
        // 从原位置删除
        if (now->pre != nullptr)
          now->pre->next = now->next;
        if (now->next != nullptr)
          now->next->pre = now->pre;
        // 插入循环开头的前面
        now->next = begin;
        now->pre = begin->pre;
        begin->pre = now;
        if (now->pre != nullptr)
          now->pre->next = now;
        // 从定值表中删除
        int head = LoopInfoStack[LoopInfoStack.size() - 1].varDef;
        DelItem(now->Midcode->op3, head);
      }
      // 结果分量加入定值
      else
        AddTable(now->Midcode->op3);
    }
    now = now->next;
  }
}

void call(midcode::MidcodeList* code) {
  int item = LoopInfoStack.size() - 1;
  while (item >= 0) {
    LoopInfoStack[item].state = 0;
    item--;
  }
}

void WhileEntry(midcode::MidcodeList* code) {
  LoopInfo now;
  now.state = 1;
  now.varDef = VarDefSet.size();
  now.whileEntry = code;
  LoopInfoStack.push_back(now);
}

void WhileEnd(midcode::MidcodeList* code) {
  int item = LoopInfoStack.size() - 1;
  if (LoopInfoStack[item].state == 1) {
    LoopInfoStack[item].whileEnd = code;
    midcode::MidcodeList* entry = LoopInfoStack[item].whileEntry;
    LoopOutside(entry);
  }
  LoopInfoStack.pop_back();
}

midcode::MidcodeList* LoopOpti() {
  VarDefSet.clear();
  midcode::MidcodeList* now = midcode::firstDummy;
  while (now != nullptr) {
    if (now->Midcode->codeKind == midcode::AADD ||
        now->Midcode->codeKind == midcode::ADD ||
        now->Midcode->codeKind == midcode::SUB ||
        now->Midcode->codeKind == midcode::MULT ||
        now->Midcode->codeKind == midcode::DIV ||
        now->Midcode->codeKind == midcode::LTC ||
        now->Midcode->codeKind == midcode::EQC) {
      AddTable(now->Midcode->op3);
    } else if (now->Midcode->codeKind == midcode::ASSIG) {
      AddTable(now->Midcode->op1);
    } else if (now->Midcode->codeKind == midcode::WHILESTART) {
      WhileEntry(now);
    } else if (now->Midcode->codeKind == midcode::ENDWHILE) {
      WhileEnd(now);
    } else if (now->Midcode->codeKind == midcode::CALL) {
      call(now);
    }
    now = now->next;
   }
   return midcode::firstDummy;
 }

} // namespace optim