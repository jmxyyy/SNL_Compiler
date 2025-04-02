//
// Created by jmxyyy.
//

 #include "../include/optim.h"

 namespace optim {
 int SearchTable(midcode::ArgRecord* arg, int head) {
   while (head < VarDefSet.size()) {
     if (arg->Attr.Addr.dataLevel == VarDefSet[head]->Attr.Addr.dataLevel &&
         arg->Attr.Addr.dataOff == VarDefSet[head]->Attr.Addr.dataOff)
       return head;
     head++;
   }
   return -1;
 }
 void AddTable(midcode::ArgRecord* arg) {
   int head = 0;
   if (!LoopInfostack.empty()) {
     head = LoopInfostack[LoopInfostack.size() - 1].varDef;
   }
   if (SearchTable(arg, head) == -1) {
     VarDefSet.push_back(arg);
   }
 }
 void DelItem(midcode::ArgRecord* arg, const int head) {
   int local = SearchTable(arg, head);
   if (local != -1) {
     for (int i = local + 1; i < VarDefSet.size(); i++) {
       VarDefSet[i - 1] = VarDefSet[i];
     }
     VarDefSet.pop_back();
   }
 }

 void LoopOutside(midcode::MidcodeList* code) {
   int level = 0;
   midcode::MidcodeList* now = code;
   now = now->next;
   midcode::MidcodeList* end = LoopInfostack[LoopInfostack.size() - 1].whileEnd;
   midcode::MidcodeList* begin =
       LoopInfostack[LoopInfostack.size() - 1].whileEntry;
   while (now != end) {
     if (now->Midcode->codekind == midcode::WHILESTART)
       level++;
     else if (now->Midcode->codekind == midcode::ENDWHILE)
       level--;
     else if (now->Midcode->codekind == midcode::AADD ||
              now->Midcode->codekind == midcode::ADD ||
              now->Midcode->codekind == midcode::SUB ||
              now->Midcode->codekind == midcode::MULT) {
       int head = LoopInfostack[LoopInfostack.size() - 1].varDef;
       int loc1 = SearchTable(now->Midcode->op1, head);
       int loc2 = SearchTable(now->Midcode->op2, head);
       if (loc1 == -1 && loc2 == -1) {
         if (now->pre != nullptr)
           now->pre->next = now->next;
         if (now->next != nullptr)
           now->next->pre = now->pre;
         now->next = begin;
         now->pre = begin->pre;
         begin->pre = now;
         if (now->pre != nullptr)
           now->pre->next = now;
         // �Ӷ�ֵ����ɾ��
         int head = LoopInfostack[LoopInfostack.size() - 1].varDef;
         DelItem(now->Midcode->op3, head);
       }
       // ����������붨ֵ
       else
         AddTable(now->Midcode->op3);
     }
     now = now->next;
   }
 }
 void call(midcode::MidcodeList* code) {
   int item = LoopInfostack.size() - 1;
   while (item >= 0) {
     LoopInfostack[item].state = 0;
     item--;
   }
 }
 void WhileEntry(midcode::MidcodeList* code) {
   LoopInfo now;
   now.state = 1;
   now.varDef = VarDefSet.size();
   now.whileEntry = code;
   LoopInfostack.push_back(now);
 }
 void WhileEnd(midcode::MidcodeList* code) {
   int item = LoopInfostack.size() - 1;
   if (LoopInfostack[item].state == 1) {
     LoopInfostack[item].whileEnd = code;
     midcode::MidcodeList* entry = LoopInfostack[item].whileEntry;
     LoopOutside(entry);
   }
   LoopInfostack.pop_back();
 }

 midcode::MidcodeList* LoopOpti() {
   VarDefSet.clear();
   midcode::MidcodeList* now = midcode::firstDummy;
   while (now != nullptr) {
     if (now->Midcode->codekind == midcode::AADD ||
         now->Midcode->codekind == midcode::ADD ||
         now->Midcode->codekind == midcode::SUB ||
         now->Midcode->codekind == midcode::MULT ||
         now->Midcode->codekind == midcode::DIV ||
         now->Midcode->codekind == midcode::LTC ||
         now->Midcode->codekind == midcode::EQC) {
       AddTable(now->Midcode->op3);
     } else if (now->Midcode->codekind == midcode::ASSIG) {
       AddTable(now->Midcode->op1);
     } else if (now->Midcode->codekind == midcode::WHILESTART) {
       WhileEntry(now);
     } else if (now->Midcode->codekind == midcode::ENDWHILE) {
       WhileEnd(now);
     } else if (now->Midcode->codekind == midcode::CALL) {
       call(now);
     }
     now = now->next;
   }
   return midcode::firstDummy;
 }
 } // namespace optim