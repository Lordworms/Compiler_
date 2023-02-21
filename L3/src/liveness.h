#pragma once
#include "utils.h"
namespace L3
{
    class FunctionLiveness
    {
        public:
            SET IN,OUT,KILL,GEN;
            Function* f;
            Liveness_visitor lv;
            Successor_visitor sv;
            FunctionLiveness(Function* f);
            FunctionLiveness();
            void getGenAndKill();
            void printGenAndKill();
            void getInAndOut();
            void printInAndOut();  
    };
    bool check(Instruction* ins,SET & in,SET & out,SET& kill,SET& gen);
    void LivenessAnalysis(Program& p);
}