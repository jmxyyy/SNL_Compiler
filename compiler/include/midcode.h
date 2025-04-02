//
// Created by jmxyyy.
//

#ifndef MIDCODE_H
#define MIDCODE_H

#include "../include/parse.h"

#include <string>

namespace midcode {

enum formKind { ValueForm, LabelForm, AddrForm };
enum codekindtype {
  ADD,
  SUB,
  MULT,
  DIV,
  EQC,
  LTC,
  READC,
  WRITEC,
  RETURNC,
  ASSIG,
  AADD,
  LABEL,
  JUMP,
  JUMP0,
  CALL,
  VARACT,
  VALACT,
  PENTRY,
  ENDPROC,
  MENTRY,
  WHILESTART,
  ENDWHILE
};
struct AddrStruct {
  std::string name;
  int dataLevel;
  int dataOff;
  parse::AccessKind access;
};
struct AttrStruct {
  int value{};
  int label{};
  AddrStruct Addr;
};
struct ArgRecord {
  formKind form;
  AttrStruct Attr;
};
struct MidcodeStruct {
  codekindtype codekind;
  ArgRecord* op1;
  ArgRecord* op2;
  ArgRecord* op3;
};
struct MidcodeList {
  MidcodeList* pre;
  MidcodeStruct* Midcode;
  MidcodeList* next;
};

inline int tmp_var_order;
inline int globalLabel;
inline int offset;
inline MidcodeList* firstDummy;
inline MidcodeList* lastDummy;
inline int off;
inline int midnum;

bool FindEntry(std::string id, bool flag, parse::symbtable** Entry);
bool Enter(std::string Id, parse::AttributeIR* AttribP,
           parse::symbtable** Entry);
bool FindField(std::string Id, parse::fieldChain* head,
               parse::fieldChain** Entry);
MidcodeList* GenMidCode(const parse::TreeNode* t);
void PrintMidCode(const MidcodeList* dummy);

ArgRecord* NewTemp(parse::AccessKind access);
ArgRecord* ARGvalue(int value);
int NewLabel();
ArgRecord* ARGLabel(int label);
ArgRecord* ARGAddr(const std::string& id, int level, int off1,
                   parse::AccessKind access);
MidcodeList* GenCode(codekindtype codekind, ArgRecord* first, ArgRecord* second,
                     ArgRecord* third);
void GenProDec(const parse::TreeNode* t);
void Genbody(const parse::TreeNode* t);
void GenStatement(parse::TreeNode* t);
void GenAssignS(const parse::TreeNode* t);
ArgRecord* GenVar(const parse::TreeNode* t);
ArgRecord* GenArray(ArgRecord* V1arg, const parse::TreeNode* t, int low, int size);
ArgRecord* GenField(ArgRecord* V1arg, const parse::TreeNode* t,
                    const parse::fieldChain* head);
ArgRecord* GenExpr(const parse::TreeNode* t);
void GenCallS(const parse::TreeNode* t);
void GenReadS(const parse::TreeNode* t);
void GenIfS(const parse::TreeNode* t);
void GenWriteS(const parse::TreeNode* t);
void GenWhileS(const parse::TreeNode* t);

} // namespace midcode
#endif // MIDCODE_H
