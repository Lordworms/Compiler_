#include "pre_process.h"
namespace LA
{
    void encode_program(Program& p)
    {

    }
    PreCheck::PreCheck(Var_Label_transformer* trans)
    {
        this->trans=trans;
        this->insts=std::vector<Instruction*>();
    }
    void PreCheck::check_alloc(Arrele_item* arr_acc)
    {
        /*
            int64 new_var
            %new_var<-%v=0
            br %new_var :F :T
            :F
            tensor-error(%lineNumber)
            :T
        */
        auto new_label_true=new Label_item(this->trans->new_label_name(""));
        auto new_label_false=new Label_item(this->trans->new_label_name(""));
        auto new_var=new Var_item(this->trans->new_var_name(""));
        //new var
        auto dec_new_var_inst=new Instruction_declare(&LA::int64_anno_ex,new_var);
        insts.push_back(dec_new_var_inst);
        //assign
        auto assign_inst=new Instruction_assignment(new Op_item(new_var,new Constant_item(0),OpType::eq),new_var);
        insts.push_back(assign_inst);
        //branch
        auto br_inst=new Instruction_branch_condi(new_label_true,new_label_false,new_var);
        insts.push_back(br_inst);
        //true label inst
        auto true_label_inst=new Instruction_label(new_label_true);
        insts.push_back(true_label_inst);
        //call tensor
        std::vector<Item*>args={new Constant_item(arr_acc->line_number)};
        auto caller=new Call_item(true,true,args,(Item*)&fname_tensor);
        auto call_tensor=new Instruction_call(caller);
        insts.push_back(call_tensor);
        //false label inst
        auto false_label_inst=new Instruction_label(new_label_false);
        insts.push_back(false_label_inst);
    }
    void PreCheck::check_single(Arrele_item* v)
    {
        /*
            %len_var<- length %arr 0
        */
        auto dim=new Constant_item(0);
        auto new_length_q=new Length_item(v->base,dim);
        auto new_len_var=new Var_item(this->trans->new_var_name(""));
        auto new_len_var_declare_inst=new Instruction_declare(&LA::int64_anno_ex,new_len_var);
        auto assign_inst=new Instruction_assignment(new_length_q,new_len_var);
        insts.push_back(new_len_var_declare_inst);
        insts.push_back(assign_inst);
        /*
            encode offset
        */
        auto encode_off_0=this->add_encode_instruction_new(v->eles[0]);
        /*
            overflow_flag<-encoded offset >= new_len_var
        */
        auto overflow_flag=new Var_item(this->trans->new_var_name(""));
        auto assign_flow=new Instruction_assignment(new Op_item(encode_off_0,new_len_var,OpType::ge),overflow_flag);
        insts.push_back(assign_flow);

        /*
            br overflow_flag :true :false
        */
       auto new_label_true=new Label_item(this->trans->new_label_name(""));
       auto new_label_false=new Label_item(this->trans->new_label_name(""));
       auto branch_inst=new Instruction_branch_condi(new_label_true,new_label_false,overflow_flag);
       insts.push_back(branch_inst);
       /*
            :true
            call tensor-error
       */
       auto line_number=new Constant_item(v->line_number);
       std::vector<Item*>args={line_number,new_len_var,encode_off_0};
       auto caller=new Call_item(true,true,args,&LA::fname_tensor);
       auto call_inst=new Instruction_call(caller);
       insts.push_back(call_inst);
       /*
            :false
       */
       auto new_label_false_inst=new Instruction_label(new_label_false);
       insts.push_back(new_label_false_inst);
    }   
    void PreCheck::check_multi(Arrele_item* arr_acc)
    {
        /*
            dim<-i
            arr_len<- length arr dim
            encode_off_i<-arr_acc->offset[i].encode
            overflow_flag<- encode_off_i >=arr_len
            br overflow_flag :true :error_execute
            :true

            :error_execute
                call tensor-error(line_number,dim,arrlen,encode_off_i)
        */
        //define four things
        auto dim_var=new Var_item(this->trans->new_var_name(""));
        auto arr_len=new Var_item(this->trans->new_var_name(""));
        auto overflow_flag=new Var_item(this->trans->new_var_name(""));
        auto encode_off_i=new Var_item(this->trans->new_var_name(""));
        auto false_label_final=new Label_item(this->trans->new_label_name(""));
        auto true_label_final=new Label_item(this->trans->new_label_name(""));//true means overflow
        for(ll dim=0;dim<arr_acc->eles.size();++dim)
        {
            
            /*
                now_dim<-encode(dim)
            */
            auto now_dim=new Constant_item(dim);
            now_dim->encode_itself();
            /*
                dim_var<-now_dim
            */
            auto assign_now_dim=new Instruction_assignment(now_dim,dim_var);
            insts.push_back(assign_now_dim);

            /*
                arr_length<- length %arr now_dim
            */

            auto dim_var=new Constant_item(dim);
            auto length_q=new Length_item(arr_acc->base,dim_var);
            auto assign_length_inst=new Instruction_assignment(length_q,arr_len);

            /*
                encode_off<-encode(arr->offset[i])
            */
            auto encode_res=add_encode_instruction_new(arr_acc->eles[dim]);
            auto assign_encode_res_inst=new Instruction_assignment(encode_res,encode_off_i);
            insts.push_back(assign_encode_res_inst);
            /*
                over_flow_flag<-encode_off>=arr_length
            */
           auto assign_overflow_inst=new Instruction_assignment(new Op_item(encode_off_i,arr_len,OpType::ge),overflow_flag);
           insts.push_back(assign_overflow_inst);
           /*
                br over_flow_flag :final_true  :now_out
           */
           auto now_out_label=new Label_item(this->trans->new_label_name(""));
           auto branch_now_end_inst=new Instruction_branch_condi(true_label_final,now_out_label,overflow_flag);
           insts.push_back(branch_now_end_inst);
           /*
                :now_out
           */
           auto now_out_inst=new Instruction_label(now_out_label);
           insts.push_back(now_out_inst);
        }
        /*
            goto out
        */
        auto goto_final_false_inst=new Instruction_branch_nocondi(false_label_final);
        insts.push_back(goto_final_false_inst);

        /*
            :final_true
        */
        auto final_true_inst=new Instruction_label(true_label_final);
        insts.push_back(final_true_inst);

        /*
            call tensor-error(line_number,dim,length,offset_encode_i)
        */
        auto line_number=new Constant_item(arr_acc->line_number);
        line_number->encode_itself();
        std::vector<Item*>args={
            line_number,    
            dim_var,
            arr_len,
            encode_off_i    
        };
        auto caller=new Call_item(true,true,args,&LA::fname_tensor);
        auto call_inst=new Instruction_call(caller);
        insts.push_back(call_inst);

        /*
            :final_false
        */
        auto final_false_inst=new Instruction_label(false_label_final);
        insts.push_back(final_false_inst);
    }
    void PreCheck::check_boundary(Arrele_item* v)
    {
        auto ori=dynamic_cast<Var_item*>(v->base);
        auto anno=dynamic_cast<TypeAnno_item*>(ori->anno);
        if(anno->a_type==AnnoType::tuple_anno)
        {
            return;
        }
        if(v->eles.size()==1)
        {
            check_single(v);
        }
        else
        {
            check_multi(v);
        }
    }
    Var_item* PreCheck::add_encode_instruction_new(Item* v)
    {   
        auto new_var=new Var_item(this->trans->new_var_name(""));
        auto sr=new Instruction_assignment(new Op_item(v,new Constant_item(1),OpType::right_shift),new_var);
        auto plus=new Instruction_assignment(new Op_item(new_var,new Constant_item(1),OpType::plus),new_var);
        this->insts.push_back(sr);
        this->insts.push_back(plus);
        return new_var;
    }

