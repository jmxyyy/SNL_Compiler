 //
 // Created by jmxyyy.
 //

 #ifndef OPTIM_H
 #define OPTIM_H

 #include "../include/midcode.h"

 #include <string>
 #include <unordered_map>
 #include <vector>

 namespace optim {

 // struct ConstDefT;

 struct ConstDefT {
   ConstDefT* former = nullptr;
   midcode::ArgRecord* variable;
   int constValue = 0;
   ConstDefT* next = nullptr;
 };

 inline ConstDefT* firstConstDefT;
 inline ConstDefT* lastConstDefT;

 struct BaseBlocktype {
   midcode::MidcodeList* begin;
   midcode::MidcodeList* end;
 };

 inline std::vector<BaseBlocktype> BaseBlock;
 inline std::vector<midcode::ArgRecord*> VarDefSet;

 struct LoopInfo {
   int state;
   midcode::MidcodeList* whileEntry;
   midcode::MidcodeList* whileEnd;
   int varDef;
 };

 inline std::vector<LoopInfo> LoopInfostack;

 inline std::unordered_map<int, std::string> codekind_output;

 midcode::MidcodeList* LoopOpti();
 midcode::MidcodeList* ConstOptimize();
 void OutBaseBlock();

 } // namespace optim
 #endif // OPTIM_H
