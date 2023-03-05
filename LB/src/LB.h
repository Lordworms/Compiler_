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
namespace LB
{
  static bool dflag=true;
  static bool GEN_IR=true;
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
      10.Fname *
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
  class Fname_item;

  class TypeAnno_item;
  class Op_item;
  class NewArr_item;
  class NewTup_item;
  class Length_item;
  
/*
  Instructions
  change: delete condition_jump and uncondition jump
  add: goto continue break while if
*/
  class Instruction;
  class Instruction_ret;
  class Instruction_var_ret;
  class Instruction_assignment;
  //new in LB
  class Instruction_goto;
  class Instruction_while;
  class Instrutcion_if;
  class Instruction_continue;
  class Instructcion_break;
  class Instruction_range;

  class Instruction_call;
  class Instruction_declare;
  class Ins_visitor;
// /*
//   basic Blocks
// */
//   class BasicBlock;
  class Function;
  enum iType{var_item,constant_item,label_item,call_item,type_anno_item,op_item,new_arr_item,new_tup_item,length_item,arr_ele_item,fname_item};
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
      Item* anno;
      std::string var_name;
      Var_item();
      Var_item(std::string name,Item* an);
      std::string print()override;
      Var_item(std::string name);
      Var_item* copy()override;
  };
  class Constant_item:public Item
  {
    public:
      bool is_encoded;
      long long value;
      Constant_item(long long v);
      Constant_item();
      void decode_itself();
      void encode_itself();
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
      bool is_on_func_name;
      Call_item();
      Call_item(bool isRuntime,std::vector<Item*>& argss,Item* calleee);
      Call_item(bool isRuntime,bool isonFuncname,std::vector<Item*>& argss,Item* calleee);
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
      ll line_number;
      std::vector<Item*>eles;
      Arrele_item();
      std::string print()override;
      Arrele_item(Item* b,std::vector<Item*>& e);
      Arrele_item(Item* b,std::vector<Item*>& e,ll line);
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
  class Fname_item: public Item
  {
      public:
        std::string fname;
        Function* fptr;
        Fname_item(std::string name);
        Fname_item(std::string name,Function* fp);
        std::string print()override;
        Fname_item* copy()override;
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

  extern Fname_item fname_print;
  extern Fname_item fname_input;
  extern Fname_item fname_allocate;
  extern Fname_item fname_tensor;
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
    ins_if,
    ins_while,
    ins_continue,
    ins_break,
    ins_range,
    ins_goto,
    ins_declare
  };
  class Instruction{
    public:
      InsType type;
      Instruction_range* scope;
      Instruction();
      Instruction(InsType t);
      void SetType(InsType t);
      virtual std::string print()=0;
      //virtual Instruction* copy()=0;
      virtual void apply(Ins_visitor& vis)=0;
  };
  class Instruction_terminator:public Instruction
  {
    public:
      
  };
  class Instruction_ret :public Instruction_terminator
  { 
    public:
      Instruction_ret(Instruction_range* scope);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
     // Instruction_ret* copy()override;
      
  };
  class Instruction_var_ret: public Instruction_terminator
  {
    public:
      Item* var_ret;
      Instruction_var_ret();
      Instruction_var_ret(Item* var_ret,Instruction_range* scope);
      std::string print()override;
      void apply(Ins_visitor& vis)override;
     // Instruction_var_ret* copy()override;
      
  };
  class Instruction_if:public Instruction
  {
    public:
      Item* location1,*location2;
      Item* condition;
      Instruction_if();
      Instruction_if(Item* loc1,Item* loc2,Item* condition,Instruction_range* scope);
     // Instruction_if* copy()override;
      void apply(Ins_visitor& vis)override;
      std::string print()override;
      
  };
  class Instruction_while:public Instruction{
    public:
      Item* location1,*location2;
      Item* condition;
      Instruction_while();
      Instruction_while(Item* loc1,Item* loc2,Item* condition,Instruction_range* scope);
      void apply(Ins_visitor& vis)override;
      std::string print()override;
  };
  class Instruction_goto:public Instruction{
    public:
      Item* dst;
      Instruction_goto();
      Instruction_goto(Item* dst,Instruction_range* scope);
      void apply(Ins_visitor& vis)override;
      std::string print()override;
  };
    class Instruction_continue:public Instruction{
    public:
      //Instruction_continue();
      Instruction_continue(Instruction_range* scope);
      void apply(Ins_visitor& vis)override;
      std::string print()override;
  };
    class Instruction_break:public Instruction{
    public:
      Instruction_break();
      Instruction_break(Instruction_range* scope);
      void apply(Ins_visitor& vis)override;
      std::string print()override;
  };
  class Instruction_range:public Instruction{
    public:
      std::vector<Instruction*>insts;
      std::unordered_map<std::string,Var_item*>var_ptr_map;
      Instruction_range();
      Instruction_range(Instruction_range* scope);
      void apply(Ins_visitor& vis)override;
      std::string print()override;
  };
  class Instruction_assignment :public Instruction
  {
    public:
      Item *s;
      Item *d;
      Instruction_assignment(Item* s,Item* d,Instruction_range* scope);
      Instruction_assignment();
      std::string print()override;
      void apply(Ins_visitor& vis)override;
      //Instruction_assignment* copy()override;
  };
  class Instruction_label: public Instruction
  {
    public:
      Item* name;
      Instruction_label(Item* name,Instruction_range* scope);
      Instruction_label();
      std::string print()override;
      void apply(Ins_visitor& vis)override;
     //Instruction_label* copy()override;
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
      Instruction_call(bool runtime,Item* var,Instruction_range* scope);
      Instruction_call(Item* caller,Instruction_range* scope);
      //Instruction_call* copy()override;
  };
  class Instruction_declare:public Instruction
  {
    public:
      Item* anno_type;
      std::vector<Item*>vars;
      Instruction_declare(Item* type,std::vector<Item*>& vars,Instruction_range* scope);
      //Instruction_declare* copy()override;
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
      Instruction_range* scope;
      std::unordered_map<std::string,Item*>label_ptr_map;
      std::unordered_map<std::string,Var_item*>args_ptr_map;
      std::set<Item*>label_set;
      std::set<Item*>var_set;
      std::set<Item*>label_insts;
      TypeAnno_item* retType;
      Function();
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

        virtual void visit(Instruction_while* ins)=0;
        virtual void visit(Instruction_if* ins)=0;
        virtual void visit(Instruction_break* ins)=0;
        virtual void visit(Instruction_continue* ins)=0;
        virtual void visit(Instruction_goto* ins)=0;
        virtual void visit(Instruction_range* ins)=0;

        virtual void visit(Instruction_assignment* ins)=0;
        virtual void visit(Instruction_label* ins)=0;
        virtual void visit(Instruction_call* ins)=0;
        virtual void visit(Instruction_declare* ins)=0;
  };

  //some useful function
  std::string op_str(LB::OpType type);
  // std::string cmp_str(L3::CmpType type);
  // AopType get_aop_type(std::string s);
  // CmpType get_cmp_type(std::string s);
  bool check_runtime_call(Item* callee);
  std::string get_anno_str(AnnoType t);
  // std::string cmp_transfer(CmpType type);
}   