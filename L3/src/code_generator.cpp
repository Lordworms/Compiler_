#include "code_generator.h"
namespace L3
{
    void generate_all(Program& p)
    {
        CODE_SET cs(p.functions.size());
        //label globalization
        Var_Label_transformer trans("_global_");
        trans.find_longest_label(p);
        trans.find_longest_var(p);
        trans.transform_label(p);
        p.var_prefix=trans.longest_var;
        if(p.var_prefix=="")p.var_prefix="%v";
        p.ret_var_prefix=trans.longest_label;
        if(p.ret_var_prefix=="")p.ret_var_prefix="%label";
        //init tiles
        std::vector<Tile*>tiles;
        init_tile(tiles,p);

        for(ll i=0;i<p.functions.size();++i)
        {
            auto now_F=p.functions[i];
            FunctionLiveness fl(now_F);
            fl.getGenAndKill();
            fl.getInAndOut();

            std::vector<Context*>contexts;
            init_context(now_F,contexts);

            for(auto c:contexts)
            {
                auto forest=new InsForest(c,&fl);
                forest->merge_trees(fl);
                forest->tilling(tiles);
                cs[i].push_back(forest);
            }

        }
        generate_code(p,cs);
    }
    void generate_code(Program& p,CODE_SET& cs)
    {
        std::ofstream out;
        out.open("prog.L2");
        out<<'('<<p.mainF->name<<'\n';
        for(ll i=0;i<p.functions.size();++i)
        {
            auto F=p.functions[i];
            out<<'('<<F->name<<' '<<F->args.size()<<'\n';
            for(ll j=0;j<F->args.size();++j)
            {
                out<<'\t'<<F->args[j]->print()<<" <- ";
                if(j<6)
                {
                    out<<args_reg[j]->print()<<'\n';
                }
                else
                {
                    ll offset=(F->args.size()-1-j)*8;
                    out<<"stack-arg "<<std::to_string(offset)<<'\n';
                }
            
            }
            for(auto* forest:cs[i])
            {
                std::vector<std::string>inst_strs;
                if(inst_strs.size()==18)
                {
                    int x=0;
                }
                forest->gen_code(inst_strs);
                for(auto ins:inst_strs)
                {
                    out<<'\t'<<ins;
                }
            }
            out<<")\n\n";
        }
        out<<")\n";
        out.close();
    }
}