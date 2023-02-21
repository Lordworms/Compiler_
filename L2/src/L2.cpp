#include "L2.h"

namespace L2
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
    //define vector for use
    std::vector<Register_item*>callee_saved={&reg_r12,&reg_r13,&reg_r14,&reg_r15,&reg_rbp,&reg_rbx};
    std::vector<Register_item*>caller_saved={&reg_r10,&reg_r11,&reg_r8,&reg_r9,&reg_rax,&reg_rcx,&reg_rdi,&reg_rdx,&reg_rsi};
    std::vector<Register_item*>args_reg={&reg_rdi,&reg_rsi,&reg_rdx,&reg_rcx,&reg_r8,&reg_r9};
    std::vector<Register_item*>gp_reg={&reg_rdi,&reg_rax,&reg_rsi,&reg_rdx,&reg_rcx,&reg_r8,&reg_r9,&reg_rbx,&reg_rbp,&reg_r10,&reg_r11,&reg_r12,&reg_r13,&reg_r14,&reg_r15};
    //color -> reg & reg->color
    std::unordered_map<Item*,Reg_color>reg_color_mp=
    {
        {&reg_rdi,rdi_color},
        {&reg_rax, rax_color},
        {&reg_rsi, rsi_color},
        {&reg_rdx, rdx_color},
        {&reg_rcx, rcx_color},
        {&reg_r8, r8_color},
        {&reg_r9, r9_color},
        {&reg_rbx, rbx_color},
        {&reg_rbp, rbp_color},
        {&reg_r10, r10_color},
        {&reg_r11, r11_color},
        {&reg_r12, r12_color},
        {&reg_r13, r13_color},
        {&reg_r14, r14_color},
        {&reg_r15, r15_color},
    };
    std::unordered_map<Reg_color,Item*>color_reg_mp=
    {
        {rdi_color,&reg_rdi},
        { rax_color,&reg_rax},
        { rsi_color,&reg_rsi},
        { rdx_color,&reg_rdx},
        { rcx_color,&reg_rcx},
        { r8_color,&reg_r8},
        { r9_color,&reg_r9},
        { rbx_color,&reg_rbx},
        { rbp_color,&reg_rbp},
        { r10_color,&reg_r10},
        { r11_color,&reg_r11},
        { r12_color,&reg_r12},
        { r13_color,&reg_r13},
        { r14_color,&reg_r14},
        { r15_color,&reg_r15},
    };
    std::vector<Reg_color>color_order=
    {
        r10_color,
        r11_color,
        r8_color,
        r9_color,
        rax_color,
        rcx_color,
        rdi_color,
        rdx_color,
        rsi_color,

        rbx_color,
        rbp_color,
        r12_color,
        r13_color,
        r14_color,
        r15_color        

    };
    Label_item runtime_print("print");
    Label_item runtime_input("input");
    Label_item runtime_allocate("allocate");
    Label_item runtime_tensor("tensor-error");
    //get aop sop cmpcharacter
    std::string aop_str(AopType type)
    {
        switch (type)
        {
        case AopType::plus_eq:
            return "+=";
            break;
        case AopType::minus_eq:
            return "-=";
            break;
        case AopType::mul_eq:
            return "*=";
            break;
        case AopType::bitand_eq:
            return "&=";
            break;
        default:
            std::cout<<"error type in transferring aop character\n";
            break;
        }
        return "";
    }
    std::string sop_str(ShiftType type)
    {
        switch (type)
        {
        case ShiftType::left:
            return "<<=";
            break;
        case ShiftType::right:
            return ">>=";
            break;
        default:
            std::cout<<"error type in transferring sop character\n";
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
    //给基类构造函数赋值
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
    Memory_item::Memory_item()
    {
        this->type=iType::memoryAccess_item;
    }
    Memory_item::Memory_item(Item* id,Item* offset)
    {
        this->type  = iType::memoryAccess_item;
        this->regId = id;
        this->offset= offset;
    }
    StackArg_item::StackArg_item()
    {
        this->type=iType::stackArg_item;
    }
    StackArg_item::StackArg_item(Item* offset)
    {
        this->type=iType::stackArg_item;
        this->offset=offset;
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
    //Instruction 构造函数
    Instruction::Instruction()
    {

    }
    Instruction_ret::Instruction_ret()
    {
        this->type=InsType::ins_ret;
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
    Instruction_aop::Instruction_aop()
    {
        this->type=InsType::ins_aop;
    }
    Instruction_sop::Instruction_sop()
    {
        this->type=InsType::ins_sop;
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
    Instruction_lea::Instruction_lea()
    {
        this->type=InsType::ins_lea;
    }
    Instruction_goto::Instruction_goto()
    {
        this->type=InsType::ins_goto;
    }
    Instruction_cjump::Instruction_cjump()
    {
        this->type=InsType::ins_cjump;
    }
    Instruction_call::Instruction_call()
    {
        this->type=InsType::ins_call;
    }
    Instruction_call::Instruction_call(bool is)
    {
        this->isRuntime=is;
        this->type=InsType::ins_call;
    }
    Instruction_inc::Instruction_inc()
    {
        this->type=InsType::ins_inc;
    }
    Instruction_dec::Instruction_dec()
    {
        this->type=InsType::ins_dec;
    }
    Instruction_common::Instruction_common()
    {
        this->type=InsType::ins_call;
        this->isRuntime=false;
    }
    Instruction_runtime::Instruction_runtime()
    {
        this->type=InsType::ins_call;
        this->isRuntime=true;
    }
    //implement the print function for Instructions
    std::string Instruction::print()
    {
        return "";
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
    std::string Instruction_aop::print()
    {
        std::string res;
        res+=this->op1->print();
        res+=' ';
        res+=aop_str(this->rule);
        res+=' ';
        res+=this->op2->print();
        res+='\n';
        return res;
    }
    std::string Instruction_sop::print()
    {
        std::string res;
        res+=this->target->print();
        res+=' ';
        res+=sop_str(this->direction);
        res+=' ';
        res+=this->offset->print();
        res+='\n';
        return res;        
    }
    std::string Instruction_label::print()
    {
        return this->name->print()+"\n";
    }
    std::string Instruction_lea::print()
    {
        std::string res;
        res+=this->des->print();
        res+='@';
        res+=this->src->print();
        res+=' ';
        res+=this->muler->print();
        res+=' ';
        res+=this->cons->print();
        res+='\n';
        return res;
    }
    std::string Instruction_goto::print()
    {
       std::string res;
       res+="goto ";
       res+=this->label->print();
       res+='\n';
       return res;
    }
    std::string Instruction_cjump::print()
    {
        std::string res;
        res+="cjump ";
        res+=this->condi->print();
        res+=' ';
        res+=this->label->print();
        res+='\n';
        return res;
    }
    // std::string Instruction_call::print()
    // {
    //     std::string res;
    //     res+="call";

    // }
    std::string Instruction_inc::print()
    {
        std::string res;
        res+=this->op->print();
        res+="++";
        res+='\n';
        return res;
    }
    std::string Instruction_dec::print()
    {
        std::string res;
        res+=this->op->print();
        res+="--";
        res+='\n';
        return res;
    }
    std::string Instruction_common::print()
    {
        std::string res="call";
        res+=' ';
        res+=this->callee->print();
        res+=' ';
        res+=this->num_cnt->print();
        res+='\n';
        return res;
    }
    std::string Instruction_runtime::print()
    {
        std::string res;
        res+="call";
        res+=' ';
        res+=this->func_name->print();
        res+=' ';
        res+=this->arg_cnt->print();
        res+='\n';
        return res;
    }
    /*
        override the copy method for Instruction

    */


    //override the fucnction apply to make it visit

    //function 构造函数
    Function::Function()
    {
        this->name="";
        this->instructions=std::vector<Instruction*>();
        this->locals=0;
        this->arguments=0;
        this->name_var_map=std::unordered_map<std::string,Item*>();
        this->label_ptr_map=std::unordered_map<std::string,Item*>();
    }
    //program 构造函数
    Program::Program()
    {

    }
    //print method!
    std::string Aop_item::print()
    {
        std::string res="Aop";
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
        //res+='\n';
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
    std::string StackArg_item::print()
    {
        std::string res;
        res+="stack-arg ";
        res+=this->offset->print();
        return res;
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
    std::string Memory_item::print()
    {
        std::string res;
        res+="mem ";
        res+=this->regId->print();
        res+=" ";
        res+=this->offset->print();
        return res;
    }
    std::string Label_item::print()
    {
        return this->label_name;
    }
    //implement copy method for item
    Register_item* Register_item::copy()
    {
        return this;
    }
    Memory_item* Memory_item::copy()
    {
        return new Memory_item(this->regId->copy(),this->offset->copy());
    }
    Constant_item* Constant_item::copy()
    {
        return new Constant_item(this->value);
    }
    Aop_item* Aop_item::copy()
    {
        auto tmp=new Aop_item();
        tmp->aType=this->aType;
        tmp->type=this->type;
        return this;
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
    StackArg_item* StackArg_item::copy()
    {
        return new StackArg_item(this->offset->copy());
    }
    //apply method for Instruction
    void Instruction_ret::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_assignment::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_aop::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_sop::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_label::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_lea::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_goto::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_cjump::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_inc::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_dec::apply(Ins_visitor& vis)
    {
         vis.visit(this);
    }
    void Instruction_common::apply(Ins_visitor& vis)
    {
        if(isPrint)std::cout<<"dealing with a commmon call\n";
        vis.visit(this);
    }
    void Instruction_runtime::apply(Ins_visitor& vis)
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
    Instruction_assignment* Instruction_assignment::copy()
    {
         return new Instruction_assignment(this->s->copy(),this->d->copy());
    }
    Instruction_aop* Instruction_aop::copy()
    {
        auto res=new Instruction_aop();
        res->op1=this->op1->copy();
        res->op2=this->op2->copy();
        res->rule=this->rule;
        return res;
    }
    Instruction_sop* Instruction_sop::copy()
    {
        auto res=new Instruction_sop();
        res->target=this->target->copy();
        res->offset=this->offset->copy();
        res->direction=this->direction;
        return res;
    }
    Instruction_label* Instruction_label::copy()
    {
         return new Instruction_label(this->name->copy());
    }
    Instruction_lea* Instruction_lea::copy()
    {
        auto res=new Instruction_lea();
        res->cons=this->cons->copy();
        res->muler=this->muler->copy();
        res->src=this->src->copy();
        res->des=this->des->copy();
        return res;
    }
    Instruction_goto* Instruction_goto::copy()
    {
        auto res=new Instruction_goto();
        res->label=this->label->copy();
        return res;
    }
    Instruction_cjump* Instruction_cjump::copy()
    {
        auto res=new Instruction_cjump();
        res->condi=this->condi->copy();
        res->label=this->label->copy();
        return res;
    }
    Instruction_inc* Instruction_inc::copy()
    {
        auto res=new Instruction_inc();
        res->op=this->op->copy();
        return res;
    }
    Instruction_dec* Instruction_dec::copy()
    {
        auto res=new Instruction_dec();
        res->op=this->op->copy();
        return res;
    }
    Instruction_common* Instruction_common::copy()
    {
        auto res=new Instruction_common();
        res->callee=this->callee->copy();
        res->num_cnt=this->num_cnt->copy();
        res->isRuntime=false;
        return res;
    }
    Instruction_runtime* Instruction_runtime::copy()
    {
        auto res=new Instruction_runtime();
        res->func_name=this->func_name->copy();
        res->arg_cnt=this->arg_cnt->copy();
        return res;
    }
    /*
        adding copy for function
    */
    Function* Function::copy()
    {
        Function* res=new Function();
        res->arguments=this->arguments;
        res->name=this->name;
        res->locals=this->locals;
        res->instructions.clear();
        for(auto ins:this->instructions)
        {
            res->instructions.push_back(ins->copy());
        }
        res->name_var_map=this->name_var_map;
        res->label_ptr_map=this->label_ptr_map;
        return res;
    }
}