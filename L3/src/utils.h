#pragma once
#include"L3.h"
#include<algorithm>
#include<fstream>
//define all the visitor and other tools
namespace L3
{
    using INDEX=std::map<Instruction*,long long>;
    using SET=std::map<Instruction*,std::set<Item*>>;
    using VEC=std::map<Instruction*,std::vector<Instruction*>>;
    using IMAP=std::map<Item*,Instruction*>;
    using VARS=std::vector<Item *>;
    /**
     * all Visitor for using
     * 
     */
    void get_used(Item* it,std::vector<Item*>& used);
    class Liveness_visitor:public Ins_visitor
    {
        public:
            Liveness_visitor();
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_var_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_branch* ins)override;
            void visit(Instruction_call* ins)override;
            SET GEN,KILL;
    };
    class Successor_visitor:public Ins_visitor
    {
        public:
            void combine_label(Function* F);
            void find_successor(Function* F);
            Successor_visitor();
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_var_ret* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_branch* ins)override;
            void visit(Instruction_call* ins)override;
            VEC successor; 
            IMAP label_map;  
    };
    /*
        used for context
    */
    class Context
    {
        public:
            Context();
            INDEX indexs;
            std::vector<Instruction*>instructions;
            long long size();
            void push_inst(Instruction* ins);

    };
    void init_context(Function* F,std::vector<Context*>&conts);
    class Var_Label_transformer
    {
        public:
            std::string suffix="_global_";
            std::string longest_var;
            std::string longest_label;
            int label_cnt,var_cnt;
            Var_Label_transformer(std::string prefix);
            std::string find_longest_label(Program& p);
            std::string find_longest_var(Program& p);
            std::string new_label_name(std::string name);
            std::string new_var_name(std::string name);
            void transform_label(Program& p);
            void transform_var(Program& p);
    };
    // /*
    //     used for tilling
    
    // */
    // class Till
    // {
    //     public:

    // };
    // class AopTile
    // {

    // };

    
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
    template<class T>
    bool has_intersect(std::set<T> & a, std::set<T> & b)
    {
        for (T item : a) {
            if (b.find(item)!=b.end()) {
                return true;
            }
        }
        return false;
    }
}