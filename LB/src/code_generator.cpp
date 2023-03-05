#include "code_generator.h"
namespace LB
{
   void gen_args(Function* f,std::ofstream* out)
   {
        for(ll i=0;i<f->args.size();++i)
        {
            if(i)
            {
                *out<<",";
            }
            auto var=dynamic_cast<Var_item*>(f->args[i]);
            *out<<var->anno->print()<<' '<<var->print();
        }
   }
   void gen_code(Program& p)
   {
        std::ofstream out;
        out.open("prog.a");
        Var_Label_transformer trans;
        trans.find_longest_label(p);
        trans.find_longest_var(p);
        trans.transform_var(p);
        for(auto F:p.functions)
        {
            auto while_label=trans.get_new_condi_label(F);
            auto ins_while=trans.get_ins_while_map(F);
            CodeVisitor cv(&out,&trans,&ins_while,&while_label);
            out<<F->retType->print()<<' '<<F->name<<" (";
            gen_args(F,&out);
            out<<"){\n";
            F->scope->apply(cv);
            out<<"}\n";
        }
   }
}