#pragma once
#include "IR.h"
#include <fstream>
#include <algorithm>
namespace IR
{
    #define OUT *this->out
    #define WIDTH 8
    class Var_Label_transformer;
    class Var_Label_transformer
    {
        public:
            std::string suffix="_global_";
            std::string longest_var;
            std::string longest_label;
            int label_cnt,var_cnt;
            Var_Label_transformer();
            Var_Label_transformer(std::string prefix);
            std::string find_longest_label(Program& p);
            std::string find_longest_var(Program& p);
            std::string new_label_name(std::string name);
            std::string new_var_name(std::string name);
            void transform_label(Program& p);
            void transform_var(Program& p);
    };
    BasicBlock* fetch_and_remove(std::vector<BasicBlock*>& bb_list,bool traced_entry);
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
    class CodeVisitor:public Ins_visitor
    {
        public:
            std::ofstream *out;
            CodeVisitor();
            CodeVisitor(Var_Label_transformer & trans,std::ofstream *out);
            Var_Label_transformer trans;
            void visit(Instruction_ret* ins);
            void visit(Instruction_var_ret* ins);
            void visit(Instruction_branch_condi* ins);
            void visit(Instruction_branch_nocondi* ins);
            void visit(Instruction_assignment* ins);
            void visit(Instruction_label* ins);
            void visit(Instruction_call* ins);
            void visit(Instruction_declare* ins);
            Item* get_addr(Item* var);
            void print_encode(Item* v);
            void print_decode(Item* v);
            void print_op(Item* dst,Item* op1,OpType op,Item* op2);
            void print_var_length(Item* dst,Item* length);
            void print_load(Item* dst,Item* src);
            void print_store(Item* dst,Item* src);
            void print_arrele_all(Arrele_item* dst,Item* src);
            void print_var_common(Var_item* dst,Item* src);
            void print_var_op(Item* dst,Item* src);
            void print_var_newArray(Item* dst,Item* src);
            void print_var_newTuple(Item* dst,Item* src);
            void print_var_Arrele(Item* dst,Item* src);
            void print_var_call(Item* dst,Item* src);
            
    };
    ll encode(ll x);
    ll decode(ll x);

    /*
        set union operator
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