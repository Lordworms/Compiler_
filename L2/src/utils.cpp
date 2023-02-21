#include "utils.h"
namespace L2
{
/*
    define visitor   
*/
//liveness visitor
Liveness_visitor::Liveness_visitor()
{

}
void Liveness_visitor:: visit(Instruction_ret* ins)
{
    if(isPrint)std::cout<<"calc GEN and KILL for instruction_ret\n";
    this->GEN[ins].insert(&L2::reg_rax);
    this->GEN[ins].insert(L2::callee_saved.begin(),L2::callee_saved.end());
    if(isPrint)
    {
        //std::cout<<"the GEN set contains"
    }
}
void Liveness_visitor:: visit(Instruction_assignment* ins)
{
    if(isPrint)std::cout<<"calc GEN and KILL for instruction asssignment\n";
    Item* dst=ins->d;
    std::vector<Item*>used;
    if(dst->type==iType::var_item||dst->type==iType::register_item)
    {
        if(isPrint)std::cout<<"the destination is a regular register\n";
        this->KILL[ins].insert(dst);
    }
    if(dst->type==iType::memoryAccess_item)
    {
        if(isPrint)std::cout<<"the destination is a mem shit\n";
        get_used(dst,used);
    }
    get_used(ins->s,used);
    this->GEN[ins].insert(used.begin(),used.end());
    if(isPrint)
    {
        std::cout<<"Now the gen set contains:\n";
        for(auto& I:this->GEN[ins])
        {
            std::cout<<I->print()<<' ';
        }
        std::cout<<'\n';
    }
}
void Liveness_visitor:: visit(Instruction_aop* ins)
{
    Item* op1=ins->op1;
    if(op1->type==iType::var_item||op1->type==iType::register_item)
    {
        this->KILL[ins].insert(op1);
    }
    std::vector<Item*>used;
    get_used(ins->op1,used);
    get_used(ins->op2,used);
    this->GEN[ins].insert(used.begin(),used.end());
}
void Liveness_visitor:: visit(Instruction_cjump* ins)
{
    std::vector<Item*>used;
    get_used(ins->condi,used);
    this->GEN[ins].insert(used.begin(),used.end());
}
void Liveness_visitor:: visit(Instruction_common* ins)
{
    if(isPrint)std::cout<<"we are doing GEN and KILL for a common call!\n";
    std::vector<Item*>used;
    get_used(ins->callee,used);
    this->GEN[ins].insert(used.begin(),used.end());
    Constant_item* num_arg=(Constant_item*)ins->num_cnt;
    long long num=std::min(num_arg->value,6ll);
    this->GEN[ins].insert(L2::args_reg.begin(),L2::args_reg.begin()+num);
    if(isPrint)
    {
        std::cout<<"the total argument is "<<num<<'\n';
        std::cout<<"now the gen set has became like\n";
        for(auto& I:this->GEN[ins])
        {
            std::cout<<I->print()<<' ';
        }
        std::cout<<'\n';
    }
    this->KILL[ins].insert(L2::caller_saved.begin(),L2::caller_saved.end());
}
void Liveness_visitor:: visit(Instruction_runtime* ins)
{
    if(isPrint)std::cout<<"we are doing GEN and KILL for a runtime call!\n";
    Constant_item* num_arg=(Constant_item*)ins->arg_cnt;
    if(isPrint)std::cout<<"the num arg is "<<num_arg->value<<'\n';
    long long num=std::min(num_arg->value,6ll);
    if(isPrint)std::cout<<"the arg number is "<<num<<'\n';
    this->GEN[ins].insert(L2::args_reg.begin(),L2::args_reg.begin()+num);
    this->KILL[ins].insert(L2::caller_saved.begin(),L2::caller_saved.end());
}
void Liveness_visitor:: visit(Instruction_dec* ins)
{
    this->GEN[ins].insert(ins->op);
    this->KILL[ins].insert(ins->op);
}
void Liveness_visitor:: visit(Instruction_inc* ins)
{
    this->GEN[ins].insert(ins->op);
    this->KILL[ins].insert(ins->op);
}
void Liveness_visitor:: visit(Instruction_label* ins)
{

}
void Liveness_visitor:: visit(Instruction_lea* ins)
{
    this->KILL[ins].insert(ins->des);
    this->GEN[ins].insert(ins->muler);
    this->GEN[ins].insert(ins->src);
}
void Liveness_visitor:: visit(Instruction_sop* ins)
{
    if(ins->target->type==iType::register_item||ins->target->type==iType::var_item)
    {
        this->KILL[ins].insert(ins->target);
    }
    std::vector<Item*>used;
    get_used(ins->offset,used);
    get_used(ins->target,used);
    this->GEN[ins].insert(used.begin(),used.end());
}
void Liveness_visitor::visit(Instruction_goto* ins)
{

}

Successor_visitor::Successor_visitor()
{

}
void Successor_visitor:: visit(Instruction_ret* ins)
{
    this->successor[ins].clear();
}
void Successor_visitor:: visit(Instruction_assignment* ins)
{

}
void Successor_visitor:: visit(Instruction_aop* ins)
{

}
void Successor_visitor:: visit(Instruction_cjump* ins)
{
    if(isPrint)std::cout<<"we are adding successor for cjump!"<<ins->label->print()<<'\n';
    this->successor[ins].push_back(this->label_map[ins->label]);
}
void Successor_visitor:: visit(Instruction_common* ins)
{

}
void Successor_visitor:: visit(Instruction_runtime* ins)
{
    //tensor label got no successor
    if(ins->func_name==&L2::runtime_tensor)
    {
        this->successor[ins].clear();
    }
}
void Successor_visitor:: visit(Instruction_goto* ins)
{
    if(isPrint)
    {
        std::cout<<"finding successor for goto !\n";
        std::cout<<"have a label"<<ins->label->print()<<'\n';
        if(label_map.find(ins->label)==label_map.end())
        {
            std::cout<<"do not have a label\n";
        }
        //std::cout<<this->label_map[ins->label]->type<<'\n';
    }
    this->successor[ins].clear();
    this->successor[ins].push_back(this->label_map[ins->label]);
}
void Successor_visitor:: visit(Instruction_dec* ins)
{

}
void Successor_visitor:: visit(Instruction_inc* ins)
{

}
void Successor_visitor:: visit(Instruction_label* ins)
{

}
void Successor_visitor:: visit(Instruction_lea* ins)
{

}
void Successor_visitor:: visit(Instruction_sop* ins)
{
    
}
void Successor_visitor::combine_label(Function* f)
{
    for(Instruction* I:f->instructions)
    {
        if(I->type==InsType::ins_label)
        {
            if(isPrint)std::cout<<"find a label inst\n";
            Instruction_label* tmp=(Instruction_label*)I;
            label_map[tmp->name]=I;
        }
    }
}
void Successor_visitor::find_successor(Function* f)
{
   combine_label(f);
   long long len=f->instructions.size();
   if(isPrint)std::cout<<"total have "<<len<<" instructions\n";
   for(long long i=0;i<len-1;++i)
   {
        if(isPrint)
        {
            if(f->instructions[i]->type==InsType::ins_goto)
            {
                std::cout<<"it is a goto Ins for successor\n";
                std::cout<<((Instruction_goto*)(f->instructions[i]))->label->print()<<'\n';
            }
        }
        this->successor[f->instructions[i]].push_back(f->instructions[i+1]);
   }
   for(long long i=0;i<len;++i)
   {
        f->instructions[i]->apply(*this);
   }
}
/*
    item_visitor apis

*/
ItemPrint_visitor::ItemPrint_visitor()
{

}
void ItemPrint_visitor::visit(Register_item* t)
{
    switch (t->regID)
    {
        case Register_ID::rdi:
        {
            std::cout<<"rdi";
            break;
        }
        case Register_ID::rbx:
        {
            std::cout<<"rbx";
            break;
        }
        case Register_ID::rsi:
        {
            std::cout<<"rsi";
            break;
        }
        case Register_ID::rax:
        {
            std::cout<<"rax";
            break;
        }
        case Register_ID::rdx:
        {
            std::cout<<"rdx";
            break;
        }
        case Register_ID::r8:
        {
            std::cout<<"r8";
            break;
        }
        case Register_ID::r9:
        {
            std::cout<<"r9";
            break;
        }
        case Register_ID::r10:
        {
            std::cout<<"r10";
            break;
        }
        case Register_ID::r11:
        {
            std::cout<<"r11";
            break;
        }
        case Register_ID::r12:
        {
            std::cout<<"r12";
            break;
        }
        case Register_ID::r13:
        {
            std::cout<<"r13";
            break;
        }
        case Register_ID::r14:
        {
            std::cout<<"r14";
            break;
        }
        case Register_ID::r15:
        {
            std::cout<<"r15";
            break;
        }
        case Register_ID::rcx:
        {
            std::cout<<"rcx";
            break;
        }
        case Register_ID::rsp:
        {
            std::cout<<"rsp";
            break;
        }
        case Register_ID::rbp:
        {
            std::cout<<"rbp";
            break;
        }
        default:
            break;
    }
}
void ItemPrint_visitor::visit(Aop_item*)
{

}
void ItemPrint_visitor::visit(Cmp_item*)
{

}
void ItemPrint_visitor::visit(Constant_item*)
{

}
void ItemPrint_visitor::visit(StackArg_item*)
{

}
void ItemPrint_visitor::visit(Memory_item*)
{

}
/*
    Spiller_visitor api
*/
bool check_spill(Item** iptr,Item* now_var,std::vector<Item **>& tmp_vars)
{
    switch ((*iptr)->type)
    {
        case iType::memoryAccess_item:
        {
            Memory_item* tmp=(Memory_item*)(*iptr);
            return check_spill(&tmp->regId,now_var,tmp_vars);
            break;
        }
        case iType::var_item:
        {
            if(now_var==*iptr)
            {
                tmp_vars.push_back(iptr);
                return true;
            }
            return false;
        }
        case iType::cmp_item:
        {
            Cmp_item* tmp=(Cmp_item*)(*iptr);
            bool used1=check_spill(&tmp->op1,now_var,tmp_vars);
            bool used2=check_spill(&tmp->op2,now_var,tmp_vars);
            return used1||used2;
        }
        default:
            return false;
            break;
    }
    return false;
}
Var_item* Spill_visitor::get_new_spill_var()
{
    std::string var_name=this->prefix->var_name+std::to_string(this->var_num);
    Var_item* var=new Var_item(var_name);
    this->var_num++;
    this->F->name_var_map[var_name]=var;
    this->spill_vars.push_back(var);
    return var;
}
Memory_item* Spill_visitor::get_new_stack_var()
{
    Constant_item* offset=new Constant_item((long long)(this->F->locals-1)*8);
    Memory_item* res=new Memory_item(
        & L2::reg_rsp,
        offset
    ); 
    return res;
}
Spill_visitor::Spill_visitor(Function* f,Var_item* now_spill_var,Var_item* preffix)
{
    this->F=f;
    this->now_spill_var=now_spill_var;
    this->prefix=preffix;
    this->var_num=0;
}
void Spill_visitor::spill(Instruction* inst,bool hasRead,bool hasWrite,std::vector<Item**>& tmp_vars)
{
    Var_item* new_var=nullptr;
    Memory_item* new_local=nullptr;
    //read that position
    if(hasRead)
    {
        new_var=this->get_new_spill_var();
        new_local=this->get_new_stack_var();
        /*
            %S0<-mem rsp 0
        */
       Instruction_assignment * read_var_ins=new Instruction_assignment();
       read_var_ins->d=new_var;
       read_var_ins->s=new_local;
       this->new_insts.push_back(read_var_ins);
    }
    if(new_var==nullptr && !tmp_vars.empty())
    {
        new_var=get_new_spill_var();
    }
    //change the old one into the new variable
    for(auto ptr:tmp_vars)
    {
        *ptr=new_var;
    }
    this->new_insts.push_back(inst);

    if(hasWrite)
    {
       if(new_var==nullptr)
       {
            new_var=this->get_new_spill_var();
       } 
       if(new_local==nullptr)
       {
            new_local=this->get_new_stack_var();
       }
       Instruction_assignment * store_var_ins=new Instruction_assignment();
       store_var_ins->d=new_local;
       store_var_ins->s=new_var;
       this->new_insts.push_back(store_var_ins);
    }
}
void Spill_visitor::visit(Instruction_ret* ins)
{
    new_insts.push_back(ins);
}
void Spill_visitor::visit(Instruction_assignment* ins)
{
   SPILL_SET tmp;
   bool des_used=check_spill(&ins->d,this->now_spill_var,tmp);
   bool src_used=check_spill(&ins->s,this->now_spill_var,tmp);
   bool hasRead=(des_used &&ins->d!=this->now_spill_var)||src_used;
   bool hasWrite=ins->d==this->now_spill_var;
   this->spill(ins,hasRead,hasWrite,tmp);
}
void Spill_visitor::visit(Instruction_dec* ins)
{
   SPILL_SET tmp;
   bool used=check_spill(&ins->op,this->now_spill_var,tmp);
   this->spill(ins,used,used,tmp);
}
void Spill_visitor::visit(Instruction_inc* ins)
{
   SPILL_SET tmp;
   bool used=check_spill(&ins->op,this->now_spill_var,tmp);
   this->spill(ins,used,used,tmp);    
}
void Spill_visitor::visit(Instruction_aop* ins)
{
    SPILL_SET tmp;
    bool op1_used=check_spill(&ins->op1,this->now_spill_var,tmp);
    bool op2_used=check_spill(&ins->op2,this->now_spill_var,tmp);

    bool hasRead=op1_used||op2_used;
    bool hasWrite=ins->op1==this->now_spill_var;
    
    this->spill(ins,hasRead,hasWrite,tmp);
}
void Spill_visitor::visit(Instruction_sop* ins)
{
    SPILL_SET tmp;
    bool target_used=check_spill(&ins->target,this->now_spill_var,tmp);
    bool offset_used=check_spill(&ins->offset,this->now_spill_var,tmp);
    bool hasRead=target_used||offset_used;
    bool hasWrite=ins->target==this->now_spill_var;
    this->spill(ins,hasRead,hasWrite,tmp);
}
void Spill_visitor::visit(Instruction_label* ins)
{
    new_insts.push_back(ins);
}
void Spill_visitor::visit(Instruction_runtime* ins)
{
    new_insts.push_back(ins);
}
void Spill_visitor::visit(Instruction_common* ins)
{
    SPILL_SET tmp;
    bool var_used=check_spill(&ins->callee,this->now_spill_var,tmp);
    this->spill(ins,var_used,false,tmp);
}
void Spill_visitor::visit(Instruction_cjump* ins)
{
    SPILL_SET tmp;
    bool var_used=check_spill(&ins->condi,this->now_spill_var,tmp);
    this->spill(ins,var_used,false,tmp);
}
void Spill_visitor::visit(Instruction_goto* ins)
{
    new_insts.push_back(ins);
}
void Spill_visitor::visit(Instruction_lea* ins)
{
    SPILL_SET tmp;
    bool dst_used=check_spill(&ins->des,this->now_spill_var,tmp);
    bool addr_used=check_spill(&ins->src,this->now_spill_var,tmp);
    bool mul_used=check_spill(&ins->muler,this->now_spill_var,tmp);

    bool hasRead=addr_used||mul_used;
    bool hasWrite=dst_used;

    this->spill(ins,hasRead,hasWrite,tmp);
}
/*

    coloring visitor apis
*/
Color_visitor::Color_visitor(std::unordered_map<Item*,Reg_color>& item_color_mp,Function* F)
{
    this->F=F;
    this->var_color_mp=item_color_mp;
}
bool Color_visitor::have_color(Item** addr,std::vector<Item**> &vec)
{
    switch ((*addr)->type)
    {
    case iType::var_item:
    {
        if(this->var_color_mp.find(*addr)!=this->var_color_mp.end())
        {
            vec.push_back(addr);
            return true;
        }
        break;
    }
    case iType::memoryAccess_item:
    {
        Memory_item* tmp=(Memory_item*)(*addr);
        return have_color(&tmp->regId,vec);
    }
    case iType::cmp_item:
    {
        Cmp_item* tmp=(Cmp_item*)(*addr);
        bool have_op1=have_color(&tmp->op1,vec);
        bool have_op2=have_color(&tmp->op2,vec);
        return have_op1|| have_op2;
    }
    default:
        return false;
        break;
    }
    return false;
}
void Color_visitor::visit(Instruction_ret* ins)
{

}
void Color_visitor::visit(Instruction_assignment* ins)
{
    std::vector<Item**>tmp;
    bool have_des=have_color(&ins->d,tmp);
    bool have_src=have_color(&ins->s,tmp);
    if(have_des||have_src)
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }
    }
}
void Color_visitor::visit(Instruction_aop* ins)
{
    std::vector<Item**>tmp;
    bool have_op1=have_color(&ins->op1,tmp);
    bool have_op2=have_color(&ins->op2,tmp);
    if(have_op1||have_op2)
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }
    }   
}
void Color_visitor::visit(Instruction_sop* ins)
{
    std::vector<Item**>tmp;
    bool have_target=have_color(&ins->target,tmp);
    bool have_offset=have_color(&ins->offset,tmp);
    if(have_target||have_offset)
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }
    }   
}
void Color_visitor::visit(Instruction_runtime* ins)
{
    
}
void Color_visitor::visit(Instruction_common* ins)
{
    std::vector<Item**>tmp;
    if(have_color(&ins->callee,tmp))
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }
    }
}
void Color_visitor::visit(Instruction_dec* ins)
{
    std::vector<Item**>tmp;
    if(have_color(&ins->op,tmp))
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }        
    }
}
void Color_visitor::visit(Instruction_inc* ins)
{
    std::vector<Item**>tmp;
    if(have_color(&ins->op,tmp))
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }        
    }   
}
void Color_visitor::visit(Instruction_label* ins)
{
    
}
void Color_visitor::visit(Instruction_goto* ins)
{
    
}
void Color_visitor::visit(Instruction_cjump* ins)
{
    std::vector<Item**>tmp;
    if(have_color(&ins->condi,tmp))
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }
    }
}
void Color_visitor::visit(Instruction_lea* ins)
{
    std::vector<Item**>tmp;
    bool have_des=have_color(&ins->des,tmp);
    bool have_muler=have_color(&ins->muler,tmp);
    bool have_src=have_color(&ins->src,tmp);
    if(have_des||have_muler||have_src)
    {
        for(auto v:tmp)
        {
            *v=color_reg_mp[this->var_color_mp[*v]];
        }
    }
}
/*
    used for L1code Visitor
*/
CodeL1_visitor::CodeL1_visitor()
{
    this->out=nullptr;
}
CodeL1_visitor::CodeL1_visitor(std::ofstream* out)
{
    this->out=out;
}
void CodeL1_visitor:: visit(Instruction_ret* ins)
{
    *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_assignment* ins)
{
    *this->out <<ins->d->print()<<" <- ";
    std::string src;
    if(ins->s->type==iType::stackArg_item)
    {
        auto arg_stack=(StackArg_item*)(ins->s);
        long long offset=this->locals_num*8;
        offset+=((Constant_item*)arg_stack->offset)->value;
        src+="mem rsp ";
        src+=std::to_string(offset);
    }
    else
    {
        src+=ins->s->print();
    }
    *this->out<<src<<'\n';
}
void CodeL1_visitor:: visit(Instruction_aop* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_cjump* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_common* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_runtime* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_dec* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_inc* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_label* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_lea* ins)
{
     *this->out<<ins->print();
}
void CodeL1_visitor:: visit(Instruction_sop* ins) 
{
     *this->out<<ins->print();
}      
void CodeL1_visitor:: visit(Instruction_goto* ins) 
{
     *this->out<<ins->print();
}      
/*
used for liveness analysis
*/
void get_used(Item* it,std::vector<Item*>&used)
{
    if(isPrint)std::cout<<"we are doing used shit! and the item type is "<<it->type<<'\n';
    switch (it->type)
    {
    case iType::register_item:
    {
        if(((Register_item*)it)->regID!=Register_ID::rsp)
        {
            if(isPrint)std::cout<<"that is a valid register!"<<it->print()<<'\n';
            used.push_back(it);
        }
        break;
    }
    case iType::memoryAccess_item:
    {
        Memory_item* mem_tmp=(Memory_item*)it;
        get_used(mem_tmp->regId,used);
        break;
    }
    case iType::cmp_item:
    {
        Cmp_item* cmp_tmp=(Cmp_item*)it;
        get_used(cmp_tmp->op1,used);
        get_used(cmp_tmp->op2,used);
        break;
    }
    case iType::aop_item:
    {
        break;
    }
    case iType::label_item:
    {
        break;
    }
    case iType::stackArg_item:
    {
        break;
    }
    case iType::var_item:
    {
        used.push_back(it);
        break;    
    }
    default:
    {
        if(isPrint)std::cout<<"invalid used type!\n";
        break;
    }
    }
}
}