    /*
        adding basic block
    */
   bool is_terminator(Instruction* ins)
   {
        return ins->type==InsType::ins_ret||ins->type==InsType::ins_var_ret||ins->type==InsType::ins_branch_condi||ins->type==InsType::ins_branch_nocondi;
   }
   void adding_basic_block(Program& p,Var_Label_transformer* trans)
   {
        for(auto F:p.functions)
        {
            bool startBB=true;
            std::vector<Instruction*>inst_list;
            for(ll i=0;i<F->insts.size();++i)
            {
                auto ins=F->insts[i];
                if(startBB)
                {
                    if(ins->type!=InsType::ins_label)
                    {
                        auto new_label=new Label_item(trans->new_label_name(""));
                        auto bb_beg_inst=new Instruction_label(new_label);
                        inst_list.push_back(bb_beg_inst);
                    }
                    startBB=false;
                }
                else if(ins->type == InsType::ins_label)
                {
                    auto label_inst=dynamic_cast<Instruction_label*>(ins);
                    auto jump_to_label=new Instruction_branch_nocondi(label_inst->name);
                    inst_list.push_back(jump_to_label);
                }
                if(is_terminator(ins))
                {
                    startBB=true;
                }
                inst_list.push_back(ins);
            }
            if(!startBB)
            {
                if(F->retType->a_type==AnnoType::void_anno)
                {
                    inst_list.push_back(new Instruction_ret());
                }
                else
                {
                    inst_list.push_back(new Instruction_var_ret(new Constant_item(0)));
                }
            }
            F->insts=inst_list;
        }
   }
}