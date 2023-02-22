#pragma once
#include "IR.h"
namespace IR
{
    
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
    BasicBlock* fetch_and_remove(std::set<BasicBlock*>& bb_list,bool traced_entry);
    BasicBlock* get_next_bb(std::set<BasicBlock*>&,std::set<BasicBlock*>&);
    class Trace
    {
        public:
            Trace();
            std::vector<BasicBlock*>bb_list;
            void add_bb(BasicBlock* bb);
    };
    class TraceGenerator
    {
        public:
            TraceGenerator();
            std::vector<Trace*> gen_trace(std::vector<BasicBlock*>bb_list);
    };
    class CodeVisitor:Ins_visitor
    {
        public:
            void visit(Instruction_ret* ins);
            void visit(Instruction_var_ret* ins);
            void visit(Instruction_branch_condi* ins);
            void visit(Instruction_branch_nocondi* ins);
            void visit(Instruction_assignment* ins);
            void visit(Instruction_label* ins);
            void visit(Instruction_call* ins);
            void visit(Instruction_declare* ins);
    };
    
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