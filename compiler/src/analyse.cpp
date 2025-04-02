#include "../include/config.h"
#include "../include/parse.h"
#include "../include/scanner.h"

#include <fstream>
#include <iostream>

namespace parse {

symbolTable Scope[10000];        // 符号表
symbolTable DestroyScope[10000]; // 撤销符号表
int destroyLevel = 0;
int level = 0;
// 用于判断自定义类型是类型声明还是变量声明，变量声明为false
bool flagTree = false;
symbolTable* table = nullptr;
int off = 7;

void CreatTable() {
  off = 7;
  level++;
}

// 撤销一个符号表
void DestroyTable() {
  DestroyScope[destroyLevel].next = Scope[level].next;
  destroyLevel++;
  Scope[level].next = nullptr;
  level = level - 1;
}

// 从表头开始，依次将节点中的标识符名字和id比较是否相同，直
// 到找到此标识符或到达表尾，若找到，返回真值，Entry为标识符
// 在符号表中的位置，否则，返回值为假。
bool SearchOneTable(const std::string& id, const int currentLevel,
                    symbolTable** Entry) {
  symbolTable* t = Scope[currentLevel].next;
  while (t != nullptr) {
    if (t->idName == id) {
      (*Entry) = t;
      return true;
    }
    // TODO
    // else {
    //   t = t->next;
    // }
  }
  return false;
}

// 符号表中查找标识符
bool FindEntry(const std::string& id, const bool flag, symbolTable** Entry) {
  if (flag) {
    for (int i = level; i > 0; i--) {
      if (SearchOneTable(id, i, Entry)) {
        return true;
      }
    }
  } else {
    return SearchOneTable(id, level, Entry);
  }
  return false;
}

// 登记标识符和属性到符号表
bool Enter(const std::string& Id, const AttributeIR* AttribP,
           symbolTable** Entry) {
  const bool present = FindEntry(Id, false, Entry);
  if (present == true) {
    std::cout << "Identifier duplicate declarations" << std::endl;
  } else {
    auto* newSymbolTable = new symbolTable;
    newSymbolTable->idName = Id;
    newSymbolTable->attrIR = *AttribP;

    if (Scope[level].next == nullptr) {
      Scope[level].next = newSymbolTable;
    } else {
      symbolTable* t = Scope[level].next;
      while (t->next != nullptr) {
        t = t->next;
      }
      t->next = newSymbolTable;
    }
    (*Entry) = newSymbolTable;
  }
  return present;
}

// 在域表中查找域名
bool FindField(const std::string& Id, fieldChain* head, fieldChain** Entry) {
  fieldChain* r = head;
  while (r != nullptr) {
    if (Id == r->idName) {
      (*Entry) = r;
      return true;
    }
    r = r->next;
  }
  return false;
}

// 打印符号表
// void Printsymbtable() {
//	for (int i = 1; i < level+1; i++) {
//		symbtable* t = Scope[i].next;
//		while (t != nullptr) {
//             if (t->attrIR.kind == typeKind) {
//                 std::cout << t->idname << " " << t->attrIR.kind << " ";
//                 std::cout << t->attrIR;
//                 std::cout << " " << std::endl;
//             }
//			else if (t->attrIR.kind == varKind)
//			{
//				std::cout << t->idname <<" "<< t->attrIR.kind
//<<" "<< t->attrIR <<" "<< t->attrIR.More.VarAttr.level <<"
//"<<t->attrIR.idtype->kind<<" "<< t->attrIR.idtype->size << std::endl;
//			}
//			else
//			t = t->next;
//		}
//	}
// }

// 输出符号表
void InputSymbol(const std::string& path) {
  std::ofstream output;
  output.open(path);
  for (int i = 1; i < level + 1; i++) {
    const symbolTable* t = Scope[i].next;
    while (t != nullptr) {
      if (t->attrIR.kind == typeKind) {
        output << t->idName << " " << t->attrIR.kind << " " << &t->attrIR << " "
               << t->attrIR.idtype << " " << t->attrIR.More.VarAttr.level
               << std::endl;
      } else if (t->attrIR.kind == varKind) {
        if (t->attrIR.idtype != nullptr) {
          output << t->idName << " " << t->attrIR.kind << " " << &t->attrIR
                 << " " << t->attrIR.idtype << " "
                 << t->attrIR.More.VarAttr.level << " "
                 << t->attrIR.idtype->kind << " " << t->attrIR.idtype->size
                 << std::endl;
        } else {
          output << t->idName << " " << t->attrIR.kind << " " << &t->attrIR
                 << " " << std::endl;
        }
      } else {
        output << t->idName << " " << t->attrIR.kind << " " << &t->attrIR << " "
               << t->attrIR.idtype << " " << t->attrIR.More.ProcAttr.level
               << " " << t->attrIR.More.ProcAttr.param << std::endl;
      }

      t = t->next;
    }
  }
  output.close();
}

// 输出已撤销地符号表
void InputDestroySymbol(const std::string& path) {
  std::ofstream ouput;
  ouput.open(path, std::ios::app);
  ouput << std::endl << std::endl << std::endl << std::endl << std::endl;
  ouput << "Destroy Symbol Table: " << std::endl;
  for (int i = 0; i < destroyLevel; i++) {
    const symbolTable* t = DestroyScope[i].next;
    while (t != nullptr) {
      if (t->attrIR.kind == typeKind) {
        ouput << t->idName << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.VarAttr.level
              << std::endl;
      } else if (t->attrIR.kind == varKind) {
        ouput << t->idName << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.VarAttr.level << " "
              << t->attrIR.idtype->kind << " " << t->attrIR.idtype->size
              << std::endl;
      } else {
        ouput << t->idName << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.ProcAttr.level << " "
              << t->attrIR.More.ProcAttr.param << std::endl;
      }

      t = t->next;
    }
  }
  ouput.close();
}

typeIR *intPtr, *charPtr, *boolPtr;

// 初始化基本类型内部表示函数
// 初始化整数类型，字符类型，布尔类型的内部表示说明
// 由于这三种类型均为基本类型，内部表示固定
void initialize() {
  intPtr = new typeIR;
  intPtr->size = 1;
  intPtr->kind = intTy;
  charPtr = new typeIR;
  charPtr->size = 1;
  charPtr->kind = charTy;
  boolPtr = new typeIR;
  boolPtr->size = 1;
  boolPtr->kind = boolTy;
}

// 自定义类型内部结构分析函数
typeIR* nameType(const TreeNode* t) {
  auto** Entry = new symbolTable*;
  int i = 0;
  if (flagTree) {
    i++;
  }
  bool flag = FindEntry(t->name[i], false, Entry);

  if (!flag) {
    std::cout << t->lineno << "Custom type " + t->name[i] + "no declare"
              << std::endl;
  } else {
    return (*Entry)->attrIR.idtype;
  }

  return nullptr;
}

// 数组类型内部表示处理函数
typeIR* arrayType(const TreeNode* t) {
  if (t->attr.arrayAttr.low > t->attr.arrayAttr.up) {
    std::cout << "The array subscript is out of bounds" << std::endl;
  } else {
    auto* newArrayType = new typeIR;
    newArrayType->kind = arrayTy;
    newArrayType->size = ((t->attr.arrayAttr.up) - (t->attr.arrayAttr.low) + 1);
    newArrayType->More.ArrayAttr.indexTy = intPtr;
    newArrayType->More.ArrayAttr.low = t->attr.arrayAttr.low;
    newArrayType->More.ArrayAttr.up = t->attr.arrayAttr.up;
    if (t->attr.arrayAttr.childType == CharK) {
      newArrayType->More.ArrayAttr.elemTy = charPtr;
    } else {
      newArrayType->More.ArrayAttr.elemTy = intPtr;
    }
    return newArrayType;
  }
  return nullptr;
}

void outputbody(const fieldChain* bodyT) {
  if (bodyT == nullptr) {
  } else {
    std::cout << "  " << bodyT->idName;
    outputbody(bodyT->next);
  }
}

// 处理记录类型的内部表示函数
typeIR* recordType(const TreeNode* t) {
  int sum_filed = 0;
  auto* newRecordType = new typeIR;
  newRecordType->kind = recordTy;
  fieldChain *newBody = nullptr, *dt = nullptr;
  TreeNode* nt = t->child[0];
  int recordOffset = 0;
  while (nt != nullptr) {
    for (int i = 0; i < nt->name.size(); i++) {
      if (newBody != nullptr) {
        auto** Entry = new fieldChain*;
        if (bool flag = FindField(nt->name[i], newBody, Entry)) {
          std::cout << t->lineno << "record rename" << std::endl;
        }
      }
      auto* newt = new fieldChain;
      if (newBody == nullptr) {
        newBody = newt;
      } else {
        dt->next = newt;
      }
      newt->unitType = TypeProcess(nt);
      newt->idName = nt->name[i];

      newt->offset = recordOffset;
      recordOffset = recordOffset + newt->unitType->size;
      sum_filed += newt->unitType->size;
      newt->next = nullptr;
      dt = newt;
    }
    nt = nt->sibling;
  }
  dt->next = nullptr;
  newRecordType->More.body = newBody;
  newRecordType->size = sum_filed;
  return newRecordType;
}

// 类型分析处理函数：类型分析处理。处理语法树的当前节点类型，构造当前类型的内部
// 表示，并将其地址返回给Ptr类型内部表示的地址。
typeIR* TypeProcess(const TreeNode* t) {
  if (t->kind.dec == ArrayK) {
    return arrayType(t);
  } else if (t->kind.dec == CharK) {
    return charPtr;
  } else if (t->kind.dec == IntegerK) {
    return intPtr;
  } else if (t->kind.dec == RecordK) {
    return recordType(t);
  } else if (t->kind.dec == IdK) {
    return nameType(t);
  }
  return nullptr;
}

// 类型声明部分分析处理函数
void TypeDecPart(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  auto** Entry = new symbolTable*;
  auto** Entry1 = new symbolTable*;
  int i = 0;
  if (t->kind.dec == IdK) {
    i++;
  }
  for (; i < t->name.size(); i++) {
    if (bool flag = FindEntry(t->name[0], false, Entry)) {
      std::cout << t->lineno
                << "type identifier " + t->name[0] + " duplicate declarations"
                << std::endl;
    } else {
      auto* a = new AttributeIR;
      a->kind = typeKind;
      flagTree = true;
      a->idtype = TypeProcess(t);
      flagTree = false;
      Enter(t->name[0], a, Entry);
      t->table.clear();
      t->table.push_back(*Entry);
      if (t->kind.dec == IdK) {
        FindEntry(t->name[1], false, Entry1);
        t->table.push_back(*Entry1);
      }
    }
  }
  TypeDecPart(t->sibling);
}

// 变量声明部分分析处理函数
// 传进来的t是声明结点的东西,加到符号表
void VarDecList(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  auto** Entry = new symbolTable*;
  auto** Entry1 = new symbolTable*;
  int i = 0;
  if (t->kind.dec == IdK) {

    i++;
  }
  int once = 0;
  for (; i < t->name.size(); i++) {

    if (bool flag = FindEntry(t->name[i], false, Entry)) {
      std::cout << t->lineno
                << "type identifier " + t->name[i] + " duplicate declarations"
                << std::endl;

    } else {
      auto* a = new AttributeIR;
      a->kind = varKind;

      a->idtype = TypeProcess(t);

      if (a->idtype != nullptr) {
        a->More.VarAttr.access = dir;
        a->More.VarAttr.level = level;
        a->More.VarAttr.off = off;
        off = off + a->idtype->size;
        if (once == 0 && t->kind.dec == IdK) {
          once++;
          FindEntry(t->name[0], false, Entry1);
          t->table.clear();
          t->table.push_back(*Entry1);
        }
      }

      Enter(t->name[i], a, Entry);
      t->table.push_back(*Entry);
    }
  }
  VarDecList(t->sibling);
}

// 形参分析处理函数
ParamTable* ParaDecList(TreeNode* t) {
  if (t == nullptr) {
    return nullptr;
  }
  ParamTable* parat = nullptr;
  ParamTable *paralist = nullptr, *dt = nullptr;
  int i = 0;
  int once = 0;
  if (t->kind.dec == IdK) {
    i++;
  }
  auto** Entry1 = new symbolTable*;
  for (; i < t->name.size(); i++) {
    auto** Entry = new symbolTable*;
    if (bool flag = FindEntry(t->name[i], false, Entry)) {
      std::cout << t->lineno
                << "Parameter identifier " + t->name[i] +
                       " Duplicate declarations"
                << std::endl;
    } else {
      auto* a = new AttributeIR;
      a->kind = varKind;
      a->idtype = TypeProcess(t);
      if (once == 0 && t->kind.dec == IdK) {
        FindEntry(t->name[0], false, Entry1);
        once++;
        t->table.push_back(*Entry1);
      }

      a->More.VarAttr.access = dir;
      if (t->attr.procAttr.paramType == VarParamType)
        a->More.VarAttr.access = indir;

      a->More.VarAttr.level = level;

      a->More.VarAttr.off = off;
      off = off + a->idtype->size;

      Enter(t->name[i], a, Entry);
      t->table.push_back(*Entry);
      parat = new ParamTable;
      if (paralist == nullptr) {
        paralist = parat;
      } else {
        dt->next = parat;
      }
      parat->entry = (*Entry);
      parat->next = nullptr;
      dt = parat;
    }
  }
  dt->next = ParaDecList(t->sibling);
  return paralist;
}

// 过程声明部分分析处理函数
void ProcDecPart(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  // 将函数添加进符号表
  auto* procIR = new AttributeIR;
  procIR->kind = procKind;
  procIR->More.ProcAttr.level = level;
  auto* paramT = new ParamTable;
  procIR->More.ProcAttr.param = paramT;
  auto** Entry = new symbolTable*;
  Enter(t->name[0], procIR, Entry);
  t->table.push_back(*Entry);
  const int toff = off;
  CreatTable();

  // 处理形参，并将形参表给函数的符号表内的param项
  (*Entry)->attrIR.More.ProcAttr.param = ParaDecList(t->child[0]);
  traverse(t->child[1]);

  (*Entry)->attrIR.More.ProcAttr.size = off;
  Body(t->child[2]->child[0]);
  DestroyTable();
  off = toff;
  ProcDecPart(t->sibling);
}

// 表达式分析处理函数
typeIR* Expr(TreeNode* t, AccessKind* Ekind) {
  if (t->kind.exp == IdEK) {
    auto** Entry = new symbolTable*;
    bool flag = FindEntry(t->name[0], true, Entry);
    if (!flag) {
      std::cout << t->lineno
                << "expression statement " + t->name[0] + " no declare"
                << std::endl;
    } else {
      t->table.push_back(*Entry);
      if (t->child[0] != nullptr) {
        if ((*Entry)->attrIR.idtype->kind == arrayTy) {
          return arrayVar(t);
        } else if ((*Entry)->attrIR.idtype->kind == recordTy) {
          return recordVar(t);
        }
      } else {
        return (*Entry)->attrIR.idtype;
      }
    }
  } else if (t->kind.exp == ConstK) {
    (*Ekind) = dir;
    return intPtr;
  } else {
    auto* EKindT = new AccessKind;
    typeIR* a = Expr(t->child[0], EKindT);
    typeIR* b = Expr(t->child[1], EKindT);
    if (a == b) {
      if (t->attr.expAttr.op == scanner::LT ||
          t->attr.expAttr.op == scanner::EQ) {
        return boolPtr;
      }
      (*Ekind) = dir;
      return a;
    }
  }
  return nullptr;
}

// 赋值语句分析函数
void assignStatement(const TreeNode* t) {
  if (t->child[0]->kind.exp != IdEK) {
    std::cout << t->lineno
              << "The left end of the assignment statement is not an identifier"
              << std::endl;
  } else {
    auto** Entry = new symbolTable*;
    if (const bool flag = FindEntry(t->child[0]->name[0], true, Entry); !flag) {
      std::cout << t->lineno
                << "identifier " + t->child[0]->name[0] + " no declare"
                << std::endl;
    } else {
      TreeNode* nowt = t->child[0];
      t->child[0]->table.push_back(*Entry);
      const symbolTable* findt = *Entry;
      typeIR* kindleft = findt->attrIR.idtype;
      if (nowt->child[0] != nullptr) {
        if (findt->attrIR.idtype->kind == arrayTy) {
          kindleft = arrayVar(nowt);
        } else if ((*Entry)->attrIR.idtype->kind == recordTy) {
          kindleft = recordVar(nowt);
          if (kindleft->kind == arrayTy)
            kindleft = kindleft->More.ArrayAttr.elemTy;
        }
      }
      auto* Ekind = new AccessKind;
      typeIR* kindright = Expr(t->child[1], Ekind);
      if (kindleft != kindright) {
        std::cout << t->lineno << " left " << kindleft->kind << std::endl;
        std::cout << t->lineno << " right " << kindright->kind << std::endl;
        std::cout << t->lineno
                  << " The left and right types of the assignment statement do "
                     "not match "
                  << std::endl;
      }
    }
  }
}

// 过程调用语句分析处理函数
void callStatement(const TreeNode* t) {
  auto** Entry = new symbolTable*;
  if (const bool flag = FindEntry(t->child[0]->name[0], true, Entry); !flag) {
    std::cout << t->lineno
              << " function " + t->child[0]->name[0] + " no declare "
              << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != procKind) {
      std::cout
          << t->lineno
          << " The identifier is not a function type and cannot be called "
          << std::endl;
    } else {
      t->child[0]->table.push_back(*Entry);
      if (t->child[1] == nullptr) {
        if ((*Entry)->attrIR.More.ProcAttr.param == nullptr) {
          return;
        } else {
          std::cout << t->lineno << " The parameters do not match "
                    << std::endl;
        }
      }
      ParamTable* a = (*Entry)->attrIR.More.ProcAttr.param;
      TreeNode* b = t->child[1];
      while (a != nullptr && b != nullptr) {
        auto* EKind = new AccessKind;
        if (const typeIR* bt = Expr(b, EKind); a->entry->attrIR.idtype != bt) {
          if (bt->kind == arrayTy && a->entry->attrIR.idtype->kind == arrayTy) {
            if (bt->size == a->entry->attrIR.idtype->size &&
                bt->More.ArrayAttr.elemTy ==
                    a->entry->attrIR.idtype->More.ArrayAttr.elemTy) {
            } else {
              std::cout << t->lineno << " The parameters do not match "
                        << std::endl;
            }
          } else {
            std::cout << t->lineno << " Maybe The parameters Type do not match "
                      << std::endl;
          }
        }

        b = b->sibling;
        a = a->next;
      }
      if (a != nullptr || b != nullptr) {
        std::cout << t->lineno << "  The parameters Num do not match "
                  << std::endl;
      }
    }
  }
}

