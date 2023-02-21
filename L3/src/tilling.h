#pragma once
#include "Instruction_select.h"
#define MAX_RUNTIME_ARGS 4
namespace L3
{
    static std::string prefix;
    static std::string L3_ret_var_prefix;
    static ll L3_new_var_cnt=0;
    static ll L3_ret_var_cnt=0;
    //pattern nodes
    class PatNode:public Node
    {
        public:
            PatNode();
            std::vector<PatNode*>childs;
            bool isOp;
            virtual bool match(GenNode*,std::vector<GenNode*>&)=0;
    };
    class PatopNode:public PatNode
    {
        public:
            PatopNode();
            PatopNode(std::set<OpType>&);
            PatopNode(OpType type);
            PatopNode(OpType type,bool isRuntime);
            std::set<OpType>prob_ops;
            bool isRuntime;//if prob_ops contains call
            void add(PatNode* leaf);
            bool match(GenNode*,std::vector<GenNode*>&)override;
    };
    class PatoperandNode:public PatNode
    {
        public:
            PatoperandNode();
            PatoperandNode(std::set<iType>&);
            PatoperandNode(iType type);
            std::set<iType>prob_operands;
            bool match(GenNode*,std::vector<GenNode*>&);
    };
    //pattern tree
    class PatTree
    {
        public:
            PatNode* head;
            bool match(GenNode*,std::vector<GenNode*>&);
    };
    //different kinds of till
    class AopTile:public Tile
    {
        public:
            AopTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };

    class AssignVarTile:public Tile
    {
        public:
            AssignVarTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };

    class AssignStoreTile:public Tile
    {
        public:
            AssignStoreTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };

    class CmpTile:public Tile
    {
        public:
            CmpTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };
    class RetTile:public Tile
    {
        public:
            RetTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };
    class RetVarTile:public Tile
    {
        public:
            RetVarTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };
    class CallTile:public Tile
    {
        public:
            bool isRuntime;
            ll arg_num;
            CallTile(bool isRuntime,ll num);
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };

    class LabelTile:public Tile
    {
        public:
            LabelTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };

    class LoadTile:public Tile
    {
        public:
            LoadTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };

    class BrTile:public Tile
    {

    };
    class UnCondiBrTile:public BrTile
    {
        public:
            UnCondiBrTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };

    class CondiBrCmpTile:public BrTile
    {
        public:
            CondiBrCmpTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };
    
    class CondiBrVarTile:public BrTile
    {
        public:
            CondiBrVarTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };
    class CondiBrConstTile:public BrTile
    {
        public:
            CondiBrConstTile();
            void gen_code(GenNode* inst_node,INSTS& inst_strs)override;
    };
    void init_tile(std::vector<Tile*>& tiles,Program& p);
    std::string translate_node(GenNode* node);
} 