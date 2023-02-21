#pragma once
#include "utils.h"
#include "L2.h"
namespace L2
{
    class FunctionSpiller//used to spill a single variable var_now_spill,may modify to multiple in the future for optimization
    {
        public:
            Function* F;
            Var_item* var_now_spill,*prefix;
            Spill_visitor* sv;
            FunctionSpiller(Function* F,Var_item* ,Var_item*);
            std::vector<Var_item*>get_replacement();
            void spill();
            void print_spilled_function();
    };
    void SpillFunction(Program& p);
}