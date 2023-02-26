#pragma once
#include "utils.h"
namespace LA
{
    void encode_program(Program& p);
    class PreCheck
    {
        public:
            PreCheck(Var_Label_transformer* trans);
            Var_Label_transformer* trans;
            std::vector<Instruction*>insts;
            void check_alloc(Arrele_item* v);
            void check_boundary(Arrele_item* v);
            void check_single(Arrele_item* v);
            void check_multi(Arrele_item* v);
            Var_item* add_encode_instruction_new(Item* v);

    };
    void adding_basic_block(Program& p,Var_Label_transformer* trans);
    bool is_terminator(Instruction* ins);
}