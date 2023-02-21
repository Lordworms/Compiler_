#include "Interference.h"

namespace L2
{
    Graph::Graph()
    {
        nodes=NODE();
        edges=EDGE();
    }
    Graph::Graph(EDGE& e,NODE& n)
    {
        this->nodes=n;
        this->edges=e;
    }
    bool Graph::add_edge(Item* u,Item* v)
    {
        nodes.insert(u);
        nodes.insert(v);
        edges[u].insert(v);
        edges[v].insert(u);
        return true;
    }
    bool Graph::delete_node(Item* node)
    {
        if(!nodes.count(node))
        {
            return false;
        }
        for(auto v:edges[node])
        {
            edges[v].erase(node);
        }
        edges.erase(node);
        nodes.erase(node);
        return true;
    }
    void Graph::pass_stats(NUM_NODES& st)
    {
        for(auto v:this->nodes)
        {
            st[this->edges[v].size()].insert(v);
        }
    }
    NODE& Graph::get_peer(Item* node)
    {
        if(this->edges.find(node)==this->edges.end())
        {
            std::cout<<"this node did not find in this graph\n";
        }
        return this->edges[node];
    }
    //FunctionInterference apis
    FunctionInterference::FunctionInterference(Function* F,SET& kill,SET& in,SET& out)
    {
        this->F=F;
        this->KILL=kill;
        this->IN=in;
        this->OUT=out;
        this->nodes=std::set<Item*>();
        this->edges=std::map<Item*,std::set<Item*>>();
    }
    void FunctionInterference::build()
    {
        connect_gp_reg();
        connect_in_out();
        connect_kill_out();
        connect_shift();
    }
    void FunctionInterference::connect_sets(NODE& st1,NODE& st2)
    {
        for(auto v:st1)
        {
            nodes.insert(v);
            edges[v].insert(st2.begin(),st2.end());
            edges[v].erase(v);
        }
        for(auto v:st2)
        {
            nodes.insert(v);
            edges[v].insert(st1.begin(),st1.end());
            edges[v].erase(v);
        }
    }
    void FunctionInterference::connect_fully(NODE & st)
    {
        for(auto var:st)
        {
            nodes.insert(var);
        }
        for(auto v1:st)
        {
            for(auto v2:st)
            {
                if(v1!=v2)
                {
                    edges[v1].insert(v2);
                    edges[v2].insert(v1);
                }
            }
        }
    }
    void FunctionInterference::connect_gp_reg()
    {
        std::set<Item*>st(gp_reg.begin(),gp_reg.end());
        this->connect_fully(st);
    }
    void FunctionInterference::connect_in_out()
    {
        for(auto inst:F->instructions)
        {
            this->connect_fully(this->IN[inst]);
            this->connect_fully(this->OUT[inst]);
        }       
    }
    void FunctionInterference::connect_kill_out()
    {
        for(auto inst:F->instructions)
        {
            // if(this->KILL[inst].size()==9)
            // {
            //     int x=0;
            //     std::cout<<"KILL set contains:";
            //     for(auto it:this->KILL[inst])
            //     {
            //         std::cout<<it->print()<<' ';
            //     }
            //     std::cout<<'\n';
            //     std::cout<<"OUT set contains:";
            //     for(auto it:this->OUT[inst])
            //     {
            //         std::cout<<it->print()<<' ';
            //     }
            //     std::cout<<'\n';
            // }
            this->connect_sets(this->KILL[inst],this->OUT[inst]);
        }
    }
    void FunctionInterference::connect_shift()
    {
        std::set<Item*>st(gp_reg.begin(),gp_reg.end());
        st.erase(&L2::reg_rcx);
        for(auto inst:F->instructions)
        {
            if(inst->type==L2::InsType::ins_sop)
            {
               Instruction_sop * tmp=(Instruction_sop*)inst;
               if(tmp->offset->type==iType::register_item||tmp->offset->type==iType::var_item)
               {
                    std::set<Item*>tt={tmp->offset};
                    connect_sets(st,tt);
               } 
            }
        }
    }
    void FunctionInterference::print_graph()
    {
        for(auto node:nodes)
        {
            std::cout<<node->print()<<' ';
            for(auto peer:edges[node])
            {
                std::cout<<peer->print()<<' ';
            }
            std::cout<<'\n';
        }
    }
    Graph FunctionInterference::pass_graph()
    {
        Graph res(this->edges,this->nodes);
        return res;
    }
    void gen_interference_graph(Program& p)
    {
        for(auto f:p.functions)
        {
            FunctionLiveness l(f);
            l.getGenAndKill();
            l.getInAndOut();
            FunctionInterference ff(f,l.lv.KILL,l.IN,l.OUT);
            ff.build();
            ff.print_graph();
        }
    }
}