#include "Instruction_select.h"
namespace L3
{
   
    // void select_instrution(Program& p)
    // {
    //     //transfer label
    //     Var_Label_transformer transfer("_global_");
    //     transfer.find_longest_label(p);
    //     transfer.find_longest_var(p);
    //     transfer.transform_label(p);

    //     //init forest
        


    //     //do tilling
        
    // }
    /*
        api about Nodes
    */
    Node::Node()
    {

    }
    GenNode::GenNode()
    {
        this->mached=false;
        this->matched_tile=nullptr;
    }

    OpNode::OpNode(OpType t)
    {
        this->isOp=true;
        this->op=t;
        this->res=nullptr;
    }
    OperandNode::OperandNode(Item* d)
    {
        this->data=d;
        this->isOp=false;
    }
    //tilling function for nodes
    bool GenNode::tilling(std::vector<Tile*>&tiles)
    {
        if(this->mached==(this->matched_tile==nullptr))
        {
            if(debug_flag)std::cout<<"this tile is occupied!\n";
            return false;
        }
        if(this->mached==true)
        {
            return true;
        }
        ll best_node_cnt=-1,best_cost=1e18;
        Tile* best_tile=nullptr;
        std::vector<GenNode*>node_set;
        if(auto opp=dynamic_cast<OpNode*>(this))
        {
            if(opp->op==OpType::op_load)
            {
                int x=1;
            }
        }
        auto check=[&](Tile* t)
        {
            return t->node_cnt>best_node_cnt||t->node_cnt==best_node_cnt&&t->cost<best_cost;
        };
        for(auto tile:tiles)
        {
            std::vector<GenNode*>tmp_set;
            if(tile->tile_name=="load_tile")
            {
                int x=1;
            }
            if(!tile->match(this,tmp_set))continue;
            if(check(tile))
            {
                best_node_cnt=tile->node_cnt;
                best_cost=tile->cost;
                best_tile=tile;
                node_set=tmp_set;
            }
        }        
        if(best_tile==nullptr)
        {
            DEBUGOUT<<"cannot be tiled!\n";
            return false;
        }
        this->nexts=node_set;
        for(auto node:node_set)
        {
            if(!node->tilling(tiles))
            {
                DEBUGOUT<<"cannot tile node set!\n";
                return false;
            }
        }
        this->mached=true;
        this->matched_tile=best_tile;
        return this->mached;
    }
    //gen code
    void GenNode::gen_code(std::vector<std::string>& inst_strs)
    {
        for(auto node:this->nexts)
        {
            node->gen_code(inst_strs);
        }
        this->matched_tile->gen_code(this,inst_strs);
    }
    //print function
    std::string OperandNode::print()
    {
        return this->data->print();
    }
    std::string OpNode::print()
    {
        return transfer_optype(this->op);
    }

    //copy function
    OpNode* OpNode::copy()
    {
        auto res=new OpNode(this->op);
        res->isOp=this->isOp;
        res->mached=this->mached;
        res->matched_tile=this->matched_tile;
        res->res=this->res;
        for(auto child:this->childs)
        {
            res->add(child->copy());
        }
        return res;
    }
    OperandNode* OperandNode::copy()
    {
        auto res=new OperandNode(this->data);
        res->mached=this->mached;
        res->matched_tile=this->matched_tile;
        res->isOp=this->isOp;
        return res;
    }
    //add function

