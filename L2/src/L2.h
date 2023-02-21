#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <unordered_map>
#include <stack>
#define CALLEE_NUM 6
#define CALLER_NUM 9
namespace L2 {
  inline static bool isPrint=false;
  class Item;
  class Label_item;
  class Instruction_ret;
  class Instruction_assignment;
  class Instruction_aop;
  class Instruction_call;
  class Instruction_cjump;
  class Instruction_common;
  class Instruction_runtime;
  class Instruction_dec;
  class Instruction_inc;
  class Instruction_label;
  class Instruction_lea;
  class Instruction_sop;
  class Instruction_goto;
  
  enum iType
  {
    register_item,
    constant_item,
    memoryAccess_item,
    label_item,
    aop_item,
    cmp_item,
    var_item,
    stackArg_item
  };
  enum Register_ID {rdi, rax, rsi, rdx, rcx, r8, r9, rbx, rbp, r10, r11, r12, r13, r14, r15, rsp};
  enum Reg_color{rdi_color, rax_color, rsi_color, rdx_color, rcx_color, r8_color, r9_color, rbx_color, rbp_color, r10_color, r11_color, r12_color, r13_color, r14_color, r15_color, rsp_color};
  enum Register_8b_ID {r10b, r11b, r12b, r13b, r14b, r15b, r8b, r9b, al, bpl, bl, cl, dil, dl, sil};
  enum AopType{plus_eq,minus_eq,mul_eq,bitand_eq};
  enum CmpType{eq,le,lt,gt,ge};
  enum ShiftType{left,right};
  class Item {
    public:
      iType type;
      Item();
      Item(iType t):type(t){}
      void setType(iType type);
      virtual std::string print()=0;
      virtual Item* copy()=0;
  };
  class Register_item:public Item
  {
    public:
      Register_ID regID;
      Register_item();
      Register_item(Register_ID id);
      std::string print()override;
      Register_item* copy()override;
  };
  //register
  //callee_saved
  extern Register_item reg_r12;
  extern Register_item reg_r13;
  extern Register_item reg_r14;
  extern Register_item reg_r15;
  extern Register_item reg_rbp;
  extern Register_item reg_rbx;
  //caller_saved
  extern Register_item reg_rdi;
  extern Register_item reg_rsi;
  extern Register_item reg_rcx;
  extern Register_item reg_r8;
  extern Register_item reg_r9;
  extern Register_item reg_rax;
  extern Register_item reg_r10;
  extern Register_item reg_r11;
  extern Register_item reg_rdx;
  //stack ptr
  extern Register_item reg_rsp;
  //define vector for use
  extern std::vector<Register_item*>callee_saved;
  extern std::vector<Register_item*>caller_saved;
  extern std::vector<Register_item*>args_reg;
  extern std::vector<Register_item*>gp_reg;
  //define runtime call
  extern Label_item runtime_print;
  extern Label_item runtime_input;
  extern Label_item runtime_allocate;
  extern Label_item runtime_tensor;
  //define coloring map
  extern std::unordered_map<Reg_color,Item*>color_reg_mp;
  extern std::unordered_map<Item*,Reg_color>reg_color_mp;
  //define color order from callersave to calle save
  extern std::vector<Reg_color>color_order;
  // extern std::unordered_map<AopType,std::string>aop_str_map;
  // extern std::unordered_map<CmpType,std::string>cmp_str_map;
  // extern std::unordered_map<ShiftType,std::string>sop_str_map;
  
