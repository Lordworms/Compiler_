/*
 * SUGGESTION FROM THE CC TEAM:
 * double check the order of actions that are fired.
 * You can do this in (at least) two ways:
 * 1) by using gdb adding breakpoints to actions
 * 2) by adding printing statements in each action
 *
 * For 2), we suggest writing the code to make it straightforward to enable/disable all of them 
 * (e.g., assuming shouldIPrint is a global variable
 *    if (shouldIPrint) std::cerr << "MY OUTPUT" << std::endl;
 * )
 */
#include <sched.h>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L2.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace L2 {
/* 
  * Tokens parsed
  */ 
  std::vector<Item *> parsed_items;

/* 
  * basic rules!
  */
  struct name:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  struct number:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};
  struct Label_rule:
    label {};

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};
  
  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};
  struct var:
    pegtl::seq<
      pegtl::one<'%'>,
      name
    >{};
/* 
  * Keywords.
  */

  //return
  struct str_return : TAOCPP_PEGTL_STRING("return"){};
  //call
  struct str_call   : TAOCPP_PEGTL_STRING("call"){};
  //runtime api
  struct str_print  : TAOCPP_PEGTL_STRING("print"){};
  struct str_input  : TAOCPP_PEGTL_STRING("input"){};
  struct str_allocate   : TAOCPP_PEGTL_STRING("allocate"){};
  struct str_tensor_error:TAOCPP_PEGTL_STRING("tensor-error"){};
  //stack arg
  struct str_stack_arg : TAOCPP_PEGTL_STRING("stack-arg"){};
  //memory access
  struct str_mem    : TAOCPP_PEGTL_STRING("mem"){};
  //cjump
  struct str_cjump  : TAOCPP_PEGTL_STRING("cjump"){};
  struct str_goto  : TAOCPP_PEGTL_STRING("goto"){};
  // lea
  struct str_lea: TAOCPP_PEGTL_STRING( "@" ) {};
  //sop and aop
  struct str_eq   : TAOCPP_PEGTL_STRING("="){};
  struct str_leq   : TAOCPP_PEGTL_STRING("<="){};
  struct str_lt   : TAOCPP_PEGTL_STRING("<"){};
  struct str_left_shift   : TAOCPP_PEGTL_STRING("<<="){};
  struct str_right_shift   : TAOCPP_PEGTL_STRING(">>="){};
  struct str_plus_eq:TAOCPP_PEGTL_STRING("+="){};
  struct str_minus_eq:TAOCPP_PEGTL_STRING("-="){};
  struct str_mul_eq:TAOCPP_PEGTL_STRING("*="){};
  struct str_bitand_eq:TAOCPP_PEGTL_STRING("&="){};
  struct str_plus_plus:TAOCPP_PEGTL_STRING("++"){};
  struct str_minus_minus:TAOCPP_PEGTL_STRING("--"){};

  //jiao ge peng you ba
  struct str_arrow  : TAOCPP_PEGTL_STRING( "<-" ) {};
