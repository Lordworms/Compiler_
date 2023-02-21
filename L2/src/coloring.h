#pragma once
#include "utils.h"
#include"Interference.h"
#include"spiller.h"
#define COLOR_NUM 15
namespace L2
{
    class Node_selector
    {
        public:
            Graph G;
            int color_num;
            Node_selector(Graph& G,int color_num);
            Item* get_remove_node();
            void init_stack(std::stack<Item*>&st);
    };
    class Color_selector
    {
        public:
            std::stack<Item*>*node_stack;
            Graph* G;
            Color_selector(Graph * G,std::stack<Item*>*st);
            //pre_color all register
            void pre_color(std::unordered_map<Item*,Reg_color>& item_color_map);
            //select a color for var
            bool select_color(Item* var,std::unordered_map<Item*,Reg_color>& item_color_mp);
            //assign color for all variables
            bool assign_all(std::unordered_map<Item*,Reg_color>& item_color_mp,std::set<Item*>& spill_variable);
    };
    class Spill_allocator
    {
        public:
            NODE* not_colored_nodes;
            NODE* pre_spilled_nodes;
            void choose_spill_nodes(std::vector<Item*>& to_be_spilled);
            Spill_allocator(NODE* ncn,NODE* psn);
    };
    class Register_allocator
    {
        public:
            std::string spill_prefix="%SPILL_SINGLE_";
            Function* F;
            Function** F_addr;
            Register_allocator(Function* F,Function** F_addr);
            void allocate();
            //replace variable into reg in all instructions
            void replace_variable(Function* F,std::unordered_map<Item*,Reg_color>& item_color_mp);
            bool check_complete(Function* F,NODE & not_colored_items);
    };
    void program_coloring(Program& p);
    
}