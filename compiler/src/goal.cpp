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

struct labelTable {
  int label;
  int destNum;
};

std::vector<std::string> goalCode;
std::vector<labelTable> labelNow;
int countNum = 0;
int max_num;

void outputInstruct(const std::string& errorInfo, const std::string& path) {
  std::ofstream output(path, std::ios::app);
  output << errorInfo << std::endl;
  output.close();
}

void operandGen(const midcode::ArgRecord* arg) {
  if (arg->form == midcode::ValueForm) {

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac,
            arg->Attr.value, 0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (arg->form == midcode::LabelForm) {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac,
            arg->Attr.label, 0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (arg->form == midcode::AddrForm) {
    FindAddr(arg);
    if (arg->Attr.Addr.access == parse::dir) {
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac, 0, ac);
      fprintf(goal_code, "\n");
      if (max_num < countNum)
        max_num = countNum;
    } else {
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac1, 0, ac);
      fprintf(goal_code, "\n");
      if (max_num < countNum)
        max_num = countNum;
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac, 0, ac1);
      fprintf(goal_code, "\n");
      if (max_num < countNum)
        max_num = countNum;
    }
  }
}

void FindAddr(const midcode::ArgRecord* arg) {
  const int varLevel = arg->Attr.Addr.dataLevel - 1;
  const int varOff = arg->Attr.Addr.dataOff;
  if (varLevel != -2) {
    FindSp(varLevel);

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac1, varOff,
            0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;

    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac, ac, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac1, varOff,
            0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac, sp, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  }
}
void FindSp(int varLevel) {
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac, varLevel,
          displayOff);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac, ac, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

void codeGen(midcode::MidcodeList* mid_code) {
  /* LDC ac,0,ac1
   LDC ac2,0,ac1
   ST ac,0,ac2*/
  // ADD, SUB, MULT, DIV, EQC
  goal_code = fopen(GOAL_MID_PATH.c_str(), "w");
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", mp, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  const int savedLoc = countNum;
  countNum++;
  if (countNum > max_num)
    max_num = countNum;

  while (mid_code != nullptr) {
    if (mid_code->Midcode->codeKind == midcode::ADD ||
        mid_code->Midcode->codeKind == midcode::SUB ||
        mid_code->Midcode->codeKind == midcode::MULT ||
        mid_code->Midcode->codeKind == midcode::DIV ||
        mid_code->Midcode->codeKind == midcode::EQC ||
        mid_code->Midcode->codeKind == midcode::LTC)
      arithGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::AADD)
      aaddGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::READC)
      readGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::WRITEC)
      writeGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::RETURNC)
      returnGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::ASSIG)
      assignGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::LABEL ||
             mid_code->Midcode->codeKind == midcode::WHILESTART ||
             mid_code->Midcode->codeKind == midcode::ENDWHILE)
      labelGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::JUMP)
      jumpGen(mid_code, 1);
    else if (mid_code->Midcode->codeKind == midcode::JUMP0)
      jump0Gen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::VALACT)
      valactGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::VARACT)
      varactGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::CALL)
      callGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::PENTRY)
      pentryGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::ENDPROC)
      endProcGen(mid_code);
    else if (mid_code->Midcode->codeKind == midcode::MENTRY)
      mentryGen(mid_code, savedLoc);
    else
      printf("codeGen bug\n");
    mid_code = mid_code->next;
  }
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "HALT", 0, 0, 0);
  if (max_num < countNum)
    max_num = countNum;

  fclose(goal_code);

  // sort
  FILE* input_file = fopen(GOAL_MID_PATH.c_str(), "r");
  if (input_file == nullptr) {
    printf("无法打开输入文件。\n");
    exit(0);
  }

  // 读取并解析文件内容
  CodeLine lines[1000];
  int line_count = 0;
  while (fscanf(input_file, "%d:%99[^\n]", &lines[line_count].index, lines[line_count].code) == 2) {
    fgetc(input_file); // 读取换行符
    line_count++;
  }
  fclose(input_file);

  // 对解析后的内容进行排序
  qsort(lines, line_count, sizeof(CodeLine), compare);

  // 将排序后的内容写入文件
  FILE* output_file = fopen(GOAL_PATH.c_str(), "w");
  if (output_file == nullptr) {
    printf("无法打开输出文件。\n");
    exit(0);
  }

  for (int i = 0; i < line_count; ++i) {
    if(i!=line_count-1)
      fprintf(output_file, "%3d: %s\n", lines[i].index, lines[i].code);
    else  fprintf(output_file, "%3d: %s", lines[i].index, lines[i].code);
  }
  fclose(output_file);
}