/*
variable rule  
*/
struct variable_rule:
var {};
/**
 * regisers & register rules
 * 
 * 
 * 
 */

  struct str_rdi : TAOCPP_PEGTL_STRING( "rdi" ) {};
  struct str_rsi : TAOCPP_PEGTL_STRING( "rsi" ) {};
  struct str_rax : TAOCPP_PEGTL_STRING( "rax" ) {};
  struct str_rdx : TAOCPP_PEGTL_STRING( "rdx" ) {};
  struct str_rcx : TAOCPP_PEGTL_STRING( "rcx" ) {};
  struct str_r8 : TAOCPP_PEGTL_STRING( "r8" ) {};
  struct str_r9 : TAOCPP_PEGTL_STRING( "r9" ) {};
  struct str_rbx : TAOCPP_PEGTL_STRING( "rbx" ) {};
  struct str_rbp : TAOCPP_PEGTL_STRING( "rbp" ) {};
  struct str_r10 : TAOCPP_PEGTL_STRING( "r10" ) {};
  struct str_r11 : TAOCPP_PEGTL_STRING( "r11" ) {};
  struct str_r12 : TAOCPP_PEGTL_STRING( "r12" ) {};
  struct str_r13 : TAOCPP_PEGTL_STRING( "r13" ) {};
  struct str_r14 : TAOCPP_PEGTL_STRING( "r14" ) {};
  struct str_r15 : TAOCPP_PEGTL_STRING( "r15" ) {};
  struct str_rsp : TAOCPP_PEGTL_STRING( "rsp" ) {};// stack ptr, need to do specific process
  

  //how to call them
  struct register_rdi_rule:
      str_rdi {};
  struct register_rsi_rule:
      str_rsi {};
  struct register_rax_rule:
    str_rax {};
  struct register_rdx_rule:
    str_rdx {};
  struct register_rcx_rule:
    str_rcx {};
  struct register_r8_rule:
    str_r8 {};
  struct register_r9_rule:
    str_r9 {};
  struct register_rbx_rule:
    str_rbx {};
  struct register_rbp_rule:
    str_rbp {};
  struct register_r10_rule:
    str_r10 {};
  struct register_r11_rule:
    str_r11 {};
  struct register_r12_rule:
    str_r12 {};
  struct register_r13_rule:
    str_r13 {};
  struct register_r14_rule:
    str_r14 {};
  struct register_r15_rule:
    str_r15 {};
  struct register_rsp_rule:
    str_rsp {};

  struct register_rule:
    pegtl::sor<
      register_rdi_rule,
      register_rsi_rule,
      register_rax_rule,
      register_rdx_rule,
      register_rcx_rule,
      register_r8_rule,
      register_r9_rule,
      register_rbx_rule,
      register_rbp_rule,
      register_r10_rule,
      register_r11_rule,
      register_r12_rule,
      register_r13_rule,
      register_r14_rule,
      register_r15_rule
    > {};

  struct stack_reg_rule:
    pegtl::sor<
      register_rsp_rule
    >{};
  
  struct register_entry_rule:
    pegtl::sor<
      register_rule,
      stack_reg_rule
    >{};
  struct register_var_rule:
    pegtl::sor<
      register_rule,
      variable_rule
    >{};
  struct register_var_entry_rule:
    pegtl::sor<
      variable_rule,
      register_entry_rule
    >{};
  /**
   * some rules about function_parameter!
   * 
   */
  struct function_name_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};
  
  struct function_name:
    label {};

  struct argument_number:
    number {};
  struct constant_number:
    number {};
  struct local_number:
    number {} ;

  struct lea_number:
    number {};
  struct label_call_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};
//stack rule
  struct stack_arg_rule:
pegtl::seq<
  str_stack_arg,
  seps,
  constant_number
