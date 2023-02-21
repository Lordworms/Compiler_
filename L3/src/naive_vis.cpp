#include "naive_vis.h"

namespace L3
{
   void L2_visitor::init_read()
   {
        this->init_read_flag();
        this->print_first_read_instruction();
   }
   void L2_visitor::init_read_flag()
   {
        this->indexs.clear();
        this->reads_args.clear();
        this->args_number=F->args.size();
        for(auto arg:this->F->args)
        {
            this->indexs[arg]=this->indexs.size();
        }
   }
   void L2_visitor::print_first_read_instruction()
   {
        std::set<Item*>used;
        // for(auto& I:this->F->instructions)
        // {
        //     if(used.size()==this->args_number)break;
        //     ins_read(I,used);
        // }
        for(auto var:this->F->args)used.insert(var);
        print_first_read_item(used);
   }
   void L2_visitor::print_first_read_item(std::set<Item*>& used)
   {
        if(used.size()!=this->indexs.size())
        {
            std::cout<<"used size do not equal to index's size\n";
            return;
        }
        long long num=used.size(),i=0;
        for(auto var:used)
        {
            auto id=this->indexs[var];
            if(id<6)
            {
                OUTT<<var->print()<<" <- "<< args_reg[id]->print()<<'\n';
            }
            else
            {
                OUTT<<var->print()<<" <- stack-arg "<< std::to_string((id-6)*8)<<'\n';
            }
        }
        
   }    
   void L2_visitor::item_read(Item* it,std::set<Item*>& vars)
   {
        if(vars.size()>=this->args_number)return;
        switch (it->type)
        {
        case iType::aop_item:
        {
            auto aop=dynamic_cast<Aop_item*>(it);
            item_read(aop->op1,vars);
            item_read(aop->op2,vars);
            break;
        }
        case iType::call_item:
        {
            auto call=dynamic_cast<Call_item*>(it);
            for(auto arg:call->args)
            {
                item_read(arg,vars);
            }
            break;
        }
        case iType::store_item:
        {   
            auto store=dynamic_cast<Store_item*>(it);
            item_read(store->ptr,vars);
            break;
        }
        case iType::load_item:
        {   auto load=dynamic_cast<Load_item*>(it);
            item_read(load->var,vars);
            break;
        }
        case iType::var_item:
        {    
            if(this->indexs.find(it)!=this->indexs.end())
            {
                vars.insert(it);
            }
            break;
        }
        default:
        {
            break;
        }
        }
   }
   void L2_visitor::ins_read(Instruction* I,std::set<Item*>& vars)
   {
        switch(I->type)
        {
            case InsType::ins_assignment:
            {
                auto assign=dynamic_cast<Instruction_assignment*>(I);
                item_read(assign->d,vars);
                item_read(assign->s,vars);
                break;
            }
            case InsType::ins_branch:
            {
                auto br=dynamic_cast<Instruction_branch*>(I);
                if(br->condition!=nullptr)
                {
                    item_read(br->condition,vars);
                }
                break;
            }
            case InsType::ins_call:
            {
                auto call=dynamic_cast<Instruction_call*>(I);
                auto caller=dynamic_cast<Call_item*>(call->caller);
                if(caller)
                {
                    for(auto arg:caller->args)
                    {
                        item_read(arg,vars);
                    }
                }
            }
            case InsType::ins_var_ret:
            {
                auto ret=dynamic_cast<Instruction_var_ret*>(I);
                item_read(ret->var_ret,vars);
            }
            default:
                break;
        }
   }
   long long check_pre_cmp(Cmp_item* cmp)
   {
        if(auto v1=dynamic_cast<Constant_item*>(cmp->op1))
        {
            if(auto v2=dynamic_cast<Constant_item*>(cmp->op2))
            {
                switch (cmp->cType)
                {
                case CmpType::eq:
                    return v1==v2;
                    break;
                case CmpType::gt:
                    return v1>v2;
                    break;
                case CmpType::ge:
                    return v1>=v2;
                    break;
                case CmpType::lt:
                    return v1<v2;
                    break;
                case CmpType::le:
                    return v1<=v2;
                    break;
                default:
                    return -1;
                    break;
                }
            }
        }
        return -1;
   }
   bool check_cmp_reverse(Cmp_item* cmp)
   {
        return cmp->cType==CmpType::ge||cmp->cType==CmpType::gt;
   }
   void L2_visitor::translate_load(Load_item* it)
   {
        OUTT<<"mem "<<it->var->print()<<" 0\n";
   }
   void L2_visitor::translate_store(Store_item* it)
   {
        OUTT<<"mem "<<it->ptr->print()<<" 0";
   }
   void L2_visitor::visit(Instruction_assignment* ins)
   {    
       /*
        store %v <- aop or other
        %v<-    aop or other
       */
      std::string left;
      if(auto store=dynamic_cast<Store_item*>(ins->d))
      {
        left="mem "+store->ptr->print()+" 0";
      }
      else if(auto var=dynamic_cast<Var_item*>(ins->d))
      {
        left=var->print();
      }
      else
      {
        std::cout<<"invalid left value!\n";
      }
      std::string right;
      if(auto aop=dynamic_cast<Aop_item*>(ins->s))
      {
        auto new_var=this->trans->new_var_name("111");
        OUTT<<new_var<<" <- "<<aop->op1->print()<<'\n';
        OUTT<<new_var<<" "<<aop_str(aop->aType)<<"= "<<aop->op2->print()<<'\n';
        //std::cout<<new_var<<" <- "<<aop->op1->print()<<'\n'<<new_var<<" "<<aop_str(aop->aType)<<"= "<<aop->op2->print()<<'\n';
        right=new_var;
      }
      else if(auto load=dynamic_cast<Load_item*>(ins->s))
      {
        right="mem "+load->var->print()+" 0";
      }
      else if(auto cmp=dynamic_cast<Cmp_item*>(ins->s))
      {
        right=print_cmp(cmp);
      }
      else
      {
        right=ins->s->print();
      }
      //std::cout<<left<<" <- "<<right<<'\n';
      OUTT<<left<<" <- "<<right<<'\n';
   }
   void L2_visitor::visit(Instruction_ret* ins)
   {
        OUTT<<ins->print();
   }
   void L2_visitor::visit(Instruction_var_ret* ins)
   {
        OUTT<<" rax <- "<<ins->var_ret->print()<<'\n';
        OUTT<<"return\n";
   }
   void L2_visitor::print_args(Instruction_call* ins)
   {
        
        
       
   }
   void L2_visitor::print_runtime(Instruction_call * ins)
   {
        if(auto caller=(Call_item*)(ins->caller))
        {
            if(auto callee=(Label_item*)(caller->callee))
            {
                if(callee==&runtime_print)
                {
                    OUTT<<"rdi <- "<<caller->args[0]->print()<<'\n';
                    OUTT<<"call print 1\n";
                }
                else if(callee==&runtime_allocate)
                {
                    
                    OUTT<<"rdi <- "<<caller->args[0]->print()<<'\n';
                    OUTT<<"rsi <- "<<caller->args[1]->print()<<'\n';
                    OUTT<<"call allocate 2\n";
                    if(ins->var_ret)
                    {
                        OUTT<<ins->var_ret->print()<<" <- "<<"rax\n";
                    }
                    
                }
                else if(callee== &runtime_input)
                {
                    OUTT<<"call input 0\n";
                    if(ins->var_ret)
                    {
                        OUTT<<ins->var_ret->print()<<"<-"<<"rax"<<'\n';
                    }
                }
                else
                {
                    for(int i=0;i<caller->args.size();++i)
                    {
                        OUTT<<args_reg[i]->print()<<" <- "<<caller->args[i]->print()<<'\n';
                    }
                    OUTT<<"call tensor-error "<<caller->args.size()<<'\n';
                }
            }
        }
   }
   void L2_visitor::print_common(Instruction_call* ins)
   {
        if(auto caller=(Call_item*)(ins->caller))
        {
            long long num=caller->args.size();
            for(int i=0;i<std::min((long long)6,num);++i)
            {
                OUTT<<args_reg[i]->print()<<" <- "<<caller->args[i]->print()<<'\n';
            }
            auto label=this->trans->new_label_name("11");
            OUTT<<"mem rsp -8"<<" <- "<<label<<'\n';
            num-=6;
            for(long long i=0;i<num;++i)
            {
                OUTT<<"mem rsp -"<<(num-i+1)*8<<" <- "<<caller->args[i+6]->print()<<'\n';
            }
            OUTT<<"call "<<caller->callee->print()<<" "<<caller->args.size()<<'\n';
            OUTT<<label<<'\n';
            if(ins->var_ret!=nullptr)
            {
                OUTT<<ins->var_ret->print()<<" <- rax\n";
            }
        }
   }
   void L2_visitor::visit(Instruction_call* ins)
   {
        
        if(ins->isRuntime)
        {
            print_runtime(ins);
        }
        else
        {
            print_common(ins);
        }
        return;
   }
   Cmp_item* L2_visitor::find_cmp(Var_item* v,Instruction* ins)
   {
        if(v==nullptr)
        {
            std::cout<<"invalid type!\n";
            return nullptr;
        }
        Item* res=nullptr;
        for(auto I:this->F->instructions)
        {
            if(I==ins)
            {
                break;
            }
            if(auto i=dynamic_cast<Instruction_assignment*>(I))
            {
                if(i->d==v && i->s->type==iType::cmp_item)
                {
                    res=i->s;
                }
            }
        }
        if(res==nullptr)
        {
            std::cout<<"wrong!\n";
            return nullptr;
        }
        return dynamic_cast<Cmp_item*>(res);
   }
   std::string L2_visitor::print_cmp(Cmp_item* cmp)
   {    
        std::string res;
        bool c=check_cmp_reverse(cmp);
        if(c)
        {
            res+=cmp->op2->print();
        }
        else
        {
            res+=cmp->op1->print();
        }
        if(c)res+=cmp_transfer(cmp->cType);
        else res+=cmp_str(cmp->cType);
        if(c)
        {
            res+=cmp->op1->print();
        }
        else
        {
           res+=cmp->op2->print();
        }
        return res;
   }
   void L2_visitor::visit(Instruction_branch* ins)
   {
        if(ins->condition==nullptr)
        {
            OUTT<<"goto "<<ins->location->print()<<'\n';
            return;
        }
        else if(auto cons=dynamic_cast<Constant_item*>(ins->condition))
        {
            if(cons->value>0)
            {
                OUTT<<"goto "<<ins->location->print()<<'\n';
            }
            return;
        }
        if(ins->condition->type==iType::cmp_item)
        {
            auto cmp=this->find_cmp(dynamic_cast<Var_item*>(ins->condition),ins);
            long long res=check_pre_cmp(cmp);
            if(res!=-1)
            {
                if(res>0)
                {
                    OUTT<<"goto "<<ins->location->print()<<'\n';
                }
            }
        }
        else
        {
            
            OUTT<<"cjump "<<ins->condition->print()<<"=1 "<<' '<<ins->location->print()<<'\n';
        }
   }
   void L2_visitor::visit(Instruction_label* ins)
   {
        OUTT<<ins->print();
   }
   L2_visitor::L2_visitor(std::ofstream* out)
   {
        this->out=out;
   }
   void generate_program(Program& p,Var_Label_transformer* trans)
   {
        std::ofstream out;
        out.open("prog.L2");
        L2_visitor vis(&out);
        vis.trans=trans;
        out<<'('<<p.mainF->name<<'\n';
        for(auto& F:p.functions)
        {
            vis.F=F;
            out<<'('<<F->name<<'\n';
            out<<F->args.size()<<'\n';
            vis.init_read();
            for(auto I:F->instructions)
            {
                I->apply(vis);
            }
            out<<")\n";
        }
        out<<")\n";
        out.close();
   }
}