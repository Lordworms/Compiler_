#include "utils.h"
namespace L3
{
//liveness visitor
Liveness_visitor::Liveness_visitor()
{

}
void Liveness_visitor:: visit(Instruction_ret* ins)
{

}
void Liveness_visitor:: visit(Instruction_var_ret* ins)
{   
    VARS used;
    get_used(ins->var_ret,used);
    GEN[ins].insert(used.begin(),used.end());
}
void Liveness_visitor:: visit(Instruction_assignment* ins)
{
   VARS used;
   if(ins->d->type==iType::store_item)
   {
        get_used(ins->d,used);
   }
   get_used(ins->s,used);
   this->GEN[ins].insert(used.begin(),used.end());
   if(ins->d->type==iType::var_item)
   {
        this->KILL[ins].insert(ins->d);
   }
}
void Liveness_visitor:: visit(Instruction_label* ins)
{

}
void Liveness_visitor:: visit(Instruction_call* ins)
{
    /*
        gen: we used the args, so every args should be in the gen set
        kill: if this call contains a return value, then it is in the kill set
    */
    VARS used;
    get_used(ins->caller,used);
    this->GEN[ins].insert(used.begin(),used.end());
    if(ins->var_ret!=nullptr)
    {
      VARS kill_used;
      get_used(ins->var_ret,kill_used);
      this->KILL[ins].insert(kill_used.begin(),kill_used.end());  
    }
}
void Liveness_visitor:: visit(Instruction_branch* ins)
{
    VARS used;
    if(ins->condition!=nullptr)
    {
        get_used(ins->condition,used);
        this->GEN[ins].insert(used.begin(),used.end());
    }
}
Successor_visitor::Successor_visitor()
{

}
void Successor_visitor:: visit(Instruction_ret* ins)
{
    this->successor[ins].clear();
}
void Successor_visitor:: visit(Instruction_var_ret* ins)
{
    this->successor[ins].clear();
}
void Successor_visitor:: visit(Instruction_assignment* ins)
{
    return;
}
void Successor_visitor:: visit(Instruction_label* ins)
{
    return;
}
void Successor_visitor:: visit(Instruction_call* ins)
{
    /*
        runtime_tensor did not has successor
    */
   if(auto callee=dynamic_cast<Call_item*>(ins->caller)->callee)
   {
        if(auto label=dynamic_cast<Label_item*>(callee))
        {
            if(label==&runtime_tensor)
            {
                this->successor[ins].clear();
            }
        }
   }    
}
void Successor_visitor:: visit(Instruction_branch* ins)
{
    if(ins->condition==nullptr)
    {
        this->successor[ins].clear();
    }
    this->successor[ins].push_back(this->label_map[ins->location]);
}
void Successor_visitor::combine_label(Function* f)
{
    for(Instruction* I:f->instructions)
    {
        if(I->type==InsType::ins_label)
        {
            if(isPrint)std::cout<<"find a label inst\n";
            Instruction_label* tmp=(Instruction_label*)I;
            this->label_map[tmp->name]=I;
        }
    }
}
void Successor_visitor::find_successor(Function* f)
{
   combine_label(f);
   int len=f->instructions.size();
   if(isPrint)std::cout<<"total have "<<len<<" instructions\n";
   for(int i=0;i<len-1;++i)
   {
        this->successor[f->instructions[i]].push_back(f->instructions[i+1]);
   }
   for(int i=0;i<len;++i)
   {
        f->instructions[i]->apply(*this);
   }
}
/*
used for liveness analysis
*/
void get_used(Item* it,VARS& used)
{
    if(isPrint)std::cout<<"we are doing used shit! and the item type is "<<it->type<<'\n';
    switch (it->type)
    {
        case iType::cmp_item:
        {
            auto cmp=(Cmp_item*)it;
            get_used(cmp->op1,used);
            get_used(cmp->op2,used);
            break;
        }
        case iType::aop_item:
        {
            auto aop=(Aop_item*)it;
            get_used(aop->op1,used);
            get_used(aop->op2,used);
            break;
        }
        case iType::load_item:
        {
            auto load=(Load_item*)it;
            get_used(load->var,used);
            break;
        }
        case iType::store_item:
        {
            auto store=(Store_item*)it;
            get_used(store->ptr,used);
            break;
        }
        case iType::call_item:
        {
            auto call=(Call_item*)it;
            get_used(call->callee,used);
            for(auto arg:call->args)
            {
                get_used(arg,used);
            }
            break;
        }
        case iType::var_item:
        {
            used.push_back(it);
            break;    
        }
        default:
        {
            if(isPrint)std::cout<<"invalid used type!\n";
            break;
        }
    }
}

Var_Label_transformer::Var_Label_transformer(std::string suf)
{
    this->suffix=suf;
    this->longest_label="";
    this->longest_var="";
    this->label_cnt=0;
    this->var_cnt=0;
}
std::string Var_Label_transformer::find_longest_var(Program& p)
{
    std::string res="";
    for(auto F:p.functions)
    {
        for(auto& pi:F->name_var_map)
        {
            if(res.size()<pi.first.size())
            {
                res=pi.first;
            }
        }
    }
    this->longest_var=res;
    return res;
}
std::string Var_Label_transformer::find_longest_label(Program& p)
{
    std::string res="";
    for(auto F:p.functions)
    {
        for(auto &pi:F->label_ptr_map)
        {
            if(res.size()<pi.first.size())
            {
                res=pi.first;
            }
        }
    }
    if(res.size()&&res[0]=='@')res[0]=':';
    this->longest_label=res;
    return res;
}
std::string Var_Label_transformer::new_label_name(std::string name)
{
    return this->longest_label+this->suffix+std::to_string(this->label_cnt++);
}
std::string Var_Label_transformer::new_var_name(std::string name)
{
    return this->longest_var+this->suffix+std::to_string(this->var_cnt++);
}   
void Var_Label_transformer::transform_label(Program& p)
{
    for(auto F:p.functions)
    {
        for(auto ptr:F->label_set)
        {
            auto label=(Label_item*)ptr;
            std::string new_label_name=this->new_label_name(label->label_name);
            F->label_ptr_map.erase(label->label_name);
            label->label_name=new_label_name;
            F->label_ptr_map[new_label_name]=ptr;
        }
    }
}
void Var_Label_transformer::transform_var(Program& p)
{
    for(auto F:p.functions)
    {
        for(auto ptr:F->var_set)
        {
            auto var=(Var_item*)ptr;
            std::string new_var_name=this->new_var_name(var->var_name);
            F->name_var_map.erase(var->var_name);
            var->var_name=new_var_name;
            F->name_var_map[new_var_name]=ptr;
        }
    }
}
Context::Context()
{
    this->instructions=std::vector<Instruction*>();
}
void Context::push_inst(Instruction* ins)
{
    this->indexs[ins]=this->instructions.size();
    this->instructions.push_back(ins);
}
long long Context::size()
{
    return this->instructions.size();
}
void init_context(Function* F,std::vector<Context*>&conts)
{
    conts.clear();
    Context* c=new Context();
    conts.push_back(c);
    for(auto& I:F->instructions)
    {
        c->push_inst(I);
        auto & t=I->type;
        if(t==InsType::ins_label||t==InsType::ins_branch||t==InsType::ins_var_ret||t==InsType::ins_ret)
        {
            /*
                :label1
                :label2
            */
            if(c->size()>0)
            {
                c=new Context();
                conts.push_back(c);
            }
        }
    }
    if(conts.back()->size()==0)
    {
        conts.pop_back();
    }
    return;
}
}