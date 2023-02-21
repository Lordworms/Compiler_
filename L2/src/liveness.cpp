#include "liveness.h"

namespace L2
{
    FunctionLiveness::FunctionLiveness()
    {

    }
    FunctionLiveness::FunctionLiveness(Function* f)
    {
        this->f=f;
    }
    void FunctionLiveness::getGenAndKill()
    {
        for(auto I:this->f->instructions)
        {
            if(isPrint)std::cout<<I->type<<'\n';
            I->apply(this->lv);
        }
    }
    void FunctionLiveness::printGenAndKill()
    {
        std::cout<<"(gen:\n";
        for(Instruction* ins:this->f->instructions)
        {
            std::cout<<"(";
            for(Item* it:this->lv.GEN[ins])
            {
                std::cout<<it->print()<<' ';
            }
            std::cout<<")\n";
        }
        std::cout<<")\n";
        std::cout<<"(KILL:\n";
        for(Instruction* ins:this->f->instructions)
        {
            std::cout<<"(";
            for(Item* it:this->lv.KILL[ins])
            {
                std::cout<<it->print()<<' ';
            }
            std::cout<<")\n";
        }
        std::cout<<")\n";
    }
    /* 
        OUT=U(successor)IN
        IN=GEN U (OUT-KILL)
    */
    bool check(Instruction* ins,SET & in,SET & out,SET& kill,SET& gen)
    {
        std::set<Item*>tmp,tmp_in;
        set_diff(out[ins],kill[ins],tmp);
        if(isPrint)
        {
            std::cout<<"after OUT-KILL\n";
            for(auto I:tmp)
            {
                std::cout<<I->print()<<' ';
            }
            std::cout<<'\n';
        }
        set_union(tmp,gen[ins],tmp_in);
        if(isPrint)
        {
            std::cout<<"after IN intercept OUT\n";
            for(auto I:tmp)
            {
                std::cout<<I->print()<<' ';
            }
            std::cout<<'\n';
        }
        bool res=tmp_in!=in[ins];
        in[ins]=tmp_in;
        return res;
    }
    void FunctionLiveness::getInAndOut()
    {
        sv.find_successor(this->f);
        bool is_changed;
        int k=0;
        do
        {
           is_changed=false;
           if(isPrint)std::cout<<"loop for "<<k++<<'\n';
           for(auto it=this->f->instructions.rbegin();it!=this->f->instructions.rend();++it)
           {
                Instruction* ins=*it;
                this->OUT[ins].clear();
                for(Instruction* sc:this->sv.successor[ins])
                {
                    if(isPrint)
                    {
                        if(ins->type==InsType::ins_goto)
                        {
                            std::cout<<"adding successor for goto inst\n";
                            std::cout<<((Instruction_goto*)(ins))->label->print()<<'\n';
                            if(sc->type==InsType::ins_label)
                            {
                                std::cout<<"sc's label name is "<<((Instruction_label*)(sc))->name->print()<<'\n';
                                std::cout<<"printing for label\n";
                                std::cout<<"label address:"<<sc<<'\n';
                                for(auto& I:IN[sc])
                                {
                                    std::cout<<I->print()<<'\n';
                                }
                            }
                        }
                        if(ins->type==InsType::ins_label)
                        {
                            std::cout<<"label:"<<ins<<'\n';
                        }
                    }
                    this->OUT[ins].insert(this->IN[sc].begin(),this->IN[sc].end());
                    if(isPrint)
                    {
                        std::cout<<"printing for goto\n";
                        if(ins->type==InsType::ins_goto)
                        {
                            for(auto k:this->OUT[ins])
                            {
                                std::cout<<k->print()<<'\n';
                            }
                            std::cout<<"printing IN\n";
                            for(auto k:this->IN[ins])
                            {
                                std::cout<<k->print()<<'\n';
                            }
                        }
                    }
                }
            
                is_changed|=check(ins,IN,OUT,lv.KILL,lv.GEN);  
           }

        } while (is_changed);
    }
    void FunctionLiveness::printInAndOut()
    {
        std::cout<<"(\n(in\n";
        for(Instruction* ins:this->f->instructions)
        {
            std::cout<<"(";
            for(Item* it:this->IN[ins])
            {
                std::cout<<it->print()<<' ';
            }
            std::cout<<")\n";
        }
        std::cout<<")\n\n";
        std::cout<<"(out\n";
        for(Instruction* ins:this->f->instructions)
        {
            std::cout<<"(";
            for(Item* it:this->OUT[ins])
            {
                std::cout<<it->print()<<' ';
            }
            std::cout<<")\n";
        }
        std::cout<<")\n\n)";
    }
    //liveness program
    void LivenessAnalysis(Program& p)
    {
        if(isPrint)std::cout<<"\n\nstart doing liveness analyss\n\n";
        for(auto f:p.functions)
        {
            FunctionLiveness solver(f);
            solver.getGenAndKill();
            solver.getInAndOut();
            if(isPrint)solver.printGenAndKill();
            if(isPrint)std::cout<<"answer!\n\n\n\n\n\n";
            solver.printInAndOut();
        }
    }
}