// 条件语句分析处理函数
void ifStatment(const TreeNode* t) {
  TreeNode* ift = t->child[0];
  if (auto* EKind = new AccessKind; Expr(ift, EKind) != boolPtr) {
    std::cout << t->lineno << " if not boolPtr " << std::endl;
  }
  TreeNode* thent = t->child[1];
  while (thent != nullptr) {
    statement(thent);
    thent = thent->sibling;
  }
  TreeNode* elset = t->child[2];
  while (elset != nullptr) {
    statement(elset);
    elset = elset->sibling;
  }
}

// 循环语句分析处理函数
void whileStatement(const TreeNode* t) {
  TreeNode* whilet = t->child[0];
  auto* EKind = new AccessKind;
  if (Expr(whilet, EKind) != boolPtr) {
    std::cout << t->lineno << " while not boolPtr " << std::endl;
  }
  TreeNode* dot = t->child[1];
  while (dot != nullptr) {
    statement(dot);
    dot = dot->sibling;
  }
}

// 读语句分析处理函数
void readStatement(TreeNode* t) {
  auto** Entry = new symbolTable*;
  if (const bool flag = FindEntry(t->name[0], true, Entry); !flag) {
    std::cout << t->lineno
              << " Read the statement identifier " + t->name[0] + " no declare "
              << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != varKind) {
      std::cout << t->lineno
                << " Read the statement identifier " + t->name[0] + " tot var "
                << std::endl;
    }
    t->table.push_back(*Entry);
  }
}

