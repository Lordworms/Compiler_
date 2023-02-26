#pragma once
#include "LA.h"
#include <fstream>
#include <algorithm>
namespace LA
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
    class EncodeVisitor:public Ins_visitor
    {
        public:
            Var_Label_transformer* trans;
            std::vector<Instruction*>new_ints;
            EncodeVisitor(Var_Label_transformer* trans);
            void visit(Instruction_ret* ins);
            void visit(Instruction_var_ret* ins);
            void visit(Instruction_branch_condi* ins);
            void visit(Instruction_branch_nocondi* ins);
            void visit(Instruction_assignment* ins);
            void visit(Instruction_label* ins);
            void visit(Instruction_call* ins);
            void visit(Instruction_declare* ins);
            void arr_ele_all(Arrele_item* a);
            void add_length_decode(Length_item* length);
            void add_op_decode(Op_item* op);
            void add_encode_instruction(Item* v);
            Item* add_decode_instruction(Item* var);
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