void arithGen(const midcode::MidcodeList* mid_code) {
  operandGen(mid_code->Midcode->op1);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  operandGen(mid_code->Midcode->op2);

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac1, 0, ac2);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  if (mid_code->Midcode->codeKind == midcode::ADD) {
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (mid_code->Midcode->codeKind == midcode::SUB) {
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "SUB", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (mid_code->Midcode->codeKind == midcode::MULT) {
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "MUL", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (mid_code->Midcode->codeKind == midcode::DIV) {
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "DIV", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (mid_code->Midcode->codeKind == midcode::LTC) {
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "SUB", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "JLT", ac, 2, pc);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac, 0, 0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", pc, 1, pc);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac, 1, 0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (mid_code->Midcode->codeKind == midcode::EQC) {
    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "SUB", ac, ac1, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "JEQ", ac, 2, pc);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac, 0, 0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", pc, 1, pc);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac, 1, 0);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  }
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  FindAddr(mid_code->Midcode->op3);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac1, 0, ac2);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac1, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

void aaddGen(const midcode::MidcodeList* mid_code) {
  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    FindAddr(mid_code->Midcode->op1);
  } else if (mid_code->Midcode->op1->Attr.Addr.access == parse::indir) {
    FindAddr(mid_code->Midcode->op1);
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  }
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  operandGen(mid_code->Midcode->op2);
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac2, ac2, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  FindAddr(mid_code->Midcode->op3);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

void readGen(const midcode::MidcodeList* mid_code) {
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "IN", ac2, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  FindAddr(mid_code->Midcode->op1);

  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac2, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac1, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac2, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  }
}

void writeGen(const midcode::MidcodeList* mid_code) {
  operandGen(mid_code->Midcode->op1);
  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "OUT", ac, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

void returnGen(midcode::MidcodeList* mid_code) { endProcGen(mid_code); }

void assignGen(const midcode::MidcodeList* mid_code) {
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  operandGen(mid_code->Midcode->op2);
  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    // ac2是被赋值的东西
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 0, ac2);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  } else if (mid_code->Midcode->op1->Attr.Addr.access == parse::indir) {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac1, 0, ac2);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  }
}

void labelGen(const midcode::MidcodeList* mid_code) {
  int label = mid_code->Midcode->op1->Attr.label;
  int currentLoc = countNum;
  int flag = 0;
  for (const auto& i : labelNow) {
    if (label == i.label) {
      flag = 1;
      countNum = i.destNum;
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", pc,
              currentLoc, 0);
      fprintf(goal_code, "\n");
      if (max_num < countNum)
        max_num = countNum;
      countNum = max_num;
      break;
    }
  }
  if (flag == 0) {
    labelNow.push_back(labelTable{label, currentLoc});
  }
}

// 预留回填地址
void jumpGen(const midcode::MidcodeList* mid_code, int i) {
  int label;
  if (i == 1)
    label = mid_code->Midcode->op1->Attr.label;
  else
    label = mid_code->Midcode->op2->Attr.label;
  int flag = 0;
  for (auto& i : labelNow) {
    if (label == i.label) {
      flag = 1;
      fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", pc,
              i.destNum, 0);
      fprintf(goal_code, "\n");
      if (max_num < countNum)
        max_num = countNum;
      break;
    }
  }
  if (flag == 0) {
    const int currentLoc = countNum;
    countNum += 1;
    if (countNum > max_num)
      max_num = countNum;
    labelNow.push_back(labelTable{label, currentLoc});
  }
}

