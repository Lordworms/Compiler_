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
namespace IR
{
  static bool dflag=true;
  #define debug if(dflag)std::cout
  inline static bool isPrint=false;
  using ll=long long;
  /*
    In IR, we have the following items:
      1.variables
      2.constant
      3.labels
      4.type annotator
      5.Arr_element
      6.op
      7.new array
      8.new tuple
      9.length
      No other items?



    For Instruction, we just have
      1.return
      2.variable_return
      3.assignment
      4.branch_nocond
      5.branch_cond
      7.inst_call
      6.label
      7.ins_declare

    Also, In IR, we have an extra interface which is the BasicBlock
  */
 /*
    items
 */
  class Item;
  class Var_item;
  class Constant_item;
  class Label_item;
  class Call_item;

  class TypeAnno_item;
  class Op_item;
  class NewArr_item;
  class NewTup_item;
  class Length_item;
  
/*
  Instructions

*/
  class Instruction_ret;
  class Instruction_var_ret;
  class Instruction_assignment;
  class Instruction_branch_nocondi;
  class Instruction_branch_condi;
  class Instruction_call;
  class Instruction_declare;
  class Ins_visitor;
/*
  basic Blocks
*/
  class BasicBlock;

  enum iType{var_item,constant_item,label_item,call_item,type_anno_item,op_item,new_arr_item,new_tup_item,length_item,arr_ele_item};
  enum OpType{plus,minus,multy,bit_and,left_shift,right_shift,eq,le,lt,gt,ge};
  enum AnnoType{void_anno,tuple_anno,int64_anno,tensor_anno,code_anno};
  
  class Item {
    public:
      iType type;
      Item();
      Item(iType t):type(t){}
      void setType(iType type);
      virtual std::string print()=0;
      virtual Item* copy()=0;
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
  class Constant_item:public Item
  {
    public:
      long long value;
      Constant_item(long long v);
      Constant_item();
      std::string print()override;
      Constant_item* copy()override;
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
  class TypeAnno_item:public Item
  {
    public:
      TypeAnno_item(AnnoType t);
      TypeAnno_item(AnnoType at,ll d);
      AnnoType a_type;
      ll dim;
      TypeAnno_item* copy()override;
      std::string print()override;
  };
  class Op_item:public Item
  {
    public:
      Item* op1,*op2;
      OpType o_type;
      Op_item();
      Op_item(OpType t,Item* o1,Item* o2);
      Op_item(Item* o1,Item* o2,OpType t);
      std::string print()override;
      Op_item* copy()override;
  };
  class Arrele_item:public Item
  {
    public:
      Item* base;
      std::vector<Item*>eles;
      Arrele_item();
      std::string print()override;
      Arrele_item(Item* b,std::vector<Item*>& e);
      Arrele_item* copy()override;
  };
  class NewArr_item :public Item
  {
    public:
      NewArr_item(std::vector<Item*>&di);
      std::vector<Item*>dims;
      std::string print()override;
      NewArr_item* copy()override;
  };
  class NewTup_item :public Item
  {
    public:
      NewTup_item(Item* siz);
      Item* siz;
      std::string print()override;
      NewTup_item* copy()override;
  };
  class Length_item :public Item
  {
      public:
        Item* base;
        Item* dim;
        Length_item(Item* b,Item* d);

        std::string print()override;
        Length_item* copy()override;
  };

  //define runtime label
  extern Label_item runtime_print;
  extern Label_item runtime_input;
  extern Label_item runtime_allocate;
  extern Label_item runtime_tensor;

  extern TypeAnno_item void_anno_ex;
  extern TypeAnno_item int64_anno_ex;
  extern TypeAnno_item tuple_anno_ex;
  extern TypeAnno_item code_anno_ex;
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
    ins_branch_nocondi,
    ins_branch_condi,
    ins_declare
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
  class Instruction_terminator:public Instruction
  {
    public:
      virtual std::set<BasicBlock*> get_successor(std::map<Label_item*,BasicBlock*>& mp)=0;
  };
  class Instruction_ret :public Instruction_terminator
  { 
    public:
      Instruction_ret();
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_ret* copy()override;
      std::set<BasicBlock*> get_successor(std::map<Label_item*,BasicBlock*>& mp)override;
  };
  class Instruction_var_ret: public Instruction_terminator
  {
    public:
      Item* var_ret;
      Instruction_var_ret();
      Instruction_var_ret(Item* var_ret);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      Instruction_var_ret* copy()override;
      std::set<BasicBlock*> get_successor(std::map<Label_item*,BasicBlock*>& mp)override;
  };
  class Instruction_branch_condi:public Instruction_terminator
  {
    public:
      Item* location1,*location2;
      Item* condition;
      Instruction_branch_condi();
      Instruction_branch_condi(Item* loc1,Item* loc2,Item* condition);
      Instruction_branch_condi* copy()override;
      void apply(Ins_visitor& vis)override;
      std::string print()override;
      std::set<BasicBlock*> get_successor(std::map<Label_item*,BasicBlock*>& mp)override;
  };
  class Instruction_branch_nocondi:public Instruction_terminator
  {
    public:
      Item* location;
      Instruction_branch_nocondi();
      Instruction_branch_nocondi(Item* location);
      Instruction_branch_nocondi* copy()override;
      void apply(Ins_visitor& vis)override;
      std::string print()override;
      std::set<BasicBlock*> get_successor(std::map<Label_item*,BasicBlock*>& mp)override;
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
  class Instruction_declare:public Instruction
  {
    public:
      Item* anno_type;
      Item* var;
      Instruction_declare(Item* type,Item* v);
      Instruction_declare* copy()override;
      std::string print()override;
      void apply(Ins_visitor& vis)override;
  };
  /*
  
  */
  class BasicBlock
  {
    public:
      Instruction* beg_label;
      Instruction* terminator;
      std::set<BasicBlock*>succs,preds;
      std::vector<Instruction*>insts;
      BasicBlock();
      void print();
  };
  class Function{
    public:
      std::string name;
      std::vector<Item*>args;
      std::unordered_map<std::string,Item*>name_var_map;
      std::unordered_map<std::string,Item*>label_ptr_map;
      std::set<Item*>label_set;
      std::set<Item*>var_set;
      std::vector<BasicBlock*>blocks;
      Item* retType;
      Function();
      //Function* copy();
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
        virtual void visit(Instruction_branch_condi* ins)=0;
        virtual void visit(Instruction_branch_nocondi* ins)=0;
        virtual void visit(Instruction_assignment* ins)=0;
        virtual void visit(Instruction_label* ins)=0;
        virtual void visit(Instruction_call* ins)=0;
        virtual void visit(Instruction_declare* ins)=0;
  };

  //some useful function
  std::string op_str(IR::OpType type);
  // std::string cmp_str(L3::CmpType type);
  // AopType get_aop_type(std::string s);
  // CmpType get_cmp_type(std::string s);
  bool check_runtime_call(Item* callee);
  std::string get_anno_str(AnnoType t);
  // std::string cmp_transfer(CmpType type);
}   