#pragma once
#include "LB.h"
#include <fstream>
#include <algorithm>
namespace LB
{
    #define OUT *this->out
    #define WIDTH 8
    using LB_WHILE=std::map<Label_item*, Instruction_while *>;
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
            std::string find_longest_var_scope(Instruction_range* scope);
            std::string new_label_name(std::string name);
            std::string new_var_name(std::string name);
            void transform_var(Program& p);//transform each to a unique name
            void transform_var_scope(Instruction_range* scope);

            std::map<Instruction_while *,Label_item*>get_new_condi_label(Function* F);
            void get_new_condi_label_scope(Instruction_range* scope,std::map<Instruction_while *,Label_item*>& while_label);

            void get_beg_end_label_scope(Instruction_range* scope,LB_WHILE & beg_while,LB_WHILE& end_while);

            std::map<Instruction *,Instruction_while*>get_ins_while_map(Function* F);
            void get_ins_while_map_scope(Instruction_range* scope,std::map<Instruction *,Instruction_while*>& ins_while_mp,std::stack<Instruction_while *>& loop_st,LB_WHILE& beg_while,LB_WHILE& end_while);
    };
    class CodeVisitor: public Ins_visitor
    {
        public:
            std::ofstream* out;
            Var_Label_transformer* trans;
            std::map<Instruction*,Instruction_while*>*ins_while_map;
            std::map<Instruction_while*,Label_item*>*while_entry_map;
            CodeVisitor(std::ofstream* out,Var_Label_transformer* trans,std::map<Instruction*,Instruction_while*>*ins_while_map,std::map<Instruction_while*,Label_item*>*while_entry_map);
            void visit(Instruction_ret* ins)override;
            void visit(Instruction_var_ret* ins)override;
            void visit(Instruction_goto* ins)override;
            void visit(Instruction_if* ins)override;
            void visit(Instruction_while* ins)override;
            void visit(Instruction_continue* ins)override;
            void visit(Instruction_break* ins)override;
            void visit(Instruction_range* ins)override;
            void visit(Instruction_assignment* ins)override;
            void visit(Instruction_label* ins)override;
            void visit(Instruction_call* ins)override;
            void visit(Instruction_declare* ins)override;
    };
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