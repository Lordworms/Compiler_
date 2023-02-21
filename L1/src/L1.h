#pragma once

#include <vector>
#include <string>

namespace L1 {
  enum iType
  {
    register_item,
    constant_item,
    memoryAccess_item,
    label_item,
    aop_item,
    cmp_item
  };
  enum Register_ID {rdi, rax, rsi, rdx, rcx, r8, r9, rbx, rbp, r10, r11, r12, r13, r14, r15, rsp};
  enum Register_8b_ID {r10b, r11b, r12b, r13b, r14b, r15b, r8b, r9b, al, bpl, bl, cl, dil, dl, sil};
  enum AopType{plus_eq,minus_eq,mul_eq,bitand_eq};
  enum CmpType{eq,le,lt,gt,ge};
  enum ShiftType{left,right};
  struct Item {
      iType type;
  };
  struct Register_item:Item
  {
    Register_ID regID;
  };
  struct Constant_item:Item
  {
    long long value;
  };
  struct Aop_item: Item
  {
    public:
      AopType aType;
  };
  struct Cmp_item:Item
  {
      Item* op1;
      Item* op2;
      CmpType cType;
  };
  struct Memory_item:Item
  {
    Register_ID regId;
    long long offset;
  };
  struct Label_item:Item
  {
    std::string label_name;
  };
  /*
   * Instruction interface.
   */
  enum InsType
  {
    ins_ret,
    ins_assignment,
    ins_crement,
    ins_aop,
    ins_sop,
    ins_label,
    ins_lea,
    ins_goto,
    ins_cjump,
    ins_call,
    ins_inc,
    ins_dec
  };
  struct Instruction{
      InsType type;
  };

  /*
   * Instructions.
   */
  struct Instruction_ret :Instruction
  {

  };
  struct Instruction_label: Instruction
  {
      std::string name;
  };
  struct Instruction_assignment :Instruction
  {
      Item *s;
      Item *d;
  };
  struct Instruction_call:Instruction
  {
    bool isRuntime;
  };
  struct Instruction_rumtime:Instruction_call
  {
    std::string func_name;
    int arg_cnt;
  };
  struct Instruction_inc:Instruction
  {
    Item* op;
  };
  struct Instruction_dec:Instruction
  {
    Item* op;
  };
  struct Instruction_common:Instruction_call
  {
    Item* callee;
    Item* num_cnt;
  };
  struct Instruction_aop:Instruction
  {
    Item *op1,*op2;
    AopType rule;
  };
  struct Instruction_sop:Instruction
  {
    Item *target;
    Item *offset;
    ShiftType direction;
  };
  struct Instruction_lea:Instruction
  {
    Item *des,*src,*cons,*muler;
  };
  struct Instruction_goto:Instruction
  {
    Item* label;
  };
  struct Instruction_cjump:Instruction
  {
    Item *label,*condi;
  };
  /*
   * change class to struct, (stupid inherent)
   */
  struct Function{
      std::string name;
      int64_t arguments;
      int64_t locals;
      std::vector<Instruction *> instructions;
  };

  struct Program{
      std::string entryPointLabel;
      std::vector<Function*> functions;
  };

}
