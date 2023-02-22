#pragma once
#include "utils.h"
namespace IR
{
    void gen_code(Program& p);
    void gen_trace_code(std::vector<Trace*>traces,CodeVisitor& cv,std::fstream& out);
    void print_args(std::ofstream& out,std::vector<Item*>&args);
    bool can_merge(BasicBlock* now,BasicBlock* next);
}