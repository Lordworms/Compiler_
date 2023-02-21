#include "coloring.h"
namespace L2
{
    Node_selector::Node_selector(Graph& G,int num)
    {
        this->G=G;
        this->color_num=num;
    }
    Item* Node_selector::get_remove_node()
    {
        Item* res=nullptr;
        NUM_NODES edge_cnt_nodes;
        this->G.pass_stats(edge_cnt_nodes);
        if(edge_cnt_nodes.size()==0)
        {
            return res;
        }
        /*
            if contains edges number less than color_num, remove them
            else remove the most edges node
        */
        int least_num=(*(edge_cnt_nodes.begin())).first;
        if(least_num>this->color_num)
        {
            res=*(edge_cnt_nodes.rbegin()->second.begin());
        }
        else
        {
            for(auto& k:edge_cnt_nodes)
            {
                if(k.first>this->color_num)break;
                res=*k.second.begin();
            }
        }
        this->G.delete_node(res);
        return res;
    }
    void Node_selector::init_stack(std::stack<Item*>&st)
    {
        for(auto it=get_remove_node();it!=nullptr;it=get_remove_node())
        {
            if(it->type==iType::var_item)
            {
                st.push(it);
            }
        }
    }

    //api for color selector
    Color_selector::Color_selector(Graph * G,std::stack<Item*>*st)
    {
        this->G=G;
        this->node_stack=st;
    }
    void Color_selector::pre_color(std::unordered_map<Item*,Reg_color>& item_color_map)
    {
        item_color_map.insert(reg_color_mp.begin(),reg_color_mp.end());
    }
    bool Color_selector::select_color(Item* var,std::unordered_map<Item*,Reg_color>& item_color_mp)
    {
        auto peer=this->G->get_peer(var);
        std::set<Reg_color>peer_color;
        for(auto v:peer)
        {
            if(item_color_mp.find(v)!=item_color_mp.end())
            {
                peer_color.insert(item_color_mp[v]);
            }
        }
        for(int i=0;i<COLOR_NUM;++i)
        {
            auto c=color_order[i];
            if(peer_color.find(c)==peer_color.end())
            {
                item_color_mp[var]=c;
                return true;
            }
        }
        return false;
    }
    bool Color_selector::assign_all(std::unordered_map<Item*,Reg_color>& item_color_mp,std::set<Item*>& spill_variable)
    {
        pre_color(item_color_mp);
        while(!this->node_stack->empty())
        {
            auto tmp=node_stack->top();
            node_stack->pop();
            if(tmp->type==iType::var_item)
            {
                if(!select_color(tmp,item_color_mp))//could find a color
                {
                    spill_variable.insert(tmp);
                }
                else
                {
                    
                }
            }
            else if(tmp->type!=iType::register_item)
            {
                std::cout<<"error type for assign\n";
            }
        }
        return spill_variable.empty();
    }
    //api for spill_allocator
    Spill_allocator::Spill_allocator(NODE* a,NODE* b)
    {
        this->not_colored_nodes=a;
        this->pre_spilled_nodes=b;
    }
    void Spill_allocator::choose_spill_nodes(std::vector<Item*> & spill_set)
    {
        NODE tmp;
        set_diff(*this->not_colored_nodes,*this->pre_spilled_nodes,tmp);
        spill_set.insert(spill_set.end(),tmp.begin(),tmp.end());
    }

    //api for register allocator
    Register_allocator::Register_allocator(Function* F,Function** F_addr)
    {
        this->F=F;
        this->F_addr=F_addr;
    }
    void Register_allocator::replace_variable(Function* F,std::unordered_map<Item*,Reg_color>& item_color_mp)
    {
        Color_visitor cv(item_color_mp,F);
        for(auto ins:F->instructions)
        {
            ins->apply(cv);
        }
    }
    bool Register_allocator::check_complete(Function* F,NODE & spill_variables)
    {
        //first run liveness analyse
        FunctionLiveness fl(F);
        fl.getGenAndKill();
        fl.getInAndOut();

        //run interference analyse
        FunctionInterference fi(F,fl.lv.KILL,fl.IN,fl.OUT);
        fi.build();
        
        //run node selector
        auto interfence_graph=fi.pass_graph();
        std::stack<Item*>node_stack;
        std::unordered_map<Item*,Reg_color>item_color_map;
        Node_selector ns(interfence_graph,COLOR_NUM);
        ns.init_stack(node_stack);

        //run coloring
        Color_selector cs(&interfence_graph,&node_stack);
        bool res=cs.assign_all(item_color_map,spill_variables);
        this->replace_variable(F,item_color_map);
        return res;
    }
    void Register_allocator::allocate()
    {
        bool spill_flag=false;//whether need to spill or not
        Function* F_copy=this->F->copy();//deep copy for F
        bool finished=false;
        long long suffix=0;
        NODE prev_spill;
        while(!finished)
        {
            NODE not_color_vars;
            std::vector<Item*>spill_vars;
            finished=check_complete(this->F,not_color_vars);//check whether it is ok to assign all color
            Spill_allocator sa(&not_color_vars,&prev_spill);//
            sa.choose_spill_nodes(spill_vars);//

            if(!not_color_vars.empty()&&spill_vars.empty())
            {
                spill_flag=true;
                for(auto v:prev_spill)
                {
                    delete (Var_item*)(v);
                }
                break;
            }
            //spill all variable in spill vars
            for(long long i=0;i<spill_vars.size();++i)
            {
                std::string spill_var_name=this->spill_prefix+std::to_string(suffix)+'_'+std::to_string(i);
                Var_item* pre=new Var_item(spill_var_name);
                FunctionSpiller fs(this->F,(Var_item*)spill_vars[i],pre);
                fs.spill();
                auto pre_replace=fs.sv->spill_vars;
                prev_spill.insert(pre_replace.begin(),pre_replace.end());
            }
            suffix++;
        }
        /*
            we need to spill everything
        */
        if(spill_flag)
        {
            std::string all_prefix="%SPILL_ALL_";
            std::vector<Item*> all_var_vec;
            for(auto& var:F_copy->name_var_map)
            {
                all_var_vec.push_back(var.second);
            }
            for(long long i=0;i<all_var_vec.size();++i)
            {
                std::string prefix=all_prefix+std::to_string(i)+'_';
                auto pre=new Var_item(prefix);
                FunctionSpiller fs(F_copy,(Var_item*)all_var_vec[i],pre);
                fs.spill();
            }
            NODE not_color_vars;
            std::vector<Item*>spill_vars;
            this->check_complete(F_copy,not_color_vars);
            *this->F_addr=F_copy;
        }
        return;
    }
    void program_coloring(Program& p)
    {
        for(long long i=0;i<p.functions.size();++i)
        {
            Register_allocator ra(p.functions[i],&p.functions[i]);
            ra.allocate();
        }
    }
}