// 写语句分析处理函数
void writeStatement(const TreeNode* t) {
  auto* EKind = new AccessKind;
  Expr(t->child[0], EKind);
}

// 返回语句分析处理程序
void returnStatment(TreeNode* t) {}

// 语句序列分析处理函数
void statement(TreeNode* t) {
  if (t->kind.stmt == IfK) {
    ifStatment(t);
  } else if (t->kind.stmt == WhileK) {
    whileStatement(t);
  } else if (t->kind.stmt == AssignK) {
    assignStatement(t);
  } else if (t->kind.stmt == ReadK) {
    readStatement(t);
  } else if (t->kind.stmt == WriteK) {
    writeStatement(t);
  } else if (t->kind.stmt == CallK) {
    callStatement(t);
  } else if (t->kind.stmt == ReturnK) {
    returnStatment(t);
  }
}

// 执行体部分分析处理函数
void Body(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  while (t != nullptr) {
    statement(t);
    t = t->sibling;
  }
}

// 数组变量的处理分析函数
typeIR* arrayVar(TreeNode* t) {
  auto** Entry = new symbolTable*;
  if (const bool flag = FindEntry(t->name[0], true, Entry); !flag) {
    std::cout << t->lineno << " array has no this value " << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != varKind) {
      std::cout << " array name not var " << std::endl;
    } else {
      if ((*Entry)->attrIR.idtype->kind != arrayTy) {
        std::cout << " array name not array var " << std::endl;
      } else {
        auto* EKind = new AccessKind;
        *EKind = indir;
        if (const typeIR* index_type = Expr(t->child[0], EKind);
            index_type != intPtr) {
          std::cout << "array index not int";
        } else {
          return (*Entry)->attrIR.idtype->More.ArrayAttr.elemTy;
        }
      }
      t->table.push_back(*Entry);
    }
  }
  return nullptr;
}

