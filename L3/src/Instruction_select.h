#pragma once
#include "L3.h"
#include "utils.h"
#include "liveness.h"
#include "naive_vis.h"
#define DEBUGOUT if(debug_flag)std::cout
namespace L3
{
    inline static bool debug_flag=false;
    using INSTS=std::vector<std::string>;
    class PatNode;
    class PatopNode;
    class PatoperandNode;
    class PatTree;
    class Tile;
    class Tree_visitor;
    class InsTree;
     /*
        we have two different node
        general_node: contains variable, constant ,label, etc
        patten_node: contains <-,+,-,*,&,<<,>>
    */
    enum NodeType {pattern,general};
    enum OpType {
        op_nodef,
        op_add,op_sub,op_mul,op_bit_and,op_left_shift,op_right_shift,
        op_lt,op_gt,op_eq,op_le,op_ge,
        op_load,op_store,
        op_assign,
        op_ret,op_branch,op_label,op_call
    };
    extern std::set<OpType>aop_ops;
    extern std::set<OpType>cmp_ops;
    extern std::set<OpType>aop_cmp_ops;
    extern std::set<iType>var_label_types;
    extern std::set<iType>var_consts_types;
    extern std::set<iType>var_consts_label_types;
    // for tree visitor
    class Tree_visitor: public Ins_visitor
    {
        public:
            Tree_visitor();
            InsTree* now_tree;
            FunctionLiveness* fl;
            Tree_visitor(FunctionLiveness* fl);
            void set_used_define(Instruction* ins);
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_var_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_branch* ins)override;
            void visit(Instruction_call* ins)override;  
    };
    // for nodes
    class Node
    {
        public:
            Node();
    };
    class GenNode :public Node//general node
    {
        public:
            bool mached;
            Tile* matched_tile;
            std::vector<GenNode*>childs;
            std::vector<GenNode*>nexts;//to the next head which may be merged
            bool isOp;//whether it is a operator or oeprand
            GenNode();
            void gen_code(std::vector<std::string>& inst_strs);
            bool tilling(std::vector<Tile*>&);
            virtual std::string print()=0;
            virtual GenNode* copy()=0;
    };
    class OpNode :public GenNode
    {
        public:
            OpType op;
            Item* res;
            OpNode(OpType o);
            void add(GenNode* leaf);
            std::string print();
            OpNode* copy();
    };
    class OperandNode:public GenNode
    {
        public:
            OperandNode(Item* d);
            Item* data;
            OperandNode* copy();
            std::string print();
    };

    // for tree and forest
    class InsTree
    {
        public:
            GenNode* head;//represent the value of the tree
            std::set<Item*>defs,used;
            std::vector<Instruction*>insts;
            bool replace_used(Item* var,GenNode* def_node);
            void gen_code(std::vector<std::string>& inst_strs);
            bool replace_used_var(GenNode* cur_node,GenNode* def_node,Item* var);
            bool tilling(std::vector<Tile*>& tiles);
            InsTree();

    };
    class InsForest
    {
        public:
            std::vector<InsTree*>trees;
            Context* context;
            Tree_visitor tv;
            InsForest(Context* conts,FunctionLiveness* fl);
            InsForest();
            void init_trees();
            void gen_code(std::vector<std::string>& inst_strs);
            void merge_trees(FunctionLiveness& fl);
            bool merge_one_trees(FunctionLiveness& fl);
            bool tilling(std::vector<Tile*>& tiles);
            bool can_merge(ll used_id,ll def_id,Item* inter,FunctionLiveness& fl);
            void merge(InsTree* tree_used,InsTree* tree_def,Item* var);
    };
    //Tile
    class Tile
    {
        public:
            Tile();
            ll node_cnt,cost;
            std::string tile_name;
            std::set<GenNode*>matched_nodes;
            PatTree *pt;
            bool match(GenNode*,std::vector<GenNode*>&);
            virtual void gen_code(GenNode*,INSTS&)=0;

    };

    

    //main function
    //void select_instrution(Program& p);
    GenNode* transfer_item(Item* item);
    std::string transfer_optype(OpType type);
    OpType transfer_aop_op(AopType t);
    OpType transfer_cmp_op(CmpType t);
    std::vector<Item*>get_intersect(InsTree* a,InsTree* b);
    bool isAopType(OpType t);
    bool isCmpType(OpType t);
    bool isCmp_Aop_Type(OpType t);
}