// TODO
void jump0Gen(const midcode::MidcodeList* mid_code) {
  operandGen(mid_code->Midcode->op1);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac2, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  const int savedLoc = countNum;

  countNum += 1;

  if (countNum > max_num)
    max_num = countNum;
  jumpGen(mid_code, 2);
  int currentLoc = countNum;
  countNum += 0;

  if (countNum > max_num)
    max_num = countNum;
  countNum = savedLoc;
  // emitRM_Abs("JNE",ac2,currentLoc,"not jump"); ac2不等于0就跳到pc
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum, "JNE", ac2,
          currentLoc - (countNum + 1), pc);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  ++countNum;
  if (max_num < countNum)
    max_num = countNum;
  countNum = max_num;
}

void valactGen(const midcode::MidcodeList* mid_code) {
  int paramOff = mid_code->Midcode->op2->Attr.value;
  operandGen(mid_code->Midcode->op1);
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, paramOff,
          top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

void varactGen(const midcode::MidcodeList* mid_code) {
  int paramOff = mid_code->Midcode->op2->Attr.value;
  if (mid_code->Midcode->op1->Attr.Addr.access == parse::dir) {
    FindAddr(mid_code->Midcode->op1);
  } else {
    FindAddr(mid_code->Midcode->op1);
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac, 0, ac);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  }
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, paramOff,
          top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

// TODO
void callGen(const midcode::MidcodeList* mid_code) {
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", displayOff, 6,
          top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  int Noff = mid_code->Midcode->op3->Attr.value;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", displayOff,
          Noff, 0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  const int currentLoc = countNum + 3;
  countNum += 0;
  if (max_num < countNum)
    max_num = countNum;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac, currentLoc,
          0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 1, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  jumpGen(mid_code, 1);
}

void pentryGen(const midcode::MidcodeList* mid_code) {
  labelGen(mid_code);
  const int activity_size = mid_code->Midcode->op2->Attr.value;
  const int level_temp = mid_code->Midcode->op3->Attr.value;
  // 保存sp
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", sp, 0, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  // 保存寄存器
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 3, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac1, 4, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac2, 5, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  // 过程信息
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac, level_temp,
          0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 2, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  // dipplay
  for (int i = 0; i < level_temp; i++) {
    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac1, 6, top);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac1, i, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;

    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac1, ac1, sp);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac1, i,
            displayOff);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;

    fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac1, ac1,
            top);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;

    fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 0, ac1);
    fprintf(goal_code, "\n");
    if (max_num < countNum)
      max_num = countNum;
  }
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", ac, level_temp,
          displayOff);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "ADD", ac, top, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", top, 0, ac);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "LDA", sp, 0, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d,%d ", countNum++, "LDA", top,
          activity_size, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

void endProcGen(midcode::MidcodeList* mid_code) {
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac, 3, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac1, 4, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", ac2, 5, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", displayOff, 6,
          sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", top, 0, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", sp, 0, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LD", pc, 1, top);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

void mentryGen(const midcode::MidcodeList* mid_code, int save) {
  const int currentLoc = countNum;
  countNum += 0;
  if (max_num < countNum)
    max_num = countNum;

  countNum = save;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", pc, currentLoc,
          0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  countNum = max_num;

  // 初始化寄存器
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac1, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;

  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", ac2, 0, 0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  // sp
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 0, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  // displayoff
  int Noff = mid_code->Midcode->op3->Attr.value;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDC", displayOff,
          Noff, 0);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  // display
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "ST", ac, 0,
          displayOff);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
  // 填写top
  const int temp = mid_code->Midcode->op2->Attr.value;
  fprintf(goal_code, "%3d:  %5s  %d,%d(%d) ", countNum++, "LDA", top, temp, sp);
  fprintf(goal_code, "\n");
  if (max_num < countNum)
    max_num = countNum;
}

} // namespace goal