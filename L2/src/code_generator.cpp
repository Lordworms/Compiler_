#include "code_generator.h"
namespace L2
{
   Code_generator_L2_to_L1::Code_generator_L2_to_L1(std::ofstream* out,Program* p)
   {
        this->out=out;
        this->p=p;
        this->cl1v=CodeL1_visitor(this->out);
   } 
   void Code_generator_L2_to_L1::print_code()
   {
        *this->out<<"("<<this->p->entryPointLabel<<'\n';
        for(auto f:this->p->functions)
        {
            this->cl1v.locals_num=f->locals;
            if(f->name=="@main"&&f->locals==6)f->locals++;
            *this->out<<'\t'<<"("<<f->name<<" "<<f->arguments<<" "<<f->locals<<'\n';
            for(auto ins:f->instructions)
            {
                *this->out<<'\t';
                ins->apply(this->cl1v);
            }
            *this->out<<")\n";
        }
        *this->out<<")\n";
   }
   void generate_code(Program& p)
   {
        std::ofstream out;
        out.open("prog.L1");
        Code_generator_L2_to_L1 generator(&out,&p);
        generator.print_code();
        out.close();
   }
}