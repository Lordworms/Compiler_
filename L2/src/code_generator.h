#pragma once
#include "utils.h"
namespace L2
{
    class Code_generator_L2_to_L1
    {
        public:
            std::ofstream* out;
            Program* p;
            CodeL1_visitor cl1v;
            void print_code();
            Code_generator_L2_to_L1(std::ofstream* out,Program* p);
    };
    void generate_code(Program & p);
}