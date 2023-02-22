#include "code_generator.h"
namespace IR
{
    bool can_merge(BasicBlock* now,BasicBlock* next)
    {
        bool one_suc=now->succs.size()==1;
        if(one_suc)
        {
            if(now->succs.find(next)!=now->succs.end())
            {
                if(next->preds.find(now)!=next->preds.end()&&next->preds.size()==1)
                {
                    return true;
                }
            }
        }
        return false;
    }
    void gen_trace_code(std::vector<Trace*>traces,CodeVisitor& cv,std::ofstream& out)
    {
        for(auto tr:traces)
        {
            bool no_next_label = false;
            bool no_terminator = false;
            
            for (ll i = 0; i < tr->bb_list.size(); i++) {
                no_terminator = false;
                auto now = tr->bb_list[i];

                if (!no_next_label) {
                    now->beg_label->apply(cv);
                }

                for (Instruction * inst : now->insts) {
                    inst->apply(cv);
                }

                if (i < tr->bb_list.size() - 1) {
                    BasicBlock * next = tr->bb_list[i + 1];
                    bool merged = can_merge(now, next);
                    no_next_label = merged;
                    no_terminator = merged;
                }

                if (!no_terminator) {
                    now->terminator->apply(cv);
                }
                out << "\n";
            }
        }
    }
    void print_args(std::ofstream& out,std::vector<Item*>&args)
    {
        for(ll i=0;i<args.size();++i)
        {
            if(i)out<<',';
            out<<args[i]->print();
        }
    }
    void gen_code(Program& p)
    {
        Var_Label_transformer trans;
        trans.find_longest_var(p);

        std::ofstream out;
        CodeVisitor cv(trans,&out);
        out.open("prog.L3");
        for(auto F:p.functions)
        {
            out<<"define "<<F->name<<'(';
            print_args(out,F->args);
            out<<')';
            out<<"{\n";
            TraceGenerator tg;
            auto traces=tg.gen_trace(F->blocks);
            gen_trace_code(traces,cv,out);
            out<<"}\n";
        }
        out.close();
    }
}