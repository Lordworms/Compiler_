#pragma once
#include "L3.h"
#include <fstream>
#include "utils.h"
#define OUTT *this->out
namespace L3
{
    class L2_visitor :public Ins_visitor
    { 
        public:
            L2_visitor(std::ofstream* out);
            Function* F;
            Var_Label_transformer* trans;
            std::ofstream* out;
            long long args_number;
            std::unordered_set<Item*>reads_args;
            std::unordered_map<Item*,long long>indexs;
            Cmp_item* find_cmp(Var_item* v,Instruction* ins);
            std::string print_cmp(Cmp_item* cmp);
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_var_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_branch* ins)override;
            void visit(Instruction_call* ins)override;
            void print_args(Instruction_call*);
            void print_runtime(Instruction_call*);
            void print_common(Instruction_call *);
            void translate_load(Load_item*);
            void translate_store(Store_item*);
            void print_first_read_instruction();
            void print_first_read_item(std::set<Item*>&used);
            void ins_read(Instruction* I,std::set<Item*>& vars);
            void item_read(Item* it,std::set<Item*>& vars);
            void init_read();
            void init_read_flag();
    };
    bool check_cmp_reverse(Cmp_item* cmp);
    long long check_pre_cmp(Cmp_item* cmp);
    void generate_program(Program& p,Var_Label_transformer*);
}