    void OpNode::add(GenNode* leaf)
    {
        this->childs.push_back(leaf);
    }
    //Tree apis
    InsTree::InsTree()
    {
        
    }
    bool InsTree::replace_used(Item* var,GenNode* def_node)
    {
        return this->replace_used_var(this->head,def_node,var);
    }
    bool InsTree::replace_used_var(GenNode* cur_node,GenNode* def_node,Item* var)
    {
        if(!cur_node->isOp)
        {
            if(debug_flag)std::cout<<"error node! it should be a operator!\n";
            return false;
        }
        ll id=0;
        auto op_node=(OpNode*)(cur_node);
        if(op_node->op==OpType::op_assign&&!op_node->childs[0]->isOp)++id;//for assignment we just repace the second operator
        bool replaced=false;
        for(;id<op_node->childs.size();++id)
        {
            auto now_node=op_node->childs[id];
            if(now_node->isOp)
            {
                //recursively check
                replaced|=replace_used_var(now_node,def_node,var);
            }
            else
            {
                auto now_operand=(OperandNode*)now_node;
                if(now_operand->data==var)
                {
                    op_node->childs[id]=def_node->copy();
                    replaced=true;
                }
            }
        }
        return replaced;
    }
    bool InsTree::tilling(std::vector<Tile*>& tiles)
    {
        return this->head->tilling(tiles);
    }
    void InsTree::gen_code(std::vector<std::string>& inst_strs)
    {
        this->head->gen_code(inst_strs);
    }
    //Forest Apis
    InsForest::InsForest(Context* conts,FunctionLiveness* fl)
    {
        this->context=conts;
        this->tv=Tree_visitor(fl);
        this->init_trees();
    }
    InsForest::InsForest()
    {
        
    }
    void InsForest::init_trees()
    {
        for(auto& I:this->context->instructions)
        {
            I->apply(this->tv);
            this->trees.push_back(this->tv.now_tree);
        }
    }
    void InsForest::merge_trees(FunctionLiveness& fl)
    {
        bool valid=true;
        while(valid)
        {
            valid=this->merge_one_trees(fl);
        }
    }
    bool InsForest::can_merge(ll used_id,ll def_id,Item* inter,FunctionLiveness& fl)
    {
        auto tree_used=this->trees[used_id];
        auto tree_def=this->trees[def_id];
        //1. do not merge load
        auto op=dynamic_cast<OpNode*>(tree_def->head);
        if(op==nullptr||op->op!=op_assign)
        {
            if(debug_flag)std::cout<<"not a assignment instruction!\n";
            return false;
        }
        if(op->childs[1]->isOp)
        {
            auto ops=dynamic_cast<OpNode*>(op->childs[1]);
            if(ops->op==op_load)
            {
                if(debug_flag)std::cout<<"do not merge load\n";
                return false;
            }
        }
        //2. merge only happen when this variable is dead after this instruction,finding the last instruction assotiate with the tree_used
        ll last_id=-1;
        Instruction* last_ins=nullptr;
        for(auto & I:tree_used->insts)
        {
            auto id=this->context->indexs[I];
            if(id>last_id)
            {
                last_id=id;
                last_ins=I;
            }
        }
        if(last_ins==nullptr)
        {
            if(debug_flag)std::cout<<"cannot merge because no last instruction assotiate with the variable!\n";
            return false;
        }
        auto last_used_out=fl.OUT[last_ins];
        if(last_used_out.find(inter)!=last_used_out.end())
        {
            if(debug_flag)std::cout<<"this variable is not dead after define\n";
            return false;
        }
        //this variable can only be used by this two tree
        for(ll id=0;id<this->trees.size();++id)
        {
            if(id!=used_id&&id!=def_id)
            {
                auto t=this->trees[id];
                if(t->used.find(inter)!=t->used.end())
                {
                    return false;
                }
            }
        }
        //no other used between this two instructions
        for(ll id=def_id+1;id<used_id;++id)
        {
            auto t=this->trees[id];
            if(t->defs.find(inter)!=t->defs.end()||t->used.find(inter)!=t->used.end())
            {
                return false;
            }
        }
        for(ll id=def_id+1;id<used_id;++id)
        {
            if(has_intersect(this->trees[id]->defs,tree_def->used))
            {
                return false;
            }
        }
        return true;
    }
    void InsForest::merge(InsTree* tree_used,InsTree* tree_def,Item* var)
    {
        if(var->type!=iType::var_item)
        {
            if(debug_flag)std::cout<<"merge not a variable !\n";
            return;
        }
        auto op_node=(OpNode*)(tree_def->head);
        if(op_node->op!=OpType::op_assign||op_node->childs.size()!=2)
        {
            if(debug_flag)std::cout<<"merge invalid assignment tree!\n";
            return;
        }
        auto operand=(OperandNode*)(op_node->childs[1]);
        bool replaced=tree_used->replace_used(var,operand);//modify the var in define node of tree_used
        if(!replaced)
        {
            if(debug_flag)std::cout<<"error replaceing!\n";
            return;
        }
        tree_used->used.erase(var);//erase original var
        tree_used->used.insert(tree_def->used.begin(),tree_def->used.end());
        tree_used->insts.insert(tree_used->insts.end(),tree_def->insts.begin(),tree_def->insts.end());
    }
    bool InsForest::merge_one_trees(FunctionLiveness& fl)
    {
        ll siz=this->trees.size();
        for(ll i=siz-1;i>=0;--i)
        {
            //used starts from back
            auto tree_used=this->trees[i];
            for(ll j=i-1;j>=0;--j)
            {
                if(i==4&&j==1)
                {
                    int x=0;
                }
                auto tree_def=this->trees[j];
                auto inter_sets=get_intersect(tree_used,tree_def);
                if(inter_sets.size()>1)
                {
                    std::cout<<"error intersect!\n";
                }
                for(auto var:inter_sets)
                {
                    if(this->can_merge(i,j,var,fl))
                    {
                        this->merge(tree_used,tree_def,var);
                        this->trees.erase(this->trees.begin()+j);
                        delete tree_def;
                        return true;
                    }
                }
            }
        }
        return false;
    }
    bool InsForest::tilling(std::vector<Tile*>& tiles)
    {
        bool is_tiled=false;
        for(auto tree:this->trees)
        {
            is_tiled&=tree->tilling(tiles);
        }
        return is_tiled;
    }
    void InsForest::gen_code(std::vector<std::string>& inst_strs)
    {
        for(auto tree:this->trees)
        {
            tree->gen_code(inst_strs);
        }
    }
    //Tree visitor's interface
    //visit
    Tree_visitor::Tree_visitor()
    {

    }
    void Tree_visitor::visit(Instruction_ret* ins)
    {
        this->now_tree=new InsTree();
        this->now_tree->head=new OpNode(OpType::op_ret);
        this->set_used_define(ins);
        this->now_tree->insts.push_back(ins);
    }
    void Tree_visitor::visit(Instruction_var_ret* ins)
    {
        this->now_tree=new InsTree();
        auto op_node=new OpNode(OpType::op_ret);
        op_node->add(transfer_item(ins->var_ret));
        this->now_tree->head=op_node;
        this->set_used_define(ins);
        this->now_tree->insts.push_back(ins);
    }
    void Tree_visitor::visit(Instruction_label* ins)
    {
        this->now_tree=new InsTree();
        auto op_node=new OpNode(OpType::op_label);
        op_node->add(transfer_item(ins->name));
        this->now_tree->head=op_node;
        this->set_used_define(ins);
        this->now_tree->insts.push_back(ins);
    }
    void Tree_visitor::visit(Instruction_assignment* ins)
    {
        this->now_tree=new InsTree();
        auto op_node=new OpNode(OpType::op_assign);
        op_node->add(transfer_item(ins->d));
        if(ins->s->type==load_item)
        {
            int x=0;
        }
        op_node->add(transfer_item(ins->s));
        this->now_tree->head=op_node;
        this->set_used_define(ins);
        this->now_tree->insts.push_back(ins);
    }
    void Tree_visitor::visit(Instruction_branch* ins)
    {
        this->now_tree=new InsTree();
        auto op_node=new OpNode(OpType::op_branch);
        if(ins->condition!=nullptr)
        {
            op_node->add(transfer_item(ins->condition));
        }
        op_node->add(transfer_item(ins->location));
        this->now_tree->head=op_node;
        this->set_used_define(ins);
        this->now_tree->insts.push_back(ins);
    }
    void Tree_visitor::visit(Instruction_call* ins)
    {
        this->now_tree=new InsTree();
        auto caller_node=transfer_item(ins->caller);
        if(ins->var_ret!=nullptr)
        {
            auto op_node=new OpNode(OpType::op_assign);
            op_node->add(transfer_item(ins->var_ret));
            op_node->add(caller_node);
            this->now_tree->head=op_node;
        }
        else
        {
            this->now_tree->head=caller_node;
        }
        this->set_used_define(ins);
        this->now_tree->insts.push_back(ins);
    }   
    void Tree_visitor::set_used_define(Instruction* ins)
    {
        this->now_tree->defs=this->fl->KILL[ins];
        this->now_tree->used=this->fl->GEN[ins];
    }
    Tree_visitor::Tree_visitor(FunctionLiveness* fl)
    {
        this->fl=fl;
    }
    //used for transferring
    GenNode* transfer_item(Item* item)
    {
        switch (item->type)
        {
            case iType::var_item:
            {
                return new OperandNode(item);
                break;
            }
            case iType::constant_item:
            {
                return new OperandNode(item);
                break;
            }
            case iType::label_item:
            {
                return new OperandNode(item);
                break;
            }
            case iType::aop_item:
            {
                auto aop=dynamic_cast<Aop_item*>(item);
                auto res=new OpNode(transfer_aop_op(aop->aType));
                res->add(transfer_item(aop->op1));
                res->add(transfer_item(aop->op2));
                return res;
                break;
            }
            case iType::cmp_item:
            {
                auto cmp=dynamic_cast<Cmp_item*>(item);
                auto res=new OpNode(transfer_cmp_op(cmp->cType));
                res->add(transfer_item(cmp->op1));
                res->add(transfer_item(cmp->op2));
                return res;                
                break;
            }
            case iType::load_item:
            {
                auto load=dynamic_cast<Load_item*>(item);
                auto res=new OpNode(OpType::op_load);
                res->add(transfer_item(load->var));
                return res;
                break;
            }           
            case iType::store_item:
            {
                auto store=dynamic_cast<Store_item*>(item);
                auto res=new OpNode(OpType::op_store);
                res->add(transfer_item(store->ptr));
                return res;
                break;
            }           
            case iType::call_item:
            {
                auto call=dynamic_cast<Call_item*>(item);
                auto res=new OpNode(OpType::op_call);
                res->add(transfer_item(call->callee));
                for(auto arg:call->args)
                {
                    res->add(transfer_item(arg));
                }
                return res;
                break;
            }           
            default:
            {
                return nullptr;
                break;
            }
        }
        return nullptr;
    }
    std::string transfer_optype(OpType type)
    {
        switch (type)
        {
            case OpType::op_add:
                return "+";
                break;
            case OpType::op_assign:
                return "<-";
                break;
            case OpType::op_bit_and:
                return "&";
                break;
            case OpType::op_eq:
                return "=";
                break;
            case OpType::op_ge:
                return ">=";
                break;
            case OpType::op_gt:
                return ">";
                break;
            case OpType::op_le:
                return "<=";
                break;
            case OpType::op_lt:
                return "<";
                break;
            case OpType::op_load:
                return "load";
                break;
            case OpType::op_call:
                return "call";
                break;
            case OpType::op_left_shift:
                return "<<";
                break;
            case OpType::op_right_shift:
                return ">>";
                break;
            case OpType::op_branch:
                return "br";
                break;
            case OpType::op_nodef:
                return "nodef";
                break;
            case OpType::op_mul:
                return "*";
                break;
            case OpType::op_ret:
                return "return";
                break;
            case OpType::op_sub:
                return "-";
                break;
            default:
                std::cout<<"invalid AopType\n";
                return "";
                break;
        }
    }
    OpType transfer_aop_op(AopType t)
    {
        switch (t)
        {
        case AopType::bit_and:
            return OpType::op_bit_and;
            break;
        case AopType::left_shift:
            return OpType::op_left_shift;
            break;
        case AopType::right_shift:
            return OpType::op_right_shift;
            break;
        case AopType::minus:
            return OpType::op_sub;
            break;
        case AopType::multy:
            return OpType::op_mul;
            break;
        case AopType::plus:
            return OpType::op_add;
            break;
        default:
            std::cout<<"invalid transfer aop to op\n";
            return OpType::op_add;
            break;
        }
    }
    OpType transfer_cmp_op(CmpType t)
    {
        switch (t)
        {
            case CmpType::eq:
                return OpType::op_eq;
                break;
            case CmpType::le:
                return OpType::op_le;
                break;
            case CmpType::lt:
                return OpType::op_lt;
                break;
            case CmpType::gt:
                return OpType::op_gt;
                break;
            case CmpType::ge:
                return OpType::op_ge;
                break;        
            default:
                break;
        }
    }
    bool isAopType(OpType t)
    {
        return t==OpType::op_add||t==OpType::op_bit_and||t==OpType::op_mul||t==OpType::op_sub||t==OpType::op_left_shift||t==OpType::op_right_shift;
    }
    bool isCmpType(OpType t)
    {
        return t==OpType::op_eq||t==OpType::op_lt||t==OpType::op_le||t==OpType::op_gt||t==OpType::op_ge;
    }
    bool isCmp_Aop_Type(OpType t)
    {
        return isAopType(t)||isCmpType(t);
    }
    std::vector<Item*>get_intersect(InsTree* tree_use,InsTree* tree_def)
    {
        std::set<Item*>tmp;
        set_intersect(tree_use->used,tree_def->defs,tmp);
        return std::vector<Item*>(tmp.begin(),tmp.end());
    }
    std::set<OpType>aop_ops={op_add,op_sub,op_mul,op_bit_and,op_left_shift,op_right_shift};
    std::set<OpType>cmp_ops={op_lt,op_le,op_eq,op_ge,op_gt};
    std::set<OpType>aop_cmp_ops={op_add,op_sub,op_mul,op_bit_and,op_left_shift,op_right_shift,op_lt,op_le,op_eq,op_ge,op_gt};
    std::set<iType>var_label_types={iType::var_item,iType::label_item};
    std::set<iType>var_consts_types={iType::var_item,iType::constant_item};
    std::set<iType>var_consts_label_types={iType::var_item,iType::constant_item,iType::label_item};
}