#include "utils.h"

namespace IR
{
/*
    Var transormer
*/
Var_Label_transformer::Var_Label_transformer()
{
    this->longest_label="";
    this->longest_var="";
    this->label_cnt=0;
    this->var_cnt=0;
}
Var_Label_transformer::Var_Label_transformer(std::string suf)
{
    this->suffix=suf;
    this->longest_label="";
    this->longest_var="";
    this->label_cnt=0;
    this->var_cnt=0;
}
std::string Var_Label_transformer::find_longest_var(Program& p)
{
    std::string res="";
    for(auto F:p.functions)
    {
        for(auto& pi:F->name_var_map)
        {
            if(res.size()<pi.first.size())
            {
                res=pi.first;
            }
        }
    }
    this->longest_var=res;
    return res;
}
std::string Var_Label_transformer::find_longest_label(Program& p)
{
    std::string res="";
    for(auto F:p.functions)
    {
        for(auto &pi:F->label_ptr_map)
        {
            if(res.size()<pi.first.size())
            {
                res=pi.first;
            }
        }
    }
    if(res.size()&&res[0]=='@')res[0]=':';
    this->longest_label=res;
    return res;
}
std::string Var_Label_transformer::new_label_name(std::string name)
{
    return this->longest_label+this->suffix+std::to_string(this->label_cnt++);
}
std::string Var_Label_transformer::new_var_name(std::string name)
{
    return this->longest_var+this->suffix+std::to_string(this->var_cnt++);
}   
void Var_Label_transformer::transform_label(Program& p)
{
    for(auto F:p.functions)
    {
        for(auto ptr:F->label_set)
        {
            auto label=(Label_item*)ptr;
            std::string new_label_name=this->new_label_name(label->label_name);
            F->label_ptr_map.erase(label->label_name);
            label->label_name=new_label_name;
            F->label_ptr_map[new_label_name]=ptr;
        }
    }
}
void Var_Label_transformer::transform_var(Program& p)
{
    for(auto F:p.functions)
    {
        for(auto ptr:F->var_set)
        {
            auto var=(Var_item*)ptr;
            std::string new_var_name=this->new_var_name(var->var_name);
            F->name_var_map.erase(var->var_name);
            var->var_name=new_var_name;
            F->name_var_map[new_var_name]=ptr;
        }
    }
}

/*

    Trace

*/
Trace::Trace()
{

}
void Trace::add_bb(BasicBlock* bb)
{
    this->bb_list.push_back(bb);
}
/*

    TraceGenerator
*/
TraceGenerator::TraceGenerator()
{

}

std::vector<Trace*>TraceGenerator::gen_trace(std::vector<BasicBlock*>bb_list)
{
    std::set<BasicBlock*>traced_bb;
    std::vector<Trace*>res_trace;
    bool traced_entry=false;
    do
    {
       auto tr=new Trace();
       auto bb=fetch_and_remove(bb_list,traced_entry);
       traced_entry=true;
       while(traced_bb.find(bb)==traced_bb.end())
       {
            traced_bb.insert(bb);
            tr->add_bb(bb);
            auto next_bb=get_next_bb(bb->succs,traced_bb);
            if(next_bb)
            {
                bb=next_bb;
            }
       }
       if(tr->bb_list.empty())
       {
            delete tr;
       } 
       else
       {
            res_trace.push_back(tr);
       }
    } while (!bb_list.empty());
    return res_trace;
}
/*
    code generator
*/
CodeVisitor::CodeVisitor()
{
    
}
CodeVisitor::CodeVisitor(Var_Label_transformer& trans,std::ofstream* out)
{
    this->trans=trans;
    this->out=out;
}
void CodeVisitor::print_encode(Item* v)
{
    auto v_str=v->print();
    OUT<<'\t'<<v_str<<" <- "<<v_str<<" << 1\n";
    OUT<<'\t'<<v_str<<" <- "<<v_str<<" + 1\n";
}
void CodeVisitor::print_decode(Item* v)
{
    auto v_str=v->print();
    OUT<<'\t'<<v_str<<" <- "<<v_str<<" >> 1\n";
}
void CodeVisitor::print_load(Item* dst,Item* src)
{
    OUT<<'\t'<<dst->print()<<"<- load "<<src->print()<<'\n';
}

void CodeVisitor::print_store(Item* dst,Item* src)
{
    OUT<<"\tstore "<<dst->print()<<"<-"<<src->print()<<'\n';
}
void CodeVisitor::print_var_common(Var_item* dst,Item* src)
{
    OUT<<'\t'<<dst->print()<<" <- "<<src->print()<<'\n';
}
void CodeVisitor::print_arrele_all(Arrele_item* it,Item* dst)
{
    auto addr=this->get_addr(it);
    this->print_store(addr,dst);
}
void CodeVisitor::print_var_call(Item* dst,Item* src)
{
    OUT<<'\t'<<dst->print()<<" <- "<<src->print()<<'\n';
}
void CodeVisitor::print_op(Item* dst,Item* op1,OpType op,Item* op2)
{
    Op_item opp(op1,op2,op);
    this->print_var_op((Var_item*)dst,&opp);
}
void CodeVisitor::print_var_length(Item* var,Item* src)
{
    auto _dst=dynamic_cast<Var_item*>(var);
    auto _src=dynamic_cast<Length_item*>(src);
    auto addr=dynamic_cast<Var_item*>(_src->base);
    auto anno_type=dynamic_cast<TypeAnno_item*>(addr->anno);
    if(anno_type->a_type==AnnoType::tuple_anno)
    {
        this->print_load(var,addr);
        return;
    }
    auto new_addr=new Var_item(this->trans.new_var_name(""));
    Constant_item byte_off(2*WIDTH);
    this->print_op(new_addr,_src->base,OpType::plus,&byte_off);
    auto off=new Var_item(this->trans.new_var_name(""));
    Constant_item bytes_off(WIDTH);
    this->print_op(off,_src->dim,OpType::multy,&bytes_off);
    this->print_op(new_addr,new_addr,OpType::plus,off);
    this->print_load(var,new_addr);
}
//get address
Item* CodeVisitor::get_addr(Item* var)
{
    auto arr_acc=dynamic_cast<Arrele_item*>(var);
    auto base=dynamic_cast<Var_item*>(arr_acc->base);
    auto anno_type=dynamic_cast<TypeAnno_item*>(base->anno);

    if(anno_type->a_type==AnnoType::tuple_anno)
    {
        Constant_item cons(WIDTH);
        auto addr=new Var_item(this->trans.new_var_name(""));
        auto offset=new Var_item(this->trans.new_var_name(""));
        this->print_op(offset,addr,OpType::plus,&cons);
        this->print_op(addr,addr,OpType::plus,offset);
        return addr;
    }
    else if (anno_type->a_type==AnnoType::tensor_anno)
    {   
        ll ndims = arr_acc->eles.size();
        auto * addr = new Var_item(this->trans.new_var_name(""));
        Constant_item init2BytesLen(2*WIDTH);
        this->print_op(addr,arr_acc->base,OpType::plus,&init2BytesLen);
        std::vector<Var_item *> decodeArrDimLen (ndims);
        Constant_item constOne(1);
        Constant_item constValWidth(WIDTH);
        for (ll i = 0; i < ndims; i++) {
            auto decoded = new Var_item(this->trans.new_var_name(""));
            decodeArrDimLen[i] = decoded;
            this->print_load(
                decoded,   
                addr        
            );
            this->print_op(
                decoded,               
                decoded,                
                OpType::right_shift,   
                &constOne              
            );
            this->print_op(
                addr,              
                addr,                
                OpType::plus,      
                &constValWidth 
            );
        }
        std::vector<Var_item *> arrDimBufSize (ndims);
        
        for (ll i = ndims - 1; i >= 0; i--) {
            Var_item * arrBufSize = new Var_item(this->trans.new_var_name(""));
            arrDimBufSize[i] = arrBufSize;

            if (i == ndims - 1) {
                this->print_var_common(
                    arrBufSize,         
                    &constOne          
                );
            } 
            else 
            {
                this->print_op(
                    arrBufSize,                             
                    arrDimBufSize[i + 1],               
                    OpType::multy,                          
                    decodeArrDimLen[i + 1]                
                );
            }
        }

        auto dimOffset = new Var_item(this->trans.new_var_name(""));
        for (ll i = 0 ; i < ndims; i++) {
            this->print_op(
                dimOffset,                           
                arrDimBufSize[i],               
                OpType::multy,                        
                &constValWidth                
            );
            
            this->print_op(
                dimOffset,                           
                dimOffset,                             
                OpType::multy,                          
                arr_acc->eles[i]                
            );
            this->print_op(
                addr,                           
                addr,                              
                OpType::plus,                          
                dimOffset                 
            );  
        }
        return addr;
    }
    else
    {
        return nullptr;
    }
    
}
void CodeVisitor::print_var_Arrele(Item* dst,Item* src)
{
    auto addr=this->get_addr(src);
    this->print_load(dst,addr);
}
void CodeVisitor::print_var_op(Item* dst,Item* src)
{
    auto op=dynamic_cast<Op_item*>(src);
    OUT<<'\t'<<dst->print()<<" <- "<<op->print()<<'\n';
}
void CodeVisitor::print_var_newArray(Item* dst,Item* src)
{
    auto new_arr=dynamic_cast<NewArr_item*>(src);
    std::vector<Var_item*>decode_dims(new_arr->dims.size());
    std::string inst;
    for(ll i=0;i<new_arr->dims.size();++i)
    {
        auto new_var=new Var_item(this->trans.new_var_name(""));
        decode_dims[i]=new_var;
        inst.clear();
        inst+=new_var->print();
        inst+=" <- ";
        inst+=new_arr->dims[i]->print();
        inst+=">> 1\n";
        OUT<<'\t'<<inst;
    }
    //get total length
    auto total_length_var=new Var_item(this->trans.new_var_name(""));
    inst.clear();
    inst+=total_length_var->print();
    inst+=" <- ";
    inst+=decode_dims[0]->print();
    inst+='\n';

    OUT<<'\t'<<inst;

    for(ll i=1;i<decode_dims.size();++i)
    {
        inst.clear();
        inst+=total_length_var->print();
        inst+=" <- ";
        inst+=total_length_var->print();
        inst+=" * ";
        inst+=decode_dims[i]->print();
        inst+='\n';
        OUT<<'\t'<<inst;
    }

    // inst.clear();
    // inst+=total_length_var->print();
    // inst+=" <- ";
    // inst+=total_length_var->print();
    // inst+=" + ";
    // inst+=std::to_string(new_arr->dims.size()+1);
    // inst+='\n';

    // OUT<<'\t'<<inst;

    print_encode(total_length_var);
    inst.clear();
    inst+=total_length_var->print();
    inst+="<-";
    inst+=total_length_var->print();
    inst+=" + ";
    inst+=std::to_string(2*new_arr->dims.size());
    inst+='\n';
    OUT<<'\t'<<inst;
    //call allocate
    inst.clear();
    inst+=dst->print();
    inst+=" <- call allocate(";
    inst+=total_length_var->print();
    inst+=",1)\n";
    OUT<<'\t'<<inst;

    //set variable
    auto ptr=new Var_item(this->trans.new_var_name(""));
    inst.clear();
    inst+=ptr->print();
    inst+=" <- ";
    inst+=dst->print();
    //inst+=" + ";
    //inst+=std::to_string(WIDTH);
    inst+='\n';
    OUT<<'\t'<<inst;

    // ll encode_dim = encode(new_arr->dims.size());
    // inst.clear();
    // inst += "store ";
    // inst += ptr->print();
    // inst += " <- ";
    // inst += std::to_string(encode_dim);
    // inst += "\n";
    // OUT<<'\t'<<inst;

    for (ll i = 0; i <new_arr->dims.size(); i++) {
        //ptr+8
        inst.clear();
        inst += ptr->print();
        inst += " <- ";
        inst += ptr->print();
        inst += " + ";
        inst += std::to_string(WIDTH);
        inst += "\n";
        OUT<<'\t'<<inst;

        //store element
        inst.clear();
        inst += "store ";
        inst += ptr->print();
        inst += " <- ";
        inst += new_arr->dims[i]->print();
        inst += "\n";
        OUT<<'\t'<<inst;
    }
}
void CodeVisitor::print_var_newTuple(Item* dst,Item* src)
{
    auto tuple=dynamic_cast<NewTup_item*>(src);
    OUT<<'\t'<<dst->print()<<" <- call allocate("<<tuple->siz->print()<<",1)\n";
}
void CodeVisitor::visit(Instruction_ret* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_var_ret* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_branch_condi* ins)
{
    std::string res1="br "+ins->condition->print()+" "+ins->location1->print()+'\n';
    std::string res2="br "+ins->location2->print()+'\n';
    OUT<<res1<<res2;
}
void CodeVisitor::visit(Instruction_branch_nocondi* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_assignment* ins)
{
    /*
        two different situation
        arr_ele<-all
        var<-all
    */
    auto dst=ins->d;
    auto src=ins->s;
    if(dst->type==iType::arr_ele_item)
    {
        this->print_arrele_all((Arrele_item*)dst,src);
    }
    else
    {
        switch (src->type)
        {
            case iType::var_item:
            {
                print_var_common((Var_item*)dst,src);
                break;
            }
            case iType::constant_item:
            {
                print_var_common((Var_item*)dst,src);
                break;
            }   
            case iType::label_item:
            {
                print_var_common((Var_item*)dst,src);
                break;
            }   
            case iType::new_arr_item:
            {
                print_var_newArray(dst,src);
                break;
            }   
            case iType::new_tup_item:
            {
                print_var_newTuple(dst,src);
                break;
            }   
            case iType::arr_ele_item:
            {
                print_var_Arrele(dst,src);
                break;
            }   
            case iType::call_item:
            {
                print_var_call(dst,src);
                break;
            }
            case iType::op_item:
            {
                print_var_op(dst,src);
                break;
            }
            case iType::length_item:
            {
                print_var_length(dst,src);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}
void CodeVisitor::visit(Instruction_label* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_call* ins)
{
    OUT<<'\t'<<ins->print();
}
void CodeVisitor::visit(Instruction_declare* ins)
{

}
/*
    other interfaces
*/
    ll encode(ll x)
    {
        return (x<<1)+1;
    }
    ll decode(ll x)
    {
        return x>>1;
    }
    BasicBlock* fetch_and_remove(std::vector<BasicBlock*>& bb_list,bool traced_entry)
    {
        if(!traced_entry)
        {
            auto res=bb_list[0];
            bb_list.erase(bb_list.begin());
            return res;
        }
        for(auto it=bb_list.begin();it!=bb_list.end();++it)
        {
            if((*it)->succs.size()==1)
            {
                auto res=*it;
                bb_list.erase(it);
                return res; 
            }
        }
        auto res=*bb_list.begin();
        bb_list.erase(bb_list.begin());
        return res;
    }
    BasicBlock* get_next_bb(std::set<BasicBlock*>&suc,std::set<BasicBlock*>&bb_list)
    {
        std::set<BasicBlock*>tmp;
        set_diff(
            suc,
            bb_list,
            tmp
        );
        for(auto B:tmp)
        {
            if(B->succs.size()==1)
            {
                return B;
            }
        }
        if(tmp.empty())
        {
            return nullptr;
        }
        else
        {
            return *tmp.begin();
        }
    }
}