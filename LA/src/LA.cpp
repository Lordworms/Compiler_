#include "LA.h"
namespace LA
{
    Label_item runtime_print("print");
    Label_item runtime_input("input");
    Label_item runtime_allocate("allocate");
    Label_item runtime_tensor("tensor-error");

    TypeAnno_item void_anno_ex(AnnoType::void_anno);
    TypeAnno_item int64_anno_ex(AnnoType::int64_anno);
    TypeAnno_item tuple_anno_ex(AnnoType::tuple_anno);
    TypeAnno_item code_anno_ex(AnnoType::code_anno);

    Fname_item fname_print("print");
    Fname_item fname_input("input");
    Fname_item fname_allocate("allocate");
    Fname_item fname_tensor("tensor-error");
    std::string get_anno_str(AnnoType t)
    {
        switch (t)
        {
        case AnnoType::code_anno:
            return "code";
            break;
        case AnnoType::int64_anno:
            return "int64";
            break;
        case AnnoType::tensor_anno:
            return "int64";
            break;
        case AnnoType::tuple_anno:
            return "tuple";
            break;
        case AnnoType::void_anno:
            return "void";
            break;
        default:
            return "";
            break;
        }
    }
    std::string op_str(LA::OpType type)
    {
        switch (type)
        {
            case OpType::bit_and:
                return "&";
                break;
            case OpType::minus:
                return "-";
                break;
            case OpType::multy:
                return "*";
                break;
            case OpType::plus:
                return "+";
                break;
            case OpType::left_shift:
                return "<<";
                break;
            case OpType::right_shift:
                return ">>";
                break;
            case OpType::eq:
                return "=";
                break;
            case OpType::ge:
                return ">=";
                break;
            case OpType::gt:
                return ">";
                break;
            case OpType::le:
                return "<=";
                break;
            case OpType::lt:
                return "<";
                break;
            default:
                std::cout<<"error type in transferring aop character\n";
                break;
        }
        return "";
    }

