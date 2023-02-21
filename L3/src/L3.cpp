#include "L3.h"
namespace L3
{
    //init extern shits
    Register_item reg_r12(Register_ID::r12);
    Register_item reg_r13(Register_ID::r13);
    Register_item reg_r14(Register_ID::r14);
    Register_item reg_r15(Register_ID::r15);
    Register_item reg_rbp(Register_ID::rbp);
    Register_item reg_rbx(Register_ID::rbx);
    //caller_saved
    Register_item reg_rdi(Register_ID::rdi);
    Register_item reg_rsi(Register_ID::rsi);
    Register_item reg_rcx(Register_ID::rcx);
    Register_item reg_r8(Register_ID::r8);
    Register_item reg_r9(Register_ID::r9);
    Register_item reg_rax(Register_ID::rax);
    Register_item reg_r10(Register_ID::r10);
    Register_item reg_r11(Register_ID::r11);
    Register_item reg_rdx(Register_ID::rdx);


    //stack ptr
    Register_item reg_rsp(Register_ID::rsp);
    
    
    //define std::vector for use
    std::vector<Register_item*>callee_saved={&reg_r12,&reg_r13,&reg_r14,&reg_r15,&reg_rbp,&reg_rbx};
    std::vector<Register_item*>caller_saved={&reg_r10,&reg_r11,&reg_r8,&reg_r9,&reg_rax,&reg_rcx,&reg_rdi,&reg_rdx,&reg_rsi};
    std::vector<Register_item*>args_reg={&reg_rdi,&reg_rsi,&reg_rdx,&reg_rcx,&reg_r8,&reg_r9};
    std::vector<Register_item*>gp_reg={&reg_rdi,&reg_rax,&reg_rsi,&reg_rdx,&reg_rcx,&reg_r8,&reg_r9,&reg_rbx,&reg_rbp,&reg_r10,&reg_r11,&reg_r12,&reg_r13,&reg_r14,&reg_r15};
    Label_item runtime_print("print");
    Label_item runtime_input("input");
    Label_item runtime_allocate("allocate");
    Label_item runtime_tensor("tensor-error");

