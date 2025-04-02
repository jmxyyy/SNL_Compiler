#include "../include/config.h"
#include "../include/parse.h"
#include "../include/scanner.h"

#include <fstream>
#include <iostream>

namespace parse {

symbtable Scope[10000];
symbtable DestroyScope[10000];
int destroylevel = 0;
int level = 0;
bool flagtree = false;
symbtable* table = nullptr;
int off = 7;

void CreatTable() {
  off = 7;
  level++;
}

void DestroyTable() {
  DestroyScope[destroylevel].next = Scope[level].next;
  destroylevel++;
  Scope[level].next = nullptr;
  level = level - 1;
}

bool SearchoneTable(const std::string& id, const int currentLevel,
                    symbtable** Entry) {
  symbtable* t = Scope[currentLevel].next;
  while (t != nullptr) {
    if (t->idname == id) {
      (*Entry) = t;
      return true;
    }
    // TODO
    else {
      t = t->next;
    }
  }
  return false;
}

bool FindEntry(const std::string& id, const bool flag, symbtable** Entry) {
  if (flag) {
    for (int i = level; i > 0; i--) {
      if (SearchoneTable(id, i, Entry)) {
        return true;
      }
    }
  } else {
    return SearchoneTable(id, level, Entry);
  }
  return false;
}

bool Enter(const std::string& Id, const AttributeIR* AttribP,
           symbtable** Entry) {
  bool present = FindEntry(Id, false, Entry);
  if (present == true) {
    std::cout << "��ʶ���ظ�����" << std::endl;
  } else {
    auto* newsymbtable = new symbtable;
    newsymbtable->idname = Id;
    newsymbtable->attrIR = *AttribP; // ��ʶ����Ϣ��

    if (Scope[level].next == nullptr) {
      Scope[level].next = newsymbtable;
    } else {
      symbtable* t = Scope[level].next;
      while (t->next != nullptr) {
        t = t->next;
      }
      t->next = newsymbtable;
    }
    (*Entry) = newsymbtable;
  }
  return present;
}

bool FindField(const std::string& Id, fieldChain* head, fieldChain** Entry) {
  fieldChain* r = head;
  while (r != nullptr) {
    if (Id == r->idname) {
      (*Entry) = r;
      return true;
    }
    r = r->next;
  }
  return false;
}

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
void Inputsymb(const std::string& path) {
  std::ofstream ouput;
  ouput.open(path);
  for (int i = 1; i < level + 1; i++) {
    const symbtable* t = Scope[i].next;
    while (t != nullptr) {
      if (t->attrIR.kind == typeKind) {
        ouput << t->idname << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.VarAttr.level
              << std::endl;
      } else if (t->attrIR.kind == varKind) {
        if (t->attrIR.idtype != nullptr) {
          ouput << t->idname << " " << t->attrIR.kind << " " << &t->attrIR
                << " " << t->attrIR.idtype << " "
                << t->attrIR.More.VarAttr.level << " " << t->attrIR.idtype->kind
                << " " << t->attrIR.idtype->size << std::endl;
        } else {
          ouput << t->idname << " " << t->attrIR.kind << " " << &t->attrIR
                << " " << std::endl;
        }
      } else {
        ouput << t->idname << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.ProcAttr.level << " "
              << t->attrIR.More.ProcAttr.param << std::endl;
      }

      t = t->next;
    }
  }
  ouput.close();
}

void Inputdestroysymb(const std::string& path) {
  std::ofstream ouput;
  ouput.open(path, std::ios::app);
  ouput << std::endl << std::endl << std::endl << std::endl << std::endl;
  ouput << "******************************��ɾ�����ű�***************************"
        << std::endl;
  for (int i = 0; i < destroylevel; i++) {
    const symbtable* t = DestroyScope[i].next;
    while (t != nullptr) {
      if (t->attrIR.kind == typeKind) {
        ouput << t->idname << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.VarAttr.level
              << std::endl;
      } else if (t->attrIR.kind == varKind) {
        ouput << t->idname << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.VarAttr.level << " "
              << t->attrIR.idtype->kind << " " << t->attrIR.idtype->size
              << std::endl;
      } else {
        ouput << t->idname << " " << t->attrIR.kind << " " << &t->attrIR << " "
              << t->attrIR.idtype << " " << t->attrIR.More.ProcAttr.level << " "
              << t->attrIR.More.ProcAttr.param << std::endl;
      }

      t = t->next;
    }
  }
  ouput.close();
}

typeIR *intPtr, *charPtr, *boolPtr;

// ��ʼ�����������ڲ���ʾ��������ʼ���������ͣ��ַ����ͣ��������͵��ڲ���ʾ˵���������������;�Ϊ�������ͣ��ڲ���ʾ�̶���
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

typeIR* nameType(const TreeNode* t) {
  auto** Entry = new symbtable*;
  int i = 0;
  if (flagtree) {
    i++;
  }
  bool flag = FindEntry(t->name[i], false, Entry);

  if (!flag) {
    std::cout << t->lineno << "�Զ�������" + t->name[i] + "δ����" << std::endl;
  } else {
    return (*Entry)->attrIR.idtype;
  }

  return nullptr;
}

typeIR* arrayType(const TreeNode* t) {
  if (t->attr.arrayAttr.low > t->attr.arrayAttr.up) {
    std::cout << "�����±�Խ��" << std::endl;
  } else {
    auto* newarrayType = new typeIR;
    newarrayType->kind = arrayTy;
    newarrayType->size = ((t->attr.arrayAttr.up) - (t->attr.arrayAttr.low) + 1);
    newarrayType->More.ArrayAttr.indexTy = intPtr;
    newarrayType->More.ArrayAttr.low = t->attr.arrayAttr.low;
    newarrayType->More.ArrayAttr.up = t->attr.arrayAttr.up;
    if (t->attr.arrayAttr.childType == CharK) {
      newarrayType->More.ArrayAttr.elemTy = charPtr;
    } else {
      newarrayType->More.ArrayAttr.elemTy = intPtr;
    }
    return newarrayType;
  }
  return nullptr;
}

void outputbody(const fieldChain* bodyt) {
  if (bodyt == nullptr) {
  } else {
    std::cout << "  " << bodyt->idname;
    outputbody(bodyt->next);
  }
}

typeIR* recordType(const TreeNode* t) {
  int sum_filed = 0;
  auto* newrecordType = new typeIR;
  newrecordType->kind = recordTy;
  fieldChain *newbody = nullptr, *dt = nullptr;
  TreeNode* nt = t->child[0];
  int recordoffset = 0;
  while (nt != nullptr) {
    for (int i = 0; i < nt->name.size(); i++) {
      if (newbody != nullptr) {
        auto** Entry = new fieldChain*;
        if (bool flag = FindField(nt->name[i], newbody, Entry)) {
          std::cout << t->lineno << "record������" << std::endl;
        }
      }
      auto* newt = new fieldChain;
      if (newbody == nullptr) {
        newbody = newt;
      } else {
        dt->next = newt;
      }
      newt->unitType = TypeProcess(nt);
      newt->idname = nt->name[i];

      newt->offset = recordoffset;
      recordoffset = recordoffset + newt->unitType->size;
      sum_filed += newt->unitType->size;
      newt->next = nullptr;
      dt = newt;
    }
    nt = nt->sibling;
  }
  dt->next = nullptr;
  newrecordType->More.body = newbody;
  // outputbody(newbody);std::cout<<std::endl;
  newrecordType->size = sum_filed;
  return newrecordType;
}

typeIR* TypeProcess(TreeNode* t) {
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

void TypeDecPart(TreeNode* t) {
  if (t == nullptr) {
    return;
  }

  auto** Entry = new symbtable*;
  auto** Entry1 = new symbtable*;
  int i = 0;
  if (t->kind.dec == IdK) {
    i++;
  }
  for (; i < t->name.size(); i++) {
    if (bool flag = FindEntry(t->name[0], false, Entry)) {
      std::cout << t->lineno << "���ͱ�ʶ��" + t->name[0] + "�ظ�����"
                << std::endl;
    } else {
      auto* a = new AttributeIR;
      a->kind = typeKind;
      flagtree = true;
      a->idtype = TypeProcess(t);
      flagtree = false;
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

void VarDecList(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  auto** Entry = new symbtable*;
  auto** Entry1 = new symbtable*;
  int i = 0;
  if (t->kind.dec == IdK) {

    i++;
  }
  int once = 0;
  for (; i < t->name.size(); i++) {

    if (bool flag = FindEntry(t->name[i], false, Entry)) {
      std::cout << t->lineno << "������ʶ��" + t->name[i] + "�ظ�����"
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
  auto** Entry1 = new symbtable*;
  for (; i < t->name.size(); i++) {
    auto** Entry = new symbtable*;
    if (bool flag = FindEntry(t->name[i], false, Entry)) {
      std::cout << t->lineno << "�βα�ʶ��" + t->name[i] + "�ظ�����" << std::endl;
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
      if (t->attr.procAttr.paramt == Varparamtype)
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
      // parat->next =new ParamTable;
      parat->next = nullptr;
      dt = parat;
    }
  }
  dt->next = ParaDecList(t->sibling);
  return paralist;
}

void ProcDecPart(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  auto* procir = new AttributeIR;
  procir->kind = procKind;
  procir->More.ProcAttr.level = level;
  auto* paramt = new ParamTable;
  procir->More.ProcAttr.param = paramt;
  auto** Entry = new symbtable*;
  Enter(t->name[0], procir, Entry);
  t->table.push_back(*Entry);
  int toff = off;
  CreatTable();
  (*Entry)->attrIR.More.ProcAttr.param = ParaDecList(t->child[0]);
  bianli(t->child[1]); // ����

  (*Entry)->attrIR.More.ProcAttr.size = off;
  Body(t->child[2]->child[0]); // ���
  DestroyTable();
  off = toff;
  ProcDecPart(t->sibling);
}

typeIR* Expr(TreeNode* t, AccessKind* Ekind) {
  if (t->kind.exp == IdEK) {
    auto** Entry = new symbtable*;
    bool flag = FindEntry(t->name[0], true, Entry);
    if (!flag) {
      std::cout << t->lineno << "����ʽ�����" + t->name[0] + "δ����"
                << std::endl;
    } else {
      t->table.push_back(*Entry);
      if (t->child[0] != nullptr) {
        if ((*Entry)->attrIR.idtype->kind == arrayTy) {
          // typeIR* tmp = Expr(t->child[0], Ekind);
          // if (tmp != nullptr)
          //{
          //     /*if (Expr(t->child[0], Ekind)->kind == arrayTy)
          //     {
          //         return Expr(t->child[0], Ekind);
          //     }*/
          //     if (tmp->kind != intTy) {
          //         std::cout << "�����±�����������";
          //     }
          //     return (*Entry)->attrIR.idtype->More.ArrayAttr.elemTy;
          // }

          return arrayVar(t);

        } else if ((*Entry)->attrIR.idtype->kind == recordTy) {
          /*  fieldChain* aa = (*Entry)->attrIR.idtype->More.body;
            while (aa != nullptr)
            {
                if (aa->idname == t->child[0]->name[0])
                {
                    return aa->unitType;
                }
                aa = aa->next;
            }
            std::cout << t->lineno << "record��û�д�Ԫ��" << std::endl;*/
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
    auto* Ekindt = new AccessKind;
    typeIR* a = Expr(t->child[0], Ekindt);
    typeIR* b = Expr(t->child[1], Ekindt);
    if (a == b) {
      if (t->attr.expAttr.op == scanner::LT ||
          t->attr.expAttr.op == scanner::EQ) {
        return boolPtr;
      }
      (*Ekind) = dir;
      return a;
    }
    // else {
    //   std::cout << t->lineno << "����ʽ���(���Ҳ�����)���Ͳ�ƥ��" << std::endl;
    // }
  }
  return nullptr;
}

// TypeKind assignstatementpart(TreeNode* t) {
//	if (t->kind.exp == IdEK) {
//		symbtable** Entry = new symbtable*;
//		bool flag = FindEntry(t->name[0], true,Entry);
//		if (!flag)
//			std::cout << "��ֵ�����" + t->name[0] + "δ����";
//		else {
//			if ((*Entry)->attrIR.idtype->kind == arrayTy)
////�������ͷ���Ԫ�ص��ڲ���ʾ��kind
//			{
//				return
//(*Entry)->attrIR.idtype->More.ArrayAttr.elemTy->kind;
//			}
//			else return (*Entry)->attrIR.idtype->kind;
//		}
//	}
//	else if (t->kind.exp == ConstK) {
//		return intTy;
//	}
//	else {
//		TypeKind a=assignstatementpart(t->child[0]);
//		TypeKind b = assignstatementpart(t->child[1]);
//		if (a == b)
//			return a;
//		else std::cout << "��ֵ����ʽ�Ҷ����Ͳ�ƥ��";
//	}
//	return intTy;
// }
void assignstatement(const TreeNode* t) {
  if (t->child[0]->kind.exp != IdEK) {
    std::cout << t->lineno << "��ֵ�����˲��Ǳ�ʶ��" << std::endl;
  } else {
    auto** Entry = new symbtable*;
    bool flag = FindEntry(t->child[0]->name[0], true, Entry);
    if (!flag) {
      std::cout << t->lineno << "��ʶ��" + t->child[0]->name[0] + "δ����"
                << std::endl;
    } else { // nowt�ǵ�ʽ��� ����������\��¼����ʽ  ����Ǳ�ʶ���͵���
      TreeNode* nowt = t->child[0];
      t->child[0]->table.push_back(*Entry);
      const symbtable* findt = *Entry;
      typeIR* kindleft = findt->attrIR.idtype;
      if (nowt->child[0] != nullptr) {
        if (findt->attrIR.idtype->kind == arrayTy) {

          // AccessKind *Ekind = new AccessKind;
          // kindleft = Expr(nowt->child[0], Ekind);
          kindleft = arrayVar(nowt);
        } else if ((*Entry)->attrIR.idtype->kind == recordTy) {
          /*            fieldChain *aa = (*Entry)->attrIR.idtype->More.body;
                      bool flagg = true;
                      while (aa != nullptr)
                      {
                          if (aa->idname == nowt->child[0]->name[0])
                          {
                              kindleft = aa->unitType;
                              flagg = false;
                              break;
                          }
                          aa = aa->next;
                      }
                      if (flagg)
                      { std::cout << t->lineno << "record��û�д�Ԫ��" <<
             std::endl; }*/
          kindleft = recordVar(nowt);
          if (kindleft->kind == arrayTy)
            kindleft = kindleft->More.ArrayAttr.elemTy;
        }
      }
      auto* Ekind = new AccessKind;
      typeIR* kindright = Expr(t->child[1], Ekind);
      if (kindleft != kindright) {
        std::cout << t->lineno << "���" << kindleft->kind << std::endl;
        std::cout << t->lineno << "�ұ�" << kindright->kind << std::endl;
        std::cout << t->lineno << "��ֵ����������Ͳ�ƥ��" << std::endl;
      }
    }
  }
}

void callstatement(const TreeNode* t) {
  auto** Entry = new symbtable*;
  const bool flag = FindEntry(t->child[0]->name[0], true, Entry);
  if (!flag) {
    std::cout << t->lineno << "����" + t->child[0]->name[0] + "δ����"
              << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != procKind) {
      std::cout
          << t->lineno
          << "��ʶ�����Ǻ������ͣ��޷����"
             "�"
          << std::endl;
    } else {
      t->child[0]->table.push_back(*Entry);
      if (t->child[1] == nullptr) {
        if ((*Entry)->attrIR.More.ProcAttr.param == nullptr) {
          return;
        } else {
          std::cout << t->lineno << "������ƥ��" << std::endl;
        }
      }
      // a��Ҫ��� b���Ҵ���
      ParamTable* a = (*Entry)->attrIR.More.ProcAttr.param;
      TreeNode* b = t->child[1];
      while (a != nullptr && b != nullptr) {
        auto* Ekind = new AccessKind;
        typeIR* bt = Expr(b, Ekind);
        if (a->entry->attrIR.idtype != bt) {
          if (bt->kind == arrayTy && a->entry->attrIR.idtype->kind == arrayTy) {
            if (bt->size == a->entry->attrIR.idtype->size &&
                bt->More.ArrayAttr.elemTy ==
                    a->entry->attrIR.idtype->More.ArrayAttr.elemTy) {
            } else {
              std::cout << t->lineno << "������ƥ��" << std::endl;
            }
          } else {
            std::cout << t->lineno << "�����ǲ������Ͳ�ƥ��" << std::endl;
          }
        }

        b = b->sibling;
        a = a->next;
      }
      if (a != nullptr || b != nullptr) {
        std::cout << t->lineno << "����������ƥ��" << std::endl;
      }

      return;
    }
  }
}

void ifstatment(const TreeNode* t) {
  // if�﷨��child[0]Ϊif����ʽ,��Ϊ������
  TreeNode* ift = t->child[0];
  auto* Ekind = new AccessKind;
  if (Expr(ift, Ekind) != boolPtr) {
    std::cout << t->lineno << "if����������ʽ����������boolPtr����" << std::endl;
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

void whilestatement(const TreeNode* t) {
  TreeNode* whilet = t->child[0];
  auto* Ekind = new AccessKind;
  if (Expr(whilet, Ekind) != boolPtr) {
    std::cout << t->lineno << "while�������" << std::endl;
  }
  TreeNode* dot = t->child[1];
  while (dot != nullptr) {
    statement(dot);
    dot = dot->sibling;
  }
}

void readstatement(TreeNode* t) {
  auto** Entry = new symbtable*;
  // for (int i = 0; i < t->name.size(); i++) {}
  bool flag = FindEntry(t->name[0], true, Entry);
  if (!flag) {
    std::cout << t->lineno << "������ʶ��" + t->name[0] + "δ����" << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != varKind) {
      std::cout << t->lineno << "������ʶ��" + t->name[0] + "���Ǳ���"
                << std::endl;
    }
    t->table.push_back(*Entry);
  }
}

// TypeKind writestatementpart(TreeNode* t) {
//	if (t->kind.exp == IdEK) {
//		symbtable** Entry = new symbtable*;
//		bool flag = FindEntry(t->name[0],true, Entry);
//		if (!flag)
//			std::cout <<t->lineno<< "д�����" + t->name[0] + "δ����";
//		else {
//			if ((*Entry)->attrIR.idtype->kind == arrayTy)
////�������ͷ���Ԫ�ص��ڲ���ʾ��kind
//			{
//				return
//(*Entry)->attrIR.idtype->More.ArrayAttr.elemTy->kind;
//			}
//			else return (*Entry)->attrIR.idtype->kind;
//		}
//	}
//	else if (t->kind.exp == ConstK) {
//		return intTy;
//	}
//	else {
//		if (assignstatementpart(t->child[0]) ==
// assignstatementpart(t->child[1])) 			return
// assignstatementpart(t->child[0]); 		else std::cout
//<<t->lineno<<"д������ʽ���Ͳ�ƥ��";
//	}
//	return intTy;
// }
// д��������������
void writestatement(const TreeNode* t) {
  auto* Ekind = new AccessKind;
  Expr(t->child[0], Ekind);
}

// ������������������
void returnstatment(TreeNode* t) {}

void statement(TreeNode* t) {
  if (t->kind.stmt == IfK) {
    ifstatment(t);
  } else if (t->kind.stmt == WhileK) {
    whilestatement(t);
  } else if (t->kind.stmt == AssignK) {
    assignstatement(t);
  } else if (t->kind.stmt == ReadK) {
    readstatement(t);
  } else if (t->kind.stmt == WriteK) {
    writestatement(t);
  } else if (t->kind.stmt == CallK) {
    callstatement(t);
  } else if (t->kind.stmt == ReturnK) {
    returnstatment(t);
  }
}

void Body(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  while (t != nullptr) {
    statement(t);
    t = t->sibling;
  }
}

// ��������Ĵ�����������
typeIR* arrayVar(TreeNode* t) {
  auto** Entry = new symbtable*;
  const bool flag = FindEntry(t->name[0], true, Entry);
  if (!flag) {
    std::cout << t->lineno << "array��û�д�Ԫ��" << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != varKind) {
      std::cout << "�������Ʋ��Ǳ���" << std::endl;
    } else {
      if ((*Entry)->attrIR.idtype->kind != arrayTy) {
        std::cout << "�����������������" << std::endl;
      } else {
        auto* ekind = new AccessKind;
        *ekind = indir;
        typeIR* index_type = Expr(t->child[0], ekind);
        if (index_type != intPtr) {
          std::cout << "�����±����ͷ�����";
        } else {
          return (*Entry)->attrIR.idtype->More.ArrayAttr.elemTy;
        }
      }
      t->table.push_back(*Entry);
    }
  }
  return nullptr;
}

// ��¼������������ķ�����������
typeIR* recordVar(TreeNode* t) {
  auto** Entry = new symbtable*;
  bool flag = FindEntry(t->name[0], true, Entry);
  if (!flag) {
    std::cout << t->lineno << "���ű���û�д˼�¼Ԫ��" << std::endl;
  } else {
    if ((*Entry)->attrIR.kind != varKind) {
      std::cout << t->lineno << "��¼���Ʋ��Ǳ���" << std::endl;
    } else {
      if ((*Entry)->attrIR.idtype->kind != recordTy) {
        std::cout << t->lineno << "��¼�����Ǽ�¼���ͱ���" << std::endl;
      } else {
        fieldChain* aa = (*Entry)->attrIR.idtype->More.body;
        while (aa != nullptr) {
          if (aa->idname == t->child[0]->name[0]) {
            return aa->unitType;
          }
          aa = aa->next;
        }
        std::cout << t->lineno << "record��û�д�Ԫ��" << std::endl;
      }
    }
    t->table.push_back(*Entry);
  }
  return nullptr;
}

void bianli(TreeNode* t) {
  if (t == nullptr) {
    return;
  }
  if (t->nodekind == ProK) {
    bianli(t->child[0]);
    bianli(t->child[1]);
    bianli(t->child[2]);
    return;
  }
  if (t->nodekind == PheadK) {
    return;
  }
  if (t->nodekind == TypeK) {
    TypeDecPart(t->child[0]);
    bianli(t->sibling);
    return;
  }
  if (t->nodekind == VarK) {
    VarDecList(t->child[0]);
    bianli(t->sibling);
    return;
  }
  if (t->nodekind == ProcDecK) {
    ProcDecPart(t);
    return;
  }
  if (t->nodekind == StmLK) {
    Body(t->child[0]);
    return;
  }
  if (t->nodekind == StmtK) {
    Body(t->child[0]);
  }
}

void Analyze(TreeNode* currentP) {
  CreatTable();
  initialize();
  bianli(currentP);
  Inputsymb(SYMBOL_PATH);
  Inputdestroysymb(SYMBOL_PATH);
  printf("\n");
}

} // namespace parse