>{}; 
/**
 * instruction rules!
 * 
 * 
 * 
 * 
 * 
 * 
 */

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > { };
  // memory access rule, which would be used in instruction
  struct mem_acc_rule:
    pegtl::seq<
      str_mem,
      seps,
      register_var_entry_rule,
      seps,
      constant_number
    >{};
  
  // there are too many rules for assignment
  // they include:
  // reg<-reg  reg<-number reg<-mem mem<-reg mem<-label  mem<-number reg<-label

  struct reg_reg_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_arrow,
      seps,
      register_var_entry_rule
    >{};

  struct reg_constant_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_arrow,
      seps,
      constant_number
    >{};
  struct reg_label_rule:
    pegtl::seq<
      register_var_rule,
      seps,
      str_arrow,
      seps,
      Label_rule
    >{};  
  struct reg_label_call_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_arrow,
      seps,
      label_call_rule
    >{};  
  struct reg_mem_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_arrow,
      seps,
      mem_acc_rule
    >{};
  struct reg_stack_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_arrow,
      seps,
      stack_arg_rule
    >{};
  struct mem_reg_rule:
    pegtl::seq<
      mem_acc_rule,
      seps,
      str_arrow,
      seps,
      register_var_entry_rule
    >{};

  struct mem_constant_rule:
    pegtl::seq<
      mem_acc_rule,
      seps,
      str_arrow,
      seps,
      constant_number
    >{};

  struct mem_label_rule:
    pegtl::seq<
      mem_acc_rule,
      seps,
      str_arrow,
      seps,
      pegtl::sor<
      Label_rule,
      label_call_rule
      >
    >{};
  struct Instruction_assignment_rule:
    pegtl::sor<
      pegtl::seq<pegtl::at<reg_reg_rule>,reg_reg_rule>,
      pegtl::seq<pegtl::at<reg_constant_rule>,reg_constant_rule>,
      pegtl::seq<pegtl::at<reg_mem_rule>,reg_mem_rule>,
      pegtl::seq<pegtl::at<reg_label_call_rule>,reg_label_call_rule>,
      pegtl::seq<pegtl::at<reg_label_rule>,reg_label_rule>,
      pegtl::seq<pegtl::at<mem_reg_rule>,mem_reg_rule>,
      pegtl::seq<pegtl::at<mem_constant_rule>,mem_constant_rule>,
      pegtl::seq<pegtl::at<mem_label_rule>,mem_label_rule>,
      pegtl::seq<pegtl::at<reg_stack_rule>,reg_stack_rule>
    > {};
  
  // let's take care of compare!!
  struct register_constant_rule:
    pegtl::sor<
      register_var_entry_rule,
      constant_number
    >{};

  struct cmp_eq_rule:
    pegtl::seq<
    pegtl::sor<
      register_constant_rule,
      variable_rule
    >,
    seps,
    str_eq,
    seps,
    pegtl::sor<
      register_constant_rule,
      variable_rule
    >
    >{};

  struct cmp_lt_rule:
    pegtl::seq<
   pegtl::sor<
      register_constant_rule,
      variable_rule
    >,
    seps,
    str_lt,
    seps,
   pegtl::sor<
      register_constant_rule,
      variable_rule
    >
    >{};

  struct cmp_leq_rule:
    pegtl::seq<
   pegtl::sor<
      register_constant_rule,
      variable_rule
    >,
    seps,
    str_leq,
    seps,
   pegtl::sor<
      register_constant_rule,
      variable_rule
    >
    >{};

  struct Instruction_cmp_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<cmp_eq_rule>,cmp_eq_rule>,
      pegtl::seq< pegtl::at<cmp_lt_rule>,cmp_lt_rule>,
      pegtl::seq< pegtl::at<cmp_leq_rule>,cmp_leq_rule>
    >{};
  
  struct Instruction_assign_and_cmp_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_arrow,
      seps,
      Instruction_cmp_rule
    >{};

  // branch
  struct Instruction_cjump_rule:
    pegtl::seq<
    str_cjump,
    seps,
    Instruction_cmp_rule,
    seps,
    Label_rule
    >{};

  struct Instruction_goto_rule:
    pegtl::seq<
      str_goto,
      seps,
      Label_rule
    >{};

  //self increase and decrease
  struct Instruction_inc_rule:
  pegtl::seq<
    register_var_entry_rule,
    seps,
    str_plus_plus
  > {};

  struct Instruction_dec_rule:
  pegtl::seq<
    register_var_entry_rule,
    seps,
    str_minus_minus
  > {};

  
  //sop
  struct Instruction_shift_right_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_right_shift,
      seps,
      pegtl::sor<
        register_rcx_rule,
        constant_number,
        variable_rule
      >
    >{};

    struct Instruction_shift_left_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_left_shift,
      seps,
      pegtl::sor<
        register_rcx_rule,
        constant_number,
        variable_rule
      >
    >{};
  //aop
  struct plus_eq_rule:
    pegtl::seq<
      pegtl::sor<register_var_entry_rule,mem_acc_rule>,
      seps,
      str_plus_eq,
      seps,
      pegtl::sor<register_var_entry_rule,mem_acc_rule,constant_number>
    >{};
  
  struct minus_eq_rule:
    pegtl::seq<
      pegtl::sor<register_var_entry_rule,mem_acc_rule>,
      seps,
      str_minus_eq,
      seps,
      pegtl::sor<register_var_entry_rule,mem_acc_rule,constant_number>
    >{};

  struct mul_eq_rule:
    pegtl::seq<
      register_var_entry_rule,
      seps,
      str_mul_eq,
      seps,
      pegtl::sor<register_var_entry_rule,constant_number>
    >{};

  struct bitand_eq_rule:
  pegtl::seq<
    pegtl::sor<register_var_entry_rule,mem_acc_rule>,
    seps,
    str_bitand_eq,
    seps,
    pegtl::sor<register_var_entry_rule,mem_acc_rule,constant_number>
  >{};

  struct Instruction_aop_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<plus_eq_rule>         , plus_eq_rule             >,
      pegtl::seq< pegtl::at<minus_eq_rule>        , minus_eq_rule             >,
      pegtl::seq< pegtl::at<mul_eq_rule>        ,   mul_eq_rule             >,
      pegtl::seq< pegtl::at<bitand_eq_rule>       , bitand_eq_rule             >
    >{};
  //label
  struct Instruction_label_rule:
   pegtl::seq<
    Label_rule
   >{};

  //lea rules!
  struct Instruction_lea_rule:
    pegtl::seq<
      register_var_rule, 
      seps,
      pegtl::sor<
      str_lea,
      pegtl::one<'@'>
      >,
      seps,
      register_var_rule,
      seps,
      register_var_rule,
      seps,
      constant_number
    > {};
  //call rules
  struct runtime_rule:
    pegtl::sor<
        str_print,
        str_input,
        str_allocate,
        str_tensor_error
    >{};
  struct  Instruction_call_runtime_rule:
    pegtl::seq<
      str_call,
      seps,
      runtime_rule,
      seps,
      constant_number
    > {}; 
  struct  Instruction_call_common_rule:
    pegtl::seq<
      str_call,
      seps,
      pegtl::sor<label_call_rule, register_rule,variable_rule>,
      seps,
      constant_number
    > {}; 
  struct Instruction_sop_rule:
    pegtl::sor< 
      pegtl::seq< pegtl::at<Instruction_shift_left_rule>         , Instruction_shift_left_rule            >,
      pegtl::seq< pegtl::at<Instruction_shift_right_rule>        , Instruction_shift_right_rule           >
    > {};
  //sum up!
  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_return_rule>            , Instruction_return_rule             >,
      pegtl::seq< pegtl::at<Instruction_assign_and_cmp_rule>    , Instruction_assign_and_cmp_rule     >,
      pegtl::seq< pegtl::at<Instruction_cjump_rule>             , Instruction_cjump_rule              >,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>        , Instruction_assignment_rule         >,
      pegtl::seq< pegtl::at<Instruction_call_runtime_rule>      , Instruction_call_runtime_rule       >,
      pegtl::seq< pegtl::at<Instruction_call_common_rule>       , Instruction_call_common_rule        >,
      pegtl::seq< pegtl::at<Instruction_label_rule>             , Instruction_label_rule              >,
      pegtl::seq< pegtl::at<Instruction_aop_rule>               , Instruction_aop_rule                >,
      pegtl::seq< pegtl::at<Instruction_sop_rule>               , Instruction_sop_rule                >,
      pegtl::seq< pegtl::at<Instruction_lea_rule>               , Instruction_lea_rule                >,
      pegtl::seq< pegtl::at<Instruction_goto_rule>              , Instruction_goto_rule               >,
      pegtl::seq< pegtl::at<Instruction_inc_rule>               , Instruction_inc_rule                >,
      pegtl::seq< pegtl::at<Instruction_dec_rule>               , Instruction_dec_rule                >
    > { };

  struct Instructions_rule:
    pegtl::plus<
      pegtl::seq<
        seps,
        Instruction_rule,
        seps
      >
    > { };
