#include "tilling.h"
namespace L3
{
    //tile interface
    Tile::Tile()
    {

    }
    AopTile::AopTile()
    {
        this->node_cnt=3;
        this->cost=2;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(L3::aop_ops);
        head->add(new PatoperandNode(var_consts_types));
        head->add(new PatoperandNode(var_consts_types));
        this->pt->head=head;
        this->tile_name="aop_tile";
    }
    AssignVarTile::AssignVarTile()
    {
        this->node_cnt=3;
        this->cost=1;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head = new PatopNode(OpType::op_assign);
        head->add(new PatoperandNode(iType::var_item));
        head->add(new PatoperandNode(var_consts_label_types));
        this->pt->head=head;
        this->tile_name="assign_var_tile";
    }
    AssignStoreTile::AssignStoreTile()
    {
        this->cost=1;
        this->node_cnt=4;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head =new PatopNode(OpType::op_assign);
        auto store=new PatopNode(OpType::op_store);
        store->add(new PatoperandNode(iType::var_item));
        head->add(store);
        head->add(new PatoperandNode(var_consts_label_types));
        this->pt->head=head;
        this->tile_name="assign_store_tile";
    }
    CmpTile::CmpTile()
    {
        this->node_cnt=3;
        this->cost=1;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(cmp_ops);
        head->add(new PatoperandNode(var_consts_types));
        head->add(new PatoperandNode(var_consts_types));
        this->pt->head=head;
        this->tile_name="cmp_tile";
    }
    RetTile::RetTile()
    {
        this->node_cnt=1;
        this->cost=1;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_ret);
        this->pt->head=head;
        this->tile_name="ret_tile";
    }
    RetVarTile::RetVarTile()
    {
        this->node_cnt=2;
        this->cost=2;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_ret);
        head->add(new PatoperandNode(var_consts_label_types));
        this->pt->head=head;
        this->tile_name="ret_var_tile";
    }
    CallTile::CallTile(bool isRuntime,ll num)
    {
        this->arg_num=num;
        this->isRuntime=isRuntime;
        if(isRuntime)
        {
            this->cost=num+1;
            this->node_cnt=arg_num+2;//callee + op
        }
        else
        {
            this->cost=num+3;//caller+return label + return addr
            this->node_cnt=arg_num+2;
        }
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_call,isRuntime);
        //push callee
        head->add(new PatoperandNode(var_label_types));
        //push args
        for(auto i=0;i<num;++i)
        {
            head->add(new PatoperandNode(var_consts_label_types));
        }
        this->pt->head=head;
        this->tile_name="call_tile_"+std::to_string(isRuntime)+'_'+std::to_string(num);
    }
    LabelTile::LabelTile()
    {
        this->cost=1;
        this->node_cnt=2;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_label);
        head->add(new PatoperandNode(iType::label_item));
        this->tile_name="label_tile";
        this->pt->head=head;
    }
    LoadTile::LoadTile()
    {
        this->cost=1;
        this->node_cnt=2;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_load);
        head->add(new PatoperandNode(iType::var_item));
        this->tile_name="load_tile";
        this->pt->head=head;
    }
    UnCondiBrTile::UnCondiBrTile()
    {
        this->cost=1;
        this->node_cnt=2;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_branch);
        head->add(new PatoperandNode(iType::label_item));
        this->tile_name="uncondibr_tile";
        this->pt->head=head;
    }
    CondiBrCmpTile::CondiBrCmpTile()
    {
        this->cost=1;
        this->node_cnt=5;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_branch);
        auto cmp=new PatopNode(cmp_ops);
        cmp->add(new PatoperandNode(var_consts_types));
        cmp->add(new PatoperandNode(var_consts_types));
        head->add(cmp);
        head->add(new PatoperandNode(iType::label_item));
        this->tile_name="condibrCmp_tile";
        this->pt->head=head;
    }
    CondiBrVarTile::CondiBrVarTile()
    {
        this->cost=1;
        this->node_cnt=3;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_branch);
        head->add(new PatoperandNode(iType::var_item));
        head->add(new PatoperandNode(iType::label_item));
        this->tile_name="condiBrVar_tile";
        this->pt->head=head;
    }
    CondiBrConstTile::CondiBrConstTile()
    {
        this->cost=1;
        this->node_cnt=3;
        this->matched_nodes=std::set<GenNode*>();
        this->pt=new PatTree;
        auto head=new PatopNode(OpType::op_branch);
        head->add(new PatoperandNode(iType::constant_item));
        head->add(new PatoperandNode(iType::label_item));
        this->tile_name="condiBrConst_tile";
        this->pt->head=head;
    }
    //gen_code
    void AopTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        ops->res=new Var_item(prefix+"_"+std::to_string(L3_new_var_cnt++));
        std::string res;
        res+=translate_node(ops);
        res+=" ";
        res+=transfer_optype(OpType::op_assign);
        res+=" ";
        res+=translate_node(ops->childs[0]);
        res+='\n';

        inst_strs.push_back(res);
        res="";

        res+=translate_node(ops);
        res+=" ";
        res+=transfer_optype(ops->op)+'=';
        res+=" ";
        res+=translate_node(ops->childs[1]);
        res+='\n';

        inst_strs.push_back(res);
    }
    void AssignVarTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        std::string res;
        if(ops->childs[0]->print()=="%v")
        {
            int x=0;
        }
        res+=translate_node(ops->childs[0]);
        res+=' ';
        res+=transfer_optype(OpType::op_assign);
        res+=' ';
        res+=translate_node(ops->childs[1]);
        res+='\n';
        inst_strs.push_back(res);
    }
    void AssignStoreTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        auto store=(OpNode*)(inst_node->childs[0]);
        std::string res;
        res+="mem ";
        res+=translate_node(store->childs[0]);
        res+=' ';
        res+='0';
        res+=transfer_optype(OpType::op_assign);
        res+=' ';
        res+=translate_node(ops->childs[1]);
        res+='\n';
        inst_strs.push_back(res);
    }
    void RetVarTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        std::string res;
        res+="rax <-";
        res+=translate_node(ops->childs[0]);
        res+='\n';
        inst_strs.push_back(res);
        res="";
        res+=ops->print();
        res+='\n';
        inst_strs.push_back(res);
    }
    void RetTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        std::string res;
        res+=ops->print();
        res+="\n";
        inst_strs.push_back(res);
    }
    void CallTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        ops->res=new Var_item(prefix+"_"+std::to_string(L3_new_var_cnt++));
        auto callee=ops->childs[0];
        auto func_name=callee->print();
        auto no_col_func_name=func_name.substr(1,func_name.size()-1);
        std::string res;
        for(ll i=0;i<this->arg_num;++i)
        {
            if(i<CALLEE_NUM)
            {
                res+=args_reg[i]->print();
            }
            else
            {
                res+="mem rsp ";
                auto offset=(i-CALLEE_NUM+2)*(-8);
                res+=std::to_string(offset);
            }
            res+=" <- ";
            res+=translate_node(ops->childs[i+1]);
            res+='\n';
            inst_strs.push_back(res);
            res="";
        }
        std::string new_ret_label;
        if(!this->isRuntime)
        {
            new_ret_label=L3_ret_var_prefix+'_'+no_col_func_name+'_'+std::to_string(L3_ret_var_cnt++);
            res="mem rsp -8 <- "+new_ret_label+'\n';
            inst_strs.push_back(res);
            res="";
        }
        res="call "+translate_node(callee)+" "+std::to_string(this->arg_num)+'\n';
        inst_strs.push_back(res);
        res="";
        if(!this->isRuntime)
        {
            res=new_ret_label+'\n';
            inst_strs.push_back(res);
            res="";
        }
        res=translate_node(ops)+" <- rax\n";
        inst_strs.push_back(res);
    }
    void CmpTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        ops->res=new Var_item(prefix+"_"+std::to_string(L3_new_var_cnt++));
        std::string res;
        if(ops->op==OpType::op_gt)
        {
            res=translate_node(ops)+" <- "+translate_node(ops->childs[1])+" "+transfer_optype(op_lt)+" "+translate_node(ops->childs[0])+'\n';
        }
        else if(ops->op==OpType::op_ge)
        {
            res=translate_node(ops)+" <- "+translate_node(ops->childs[1])+" "+transfer_optype(op_le)+" "+translate_node(ops->childs[0])+'\n';
        }
        else
        {
            res=translate_node(ops)+" <- "+translate_node(ops->childs[0])+" "+transfer_optype(ops->op)+" "+translate_node(ops->childs[1])+'\n';
        }
        inst_strs.push_back(res);
    }
    void LabelTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        std::string res=ops->childs[0]->print()+'\n';
        inst_strs.push_back(res);
    }
    void LoadTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        ops->res=new Var_item(prefix+"_"+std::to_string(L3_new_var_cnt++));
        auto load=(OpNode*)(ops->childs[0]);
        std::string res=translate_node(ops)+" <- mem "+translate_node(ops->childs[0])+" 0\n";
        inst_strs.push_back(res);
    }
    void UnCondiBrTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        std::string res;
        res="goto "+ ops->childs[0]->print()+ '\n';
        inst_strs.push_back(res);
    }
    void CondiBrCmpTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node->childs[0]);
        auto opss=(OpNode*)(inst_node);
        std::string res="cjump ";
        if(ops->op==op_ge)
        {
            res+=(translate_node(ops->childs[1])+" <= "+translate_node(ops->childs[0]));
        }
        else if(ops->op==op_gt)
        {
            res+=(translate_node(ops->childs[1])+" < "+translate_node(ops->childs[0]));
        }
        else
        {
            res+=(translate_node(ops->childs[0])+' '+transfer_optype(ops->op)+' '+translate_node(ops->childs[1]));
        }
        res+=opss->childs[1]->print();
        res+='\n';
        inst_strs.push_back(res);
    }
    void CondiBrVarTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        std::string res="cjump "+translate_node(ops->childs[0])+" = 1 "+ops->childs[1]->print()+'\n';
        inst_strs.push_back(res);
    }
    void CondiBrConstTile::gen_code(GenNode* inst_node,INSTS& inst_strs)
    {
        if(this->matched_nodes.find(inst_node)==this->matched_nodes.end())
        {
            if(debug_flag)std::cout<<"this node is not in the aopTile's matched nodes!\n";
            return;
        }
        if(!inst_node->isOp)
        {
            if(debug_flag)std::cout<<"the inst_node is not a operator in aopTile!\n";
        }
        auto ops=(OpNode*)(inst_node);
        auto cons_ops=dynamic_cast<OperandNode*>(ops->childs[0]);
        auto constant=dynamic_cast<Constant_item*>(cons_ops->data);
        if(constant->value==1)
        {   
            std::string res="goto "+ops->childs[1]->print()+'\n';
            inst_strs.push_back(res);
        }
    }
    //init tiles;
    void init_tile(std::vector<Tile*>& tiles,Program& p)
    {
        prefix=p.var_prefix;
        L3_ret_var_prefix=p.ret_var_prefix;
        L3_new_var_cnt=L3_ret_var_cnt=0;
        /*
            push tiles
        */
        tiles.push_back(new AopTile());
        tiles.push_back(new AssignVarTile());
        tiles.push_back(new AssignStoreTile());
        tiles.push_back(new RetTile());
        tiles.push_back(new RetVarTile());
        //call tiles
        for(ll i=0;i<=MAX_RUNTIME_ARGS;++i)
        {
            tiles.push_back(new CallTile(true,i));
        }
        for(auto& F:p.functions)
        {
           tiles.push_back(new CallTile(false,F->args.size()));
        }
        tiles.push_back(new LoadTile());
        tiles.push_back(new CmpTile());
        tiles.push_back(new LabelTile());
        tiles.push_back(new UnCondiBrTile());
        tiles.push_back(new CondiBrCmpTile());
        tiles.push_back(new CondiBrVarTile);
        tiles.push_back(new CondiBrConstTile);
    }
    /*
        patten node interface
    */
    PatNode::PatNode()
    {
        
    }
    PatopNode::PatopNode()
    {
        this->isOp=true;
        this->isRuntime=false;
    }
    PatopNode::PatopNode(OpType type)
    {
        this->isOp=true;
        this->isRuntime=false;
        this->prob_ops.insert(type);
    }
    PatopNode::PatopNode(OpType type,bool isRuntime)
    {
        this->isOp=true;
        this->isRuntime=isRuntime;
        this->prob_ops.insert(type);
    }
    PatopNode::PatopNode(std::set<OpType>& ops)
    {
        this->prob_ops.insert(ops.begin(),ops.end());
        this->isOp=true;
        this->isRuntime=false;
    }
    PatoperandNode::PatoperandNode()
    {

    }
    PatoperandNode::PatoperandNode(std::set<iType>&types)
    {
        this->isOp=false;
        this->prob_operands.insert(types.begin(),types.end());
    }
    PatoperandNode::PatoperandNode(iType type)
    {
        this->isOp=false;
        this->prob_operands.insert(type);
    }
    //add node
    void PatopNode::add(PatNode* leaf)
    {
        this->childs.push_back(leaf);
    }
    //match function
    bool PatopNode::match(GenNode* node,std::vector<GenNode*>&node_set)
    {
        if(node->isOp!=this->isOp)
        {
            DEBUGOUT<<"this node is not matched with op!\n";
            return false;
        }
        auto op_node=dynamic_cast<OpNode*>(node);
        if(this->prob_ops.find(op_node->op)==this->prob_ops.end())
        {
            DEBUGOUT<<"did not find a probable op for the node!\n";
            return false;
        }
        if(op_node->op==OpType::op_call)
        {
            bool isRuntime=false;
            if(op_node->childs[0]->isOp)//a operator so not a runtie call
            {
                isRuntime=false;
            }
            else
            {
                auto callee=(OperandNode*)(op_node->childs[0]);
                isRuntime=check_runtime_call(callee->data);
            }
            if(this->isRuntime!=isRuntime)
                return false;
        }
        if(this->childs.size()!=op_node->childs.size())return false;
        for(ll i=0;i<this->childs.size();++i)
        {
            auto pat_node=this->childs[i];
            auto next_node=node->childs[i];
            if(!pat_node->match(next_node,node_set))
            {
                return false;
            }
        }
        return true;
    }
    bool PatoperandNode::match(GenNode* node,std::vector<GenNode*>&node_set)
    {
        if(node->isOp==this->isOp)
        {
            auto operand_node=dynamic_cast<OperandNode*>(node);
            return this->prob_operands.find(operand_node->data->type)!=this->prob_operands.end();
        }   
        else
        {   
            auto op_node=dynamic_cast<OpNode*>(node);
            bool can_ret=aop_cmp_ops.count(op_node->op);
            can_ret|=op_node->op==OpType::op_load;
            can_ret|=op_node->op==OpType::op_call;
            can_ret&=(this->prob_operands.find(iType::var_item)!=this->prob_operands.end());
            if(can_ret)
            {
                node_set.push_back(op_node);
            }
            return can_ret;
        }
    }
    bool PatTree::match(GenNode* node,std::vector<GenNode*>&node_set)
    {
        return this->head->match(node,node_set);
    }
    bool Tile::match(GenNode* node,std::vector<GenNode*>&node_set)
    {
        bool is_matched=this->pt->match(node,node_set);
        if(is_matched)
            this->matched_nodes.insert(node);
        return is_matched;
    }
    //
    std::string translate_node(GenNode* node)
    {
        if(node->isOp)
        {
            auto ops=dynamic_cast<OpNode*>(node);
            if(ops->res==nullptr)
            {
                DEBUGOUT<<"translate OpNode failed!\n";
            }
            else return ops->res->print();
        }
        else
        {
            auto operand=dynamic_cast<OperandNode*>(node);
            return operand->data->print();
        }
        return "";
    }
}