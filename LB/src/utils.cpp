#include "utils.h"
namespace LB
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
std::string Var_Label_transformer::find_longest_var_scope(Instruction_range* scope)
{
    std::string res;
    for(auto& k:scope->var_ptr_map)
    {
        if(k.first.size()>res.size())
        {
            res=k.first;
        }
    }
    for(auto ins:scope->insts)
    {
        if(ins->type==InsType::ins_range)
        {
            auto scope_str=find_longest_var_scope(dynamic_cast<Instruction_range*>(ins));
            if(scope_str.size()>res.size())
            {
                res=scope_str;
            }
        }
    }
    return res;
}
std::string Var_Label_transformer::find_longest_var(Program& p)
{
   std::string res;
   for(auto F:p.functions)
   {
        for(auto& k:F->args_ptr_map)
        {
            if(k.first.size()>res.size())
            {
                res=k.first;
            }
        }
        auto tmp=find_longest_var_scope(F->scope);
        if(tmp.size()>res.size())
        {
            res=tmp;
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
void Var_Label_transformer::transform_var_scope(Instruction_range* scope)
{
    std::unordered_map<std::string,Var_item*>mp;
    for(auto &k:scope->var_ptr_map)
    {
        auto var=k.second;
        var->var_name=this->new_var_name("");
        mp[var->var_name]=var;
    }
    for(auto ins:scope->insts)
    {
        if(ins->type==InsType::ins_range)
        {
            transform_var_scope(dynamic_cast<Instruction_range*>(ins));
        }
    }
    scope->var_ptr_map=mp;
}
void Var_Label_transformer::transform_var(Program& p)
{
    for(auto F:p.functions)
    {
        transform_var_scope(F->scope);
    }   
}
void Var_Label_transformer::get_beg_end_label_scope(Instruction_range* scope,LB_WHILE & beg_while,LB_WHILE& end_while)
{
    for(auto ins:scope->insts)
    {
        if(ins->type==InsType::ins_while)
        {
            auto while_ins=dynamic_cast<Instruction_while*>(ins);
            auto label_beg=dynamic_cast<Label_item*>(while_ins->location1);
            auto label_end=dynamic_cast<Label_item*>(while_ins->location2);
            beg_while[label_beg]=while_ins;
            end_while[label_end]=while_ins;
        }
        if(ins->type==InsType::ins_range)
        {
            get_beg_end_label_scope(dynamic_cast<Instruction_range*>(ins),beg_while,end_while);
        }
    }
}
std::map<Instruction_while *,Label_item*> Var_Label_transformer::get_new_condi_label(Function* F)
{   
    std::map<Instruction_while *,Label_item*>while_label;
    this->get_new_condi_label_scope(F->scope,while_label);
    return while_label;
}
void Var_Label_transformer::get_new_condi_label_scope(Instruction_range* scope,std::map<Instruction_while *,Label_item*>& while_label)
{
    for(auto inst:scope->insts)
    {
        if(inst->type==InsType::ins_while)
        {
            auto while_inst=dynamic_cast<Instruction_while*>(inst);
            auto label=new Label_item(this->new_var_name(""));
            while_label[while_inst]=label;
        }
        if(inst->type==InsType::ins_range)
        {
            get_new_condi_label_scope(dynamic_cast<Instruction_range*>(inst),while_label);
        }
    }
}
void Var_Label_transformer::get_ins_while_map_scope(Instruction_range* scope,std::map<Instruction *,Instruction_while*>& ins_while_mp,std::stack<Instruction_while *>& loop_st,LB_WHILE& beg_while,LB_WHILE& end_while)
{
    for(auto ins:scope->insts)
    {
        if(loop_st.size()>0)
        {
            ins_while_mp[ins]=loop_st.top();
        }
        if(ins->type==InsType::ins_range)
        {
            get_ins_while_map_scope(dynamic_cast<Instruction_range*>(ins),ins_while_mp,loop_st,beg_while,end_while);
        }
        if(ins->type==InsType::ins_label)
        {
            auto label_inst=dynamic_cast<Instruction_label*>(ins);
            auto label=dynamic_cast<Label_item*>(label_inst->name);
            if(beg_while.find(label)!=beg_while.end())
            {
                loop_st.push(beg_while[label]);
            }
            else if(end_while.find(label)!=end_while.end())
            {
                loop_st.pop();
            }
        }
    }
}
std::map<Instruction *,Instruction_while*> Var_Label_transformer::get_ins_while_map(Function* F)
{
    std::map<Instruction *,Instruction_while*>ins_while_mp;
    std::stack<Instruction_while *>loop_st;
    LB_WHILE beg_while_mp;
    LB_WHILE end_while_mp;
    get_beg_end_label_scope(F->scope,beg_while_mp,end_while_mp);
    get_ins_while_map_scope(F->scope,ins_while_mp,loop_st,beg_while_mp,end_while_mp);
    return ins_while_mp;
}


/*
    code generator
*/
CodeVisitor:: CodeVisitor(std::ofstream* out,Var_Label_transformer* trans,std::map<Instruction*,Instruction_while*>*ins_while_map,std::map<Instruction_while*,Label_item*>*while_entry_map)
{
    this->out=out;
    this->trans=trans;
    this->ins_while_map=ins_while_map;
    this->while_entry_map=while_entry_map;
}
void CodeVisitor::visit(Instruction_ret* ins)
{   
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_var_ret* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_goto* ins)
{
    OUT<<'\t'<<"br:"<<ins->dst->print()<<'\n';
}
void CodeVisitor::visit(Instruction_if* ins)
{
    /*
        if(condi):true :false

        int64 v
        v<-condi
        br condi true false
    */
   auto tmp_var=new Var_item(this->trans->new_var_name(""),&LB::int64_anno_ex);
   std::vector<Item*>dec_vec={tmp_var};
   
   auto declare=new Instruction_declare(&LB::int64_anno_ex,dec_vec,ins->scope);
   declare->apply(*this);

   auto assign=new Instruction_assignment(ins->condition,tmp_var,nullptr);
   assign->apply(*this);

   OUT<<"\tbr "<<tmp_var->print()<<' '<<ins->location1->print()<<' '<<ins->location2->print()<<'\n';

}
void CodeVisitor::visit(Instruction_while* ins)
{
    /*
    
        while(condi) :true :false
        
        :beg
        int64 v<- condi
        br condi :beg :end

        :end
    */
    auto beg_label=(*this->while_entry_map)[ins];
    auto beg_label_inst=new Instruction_label(beg_label,nullptr);
    beg_label_inst->apply(*this);

    auto new_var=new Var_item(this->trans->new_var_name(""),&LB::int64_anno_ex);
    std::vector<Item*>vars={new_var};
    auto declare=new Instruction_declare(&LB::int64_anno_ex,vars,nullptr);
    declare->apply(*this);

    auto assign=new Instruction_assignment(new_var,ins->condition,nullptr);
    assign->apply(*this);

    OUT<<'\t'<<"br "<<ins->location1->print()<<' '<<ins->location2->print()<<'\n';
}
void CodeVisitor::visit(Instruction_continue* ins)
{
    auto while_inst=(*this->ins_while_map)[ins];
    auto beg=(*this->while_entry_map)[while_inst];

    OUT<<"\tbr "<<beg->print()<<'\n';
}
void CodeVisitor::visit(Instruction_break* ins)
{
    auto while_inst=(*this->ins_while_map)[ins];
    auto end=while_inst->location2;
    OUT<<"\tbr "<<end->print()<<'\n';
}
void CodeVisitor::visit(Instruction_range* ins)
{
    OUT<<'\n';
    for(auto I:ins->insts)
    {
        I->apply(*this);
    }
    OUT<<'\n';
}
void CodeVisitor::visit(Instruction_assignment* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_label* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_call* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_declare* ins)
{
    for(auto var_raw:ins->vars)
    {
        auto var=dynamic_cast<Var_item*>(var_raw);
        OUT<<'\t'<<var->anno->print()<<' '<<var->print()<<'\n';
    }
}
}