/**
 * function rules! 
 * 
 * 
 */
  struct Function_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      function_name_rule,
      seps,
      argument_number,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< ')' >
    > {};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};
  struct Function_test_name_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};
  struct Function_test_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      Function_test_name_rule,
      seps,
      argument_number,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< ')' >
    >{};
  struct Function_grammar:
    pegtl::must<
      Function_test_rule
    >{};
  struct spill_var:
    variable_rule{};
  struct spill_replace:
    variable_rule{};
  struct spill_rule:
    pegtl::seq<
      Function_test_rule,
      seps,
      spill_var,
      seps,
      spill_replace,
      seps
    >{};
  struct Spill_grammar:
    pegtl::must<
      spill_rule
    >{};
  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      function_name_rule,
      seps,
      Functions_rule,
      seps,
      pegtl::one< ')' >,
      seps
    > { };

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};

  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};
  template<> struct action < Function_test_name_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"enter function!"<<in.string()<<'\n';
        auto newF = new Function();
        newF->name = in.string();
        p.functions.push_back(newF);
        if(isPrint)std::cout<<"it is a test function\n";
      if(isPrint)std::cout<<"end function:!"<<in.string()<<'\n';
    }
  };
  template<> struct action < function_name_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"enter function!"<<in.string()<<'\n';
      if (p.entryPointLabel.empty()){
        p.entryPointLabel = in.string();
        if(isPrint)std::cout<<"it is a entry label function\n";
      } else {
        auto newF = new Function();
        newF->name = in.string();
        p.functions.push_back(newF);
        if(isPrint)std::cout<<"it is a new function\n";
      }
      if(isPrint)std::cout<<"end function:!"<<in.string()<<'\n';
    }
  };
  template<> struct action < str_plus_eq > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
    if(isPrint)std::cout<<"doing plus equal!"<<'\n';
    Aop_item *tmp = new Aop_item;
    tmp->aType = AopType::plus_eq;
    tmp->setType(iType::aop_item);
    parsed_items.push_back(tmp);
    }
  };
  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we got a label! and its name is "<<in.string()<<'\n';
      Function* f=p.functions.back();
      if(f->label_ptr_map.find(in.string())!=f->label_ptr_map.end())
      {
        if(isPrint)std::cout<<"the label is in the name_var_map already\n";
        parsed_items.push_back(f->label_ptr_map[in.string()]);
        return;
      }
      Label_item* lab=new Label_item;
      lab->label_name=in.string();
      f->label_ptr_map[in.string()]=lab;
      parsed_items.push_back(lab);
    }
  };
  template<> struct action <label_call_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are getting a label call and the stack size is "<<parsed_items.size()<<'\n';
      Label_item* lab=new Label_item;
      lab->type=iType::label_item;
      lab->label_name=in.string();
      parsed_items.push_back(lab);
      if(isPrint)std::cout<<"the label's name is"<<in.string()<<" now the stack size is"<<parsed_items.size()<<"\n\n";
    }
  };
  template<> struct action < str_mul_eq > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
    if(isPrint)std::cout<<"get a *=!"<<'\n';
    Aop_item *tmp = new Aop_item;
    tmp->aType = AopType::mul_eq;
    tmp->type = iType::aop_item;
    parsed_items.push_back(tmp);
    }
  };
  template<> struct action < str_minus_eq > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
    Aop_item *tmp = new Aop_item;
    tmp->aType = AopType::minus_eq;
    tmp->type = iType::aop_item;
    parsed_items.push_back(tmp);
    }
  };
  template<> struct action < str_bitand_eq > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
    Aop_item *tmp = new Aop_item;
    tmp->aType = AopType::bitand_eq;
    tmp->type = iType::aop_item;
    parsed_items.push_back(tmp);
    }
  };
  template<> struct action < argument_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"get argument number:"<<in.string()<<'\n';
      auto currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };

  template<> struct action < local_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"local number:"<<in.string()<<'\n';
      auto currentF = p.functions.back();
      currentF->locals = std::stoll(in.string());
    }
  };
  template<> struct action < constant_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"get number:"<<in.string()<<'\n';
      auto currentF = p.functions.back();
      Constant_item* tmp=new Constant_item;
      tmp->type=iType::constant_item;
      tmp->value=std::stoll(in.string());
      parsed_items.push_back(tmp);
      if(isPrint)std::cout<<"pushed a number in parsed_items and its value is "<<tmp->value<<'\n';
    }
  };
  //rdi rsi rax rdx rcx r8 r9 rbx rbp |||| r10 r11 r12 r13 r14 r15 ||| rsp
  template<> struct action < register_rdi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_rdi);
    }
  };
  template<> struct action < register_rsi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_rsi);
    }
  };

  template<> struct action < register_rax_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_rax);
    }
  };

  template<> struct action < register_rdx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_rdx);
    }
  };

  template<> struct action < register_rcx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_rcx);
    }
  };

  template<> struct action < register_r8_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r8);
    }
  };

  template<> struct action < register_r9_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r9);
    }
  };

  template<> struct action < register_rbx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_rbx);
    }
  };

  template<> struct action < register_rbp_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_rbp);
    }
  };

  template<> struct action < register_r10_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r10);
    }
  };

  template<> struct action < register_r11_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r11);
    }
  };

  template<> struct action < register_r12_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r12);
    }
  };

  template<> struct action < register_r13_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r13);
    }
  };

  template<> struct action < register_r14_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r14);
    }
  };

  template<> struct action < register_r15_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(&L2::reg_r15);
    }
  };

  template<> struct action < stack_reg_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register_item;
      r->type=iType::register_item;
      r->regID=Register_ID::rsp;
      parsed_items.push_back(r);
    }
  };
