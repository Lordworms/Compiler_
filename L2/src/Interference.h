#pragma once
#include "utils.h"
#include "liveness.h"
namespace L2
{
    using EDGE=std::map<Item*,std::set<Item*>>;
    using NODE=std::set<Item*>;
    using NUM_NODES=std::map<int,std::set<Item*>>;
    class Graph
    {
        public:
            Graph();
            Graph(EDGE & E,NODE& N);
            EDGE edges;
            NODE nodes;
            bool add_edge(Item* u,Item* v);
            bool delete_node(Item* node);  
            void pass_stats(NUM_NODES& st);
            NODE& get_peer(Item* node);

    };
    class FunctionInterference
    {
        public:
            SET KILL,IN,OUT;
            Function* F;
            ItemPrint_visitor iv;
            EDGE edges;
            NODE nodes;
            FunctionInterference(Function* F,SET& kill,SET& in,SET& out);
            void build();
            void print_graph();
            void connect_sets(NODE & st1,NODE& st2);
            void connect_fully(NODE& st);//fully connect the sets
            void connect_gp_reg();
            void connect_in_out();
            void connect_kill_out();
            void connect_shift();
            Graph pass_graph();
    };
    void gen_interference_graph(Program& p);
}