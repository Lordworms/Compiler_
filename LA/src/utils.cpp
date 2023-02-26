#include "utils.h"

namespace LA
{
/*
    Var transormer
*/
Var_Label_transformer::Var_Label_transformer()
{
    this->longest_label="";
    this->longest_var="";
    this->label_cnt=0;
    this->var_cnt=0;
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
/*
    Encode Visitor
*/
    EncodeVisitor::EncodeVisitor(Var_Label_transformer* trans)
    {
        this->trans=trans;
    }
    void EncodeVisitor::visit(Instruction_ret* ins)
    {
        this->new_ints.push_back(ins);
    }
    void EncodeVisitor::visit(Instruction_var_ret* ins)
    {
        this->new_ints.push_back(ins);
    }
    void EncodeVisitor::visit(Instruction_branch_condi* ins)
    {
        if(ins->condition->type==iType::constant_item)
        {
            auto cons=dynamic_cast<Constant_item*>(ins->condition);
            cons->decode_itself();
        }
        else if(ins->condition->type==iType::var_item)
        {
            auto cons=dynamic_cast<Var_item*>(ins->condition);
            this->add_decode_instruction(cons);
        }
        this->new_ints.push_back(ins);
        
    }
    void EncodeVisitor::visit(Instruction_branch_nocondi* ins)
    {
        this->new_ints.push_back(ins);
    }
    void EncodeVisitor::visit(Instruction_assignment* ins)
    {
        if(ins->d->type==arr_ele_item)
        {
            this->arr_ele_all(dynamic_cast<Arrele_item*>(ins->d));
            this->new_ints.push_back(ins);
        }  
        else//dst is variable
        {

        } 
    }
    void EncodeVisitor::visit(Instruction_label* ins)
    {
        this->new_ints.push_back(ins);
    }
    void EncodeVisitor::visit(Instruction_call* ins)
    {   
        this->new_ints.push_back(ins);
    }
    void EncodeVisitor::visit(Instruction_declare* ins)
    {   
        this->new_ints.push_back(ins);
        if(ins->anno_type==&LA::int64_anno_ex)
        {
            auto cons=new Constant_item(1);
            cons->is_encoded=true;
            auto assign=new Instruction_assignment(cons,ins->var);
            this->new_ints.push_back(assign);
        }
        else if(ins->anno_type==&LA::void_anno_ex||ins->anno_type==&LA::code_anno_ex||ins->anno_type==&LA::tuple_anno_ex)
        {
            auto cons=new Constant_item(0);
            cons->is_encoded=false;
            auto assign=new Instruction_assignment(cons,ins->var);
            this->new_ints.push_back(assign);
        }
    }   
    Item* EncodeVisitor::add_decode_instruction(Item* var)
    {
        auto new_var=new Var_item(this->trans->new_var_name(""));
        auto new_declare=new Instruction_declare(&LA::int64_anno_ex,new_var);
        this->new_ints.push_back(new_declare);
        auto decode_ins=new Instruction_assignment(new Op_item(var,new Constant_item(1),OpType::right_shift),new_var);
        this->new_ints.push_back(decode_ins);
        return new_var;
    }
    void EncodeVisitor::arr_ele_all(Arrele_item* a)
    {
        for(ll i=0;i<a->eles.size();++i)
        {
            auto off=a->eles[i];
            if(off->type==iType::var_item)
            {
                a->eles[i]=this->add_decode_instruction(off);
            }
        }
    }
/*
    other interfaces
*/
    ll encode(ll x)
    {
        return (x<<1)+1;
    }
    ll decode(ll x)
    {
        return x>>1;
    }
   
}