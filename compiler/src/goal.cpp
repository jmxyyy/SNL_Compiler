//
// created by jmxyyy
//

#include "../include/goal.h"
#include "../include/config.h"

#include <iostream>
#include <string>
#include <vector>

namespace goal {

#define pc 7
#define sp 6
#define top 5
#define displayOff 4
#define mp 3
#define ac2 2
#define ac1 1
#define ac 0

std::vector<std::string> gogalcode;
struct labeltable {
  int label;
  int destnum;
};
std::vector<labeltable> labelnow;
int countnum = 0;
// int currentLoc;
int max_num;
// FILE* goal_code;
// extern FILE* goalcode;
void outputInstruct(const std::string& errorInfo, const std::string& path) {
  std::ofstream output(path, std::ios::app);
  output << errorInfo << std::endl;
  output.close();
}

void operandGen(const midcode::ArgRecord* arg) {
  if (arg->form == midcode::ValueForm) {

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac,
            arg->Attr.value, 0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (arg->form == midcode::LabelForm) {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac,
            arg->Attr.label, 0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (arg->form == midcode::AddrForm) {
    FindAddr(arg);
    if (arg->Attr.Addr.access == parse::dir) {
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac, 0, ac);
      fprintf(goal_code, "\n");
      if (max_num < countnum)
        max_num = countnum;
    } else {
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac1, 0, ac);
      fprintf(goal_code, "\n");
      if (max_num < countnum)
        max_num = countnum;
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac, 0, ac1);
      fprintf(goal_code, "\n");
      if (max_num < countnum)
        max_num = countnum;
    }
  }
}
void FindAddr(const midcode::ArgRecord* arg) {
  const int varLevel = arg->Attr.Addr.dataLevel - 1;
  const int varOff = arg->Attr.Addr.dataOff;
  if (varLevel != -2) {
    FindSp(varLevel);

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac1, varOff,
            0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;

    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac, ac, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else {
    // stringstream ss;
    // ss << varOff << ",0";
    // string valueString = ss.str();
    // outputinstruct("LDC,ac1," + valueString, outputFile);
    // outputFile << "\n";
    // gogalcode.push_back("LDC,ac1," + valueString);

    // stringstream ss1;
    // ss1 << "ADD,ac,sp,ac1";
    // string valueString1 = ss1.str();
    // outputinstruct(valueString1, outputFile);
    // outputFile << "\n";
    // gogalcode.push_back("ADD,ac,sp,ac1");
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac1, varOff,
            0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac, sp, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  }
}
void FindSp(int varlevel) {
  // stringstream ss;
  // ss << varlevel << ",displayOff";
  // string valueString = ss.str();
  // outputinstruct("LDC,ac," + valueString, outputFile);
  // outputFile << "\n";
  // gogalcode.push_back("LDC,ac," + valueString);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac, varlevel,
          displayOff);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // stringstream ss1;
  // ss1 << "LD,ac,ac,sp" << endl;
  // string valueString1 = ss1.str();
  // outputinstruct(valueString1, outputFile);
  // gogalcode.push_back("LD,ac,ac,sp");
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac, ac, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // stringstream ss2;
  // ss2 << "LD,ac,0,ac" << endl;
  // string valueString2 = ss2.str();
  // outputinstruct(valueString2, outputFile);
  // gogalcode.push_back("LD,ac,ac,sp");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}
void codeGen(midcode::MidcodeList* mid_code) {
  /* LDC ac,0,ac1
   LDC ac2,0,ac1
   ST ac,0,ac2*/
  // stringstream ss;
  // ss << "LDC,ac,0,ac1" << endl<< "LDC,ac2,0,ac1" << endl<< "ST,ac,0,ac2" <<
  // endl; gogalcode.push_back("LDC,ac,0,ac1");
  // gogalcode.push_back("LDC,ac2,0,ac1");
  // gogalcode.push_back("ST,ac,0,ac2");
  // string valueString = ss.str();
  // outputinstruct(valueString, outputFile);
  // ADD, SUB, MULT, DIV, EQC

  goal_code = fopen(GOAL_PATH.c_str(), "w");

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", mp, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  int savedLoc = countnum;
  countnum++;
  if (countnum > max_num)
    max_num = countnum;

  while (mid_code != nullptr) {
    if (mid_code->Midcode->codekind == midcode::ADD ||
        mid_code->Midcode->codekind == midcode::SUB ||
        mid_code->Midcode->codekind == midcode::MULT ||
        mid_code->Midcode->codekind == midcode::DIV ||
        mid_code->Midcode->codekind == midcode::EQC ||
        mid_code->Midcode->codekind == midcode::LTC)
      arithGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::AADD)
      aaddGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::READC)
      readGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::WRITEC)
      writeGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::RETURNC)
      returnGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::ASSIG)
      assignGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::LABEL ||
             mid_code->Midcode->codekind == midcode::WHILESTART ||
             mid_code->Midcode->codekind == midcode::ENDWHILE)
      labelGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::JUMP)
      jumpGen(mid_code, 1);
    else if (mid_code->Midcode->codekind == midcode::JUMP0)
      jump0Gen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::VALACT)
      valactGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::VARACT)
      varactGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::CALL)
      callGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::PENTRY)
      pentryGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::ENDPROC)
      endprocGen(mid_code);
    else if (mid_code->Midcode->codekind == midcode::MENTRY)
      mentryGen(mid_code, savedLoc);
    else
      printf("codeGen bug\n");
    mid_code = mid_code->next;
  }
  // stringstream ss1;
  // ss1 << "HALT,0,0,0" << endl;
  // string valueString = ss1.str();
  // outputinstruct(valueString1, outputFile);
  // gogalcode.push_back("HALT,0,0,0");
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "HALT", 0, 0, 0);
  if (max_num < countnum)
    max_num = countnum;

  fclose(goal_code);
}
void arithGen(const midcode::MidcodeList* mid_code) {
  operandGen(mid_code->Midcode->op1);
  // stringstream ss;
  // ss << "LDA,ac2,0,ac" <<endl;
  // string valueString = ss.str();
  // outputinstruct(valueString, outputFile);
  // gogalcode.push_back("LDA,ac2,0,ac");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  operandGen(mid_code->Midcode->op2);
  /* stringstream ss1;
   ss1 << "LDA,ac1,0,ac2" << endl;
   string valueString1 = ss1.str();
   outputinstruct(valueString1, outputFile);
   gogalcode.push_back("LDA,ac1,0,ac2");*/
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac1, 0, ac2);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  if (mid_code->Midcode->codekind == midcode::ADD) {
    // stringstream ss2;
    // ss2 << "ADD,ac,ac1,ac" << endl;
    // string valueString2 = ss2.str();
    // outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("ADD,ac,ac1,ac");
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (mid_code->Midcode->codekind == midcode::SUB) {
    // stringstream ss2;
    // ss2 << "SUB,ac,ac1,ac" << endl;
    // string valueString2 = ss2.str();
    // outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("SUB,ac,ac1,ac");
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "SUB", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (mid_code->Midcode->codekind == midcode::MULT) {
    // stringstream ss2;
    // ss2 << "MUL,ac,ac1,ac" << endl;
    // string valueString2 = ss2.str();
    // outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("MUL,ac,ac1,ac");
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "MUL", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (mid_code->Midcode->codekind == midcode::DIV) {
    // stringstream ss2;
    // ss2 << "DIV,ac,ac1,ac" << endl;
    // string valueString2 = ss2.str();
    // outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("DIV,ac,ac1,ac");
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "DIV", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (mid_code->Midcode->codekind == midcode::LTC) {
    // stringstream ss2;
    // ss2 <<"SUB,ac,ac1,ac"<< endl<< "JLT,ac,2,pc"<<endl<< "LDC,ac,0,0"<<endl<<
    // "LDA,pc,1,pc"<<endl<< "LDC,ac,1,0"<<endl; string valueString2 =
    // ss2.str(); outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("SUB,ac,ac1,ac");
    // gogalcode.push_back("JLT,ac,2,pc");
    // gogalcode.push_back("LDC,ac,0,0");
    // gogalcode.push_back("LDA,pc,1,pc");
    // gogalcode.push_back("LDC,ac,1,0");
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "SUB", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "JLT", ac, 2, pc);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac, 0, 0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", pc, 1, pc);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac, 1, 0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (mid_code->Midcode->codekind == midcode::EQC) {
    // stringstream ss2;
    // ss2 << "SUB,ac,ac1,ac" << endl << "JEQ,ac,2,pc" << endl << "LDC,ac,0,0"
    // << endl << "LDA,pc,1,pc" << endl << "LDC,ac,1,0" << endl; string
    // valueString2 = ss2.str(); outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("SUB,ac,ac1,ac");
    // gogalcode.push_back("JEQ,ac,2,pc");
    // gogalcode.push_back("LDC,ac,0,0");
    // gogalcode.push_back("LDA,pc,1,pc");
    // gogalcode.push_back("LDC,ac,1,0");
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "SUB", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "JEQ", ac, 2, pc);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac, 0, 0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", pc, 1, pc);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac, 1, 0);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  }
  // stringstream ss3;
  // ss3 << "LDA,ac2,0,ac" << endl;
  // string valueString3 = ss3.str();
  // outputinstruct(valueString3, outputFile);
  // gogalcode.push_back("LDA,ac2,0,ac");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  FindAddr(mid_code->Midcode->op3);

  // stringstream ss4;
  // ss4<< "LDA,ac1,0,ac2" << endl <<"ST,ac1,0,ac"<<endl;
  // string valueString4 = ss4.str();
  // outputinstruct(valueString4, outputFile);
  // gogalcode.push_back("LDA,ac1,0,ac2");
  // gogalcode.push_back("ST,ac1,0,ac");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac1, 0, ac2);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac1, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}
void aaddGen(const midcode::MidcodeList* mid_code) {
  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    FindAddr(mid_code->Midcode->op1);
  } else if (mid_code->Midcode->op1->Attr.Addr.access == parse::indir) {
    FindAddr(mid_code->Midcode->op1);
    // stringstream ss;
    // ss << "LD,ac,0,ac" << endl;
    // string valueString = ss.str();
    // outputinstruct(valueString, outputFile);
    // gogalcode.push_back("LD,ac,0,ac");
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  }
  // stringstream ss1;
  // ss1 << "LDA,ac2,0,ac" << endl;
  // string valueString1 = ss1.str();
  // outputinstruct(valueString1, outputFile);
  // gogalcode.push_back("LDA,ac2,0,ac");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  operandGen(mid_code->Midcode->op2);

  // stringstream ss2;
  // ss2 << "ADD,ac2,ac2,ac" << endl;
  // string valueString2 = ss2.str();
  // outputinstruct(valueString2, outputFile);
  // gogalcode.push_back("ADD,ac2,ac2,ac");
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac2, ac2, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  FindAddr(mid_code->Midcode->op3);

  // stringstream ss3;
  // ss3 << "ST,ac2,0,ac" << endl;
  // string valueString3 = ss3.str();
  // outputinstruct(valueString3, outputFile)
  // gogalcode.push_back("ST,ac2,0,ac");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}
void readGen(const midcode::MidcodeList* mid_code) {
  // stringstream ss1;
  // ss1 << "IN,ac2,0,0" << endl;
  // string valueString1 = ss1.str();
  // outputinstruct(valueString1, outputFile);
  // gogalcode.push_back("IN,ac2,0,0");
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "IN", ac2, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  FindAddr(mid_code->Midcode->op1);

  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    // stringstream ss2;
    // ss2 << "LD,ac1,0,ac" << endl<<"ST,ac2,0,ac1"<<endl;
    // string valueString2 = ss2.str();
    // outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("LD,ac1,0,ac");
    // gogalcode.push_back("ST,ac2,0,ac1");
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac2, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else {
    // stringstream ss2;
    // ss2 <<"ST,ac2,0,ac1" << endl;
    // string valueString2 = ss2.str();
    // outputinstruct(valueString2, outputFile);
    // gogalcode.push_back("ST,ac2,0,ac1");
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac1, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac2, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  }
}
void writeGen(const midcode::MidcodeList* mid_code) {
  operandGen(mid_code->Midcode->op1);
  // stringstream ss2;
  // ss2 << "OUT,ac,0,0" << endl;
  // string valueString2 = ss2.str();
  // outputinstruct(valueString2, outputFile);
  // gogalcode.push_back"OUT,ac,0,0");
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "OUT", ac, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}
void returnGen(midcode::MidcodeList* mid_code) { endprocGen(mid_code); }

void assignGen(const midcode::MidcodeList* mid_code) {
  FindAddr(mid_code->Midcode->op1);
  // stringstream ss2;
  // ss2 << "LDA,ac2,0,ac" << endl;
  // string valueString2 = ss2.str();
  // outputinstruct(valueString2, outputFile);
  // gogalcode.push_back("LDA,ac2,0,ac");

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  operandGen(mid_code->Midcode->op2);
  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    // stringstream ss3;
    // ss3 << "LD,ac1,0,ac2" << endl<<"ST,ac,0,ac1"<<endl;
    // string valueString3 = ss3.str();
    // outputinstruct(valueString3, outputFile);
    // gogalcode.push_back("LD,ac1,0,ac2");
    // gogalcode.push_back("ST,ac,0,ac1");
    // ac2�Ǳ���ֵ�Ķ���

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 0, ac2);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  } else if (mid_code->Midcode->op1->Attr.Addr.access == parse::indir) {
    // stringstream ss3;
    // ss3 << "ST,ac,0,ac2" << endl;
    // string valueString3 = ss3.str();
    // outputinstruct(valueString3, outputFile);
    // gogalcode.push_back("ST,ac,0,ac2");
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac1, 0, ac2);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  }
}