    bool check_runtime_call(Item* callee)
    {
        return callee==&LA::runtime_print||callee==&LA::runtime_allocate||callee==&LA::runtime_input||callee==&LA::runtime_tensor;
    }
    /*
        constructor for items
    */
    Item::Item()
    {

    }
    void Item::setType(iType t)
    {
        this->type=t;
    }
    Var_item::Var_item()
    {
        this->type=iType::var_item;
        this->anno=nullptr;
    }
    Var_item::Var_item(std::string name,Item* an)
    {
        this->type=iType::var_item;
        this->anno=an;
        this->var_name=name;
    }
    Var_item::Var_item(std::string str)
    {
        this->type=iType::var_item;
        this->var_name=str;
    }
    Constant_item::Constant_item()
    {
        this->type=iType::constant_item;
        this->is_encoded=false;
    }
    Constant_item::Constant_item(long long v)
    {
        this->type=iType::constant_item;
        this->value=v;
        is_encoded=false;
    }
    Label_item::Label_item()
    {
        this->type=iType::label_item;
    }
    Label_item::Label_item(std::string name)
    {
        this->label_name=name;
        this->type=iType::label_item;
    }
    Op_item::Op_item()
    {
        this->type=iType::op_item;
    }
    Op_item::Op_item(OpType t,Item* o1,Item* o2)
    {
        this->op1=o1;
        this->op2=o2;
        this->o_type=t;
        this->type=iType::op_item;
    }
    Op_item::Op_item(Item* o1,Item* o2,OpType t)
    {
        this->op1=o1;
        this->op2=o2;
        this->o_type=t;
        this->type=iType::op_item;
    }
    Arrele_item::Arrele_item()
    {
        this->type=iType::arr_ele_item;
    }
    Arrele_item::Arrele_item(Item* b,std::vector<Item*>& e)
    {
        this->type=iType::arr_ele_item;
        this->base=b;
        this->eles=e;
    }
    Call_item::Call_item()
    {
        this->type=iType::call_item;
    }
    Call_item::Call_item(bool isRuntime,std::vector<Item*>& argss,Item* calleee)
    {
        this->isRuntime=isRuntime;
        this->args=argss;
        this->callee=calleee;
        this->type=iType::call_item;
    }
    Call_item::Call_item(bool isRuntime,bool isonFuncname,std::vector<Item*>& argss,Item* calleee)
    {
        this->isRuntime=isRuntime;
        this->args=argss;
        this->callee=calleee;
        this->type=iType::call_item;
        this->is_on_func_name=isonFuncname;
    }
    TypeAnno_item::TypeAnno_item(AnnoType at,ll d)
    {
        this->type=iType::type_anno_item;
        this->a_type=at;
        this->dim=d;
    }
    TypeAnno_item::TypeAnno_item(AnnoType t)
    {
        this->type=iType::type_anno_item;
        this->a_type=t;
    }
    NewArr_item::NewArr_item(std::vector<Item*>&di)
    {
        this->type=iType::new_arr_item;
        this->dims=di;
    }
    NewTup_item::NewTup_item(Item* siz)
    {
        this->siz=siz;
        this->type=iType::new_tup_item;
    }
    Length_item::Length_item(Item* b,Item* d)
    {
        this->type=iType::length_item;
        this->base=b;
        if(d==nullptr)
        {
            this->dim=new Constant_item(0);
        }
        else
        {
            this->dim=d;
        }
    }
    Fname_item::Fname_item(std::string name)
    {
        this->fname=name;
        this->fptr=nullptr;
        this->type=fname_item;
    }
    Fname_item::Fname_item(std::string name,Function* fp)
    {
        this->fname=name;
        this->fptr=fp;
        this->type=fname_item;
    }
    /*
        print function for items
    */
    std::string TypeAnno_item::print()
    {
        std::string res;
        res+=get_anno_str(this->a_type);
        if(this->a_type==AnnoType::tensor_anno)
        {
            for(int i=0;i<this->dim;++i)
            {
                res+="[]";
            }
        }
        return res;
        
    }
    std::string Op_item::print()
    {
        std::string res;
        res+=this->op1->print();
        res+=' ';
        res+=op_str(this->o_type);
        res+=' ';
        res+=this->op2->print();
        return res;
    }
    std::string Var_item::print()
    {
        std::string res;
        if(GEN_IR)res+="%";
        res+=this->var_name;
        return res;
    }
    std::string Constant_item::print()
    {
        return std::to_string(this->value);
    }
    std::string Label_item::print()
    {
        return this->label_name;
    }
    std::string Call_item::print()
    {
        std::string res;
        if(GEN_IR)res+="call ";
        res+=this->callee->print();
        res+="(";
        for(ll i=0;i<this->args.size();++i)
        {
            if(i)res+=',';
            res+=args[i]->print();
        }
        res+=')';
        return res;
    }
    std::string Arrele_item::print()
    {
        std::string res;
        res+=this->base->print();
        for(int i=0;i<this->eles.size();++i)
        {
            res+='[';
            res+=this->eles[i]->print();
            res+=']';
        }
        return res;
    }
    std::string NewArr_item::print()
    {
        std::string res;
        res+="new Array";
        res+='(';
        for(auto i=0;i<this->dims.size();++i)
        {
            if(i)
            {
                res+=',';
            }
            res+=dims[i]->print();
        }
        res+=')';
        return res;
    }
    std::string NewTup_item::print()
    {
        std::string res;
        res+="new Tuple (";
        res+=this->siz->print();
        res+=')';
        return res;
    }
    std::string Length_item::print()
    {
        std::string res;
        res+="length ";
        res+=this->base->print();
        res+=" ";
        res+=this->dim->print();
        return res;
    }
    std::string Fname_item::print()
    {
        std::string res;
        if(GEN_IR)
        {
            res+="@";
        }
        res+=this->fname;
        return res;
    }
    /*
        copy function for items
    
    */
    Var_item* Var_item::copy()
    {
        return this;
    }
    Constant_item* Constant_item::copy()
    {
        return new Constant_item(this->value);
    }
    Op_item* Op_item::copy()
    {
        auto res=new Op_item(this->o_type,this->op1->copy(),this->op2->copy());
        return res;
    }
    Label_item* Label_item::copy()
    {
        return new Label_item(this->label_name);
    }
    Call_item* Call_item::copy()
    {
        auto res=new Call_item();
        res->isRuntime=this->isRuntime;
        res->args=std::vector<Item*>();
        for(auto arg:this->args)
        {
            res->args.push_back(arg->copy());
        }
        res->callee=this->callee->copy();
        return res;
    }
    TypeAnno_item* TypeAnno_item::copy()
    {
        return this;
    }
    Arrele_item* Arrele_item::copy()
    {
        std::vector<Item*>vec(this->eles.size());
        for(auto i=0;i<this->eles.size();++i)
        {
            vec[i]=this->eles[i]->copy();
        }
        return new Arrele_item(this->base->copy(),vec);
    }
    NewArr_item* NewArr_item::copy()
    {
        std::vector<Item*>dims(this->dims.size());
        for(int i=0;i<dims.size();++i)
        {
            dims[i]=this->dims[i]->copy();
        }
        return new NewArr_item(dims);
    }
    NewTup_item* NewTup_item::copy()
    {
        return new NewTup_item(this->siz->copy());
    }
    Length_item* Length_item::copy()
    {
        return new Length_item(this->base->copy(),this->dim->copy());
    }
    Fname_item* Fname_item::copy()
    {
        return this;
    }
    /*
        other interface
    */
    void Constant_item::encode_itself()
    {
        if(is_encoded)return;
        this->value<<=1;
        this->value+=1;
        this->is_encoded=true;
    }
    void Constant_item::decode_itself()
    {
        if(!is_encoded)return;
        this->value>>=1;
        this->is_encoded=false;
    }
    /*
        constructor for Instruction
    */
    
