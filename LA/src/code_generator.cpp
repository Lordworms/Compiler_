#include "code_generator.h"
namespace LA
{
   void print_args(std::ofstream& out,Function* F)
   {
        for(ll i=0;i<F->args.size();++i)
        {
            if(i>0)out<<',';
            auto var=dynamic_cast<Var_item*>(F->args[i]);
            out<<var->anno->print()<<' '<<var->print();
        }
   }
   void gen_code(Program& p)
   {
        Var_Label_transformer trans;
        trans.find_longest_var(p);
        trans.find_longest_label(p);
        /*
            encode program
        */
        for(auto F:p.functions)
        {
            for(auto c:F->need_encode)
            {
                c->encode_itself();
            }
        } 
        for(auto F:p.functions)
        {
            EncodeVisitor ev(&trans);
            for(auto ins:F->insts)
            {
                ins->apply(ev);
            }
            F->insts=ev.new_ints;
        }
        /*
            check memory access
        */
        for(auto F:p.functions)
        {
            PreCheck pc(&trans);
            for(auto ins:F->insts)
            {
                if(ins->type==InsType::ins_assignment)
                {
                    auto assign=dynamic_cast<Instruction_assignment*>(ins);
                    if(assign->s->type==arr_ele_item)
                    {
                        pc.check_alloc((Arrele_item*)assign->s);
                        pc.check_boundary((Arrele_item*)assign->s);
                    }
                    if(assign->d->type==arr_ele_item)
                    {
                        pc.check_alloc((Arrele_item*)assign->d);
                        pc.check_boundary((Arrele_item*)assign->d);
                    }
                }
                pc.insts.push_back(ins);
            }
            F->insts=pc.insts;
        }
        /*
            enforce basic block
        
        */
        adding_basic_block(p,&trans);
        /*
            gen code
        */
        std::ofstream out;
        out.open("prog.IR");
        for(auto F:p.functions)
        {
            out<<"define "<<F->retType->print()<<" "<<"@"<<F->name<<" (";
            print_args(out,F);
            out<<"){\n";
            for(auto ins:F->insts)
            {
                out<<'\t'<<ins->print();
            }
            out<<"}\n";
        }
   }
}