// 记录变量中域变量的分析处理函数
typeIR* recordVar(TreeNode* t) {
  auto** Entry = new symbolTable*;
  if (const bool flag = FindEntry(t->name[0], true, Entry); !flag) {
    std::cout << t->lineno
              << " This record element is not present in the symbol table "
              << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != varKind) {
      std::cout << t->lineno << " The record name is not a variable "
                << std::endl;
    } else {
      if ((*Entry)->attrIR.idtype->kind != recordTy) {
        std::cout << t->lineno
                  << " The record name is not a record type variable "
                  << std::endl;
      } else {
        const fieldChain* aa = (*Entry)->attrIR.idtype->More.body;
        while (aa != nullptr) {
          if (aa->idName == t->child[0]->name[0]) {
            return aa->unitType;
          }
          aa = aa->next;
        }
        std::cout << t->lineno << " There is no such element in the record "
                  << std::endl;
      }
    }
    t->table.push_back(*Entry);
  }
  return nullptr;
}

// 遍历语法树，调用相应的函数处理语法树结点
void traverse(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  if (t->nodeKind == ProK) {
    traverse(t->child[0]);
    traverse(t->child[1]);
    traverse(t->child[2]);
    return;
  }
  if (t->nodeKind == PheadK) {
    return;
  }
  if (t->nodeKind == TypeK) {
    TypeDecPart(t->child[0]);
    traverse(t->sibling);
    return;
  }
  if (t->nodeKind == VarK) {
    VarDecList(t->child[0]);
    traverse(t->sibling);
    return;
  }
  if (t->nodeKind == ProcDecK) {
    ProcDecPart(t);
    return;
  }
  if (t->nodeKind == StmLK) {
    Body(t->child[0]);
    return;
  }
  if (t->nodeKind == StmtK) {
    Body(t->child[0]);
  }
}

// 语义分析主程序
void Analyze(TreeNode* currentP) {
  CreatTable();
  initialize();
  traverse(currentP);
  InputSymbol(SYMBOL_PATH);
  InputDestroySymbol(SYMBOL_PATH);
  printf("\n");
}

} // namespace parse