void labelGen(const midcode::MidcodeList* mid_code) {
  int label = mid_code->Midcode->op1->Attr.label;
  int currentLoc = countnum;
  int flag = 0;
  for (const auto& i : labelnow) {
    if (label == i.label) {
      flag = 1;
      countnum = i.destnum;
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", pc,
              currentLoc, 0);
      fprintf(goal_code, "\n");
      if (max_num < countnum)
        max_num = countnum;
      countnum = max_num;
      break;
    }
  }
  if (flag == 0) {
    labelnow.push_back(labeltable{label, currentLoc});
  }
}
// Ԥ�������ַ��
void jumpGen(const midcode::MidcodeList* mid_code, int i) {
  int label;
  if (i == 1)
    label = mid_code->Midcode->op1->Attr.label;
  else
    label = mid_code->Midcode->op2->Attr.label;
  int flag = 0;
  for (auto& i : labelnow) {
    if (label == i.label) {
      flag = 1;
      // stringstream ss;
      // ss <<labelnow[i].destnum<< ",0";
      // string valueString = ss.str();
      // outputinstruct("LDC,pc," + valueString, outputFile);
      // outputFile << "\n";
      // gogalcode.push_back("LDC,pc," + valueString);
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", pc,
              i.destnum, 0);
      fprintf(goal_code, "\n");
      if (max_num < countnum)
        max_num = countnum;
      break;
    }
  }
  if (flag == 0) {
    int currentLoc = countnum;
    countnum += 1;
    if (countnum > max_num)
      max_num = countnum;
    labelnow.push_back(labeltable{label, currentLoc});
  }
}
// ������
void jump0Gen(const midcode::MidcodeList* mid_code) {
  operandGen(mid_code->Midcode->op1);
  // outputinstruct("LDA,ac2,0,ac", outputFile);
  // outputFile << "\n";
  // gogalcode.push_back("LDA,ac2,0,ac");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  int savedLoc = countnum;

  countnum += 1;

  if (countnum > max_num)
    max_num = countnum;
  jumpGen(mid_code, 2);
  int currentLoc = countnum;
  countnum += 0;

  if (countnum > max_num)
    max_num = countnum;
  countnum = savedLoc;
  // emitRM_Abs("JNE",ac2,currentLoc,"not jump");ac2������0������pc
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum, "JNE", ac2,
          currentLoc - (countnum + 1), pc);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  ++countnum;
  if (max_num < countnum)
    max_num = countnum;
  countnum = max_num;
}
void valactGen(const midcode::MidcodeList* mid_code) {
  int paramOff = mid_code->Midcode->op2->Attr.value;
  operandGen(mid_code->Midcode->op1);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, paramOff,
          top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // stringstream ss3;
  // ss3 <<paramOff<<",top";
  // string valueString3 = ss3.str();
  // outputinstruct("ST,ac," + valueString3, outputFile);
  // outputFile << "\n";
  // gogalcode.push_back("ST,ac," + valueString3);
}
void varactGen(const midcode::MidcodeList* mid_code) {
  int paramOff = mid_code->Midcode->op2->Attr.value;
  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    FindAddr(mid_code->Midcode->op1);
    // outputinstruct("LD,ac,0,ac", outputFile);
    // outputFile << "\n";
    // gogalcode.push_back("LD,ac,0,ac");
  } else {
    FindAddr(mid_code->Midcode->op1);
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  }
  // stringstream ss3;
  // ss3 << paramOff << ",top";
  // string valueString3 = ss3.str();
  // outputinstruct("ST,ac," + valueString3, outputFile);
  // outputFile << "\n";
  // gogalcode.push_back("ST,ac," + valueString3);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, paramOff,
          top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}
