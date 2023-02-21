#include <string>
#include <iostream>
#include <fstream>
#include "L1.h"
#include <code_generator.h>

using namespace std;
const int STACK_VAL_SIZE=8;
namespace L1{
  bool printFlag=false;
  void print_header(ofstream& out)
  {
    out << ".text\n";
    out << "  .globl go\n";
    out << "go:\n";
  }
  void push_caller_save_reg(ofstream& out)
  {
    out << "pushq %rbx"<<"\n";
    out << "pushq %rbp"<<"\n";
    out << "pushq %r12"<<"\n";
    out << "pushq %r13"<<"\n";
    out << "pushq %r14"<<"\n";
    out << "pushq %r15"<<"\n";
  }
  void pop_caller_save_reg(ofstream& out)
  {
    out << "popq %rbx"<<"\n";
    out << "popq %rbp"<<"\n";
    out << "popq %r12"<<"\n";
    out << "popq %r13"<<"\n";
    out << "popq %r14"<<"\n";
    out << "popq %r15"<<"\n";
  }
  void print_entry(ofstream& out,Program& p)
  {
    out<<"call _"<<p.entryPointLabel.substr(1,p.entryPointLabel.size()-1)<<"\n";
  }
  void allocate_stack(ofstream& out,long long size)
  {
    out<<"subq "<<"$"<<size<<", "<<"%rsp"<<"\n";
  }
  void check_local_variable(ofstream& out,Function* f)
  {
    if(f->locals>0)
    {
      allocate_stack(out,f->locals*STACK_VAL_SIZE);
    }
  }
  void deallocate_stack(ofstream& out,long long size)
  {
    out<<"addq "<<"$"<<size<<", "<<"%rsp"<<"\n";
  }
  void deallocate(ofstream& out,Function* f)
  {
    
    int num=f->locals+max(f->arguments-6,(int64_t)0);
    if(printFlag)std::cout<<"the argument number is"<<num<<'\n';
    if(num>0)
    {
      deallocate_stack(out,num*STACK_VAL_SIZE);
    }
  }
  Register_8b_ID trans_reg_id(Register_ID id)
  {
    switch (id)
    {
    case Register_ID::rdi:
      return Register_8b_ID::dil;
      break;
    case Register_ID::rsi:
      return Register_8b_ID::sil;
      break;
    case Register_ID::rax:
      return Register_8b_ID::al;
      break;
    case Register_ID::rcx:
      return Register_8b_ID::cl;
      break;
    case Register_ID::rdx:
      return Register_8b_ID::dl;
      break;
    case Register_ID::rbp:
      return Register_8b_ID::bpl;
      break;
    case Register_ID::rbx:
      return Register_8b_ID::bl;
      break;
    case Register_ID::r10:
      return Register_8b_ID::r10b;
      break;
    case Register_ID::r11:
      return Register_8b_ID::r11b;
      break;
    case Register_ID::r9:
      return Register_8b_ID::r9b;
      break;
    case Register_ID::r8:
      return Register_8b_ID::r8b;
      break;
    case Register_ID::r12:
      return Register_8b_ID::r12b;
      break;
    case Register_ID::r13:
      return Register_8b_ID::r13b;
      break;
    case Register_ID::r14:
      return Register_8b_ID::r14b;
      break;
    case Register_ID::r15:
      return Register_8b_ID::r15b;
      break;
    default:
      std::cerr<<"transfer to 8 bit register failed!"<<'\n';
      return al;
    }
  }
  void print_8bregister(std::ofstream & out, Register_8b_ID rtype) {
    out << '%';
    switch(rtype) {
      case r10b : 
        out << "r10b";
        break;
      case r11b : 
        out << "r11b";
        break;
      case r12b:
        out << "r12b";
        break;
      case r13b : 
        out << "r13b";
        break;
      case r14b : 
        out << "r14b";
        break;
      case r15b:
        out << "r15b";
        break;
      case r8b : 
        out << "r8b";
        break;
      case r9b : 
        out << "r9b";
        break;
      case al:
        out << "al";
        break;
      case bpl : 
        out << "bpl";
        break;
      case bl : 
        out << "bl";
        break;
      case cl:
        out << "cl";
        break;
      case dil : 
        out << "dil";
        break;
      case dl : 
        out << "dl";
        break;
      case sil:
        out << "sil";
        break;
      default:
        std::cerr << "Erorr Register_8b_type in output_8bregister!\n";
        break;
    }
  }
  void print_register(ofstream& out,Register_ID id)
  {
    out<<"%";
    switch (id)
    {
      case Register_ID::rdi:
      {
        out<<"rdi";
        break;
      }
      case Register_ID::rbx:
      {
        out<<"rbx";
        break;
      }
      case Register_ID::rsi:
      {
        out<<"rsi";
        break;
      }
      case Register_ID::rax:
      {
        out<<"rax";
        break;
      }
      case Register_ID::rdx:
      {
        out<<"rdx";
        break;
      }
      case Register_ID::r8:
      {
        out<<"r8";
        break;
      }
      case Register_ID::r9:
      {
        out<<"r9";
        break;
      }
      case Register_ID::r10:
      {
        out<<"r10";
        break;
      }
      case Register_ID::r11:
      {
        out<<"r11";
        break;
      }
      case Register_ID::r12:
      {
        out<<"r12";
        break;
      }
      case Register_ID::r13:
      {
        out<<"r13";
        break;
      }
      case Register_ID::r14:
      {
        out<<"r14";
        break;
      }
      case Register_ID::r15:
      {
        out<<"r15";
        break;
      }
      case Register_ID::rcx:
      {
        out<<"rcx";
        break;
      }
      case Register_ID::rsp:
      {
        out<<"rsp";
        if(printFlag)std::cout<<"printing rsp!\n";
        break;
      }
      case Register_ID::rbp:
      {
        out<<"rbp";
        break;
      }
      default:
        break;
    }
  }
  void print_ret(ofstream& out)
  {
    out<<"retq"<<"\n";
  }
  void print_movzbq(ofstream& out,Register_ID id)
  {
    out<<"movzbq ";
    print_8bregister(out,trans_reg_id(id));
    out<<", ";
    print_register(out,id);
    out<<"\n";
  }
  CmpType reverse_cmp(CmpType type)//used to deal with cmp 10 rax situation
  {
    switch (type)
    {
      case CmpType::ge:
      {
        return CmpType::le;
        break;
      }
      case CmpType::eq:
      {
        return CmpType::eq;
        break;
      }
      case CmpType::le:
      {
        return CmpType::ge;
        break;
      }
      case CmpType::lt:
      {
        return CmpType::gt;
        break;
      }
      case CmpType::gt:
      {
        return CmpType::lt;
        break;
      }
      default:
        cout<<"error reverse"<<'\n';
        return CmpType::eq;
        break;
    }
  }
  int cmp_res(long long v1,long long v2,CmpType type)
  {
    switch (type)
    {
      case CmpType::eq:
      {
        return v1==v2;
        break;
      }
      case CmpType::le:
      {
        return v1<=v2;
        break;
      }   
      case CmpType::lt:
      {
        return v1<v2;
        break;
      }
      case CmpType::ge:
      {
        return v1>=v2;
        break;
      }
      case CmpType::gt:
      {
        return v1>v2;
        break;
      }
      default:
        cout<<"false cmp type"<<'\n';
        return false;
        break;
    }
  }
  void print_enter(ofstream& out,int cnt)
  {
    for(int i=0;i<cnt;++i)
    {
      out<<"\n";
    }
  }
   void print_label_def(ofstream& out,std::string& label_name)
  {
    if(printFlag)std::cout<<"we are printing a label defination\n";
    out<<"$_"<<label_name.substr(1,label_name.size()-1);
  }
  void print_constant(ofstream& out,long long val)
  {
    out<<"$"<<val;
  }
  void print_memory(ofstream& out,Register_ID regId,long long offset)
  {
    if(printFlag)std::cout<<"we are printing memory! and the offset is "<<offset<<" the register id is"<<regId<<'\n';
    out<<offset<<'(';
    print_register(out,regId);
    out<<')';
  }
  void print_label(ofstream& out,std::string label_name)
  {
    out<<"_"<<label_name.substr(1,label_name.size()-1)<<":";
  }
  void print_item(ofstream& out,Item* it)
  {
    if(printFlag)std::cout<<"the item type is"<<it->type<<"\n";
    switch (it->type)
    {
      case iType::register_item:
      {
        Register_item* tmp=(Register_item*)it;
        if(printFlag)cout<<"print a register and its id is "<<tmp->regID<<'\n';
        print_register(out,tmp->regID);
        break;
      }
      case iType::constant_item:
      {
        Constant_item* c=(Constant_item*)it;
        print_constant(out,c->value);
        break;
      }
      case iType::memoryAccess_item:
      {
        Memory_item* m=(Memory_item*)(it);
        print_memory(out,m->regId,m->offset);
        if(printFlag)std::cout<<"finished memory print\n";
        break;
      }
      case iType::label_item:
      {
        if(printFlag)std::cout<<"printing a label"<<'\n';
        Label_item* lb=(Label_item*)(it);
        print_label_def(out,lb->label_name);
        break;
      }
    }
  }
  void print_set_cmp(ofstream& out,CmpType type)
  {
    switch(type) {
        case CmpType::lt:
          out << "setl";
          break;

        case CmpType::le:
          out << "setle";
          break;

        case CmpType::eq:
          out << "sete";
          break;

        case CmpType::gt :
          out << "setg";
          break;

        case CmpType::ge:
          out << "setge";
          break;

        default:
          break;
      }
  }
  void print_assignment(ofstream& out,Instruction_assignment* ins)
  {
    if(ins->s->type==iType::cmp_item)
    {
      Cmp_item* c=(Cmp_item*)ins->s;
      CmpType cmp=c->cType;
      if(c->op1->type==iType::constant_item&&c->op2->type==iType::constant_item)// all constant, can calculate in precompile
      {
        long long v1=((Constant_item*)(c->op1))->value;
        long long v2=((Constant_item*)(c->op2))->value;
        int cmp=cmp_res(v1,v2,c->cType);
        out<<"movq ";
        print_constant(out,(long long)cmp);
        out<<",";
        print_item(out,ins->d);
        out<<'\n';
      }
      else
      {
        out<<"cmpq ";
        if(c->op1->type==iType::constant_item)//we need to print constant first
        {
          print_item(out,c->op1);
          out<<",";
          print_item(out,c->op2);
          out<<"\n";
          cmp=reverse_cmp(cmp);
        }
        else
        {
          print_item(out,c->op2);
          out<<',';
          print_item(out,c->op1);
          out<<"\n";
        }
        print_set_cmp(out,cmp);
        out<<" ";
        Register_item* reg=(Register_item*)ins->d;
        print_8bregister(out,trans_reg_id(reg->regID));
        out<<"\n";
        print_movzbq(out,reg->regID);
      }
    }
    else
    {
        if(printFlag)cout<<"it is a common assignment!"<<'\n';
        out<<"movq ";
        print_item(out,ins->s);
        out<<",";
        if(printFlag)std::cout<<"finished the first operand\n";
        print_item(out,ins->d);
        if(printFlag)std::cout<<"finished the second operand\n";
        out<<"\n";
    }
  }
  void print_jmp_label(ofstream& out,std::string label_name)
  {
    if(printFlag)std::cout<<"we are printing a jmp_label inside\n";
    out<<"_"<<label_name.substr(1,label_name.size()-1)<<'\n';
  }
  void print_jmp_inst(ofstream& out,Label_item* label)
  {
    out<<"jmp ";
    print_jmp_label(out,label->label_name);
    out<<"\n";
  }
  void print_jmp_cmp(ofstream& out,CmpType type)
  {
    switch(type) {
      case CmpType::lt:
        out << "jl";
        break;

      case CmpType::le:
        out << "jle";
        break;

      case CmpType::eq:
        out << "je";
        break;

      case CmpType::gt :
        out << "jg";
        break;

      case CmpType::ge :
        out << "jge";
        break;

      default:
        std::cerr << "Error cmp type!\n";
    }   
  }
  void print_common_call(ofstream& out,Instruction_common* ins)
  {
    int cnt=1;//ret
    Constant_item* c=(Constant_item*)(ins->num_cnt);
    cnt+=max(c->value-6,0ll);
    allocate_stack(out,cnt*STACK_VAL_SIZE);
    out<<"jmp ";
    //jum *%rdi
    if(ins->callee->type==iType::register_item)
    {
      out<<"*";
      print_item(out,ins->callee);
      out<<'\n';
    }
    //direct call
    else
    {
      print_jmp_label(out,((Label_item*)(ins->callee))->label_name);
    }
  }
  void print_runtime_call(ofstream& out,Instruction_rumtime* ins)
  {
    out<<"call ";
    if(ins->func_name=="tensor-error")
    {
      switch (ins->arg_cnt)
      {
      case 1:
      {
        out<<"array_tensor_error_null";
        break;
      }
      case 4:
      {
        out << "tensor_error";
        break;
      }
      case 3:
      {
        out << "array_error";
        break;
      }
      default:
        break;
      }
    }
    else
    {
      out<<ins->func_name;
    }
    out<<"\n";
  }
  void print_call(ofstream& out,Instruction_call* ins)
  {
    if(ins->isRuntime)
    {
      if(printFlag)std::cout<<"we are calling runtime shit\n";
      print_runtime_call(out,(Instruction_rumtime*)(ins));
    }
    else
    {
      if(printFlag)std::cout<<"we are calling common shit\n";
      print_common_call(out,(Instruction_common*)(ins));
    }
  }
  void print_aop(ofstream& out,Instruction_aop* aop)
  {
    if(printFlag)cout<<"we are printing aop!"<<'\n';
    //trans from a+=b to addq b a
    switch (aop->rule)
    {
      case AopType::plus_eq:
      {
        out<<"addq ";
        break;
      }
      case AopType::mul_eq:
      {
        out<<"imulq ";
        break;
      }
      case AopType::minus_eq:
      {
        out<<"subq ";
        break;
      }
      case AopType::bitand_eq:
      {
        out<<"andq ";
        break;
      }   
      default:
        break;
    }
    print_item(out,aop->op2);
    out<<",";
    print_item(out,aop->op1);
    out<<"\n";
    if(printFlag)cout<<"finishing printing aop!"<<'\n';
  }
  void print_sop(ofstream& out,Instruction_sop* ins)
  {
    // a>>=b > sarq op2 op1
    if(printFlag)std::cout<<"we are doing sop!\n";
    switch (ins->direction)
    {
      case ShiftType::left:
        out<<"salq ";
        break;
      case ShiftType::right:
        out<<"sarq ";
        break;
      default:
        break;
    }
    if(ins->offset->type==iType::register_item)
    {
      Register_item* reg=(Register_item*)ins->offset;
      print_8bregister(out,trans_reg_id(reg->regID));
    } 
    else
    {
      print_item(out,ins->offset);
    }
    out<<",";
    print_item(out,ins->target);
    out<<"\n";
  }
  //rax @ rdi rsi 4  ==>> lea (%rdi, %rsi, 4), %rax
  void print_lea(ofstream& out,Instruction_lea* ins)
  {
    out<<"lea (";
    print_item(out,ins->src);
    out<<", ";
    print_item(out,ins->muler);
    out<<", ";
    Constant_item* c=(Constant_item*)ins->cons;
    out<<c->value;
    out<<"), ";
    print_item(out,ins->des);
    out<<"\n";
  }
  void print_goto(ofstream& out,Instruction_goto* ins)
  {
    if(printFlag)std::cout<<"we are printing a goto instruction\n";
    out<<"jmp ";
    Label_item* lab=((Label_item*)(ins->label));
    print_jmp_label(out,lab->label_name);
    out<<"\n";
  }
  void print_inc(ofstream& out,Instruction_inc* ins)
  {
    //++ --
    out<<"inc ";
    Register_item* reg=(Register_item*)ins->op;
    print_register(out,reg->regID);
    out<<"\n";
  }
  void print_dec(ofstream& out,Instruction_dec* ins)
  {
    out<<"dec ";
    Register_item* reg=(Register_item*)ins->op;
    print_register(out,reg->regID);
    out<<"\n";
  }
  void print_label_inst(ofstream& out,std::string& label_name)
  {
    if(printFlag)std::cout<<"printing _label_inst!\n";
    out<<"_"<<label_name.substr(1,label_name.size()-1)<<':'<<"\n";
  }