    AopType get_aop_type(std::string s)
    {
        if(s=="+")return AopType::plus;
        if(s=="-")return AopType::minus;
        if(s=="-")return AopType::multy;
        if(s=="-")return AopType::bit_and;
        if(s=="-")return AopType::left_shift;
        if(s=="-")return AopType::right_shift;
        std::cout<<"invalid aop type!\n";
        return AopType::plus;
    }
    CmpType get_cmp_type(std::string s)
    {
        if(s=="<")return CmpType::lt;
        if(s=="<=")return CmpType::le;
        if(s=="=")return CmpType::eq;
        if(s==">")return CmpType::gt;
        if(s==">=")return CmpType::ge;
        std::cout<<"invalid cmp type!\n";
        return CmpType::eq;
    }
    //get aop character
    std::string aop_str(L3::AopType type)
    {
        switch (type)
        {
            case AopType::bit_and:
                return "&";
                break;
            case AopType::minus:
                return "-";
                break;
            case AopType::multy:
                return "*";
                break;
            case AopType::plus:
                return "+";
                break;
            case AopType::left_shift:
                return "<<";
                break;
            case AopType::right_shift:
                return ">>";
                break;
            default:
                std::cout<<"error type in transferring aop character\n";
                break;
        }
        return "";
    }
    std::string cmp_str(CmpType type)
    {
        switch (type)
        {
            case CmpType::eq:
                return "=";
                break;
            case CmpType::ge:
                return ">=";
                break;
            case CmpType::gt:
                return ">";
                break;
            case CmpType::le:
                return "<=";
                break;
            case CmpType::lt:
                return "<";
                break;
            default:
                std::cout<<"error type in transferring cmp character\n";
                break;
        }
        return "";
    }
    std::string cmp_transfer(CmpType type)
    {
        switch (type)
        {
            case CmpType::ge:
                return "<=";
                break;
            case CmpType::gt:
                return "<";
                break;
            case CmpType::eq:
                return "=";
                break;
            case CmpType::le:
                return "<=";
                break;
            case CmpType::lt:
                return "<";
                break;
            default:
                std::cout<<"inavlide transfer!\n";
                return "";
                break;
        }
    }
    bool check_runtime_call(Item* callee)
    {
        if(callee==&runtime_allocate)
        {
            
        }
        return callee==&L3::runtime_print||callee==&L3::runtime_allocate||callee==&L3::runtime_input||callee==&L3::runtime_tensor;
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
    Register_item::Register_item()
    {
        this->type=iType::register_item;
    }
    Register_item::Register_item(Register_ID id)
    {
        this->regID=id;
        this->type=iType::register_item;
    }
    Constant_item::Constant_item()
    {
        this->type=iType::constant_item;
    }
    Constant_item::Constant_item(long long v)
    {
        this->type=iType::constant_item;
        this->value=v;
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
    Aop_item::Aop_item()
    {
        this->type=iType::aop_item;
    }
    Aop_item::Aop_item(Item* o1,Item* o2,AopType t)
    {
        this->op1=o1;
        this->op2=o2;
        this->aType=t;
        this->type=iType::aop_item;
    }
    
    Cmp_item::Cmp_item()
    {
        this->type=iType::cmp_item;
    }
    Cmp_item::Cmp_item(Item* o1,Item* o2,CmpType t)
    {
        this->op1=o1;
        this->op2=o2;
        this->type=iType::cmp_item;
        this->cType=t;
    }
    Var_item::Var_item()
    {
        this->type=iType::var_item;
    }
    Var_item::Var_item(std::string str)
    {
        this->type=iType::var_item;
        this->var_name=str;
    }
    Store_item::Store_item()
    {
        this->type=iType::store_item;
    }
    Store_item::Store_item(Item* ptr)
    {
        this->type=iType::store_item;
        this->ptr=ptr;
    }
    Load_item::Load_item(Item* var)
    {
        this->type=iType::load_item;
        this->var=var;
    }
    Load_item::Load_item()
    {
        this->type=iType::load_item;
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
    /*
        print function for items
    */
    std::string Aop_item::print()
    {
        std::string res;
        res+=this->op1->print();
        res+=' ';
        res+=aop_str(this->aType);
        res+=' ';
        res+=this->op2->print();
        return res;
    }
    std::string Cmp_item::print()
    {
        std::string res;
        res+=this->op1->print();
        res+=' ';
        res+=cmp_str(this->cType);
        res+=' ';
        res+=this->op2->print();
        return res;
    }
    std::string Var_item::print()
    {
        return this->var_name;
    }
    std::string Constant_item::print()
    {
        return std::to_string(this->value);
    }
    std::string Register_item::print()
    {
        std::string res;
        switch (this->regID)
        {
            case Register_ID::rdi:
            {
                res+="rdi";
                break;
            }
            case Register_ID::rbx:
            {
                res+="rbx";
                break;
            }
            case Register_ID::rsi:
            {
                res+="rsi";
                break;
            }
            case Register_ID::rax:
            {
                res+="rax";
                break;
            }
            case Register_ID::rdx:
            {
                res+="rdx";
                break;
            }
            case Register_ID::r8:
            {
                res+="r8";
                break;
            }
            case Register_ID::r9:
            {
                res+="r9";
                break;
            }
            case Register_ID::r10:
            {
                res+="r10";
                break;
            }
            case Register_ID::r11:
            {
                res+="r11";
                break;
            }
            case Register_ID::r12:
            {
                res+="r12";
                break;
            }
            case Register_ID::r13:
            {
                res+="r13";
                break;
            }
            case Register_ID::r14:
            {
                res+="r14";
                break;
            }
            case Register_ID::r15:
            {
                res+="r15";
                break;
            }
            case Register_ID::rcx:
            {
                res+="rcx";
                break;
            }
            case Register_ID::rsp:
            {
                res+="rsp";
                break;
            }
            case Register_ID::rbp:
            {
                res+="rbp";
                break;
            }
            default:
                break;
        }
        return res;
    }
    std::string Label_item::print()
    {
        return this->label_name;
    }
    std::string Store_item::print()
    {
        std::string res;
        res+="store ";
        res+=this->ptr->print();
        return res;
    }
    std::string Load_item::print()
    {
        std::string res;
        res+="load";
        res+=this->var->print();
        return res;
    }
    std::string Call_item::print()
    {
        std::string res;
        res+="call ";
        res+=this->callee->print();
        res+="(";
        for(auto arg:this->args)
        {
            res+=arg->print();
            res+=' ';
        }
        res+=')';
        return res;
    }



    /*
        copy function for items
    
    */
    Register_item* Register_item::copy()
    {
        return this;
    }
    Constant_item* Constant_item::copy()
    {
        return new Constant_item(this->value);
    }
    Aop_item* Aop_item::copy()
    {
        auto res=new Aop_item(this->op1->copy(),this->op2->copy(),this->aType);
        return res;
    }
    Cmp_item* Cmp_item::copy()
    {
        return new Cmp_item(this->op1->copy(),this->op2->copy(),this->cType);
    }
    Label_item* Label_item::copy()
    {
        return new Label_item(this->label_name);
    }
    Var_item* Var_item::copy()
    {
        return this;
    }
    Load_item* Load_item::copy()
    {
        return new Load_item(this->var->copy());
    }
    Store_item* Store_item::copy()
    {
        return new Store_item(this->ptr->copy());
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
    Instruction_call::Instruction_call(bool isRuntime,Item* var_ret)
    {
        this->type=InsType::ins_call;
        this->caller=var_ret;
    }
    Instruction_branch::Instruction_branch()
    {
        this->type=InsType::ins_branch;
    }
    Instruction_branch::Instruction_branch(Item* dst,Item* condition)
    {
        this->type=InsType::ins_branch;
        this->location=dst;
        this->condition=condition;
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
    std::string Instruction_branch::print()
    {
        std::string res;
        res+="br ";
        if(this->condition)
        {
            res+=this->condition->print();
            res+=' ';
        }
        res+=this->location->print();
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


    /*
        constructor for Function
    */
    Function::Function()
    {
        this->name="";
        this->var_set=std::set<Item*>();
        this->label_set=std::set<Item*>();
        this->instructions=std::vector<Instruction*>();
        this->args=std::vector<Item*>();
        this->name_var_map=std::unordered_map<std::string,Item*>();
        this->label_ptr_map=std::unordered_map<std::string,Item*>();
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
    void Instruction_branch::apply(Ins_visitor& vis)
    {
        vis.visit(this);
    }
    void Instruction_call::apply(Ins_visitor& vis)
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
    Instruction_branch* Instruction_branch::copy()
    {
        auto condi=this->condition==nullptr?nullptr:this->condition->copy();
        return new Instruction_branch(this->location->copy(),condi);
    }
    Instruction_call* Instruction_call::copy()
    {
        auto res=new Instruction_call(this->isRuntime,this->caller);
        res->var_ret=this->var_ret==nullptr?nullptr:this->var_ret->copy();
        return res;
    }
    /*
        copy function for Function
    */
    Function* Function::copy()
    {
        Function* res=new Function();
        res->name=this->name;
        res->instructions.clear();
        for(auto ins:this->instructions)
        {
            res->instructions.push_back(ins->copy());
        }
        res->name_var_map=this->name_var_map;
        res->label_ptr_map=this->label_ptr_map;
        res->var_set=this->var_set;
        res->label_set=this->label_set;
        return res;
    }
}