  class Constant_item:public Item
  {
    public:
      long long value;
      Constant_item(long long v);
      Constant_item();
      std::string print()override;
      Constant_item* copy()override;
  };
  class Aop_item:public Item
  {
    public:
      AopType aType;
      Aop_item(AopType type);
      Aop_item();
      std::string print()override;
      Aop_item* copy()override;
  };
  class StackArg_item:public Item
  {
   public:
    Item* offset;
    StackArg_item(Item* it);
    StackArg_item();
    std::string print()override;
    StackArg_item* copy()override;
  };
  class Cmp_item:public Item
  {
    public:
      Item* op1;
      Item* op2;
      CmpType cType;
      Cmp_item();
      std::string print()override;
      Cmp_item(Item* o1,Item* o2,CmpType t);
      Cmp_item* copy()override;
  };
  class Memory_item:public Item
  {
    public:
      Item* regId;//change!!!
      Item* offset;
      Memory_item();
      std::string print()override;
      Memory_item(Item* id,Item* offset);
      Memory_item* copy()override;
  };
  class Label_item:public Item
  {
    public:
      std::string label_name;
      Label_item();
      std::string print()override;
      Label_item(std::string name);
      Label_item* copy()override;
  };
  class Var_item:public Item
  {
    public:
      std::string var_name;
      Var_item();
      std::string print()override;
      Var_item(std::string name);
      Var_item* copy()override;
  };
  class Ins_visitor //base class
  {
    public:
        virtual void visit(Instruction_ret* ins)=0;
        virtual void visit(Instruction_assignment* ins)=0;
        virtual void visit(Instruction_aop* ins)=0;
        virtual void visit(Instruction_cjump* ins)=0;
        virtual void visit(Instruction_common* ins)=0;
        virtual void visit(Instruction_runtime* ins)=0;
        virtual void visit(Instruction_dec* ins)=0;
        virtual void visit(Instruction_inc* ins)=0;
        virtual void visit(Instruction_label* ins)=0;
        virtual void visit(Instruction_lea* ins)=0;
        virtual void visit(Instruction_sop* ins)=0;
        virtual void visit(Instruction_goto* ins)=0;
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
  class Instruction{
    public:
      InsType type;
      Instruction();
      Instruction(InsType t);
      void SetType(InsType t);
      virtual std::string print()=0;
      virtual Instruction* copy()=0;
      virtual void apply(Ins_visitor& vis)=0;
  };
  class Instruction_ret :public Instruction
  { 
    public:
      Instruction_ret();
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_ret* copy()override;
  };
  class Instruction_label: public Instruction
  {
    public:
      Item* name;
      Instruction_label(Item* name);
      Instruction_label();
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_label* copy()override;
  };
  class Instruction_assignment :public Instruction
  {
    public:
      Item *s;
      Item *d;
      Instruction_assignment(Item* s,Item* d);
      Instruction_assignment();
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_assignment* copy()override;
  };
  class Instruction_call:public Instruction
  {
    public:
      bool isRuntime;
      Instruction_call();
      Instruction_call(bool runtime);
      //Instruction_call* copy()override;
      //std::string print()override;
      //void apply(Ins_visitor& vis)override;
  };
  class Instruction_runtime:public Instruction_call
  {
    public:
      //std::string func_name;
      Item* func_name;
      Item* arg_cnt;
      Instruction_runtime();
      Instruction_runtime(std::string name,int cnt);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_runtime* copy()override;
  };
  class Instruction_inc:public Instruction
  {
    public:
      Item* op;
      Instruction_inc();
      Instruction_inc(Item* o);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_inc* copy()override;
  };
  class Instruction_dec:public Instruction
  {
    public:
      Item* op;
      Instruction_dec();
      Instruction_dec(Item* o);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_dec* copy()override;
  };
  class Instruction_common:public Instruction_call
  {
    public:
      Item* callee;
      Item* num_cnt;
      Instruction_common();
      Instruction_common(Item* calle,Item* cnt);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_common* copy()override;
  };
  class Instruction_aop:public Instruction
  {
    public:
      Item *op1,*op2;
      AopType rule;
      Instruction_aop();
      Instruction_aop(Item* o1,Item* o2,AopType r);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_aop* copy()override;
  };
  class Instruction_sop:public Instruction
  {
    public:
      Item *target;
      Item *offset;
      ShiftType direction;
      Instruction_sop();
      Instruction_sop(Item* t,Item* o,ShiftType st);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_sop* copy()override;
  };
  class Instruction_lea:public Instruction
  {
    public:
      Item *des,*src,*cons,*muler;
      Instruction_lea(Item* d,Item* s,Item* c,Item* m);
      Instruction_lea();
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_lea* copy()override;
  };
  class Instruction_goto:public Instruction
  {
    public:
      Item* label;
      Instruction_goto();
      Instruction_goto(Item* l);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_goto* copy()override;
  };
  class Instruction_cjump:public Instruction
  {
    public:
      Item *label,*condi;
      Instruction_cjump();
      Instruction_cjump(Item* l,Item* c);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_cjump* copy()override;
  };
  class Function{
    public:
      std::string name;
      int64_t arguments;
      int64_t locals;
      std::unordered_map<std::string,Item*>name_var_map;
      std::unordered_map<std::string,Item*>label_ptr_map;
      std::vector<Instruction *> instructions;
      Function();
      Function* copy();
  };
  class Program{
      public:
        Var_item* now_spill_var;
        Var_item* prefix;
        std::string entryPointLabel;
        std::vector<Function*> functions;
        Program();
  };

  //some useful function
  std::string sop_str(AopType type);
  std::string aop_str(ShiftType type);
  std::string cmp_str(CmpType type);
}