  //cjump rax <= rdi :yes => cmpq %rdi, %rax jle _yes
  void print_cjump(ofstream& out, Instruction_cjump* ins)
  {
    Cmp_item* c=(Cmp_item*)ins->condi;
    if(c->op1->type==iType::constant_item&&c->op2->type==iType::constant_item)
    {
      long long v1=((Constant_item*)(c->op1))->value;
      long long v2=((Constant_item*)(c->op2))->value;
      int cmp=cmp_res(v1,v2,c->cType);
      if(cmp)//success
      {
        Label_item* lab=(Label_item*)(ins->label);
        print_jmp_inst(out,lab);
      }
    }
    else //can not calculate in precompile
    {
      out<<"cmpq ";
      CmpType cmptype=c->cType;
      if(c->op1->type==iType::constant_item)
      {
        print_item(out,c->op1);
        out<<", ";
        print_item(out,c->op2);
        out<<"\n";
        cmptype=reverse_cmp(cmptype);
      }
      else
      {
        print_item(out,c->op2);
        out<<", ";
        print_item(out,c->op1);
        out<<"\n";
      }
      print_jmp_cmp(out,cmptype);
      out<<" ";
      Label_item* it=(Label_item*)ins->label;
      print_jmp_label(out,it->label_name);
      out<<"\n";
    }
  }
  void print_instruction(ofstream& out,Instruction* ins,Function* f)
  {
    if(printFlag)cout<<"instruction type is"<<ins->type<<'\n';
    switch (ins->type)
    {
      case InsType::ins_ret:
      { 
          deallocate(out,f);
          print_ret(out);
          break;
      }
      case InsType::ins_assignment:
      { 
          print_assignment(out,(Instruction_assignment*)ins);
          break;
      }
      case InsType::ins_call:
      {
        print_call(out,(Instruction_call*)(ins));
        break;
      }
      case InsType::ins_aop:
      { 
        print_aop(out,(Instruction_aop*)(ins));  
        break;
      }
      case InsType::ins_sop:
      { 
        print_sop(out,(Instruction_sop*)(ins));
        break;
      }
      case InsType::ins_cjump:
      { 
        print_cjump(out,(Instruction_cjump*)(ins));  
        break;
      }
      case InsType::ins_goto:
      { 
        print_goto(out,(Instruction_goto*)(ins));  
        break;
      }
      case InsType::ins_label:
      { 
        auto label=((Label_item*)(ins))->label_name;
        print_label_inst(out,label);
        break;
      }
      case InsType::ins_lea:
      { 
        print_lea(out,(Instruction_lea*)(ins));
        break;
      }
      case InsType::ins_inc:
      { 
        print_inc(out,(Instruction_inc*)(ins));
        break;
      }
      case InsType::ins_dec:
      { 
        print_dec(out,(Instruction_dec*)(ins));
        break;
      }
    }
  }
  void print_function(ofstream& out,Function* fun)
  {
    print_label(out,fun->name);
    out<<'\n';
    check_local_variable(out,fun);
    for(auto& inst:fun->instructions)
    {
      print_instruction(out,inst,fun);
    }
  }
  void generate_code(Program p){
    std::ofstream out;
    out.open("prog.S");
    //pre-process
    print_header(out);
    if(printFlag)std::cout<<"start to print instruction\n\n\n\n";
    push_caller_save_reg(out);
    print_entry(out,p);
    pop_caller_save_reg(out);
    print_ret(out);
    print_enter(out,2);
    for(auto& f:p.functions)
    {
      if(printFlag)std::cout<<"printing function and the function name is "<<f->name<<'\n';
      print_function(out,f);
      print_enter(out,2);
    }
    out.close();
    return ;
  }
}
