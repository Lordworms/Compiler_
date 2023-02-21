#pragma once
#include"L2.h"
#include<algorithm>
#include<fstream>
//define all the visitor and other tools
namespace L2
{
    
    using SET=std::map<Instruction*,std::set<Item*>>;
    using VEC=std::map<Instruction*,std::vector<Instruction*>>;
    using IMAP=std::map<Item*,Instruction*>;
    using SPILL_SET=std::vector<Item **>;
    /**
     * all Visitor for using
     * 
     */
    void get_used(Item* it,std::vector<Item*>& used);
    class Item_visitor
    {
        public:
            virtual void visit(Register_item* it)=0;
            virtual void visit(Aop_item* it)=0;
            virtual void visit(Cmp_item* it)=0;
            virtual void visit(Constant_item* it)=0;
            virtual void visit(StackArg_item* it)=0;
            virtual void visit(Memory_item* it)=0;
    };
    class ItemPrint_visitor:public Item_visitor
    {
        public:  
            ItemPrint_visitor();
            void visit(Register_item* it)override;
            void visit(Aop_item* it)override;
            void visit(Cmp_item* it)override;
            void visit(Constant_item* it)override;
            void visit(StackArg_item* it)override;
            void visit(Memory_item* it)override;
    };
    class Liveness_visitor:public Ins_visitor
    {
        public:
            Liveness_visitor();
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_aop* ins)override;
            //void visit(Instruction_call* ins)override;
            void visit(Instruction_cjump* ins)override;
            void visit(Instruction_common* ins)override;
            void visit(Instruction_runtime* ins)override;
            void visit(Instruction_dec* ins)override;
            void visit(Instruction_inc* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_lea* ins)override;
            void visit(Instruction_sop* ins)override;
            void visit(Instruction_goto* ins)override;
            SET GEN,KILL;
    };
    class Successor_visitor:public Ins_visitor
    {
        public:
            Successor_visitor();
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_aop* ins)override;
            void visit(Instruction_cjump* ins)override;
            void visit(Instruction_common* ins)override;
            void visit(Instruction_runtime* ins)override;
            void visit(Instruction_dec* ins)override;
            void visit(Instruction_inc* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_lea* ins)override;
            void visit(Instruction_sop* ins)override;       
            void visit(Instruction_goto* ins)override;  
            void combine_label(Function* f);
            void find_successor(Function* f);
            VEC successor; 
            IMAP label_map;  
    };
    bool check_spill(Item** iptr,Item* now_var,std::vector<Item **>& tmp_vars);
    class Spill_visitor:public Ins_visitor
    {
        public:
            Function* F;
            Var_item* now_spill_var;//indicate the now variable
            Var_item* prefix;//indicate the prefix of a spiiled variable
            long long var_num;//how many number it has used
            std::vector<Instruction*>new_insts;
            std::vector<Var_item*>spill_vars;
            Spill_visitor();
            Spill_visitor(Function* f,Var_item* now_spill_var,Var_item* preffix);
            void spill(Instruction* inst,bool hasRead,bool hasWrite,std::vector<Item**>& tmp_vars);

            Var_item* get_new_spill_var();
            Memory_item* get_new_stack_var();
            
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_aop* ins)override;
            void visit(Instruction_cjump* ins)override;
            void visit(Instruction_common* ins)override;
            void visit(Instruction_runtime* ins)override;
            void visit(Instruction_dec* ins)override;
            void visit(Instruction_inc* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_lea* ins)override;
            void visit(Instruction_sop* ins)override;       
            void visit(Instruction_goto* ins)override;
    };  
    /*
        coloring_visitor
    */
    class Color_visitor:public Ins_visitor
    {
        public:
            Function* F;
            //std::unordered_map<Item*,Reg_color>item_color_mp;
            Color_visitor(std::unordered_map<Item*,Reg_color>& item_color_mp,Function* F);
            std::unordered_map<Item*,Reg_color>var_color_mp;
            //check whether an Item has variable to color or not
            bool have_color(Item** addr,std::vector<Item**>& vec);
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_aop* ins)override;
            void visit(Instruction_cjump* ins)override;
            void visit(Instruction_common* ins)override;
            void visit(Instruction_runtime* ins)override;
            void visit(Instruction_dec* ins)override;
            void visit(Instruction_inc* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_lea* ins)override;
            void visit(Instruction_sop* ins)override;       
            void visit(Instruction_goto* ins)override;            
    };
    /*
        visitor used for generate L1 code 
    */
    class CodeL1_visitor:public Ins_visitor
    {
        public:
            std::ofstream* out;
            long long locals_num;
            CodeL1_visitor();
            CodeL1_visitor(std::ofstream* out);
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_aop* ins)override;
            void visit(Instruction_cjump* ins)override;
            void visit(Instruction_common* ins)override;
            void visit(Instruction_runtime* ins)override;
            void visit(Instruction_dec* ins)override;
            void visit(Instruction_inc* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_lea* ins)override;
            void visit(Instruction_sop* ins)override;       
            void visit(Instruction_goto* ins)override;            

    };
    /**
     * define the set operations
     * we have to put the template inside the .h file
     */
    template<class T>
    void set_union(std::set<T>&a,std::set<T>&b,std::set<T>&res)
    {
        std::vector<T>t(a.size()+b.size());
        auto tmp=std::set_union(a.begin(),a.end(),
        b.begin(),b.end(),t.begin()       
        );
        t.resize(tmp-t.begin());
        res=std::set<T>(t.begin(),t.end());
    }
    template<class T>
    void set_intersect(std::set<T>&a,std::set<T>&b,std::set<T>&res)
    {
        std::vector<T>t(a.size()+b.size());
        auto tmp=std::set_intersection(a.begin(),a.end(),
        b.begin(),b.end(),t.begin()       
        );
        t.resize(tmp-t.begin());
        res=std::set<T>(t.begin(),t.end());
    }
    template<class T>
    void set_diff(std::set<T>&a,std::set<T>&b,std::set<T>&res)
    {
        std::vector<T>t(a.size()+b.size());
        auto tmp=std::set_difference(a.begin(),a.end(),
        b.begin(),b.end(),t.begin()       
        );
        t.resize(tmp-t.begin());
        res=std::set<T>(t.begin(),t.end());
    }
}