    Instruction::Instruction()
    {

    }
    Instruction_ret::Instruction_ret()
    {
        this->type=InsType::ins_ret;
    }
    Instruction_var_ret::Instruction_var_ret()
    {
        this->type=InsType::ins_var_ret;
    }
    Instruction_var_ret::Instruction_var_ret(Item* var)
    {
        this->type=InsType::ins_var_ret;
        this->var_ret=var;
    }
    Instruction_assignment::Instruction_assignment()
    {
         this->type=InsType::ins_assignment;
    }
    Instruction_assignment::Instruction_assignment(Item* src,Item* des)
    {
         this->type=InsType::ins_assignment;
         this->d=des;
         this->s=src;
    }
    Instruction_label::Instruction_label()
    {
        this->type=InsType::ins_label;
    }
    Instruction_label::Instruction_label(Item* name)
    {
        this->type=InsType::ins_label;
        this->name=name;
    }
    Instruction_call::Instruction_call()
    {
        this->type=InsType::ins_call;
    }
    Instruction_declare::Instruction_declare(Item* type,Item* v)
    {
        this->type=InsType::ins_declare;
        this->anno_type=type;
        this->var=v;
    }
    Instruction_call::Instruction_call(bool isRuntime,Item* var_ret)
    {
        this->type=InsType::ins_call;
        this->caller=var_ret;
    }
    Instruction_call::Instruction_call(Item* caller)
    {
        this->type=InsType::ins_call;
        this->caller=caller;
    }
    Instruction_branch_condi::Instruction_branch_condi()
    {
        this->type=InsType::ins_branch_condi;
    }
    Instruction_branch_condi::Instruction_branch_condi(Item* loc1,Item* loc2,Item* condition)
    {
        this->type=InsType::ins_branch_condi;
        this->location1=loc1;
        this->location2=loc2;
        this->condition=condition;
    }
    Instruction_branch_nocondi::Instruction_branch_nocondi()
    {
        this->type=InsType::ins_branch_nocondi;
    }
    Instruction_branch_nocondi::Instruction_branch_nocondi(Item* dst)
    {
        this->type=InsType::ins_branch_nocondi;
        this->location=dst;
    }