// ������
void callGen(const midcode::MidcodeList* mid_code) {
  // outputinstruct("ST,displayOff,6,top",outputFile);
  // outputFile << "\n";
  // gogalcode.push_back("ST,displayOff,6,top");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", displayOff, 6,
          top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  int Noff = mid_code->Midcode->op3->Attr.value;

  // outputinstruct("LDC,displayOff,Noff,0", outputFile);
  // outputFile << "\n";
  // gogalcode.push_back("LDC,displayOff,Noff,0");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", displayOff,
          Noff, 0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  int currentLoc = countnum + 3;
  countnum += 0;
  if (max_num < countnum)
    max_num = countnum;
  // stringstream ss3;
  // ss3 << currentLoc<< ",0";
  // string valueString3 = ss3.str();
  // outputinstruct("LDC,ac," + valueString3, outputFile);
  // outputFile << "\n";
  // gogalcode.push_back("LDC,ac," + valueString);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac, currentLoc,
          0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // outputinstruct("ST,ac,1,top", outputFile);
  // gogalcode.push_back("ST,ac,1,top");
  // jumpGen(midcode->Midcode->op1);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 1, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  jumpGen(mid_code, 1);
}

void pentryGen(const midcode::MidcodeList* mid_code) {
  labelGen(mid_code);
  int activity_size = mid_code->Midcode->op2->Attr.value;
  // level
  // �Ҹ���һ�£�������������������������������������������������������������������������������������
  int level_temp = mid_code->Midcode->op3->Attr.value;
  // ����sp
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", sp, 0, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // ����Ĵ���
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 3, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac1, 4, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac2, 5, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // ������Ϣ
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac, level_temp,
          0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 2, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // dipplay
  for (int i = 0; i < level_temp; i++) {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac1, 6, top);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac1, i, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;

    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac1, ac1, sp);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac1, i,
            displayOff);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;

    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac1, ac1, top);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countnum)
      max_num = countnum;
  }
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", ac, level_temp,
          displayOff);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "ADD", ac, top, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", top, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "LDA", sp, 0, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countnum++, "LDA", top,
          activity_size, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}

void endprocGen(midcode::MidcodeList* mid_code) {
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac, 3, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac1, 4, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", ac2, 5, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", displayOff, 6,
          sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", top, 0, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", sp, 0, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LD", pc, 1, top);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}
void mentryGen(const midcode::MidcodeList* mid_code, int save) {
  int currentLoc = countnum;
  countnum += 0;
  if (max_num < countnum)
    max_num = countnum;

  countnum = save;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", pc, currentLoc,
          0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  countnum = max_num;

  // ��ʼ���Ĵ���
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac1, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", ac2, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // sp
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 0, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // displayoff
  int Noff = mid_code->Midcode->op3->Attr.value;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDC", displayOff,
          Noff, 0);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // display��
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "ST", ac, 0,
          displayOff);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
  // ��дtop
  const int temp = mid_code->Midcode->op2->Attr.value;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countnum++, "LDA", top, temp, sp);
  fprintf(goal_code, "\n");
  if (max_num < countnum)
    max_num = countnum;
}

} // namespace goal