// Instruction rules
  template<> struct action < str_return > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"get a return instruction!\n";
      auto currentF = p.functions.back();
      auto i = new Instruction_ret();
      i->type=InsType::ins_ret;
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_assignment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"Now we are assigning! and the items size is: "<<parsed_items.size()<<'\n';
      auto currentF = p.functions.back();
      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();
      auto i = new Instruction_assignment;
      i->d=dst;
      i->s=src;
      i->type=InsType::ins_assignment;
      currentF->instructions.push_back(i);
      if(isPrint)std::cout<<"assigning finished! and the stack size is "<<parsed_items.size()<<'\n';
    }
  };

  template<> struct action < Instruction_assign_and_cmp_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"doing assign_and_cmp!"<<'\n';
      auto currentF = p.functions.back();
      auto i = new Instruction_assignment;
      i->type = InsType::ins_assignment;
      i->s = parsed_items.back();
      parsed_items.pop_back();
      i->d = parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
    }
  };
  template<> struct action <Instruction_cjump_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are doing a cjum_rule!\n";
      auto currentF = p.functions.back();
      auto i = new Instruction_cjump();
      i->type = InsType::ins_cjump;
      i->label= parsed_items.back();
      parsed_items.pop_back();
      i->condi = parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_goto_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are doing goto instruction! and the stack size is "<<parsed_items.size()<<'\n';
      auto currentF = p.functions.back();
      auto i = new Instruction_goto();
      i->type = InsType::ins_goto;
      i->label = parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
      if(isPrint)std::cout<<"finished goto instruction!"<<"\n\n";
    }
  };

  template<> struct action <Instruction_inc_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are doing self increasing! and the stack size is"<<parsed_items.size()<<'\n';
      auto currentF = p.functions.back();
      auto i = new Instruction_inc();
      i->type = InsType::ins_inc;
      i->op = parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
    }
  };
  template<> struct action <Instruction_dec_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_dec();
      i->type = InsType::ins_dec;
      i->op = parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
    }
  };
 template<> struct action <runtime_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      std::string name=in.string();
      if(name=="print")
      {
        parsed_items.push_back(&L2::runtime_print);
      }
      else if(name=="input")
      {
         parsed_items.push_back(&L2::runtime_input);
      }
      else if(name=="allocate")
      {
         parsed_items.push_back(&L2::runtime_allocate);
      }
      else if(name=="tensor-error")
      {
         parsed_items.push_back(&L2::runtime_tensor);
      }
      else
      {
        if(isPrint)std::cout<<"invalid type for runtime call\n";
      }
    }
  };
  template<> struct action <Instruction_call_runtime_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      if(isPrint)std::cout<<"calling runtime rule!"<<'\n'<<"parsed_size:"<<parsed_items.size()<<'\n';
      auto i = new Instruction_runtime();
      i->type=InsType::ins_call;
      i->isRuntime=true;
      Constant_item* c=(Constant_item*)parsed_items.back();
      if(isPrint)std::cout<<"the argument number for the runtime call is "<<c->value<<'\n';
      i->arg_cnt=c;
      parsed_items.pop_back();
      Label_item* l=(Label_item*)parsed_items.back();
      i->func_name=l;
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
    }
  };
  template<> struct action <Instruction_call_common_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are calling common rule! and the stack size is"<<parsed_items.size()<<'\n';
      auto currentF = p.functions.back();
      auto i = new Instruction_common();
      i->type=InsType::ins_call;
      i->isRuntime=false;
      i->num_cnt=parsed_items.back();
      parsed_items.pop_back();
      i->callee=parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
    }
  };
  template<> struct action <Instruction_label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are dealing with a label instruction!\n";
      auto currentF = p.functions.back();
      auto i = new Instruction_label();
      i->name=parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
      if(isPrint)std::cout<<"the label's name is"<<in.string()<<"\n\n";
    }
  };
  template<> struct action <Instruction_aop_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"now we are doing aop! and the parsed size is "<<parsed_items.size()<<'\n';
      auto currentF = p.functions.back();
      auto i = new Instruction_aop();
      i->type=InsType::ins_aop;
      i->op2=parsed_items.back();
      parsed_items.pop_back();
      Aop_item* c=(Aop_item*)parsed_items.back();
      i->rule=c->aType;
      parsed_items.pop_back();
      i->op1=parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i);
      if(isPrint)std::cout<<"the op1 type is"<<i->op1->type<<"  op2 type is"<<i->op2->type<<'\n';
      if(isPrint)std::cout<<"exit doing aop!"<<'\n';
    }
  };
  template<> struct action <cmp_eq_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Cmp_item* i=new Cmp_item();
      i->type=iType::cmp_item;
      i->op2=parsed_items.back();
      parsed_items.pop_back();
      i->cType=CmpType::eq;
      i->op1=parsed_items.back();
      parsed_items.pop_back();
      parsed_items.push_back(i);
    }
  };
  template<> struct action <cmp_lt_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Cmp_item* i=new Cmp_item();
      i->type=iType::cmp_item;
      i->op2=parsed_items.back();
      parsed_items.pop_back();
      i->cType=CmpType::lt;
      i->op1=parsed_items.back();
      parsed_items.pop_back();
      parsed_items.push_back(i);
    }
  };
  template<> struct action <cmp_leq_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Cmp_item* i=new Cmp_item();
      i->type=iType::cmp_item;
      i->op2=parsed_items.back();
      parsed_items.pop_back();
      i->cType=CmpType::le;
      i->op1=parsed_items.back();
      parsed_items.pop_back();
      parsed_items.push_back(i);
    }
  };
  template<> struct action <Instruction_lea_rule> {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto F=p.functions.back();
      auto i=new Instruction_lea();
      i->type=InsType::ins_lea;
      i->cons=parsed_items.back();
      parsed_items.pop_back();
      i->muler=parsed_items.back();
      parsed_items.pop_back();
      i->src=parsed_items.back();
      parsed_items.pop_back();
      i->des=parsed_items.back();
      parsed_items.pop_back();
      F->instructions.push_back(i);
    }
  };
  template<> struct action <Instruction_shift_right_rule> {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are doing right shift!"<<'\n';
      auto F=p.functions.back();
      auto i=new Instruction_sop();
      i->type=InsType::ins_sop;
      i->direction=ShiftType::right;
      i->offset=parsed_items.back();
      parsed_items.pop_back();
      i->target=parsed_items.back();
      parsed_items.pop_back();
      F->instructions.push_back(i);
      if(isPrint)std::cout<<"right shift finished!"<<'\n';
    }
  };
  template<> struct action <Instruction_shift_left_rule> {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto F=p.functions.back();
      auto i=new Instruction_sop();
      i->type=InsType::ins_sop;
      i->direction=ShiftType::left;
      i->offset=parsed_items.back();
      parsed_items.pop_back();
      i->target=parsed_items.back();
      parsed_items.pop_back();
      F->instructions.push_back(i);
    }
  };
  template<> struct action <mem_acc_rule> {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if(isPrint)std::cout<<"we are doing a mem access rule! cool and the stack size is "<<parsed_items.size()<<"\n";
      auto F=p.functions.back();
      Memory_item* m=new Memory_item();
      Constant_item* c=(Constant_item*)parsed_items.back();
      parsed_items.pop_back();
      m->offset=c;
      Register_item* r=(Register_item*)parsed_items.back();
      parsed_items.pop_back();
      m->regId=r;
      parsed_items.push_back(m);
      if(isPrint)std::cout<<"the mem offset is "<<((Constant_item*)(m->offset))->value<<"\n\n";
    }
  };
  template<> struct action < stack_arg_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
        StackArg_item* tmp=new StackArg_item;
        tmp->type=iType::stackArg_item;
        tmp->offset = parsed_items.back();
        parsed_items.pop_back();
        parsed_items.push_back(tmp);
    }
  };
  template<> struct action <variable_rule> {
    template< typename Input >
    static void apply( const Input & in, Program & p){
       if(isPrint)std::cout<<"it is a variable!"<<in.string()<<'\n';
       Function* f=p.functions.back();
       if(f->name_var_map.find(in.string())!=f->name_var_map.end())
       {
          parsed_items.push_back(f->name_var_map[in.string()]);
          return;
       }
       Var_item* tmp=new Var_item;
       tmp->var_name=in.string();
       f->name_var_map[in.string()]=tmp;
       parsed_items.push_back(tmp);
       return;
    }
  };
  template<> struct action < spill_replace > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
        p.prefix =  new Var_item(in.string());
    }
  };
  template<> struct action < spill_var > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
        Function *F = p.functions.back();
        if (F->name_var_map.find(in.string())!=F->name_var_map.end())
        {
            p.now_spill_var=(Var_item*)F->name_var_map[in.string()];
        } 
        else 
        {
            p.now_spill_var=new Var_item(in.string());
        }
    }
    };
  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< grammar >();

    /*
     * Parse.
     */
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);
    return p;
  }
  Program parse_function_file(char* fileName)
  {
    //if(isPrint)std::cout<<"doing check\n";
    pegtl::analyze<Function_grammar>();
    //if(isPrint)std::cout<<"finished analyse check and the filename is "<<fileName<<'\n';
    Program p;
    file_input< >fileInput(fileName);
    //if(isPrint)std::cout<<"open file success!\n";
    parse<Function_grammar, action>(fileInput,p);
    return p;
  }
  Program parse_spill_file(char *fileName)
  {
    Program p;
    file_input <> fileInput(fileName);
    if(isPrint)std::cout<<"ready to parse"<<'\n';
    parse<Spill_grammar,action>(fileInput,p);
    return p;
  }
}