    /*
        print function for Instruction
    */
    std::string Instruction::print()
    {
        return "";
    }
    std::string Instruction_var_ret::print()
    {
        std::string res="return ";
        res+=this->var_ret->print();
        res+='\n';
        return res;
    }
    std::string Instruction_ret::print()
    {
        std::string res="return\n";
        return res;
    }
    std::string Instruction_assignment::print()
    {
        std::string res;
        res+=this->d->print();
        res+=" <- ";
        res+=this->s->print();
        res+='\n';
        return res;
    }
    std::string Instruction_label::print()
    {
        return this->name->print()+"\n";
    }
    std::string Instruction_branch_nocondi::print()
    {
        std::string res;
        res+="br ";
        res+=this->location->print();
        res+='\n';
        return res;
    }
    std::string Instruction_branch_condi::print()
    {
        std::string res;
        res+="br ";
        if(this->condition)
        {
            res+=this->condition->print();
            res+=' ';
        }
        res+=this->location1->print();
        res+=':';
        res+=this->location2->print();
        res+='\n';
        return res;
    }
    std::string Instruction_call::print()
    {
        std::string res;
        if(this->var_ret)
        {
            res+=var_ret->print();
            res+=" <- ";
        }
        res+=this->caller->print();
        res+='\n';
        return res;
    }
    std::string Instruction_declare::print()
    {
        std::string res;
        res+=this->anno_type->print();
        res+=' ';
        res+=this->var->print();
        res+='\n';
        return res;
    }
    /*
        constructor for BasicBlock
    */

    /*
        constructor for Function
    */
    Function::Function()
    {
        this->name="";
        this->var_set=std::set<Item*>();
        this->label_set=std::set<Item*>();
        this->insts=std::vector<Instruction*>();
        this->args=std::vector<Item*>();
        this->name_var_map=std::unordered_map<std::string,Item*>();
        this->label_ptr_map=std::unordered_map<std::string,Item*>();
        this->need_encode=std::set<Constant_item*>();
        this->retType=nullptr;
    }

    
    /*
        constructor for program
    */
    Program::Program()
    {
        this->mainF=nullptr;
    }

 
    /*
        visitor mode
    */
    void Instruction_ret::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_assignment::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_label::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_var_ret::apply(Ins_visitor& vis)
    {
        vis.visit(this);
    }
    void Instruction_branch_nocondi::apply(Ins_visitor& vis)
    {
        vis.visit(this);
    }
    void Instruction_branch_condi::apply(Ins_visitor& vis)
    {
        vis.visit(this);
    }
    void Instruction_call::apply(Ins_visitor& vis)
    {
        vis.visit(this);
    }
    void Instruction_declare::apply(Ins_visitor& vis)
    {
        vis.visit(this);
    }
    /*
        implement copy for all instruction
    */
    Instruction_ret* Instruction_ret::copy()
    {
        return new Instruction_ret();
    }
    Instruction_var_ret* Instruction_var_ret::copy()
    {
        return new Instruction_var_ret(this->var_ret->copy());
    }
    Instruction_assignment* Instruction_assignment::copy()
    {
         return new Instruction_assignment(this->s->copy(),this->d->copy());
    }
    Instruction_label* Instruction_label::copy()
    {
         return new Instruction_label(this->name->copy());
    }
    Instruction_branch_condi* Instruction_branch_condi::copy()
    {
        auto loc1=this->location1->copy();
        auto loc2=this->location2->copy();
        return new Instruction_branch_condi(this->condition->copy(),loc1,loc2);
    }
    Instruction_branch_nocondi* Instruction_branch_nocondi::copy()
    {
        auto dst=this->location->copy();
        return new Instruction_branch_nocondi(dst);
    }
    Instruction_call* Instruction_call::copy()
    {
        auto res=new Instruction_call(this->isRuntime,this->caller);
        res->var_ret=this->var_ret==nullptr?nullptr:this->var_ret->copy();
        return res;
    }
    Instruction_declare* Instruction_declare::copy()
    {
        return new Instruction_declare(this->anno_type->copy(),this->var->copy());
    }
    /*
        basic blocks
    */
    BasicBlock::BasicBlock()
    {
        beg_label=nullptr;
        terminator=nullptr;
        succs=std::set<BasicBlock*>();
        preds=std::set<BasicBlock*>();
    }
    void BasicBlock::print()
    {
        std::cout<<this->beg_label->print();
        for(auto ins:this->insts)
        {
            std::cout<<ins->print();
        }
        std::cout<<this->terminator->print();
    }
    /*
        copy function for Function
    */
    // Function* Function::copy()
    // {
    //     Function* res=new Function();
    //     res->name=this->name;
    //     res->blocks.clear();
    //     for(auto ins:this->blocks)
    //     {
    //         res->instructions.push_back(ins->copy());
    //     }
    //     res->name_var_map=this->name_var_map;
    //     res->label_ptr_map=this->label_ptr_map;
    //     res->var_set=this->var_set;
    //     res->label_set=this->label_set;
    //     return res;
    // }
}