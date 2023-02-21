#include "spiller.h"
namespace L2
{
    FunctionSpiller::FunctionSpiller(Function* F,Var_item* now_var,Var_item* prefixx)
    {
        this->F=F;
        this->var_now_spill=now_var;
        this->prefix=prefixx;
        this->sv=new Spill_visitor(F,now_var,prefixx);
    }
    void FunctionSpiller::spill()
    {
        if(F->name_var_map.find(var_now_spill->print())==F->name_var_map.end())
        {
            return;
        }
        F->locals++;
        for(Instruction* ins:F->instructions)
        {
            ins->apply(*this->sv);
        }
        F->instructions=sv->new_insts;
    }
    void FunctionSpiller::print_spilled_function()
    {
        std::cout<<'('<<this->F->name<<'\n';
        std::cout<<'\t'<<this->F->arguments<<' '<<this->F->locals<<'\n';
        for(Instruction* ins:this->F->instructions)
        {
            std::cout<<'\t'<<ins->print();
        }
        std::cout<<")\n";

    }
    void SpillFunction(Program& p)
    {
        for(auto f:p.functions)
        {
            FunctionSpiller sf(f,p.now_spill_var,p.prefix);
            sf.spill();
            sf.print_spilled_function();
        }
    }
}