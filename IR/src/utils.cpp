#include "utils.h"

namespace IR
{
/*
    Var transormer
*/
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

    Trace

*/
Trace::Trace()
{

}
void Trace::add_bb(BasicBlock* bb)
{
    this->bb_list.push_back(bb);
}
/*

    TraceGenerator
*/
TraceGenerator::TraceGenerator()
{

}

std::vector<Trace*>TraceGenerator::gen_trace(std::vector<BasicBlock*>bb_list)
{
    std::set<BasicBlock*>traced_bb;
    std::vector<Trace*>res_trace;
    bool traced_entry=false;
    do
    {
       auto tr=new Trace();
       auto bb=fetch_and_remove(traced_bb,traced_entry);
       traced_entry=true;
       while(traced_bb.find(bb)==traced_bb.end())
       {
            traced_bb.insert(bb);
            tr->add_bb(bb);
            auto next_bb=get_next_bb(bb->succs,traced_bb);
            if(next_bb)
            {
                bb=next_bb;
            }
       }
       if(tr->bb_list.empty())
       {
            delete tr;
       } 
       else
       {
            res_trace.push_back(tr);
       }
    } while (!bb_list.empty());
    return res_trace;
}


/*
    other interfaces
*/
    BasicBlock* fetch_and_remove(std::set<BasicBlock*>& bb_list,bool traced_entry)
    {
        
    }
    BasicBlock* get_next_bb(std::set<BasicBlock*>&,std::set<BasicBlock*>&)
    {

    }
}