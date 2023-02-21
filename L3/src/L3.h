#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#define CALLEE_NUM 6
#define CALLER_NUM 9

namespace L3 {
  inline static bool isPrint=false;
  using ll=long long;
  /*
    In L3, we have the following items:
      1. register: used for transferring
      2. constant 
      3. label
      4. compare
      5. load
      6. store
      7. variable
      8. aop item
      9. call_item
      No other items?



    For Instruction, we just have
      1.return
      2.variable_return
      3.assignment
      4.branch
      5.call
      6.label
  */
 /*
    items
 */
  class Item;
  class Register_item;
  class Label_item;
  class Cmp_item;
  class Load_item;
  class Store_item;
  class Var_item;
  class Aop_item;
  class Call_item;
/*
  Instructions

*/
  class Instruction_ret;
  class Instruction_var_ret;
  class Instruction_assignment;
  class Instruction_branch;
  class Instruction_call;
  class Ins_visitor;
  
  enum iType{register_item,constant_item,label_item,cmp_item,var_item,aop_item,call_item,load_item,store_item};
  enum Register_ID {rdi, rax, rsi, rdx, rcx, r8, r9, rbx, rbp, r10, r11, r12, r13, r14, r15, rsp};
  enum CmpType{eq,le,lt,gt,ge};
  enum AopType{plus,minus,multy,bit_and,left_shift,right_shift};
  
  
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
      Item *op1, *op2;
      AopType aType;
      Aop_item(Item* o1,Item* o2,AopType type);
      Aop_item();
      std::string print()override;
      Aop_item* copy()override;
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
  class Load_item: public Item
  {
    public:
      Item* var;
      Load_item();
      std::string print()override;
      Load_item(Item* v);
      Load_item* copy()override;
  };
  class Store_item: public Item
  {
    public:
      Item* ptr;
      Store_item();
      std::string print()override;
      Store_item(Item* v);
      Store_item* copy()override;
  };
  class Call_item:public Item
  {
    public:
      std::vector<Item*>args;
      Item* callee;
      bool isRuntime;
      Call_item();
      Call_item(bool isRuntime,std::vector<Item*>& argss,Item* calleee);
      Call_item* copy()override;
      std::string print()override;
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
  //define std::vector for use
  extern std::vector<Register_item*>callee_saved;
  extern std::vector<Register_item*>caller_saved;
  extern std::vector<Register_item*>args_reg;
  extern std::vector<Register_item*>gp_reg;
  //define runtime call
  extern Label_item runtime_print;
  extern Label_item runtime_input;
  extern Label_item runtime_allocate;
  extern Label_item runtime_tensor;
  

  /*
   * Instruction interface.
   */
  enum InsType
  {
    ins_ret,
    ins_var_ret,
    ins_assignment,
    ins_label,
    ins_call,
    ins_branch
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
  class Instruction_var_ret: public Instruction
  {
    public:
      Item* var_ret;
      Instruction_var_ret();
      Instruction_var_ret(Item* var_ret);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_var_ret* copy()override;
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
  class Instruction_call:public Instruction
  {
    public:
      Item* var_ret;
      bool isRuntime;
      Item* caller;
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_call();
      Instruction_call(bool runtime,Item* var);
      Instruction_call* copy()override;
  };
  class Instruction_branch:public Instruction
  {
    public:
      Item* location;
      Item* condition;
      Instruction_branch();
      Instruction_branch(Item* location,Item* condition);
      Instruction_branch* copy()override;
      void apply(Ins_visitor& vis)override;
      std::string print()override;
  };
  class Function{
    public:
      std::string name;
      std::vector<Item*>args;
      std::unordered_map<std::string,Item*>name_var_map;
      std::unordered_map<std::string,Item*>label_ptr_map;
      std::set<Item*>label_set;
      std::set<Item*>var_set;
      std::vector<Instruction *> instructions;
      Function();
      Function* copy();
  };
  /*
    Function
  
  */
  class Program{
      public:
        std::string var_prefix="",ret_var_prefix="";
        Function* mainF;
        std::vector<Function*> functions;
        Program();
  };
  /*
    visitor base class for Instruction
  */
  class Ins_visitor //base class
  {
    public:
        virtual void visit(Instruction_ret* ins)=0;
        virtual void visit(Instruction_var_ret* ins)=0;
        virtual void visit(Instruction_branch* ins)=0;
        virtual void visit(Instruction_assignment* ins)=0;
        virtual void visit(Instruction_label* ins)=0;
        virtual void visit(Instruction_call* ins)=0;
  };
  //some useful function
  std::string aop_str(L3::AopType type);
  std::string cmp_str(L3::CmpType type);
  AopType get_aop_type(std::string s);
  CmpType get_cmp_type(std::string s);
  bool check_runtime_call(Item* callee);
  std::string cmp_transfer(CmpType type);
}
