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
#include "parser.h"
namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
namespace L3 {
    std::deque<Item *> parsed_items;

  /*
    basic variable
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
   struct variable:
    pegtl::seq<
        pegtl::one<'%'>,
        name
    > {};
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
    struct variable_rule:
        variable {};
    
    struct variable_func_rule:
        variable {};

   /*
    key words
   */
  
   //basic string
   struct str_arrow:
    pegtl::seq<
        pegtl::one<'<'>,
        pegtl::one<'-'>
    > {};
    
    struct str_return : TAOCPP_PEGTL_STRING( "return" ) {};
    struct str_define: TAOCPP_PEGTL_STRING( "define" ) {};
    struct str_call : TAOCPP_PEGTL_STRING( "call" ) {};
    struct str_br: TAOCPP_PEGTL_STRING( "br" ) {};
    struct str_load : TAOCPP_PEGTL_STRING( "load" ) {};
    struct str_store: TAOCPP_PEGTL_STRING( "store" ) {};
    //runtime string
    struct str_print : TAOCPP_PEGTL_STRING( "print" ) {};
    struct str_input : TAOCPP_PEGTL_STRING( "input" ) {};
    struct str_allocate : TAOCPP_PEGTL_STRING( "allocate" ) {};
    struct str_tensor: TAOCPP_PEGTL_STRING( "tensor-error" ) {};
    
    // cmp arithmatic
    struct str_gt : TAOCPP_PEGTL_STRING( ">" ) {};
    struct str_ge: TAOCPP_PEGTL_STRING( ">=" ) {};
    struct str_lt:pegtl::seq<pegtl::one<'<'>> {};
    struct str_le:pegtl::seq<pegtl::one<'<'>,pegtl::one<'='>> {};
    struct str_eq: TAOCPP_PEGTL_STRING( "=" ) {};

    // aop arithmatic
    struct str_plus : TAOCPP_PEGTL_STRING( "+" ) {};
    struct str_minus : TAOCPP_PEGTL_STRING( "-" ) {};
    struct str_mult : TAOCPP_PEGTL_STRING( "*" ) {};
    struct str_bitand : TAOCPP_PEGTL_STRING( "&" ) {};
    struct str_left_shift : TAOCPP_PEGTL_STRING( "<<" ) {};
    struct str_shift_right : TAOCPP_PEGTL_STRING( ">>" ) {};
    
 
  /*
    item rules
  */

   //label Item
   struct Label_rule:
    label {};
    
   struct func_variables_rule:
    pegtl::opt<
        pegtl::seq<
            seps,
            variable_func_rule,
            seps, 
        pegtl::star<
          pegtl::seq<
            seps,
            pegtl::one<','>,
            seps,
            variable_func_rule,
            seps
          >
        >
      >   
    >{};
  //runtime item
   struct runtime_rule:
    pegtl::sor<
        str_print,
        str_input,
        str_allocate,
        str_tensor
        >{};
  //constant item
  struct constant_number:
    number {} ;

  struct function_name:
    pegtl::seq<
        pegtl::one<'@'>,
        name
    > {};
   
  //combination of var num and label
  struct var_num_rule:
  pegtl::sor<
      variable_rule,
      constant_number
  > {};
   
  struct Label_call_rule:
  function_name{};
  struct var_label_rule:
  pegtl::sor<
      variable_rule,
      Label_rule,
      Label_call_rule
  > {};
   
  struct var_num_label_rule:
  pegtl::sor<
      variable_rule,
      Label_rule,
      Label_call_rule,
      constant_number
  > {};

  //arguments list rule                                
  struct args_rule:
  pegtl::opt<
    pegtl::seq<
      seps,
      var_num_rule,
      seps,
      pegtl::star<
        pegtl::seq<
          seps,
          pegtl::one<','>,
          seps,
          var_num_rule,
          seps
        >
      >
    >
  > {};

  //load and store items
  struct load_rule:
  pegtl::seq<
      str_load,
      seps,
      variable_rule
  > {};
  struct store_rule:
  pegtl::seq<
      str_store,
      seps, 
      variable_rule
  > {};


  //used for Aop items
  struct plus_rule:
  pegtl::seq<
      var_num_rule,
      seps,
      str_plus,
      seps,
      var_num_rule
  > {};
  struct minus_rule:
  pegtl::seq<
      var_num_rule,
      seps,
      str_minus,
      seps,
      var_num_rule
  > {};
  struct multy_rule:
  pegtl::seq<
      var_num_rule,
      seps,
      str_mult,
      seps,
      var_num_rule
  > {};
  struct bitand_rule:
  pegtl::seq<
      var_num_rule,
      seps,
      str_bitand,
      seps,
      var_num_rule
  > {};
  struct shift_left_rule:
  pegtl::seq<
      var_num_rule,
      seps, 
      str_left_shift,
      seps,
      var_num_rule
  > {};
  struct shift_right_rule:
  pegtl::seq<
      var_num_rule,
      seps, 
      str_shift_right,
      seps,
      var_num_rule
  > {};
  struct aop_rule: 
    pegtl::sor<
        pegtl::seq< pegtl::at<plus_rule>         ,   plus_rule              >,
        pegtl::seq< pegtl::at<minus_rule>        ,   minus_rule             >,
        pegtl::seq< pegtl::at<multy_rule>        ,   multy_rule             >,
        pegtl::seq< pegtl::at<bitand_rule>       ,   bitand_rule            >,
        pegtl::seq< pegtl::at<shift_left_rule>   ,   shift_left_rule        >,
        pegtl::seq< pegtl::at<shift_right_rule>  ,   shift_right_rule       >
  > {};

  //used for compare item
  struct cmp_lt_rule:
  pegtl::seq<
      var_num_rule,
      seps,
      str_lt,
      seps,
      var_num_rule
  > {};
  struct cmp_le_rule:
  pegtl::seq<
      var_num_rule,
              seps,
      str_le,
      seps,
      var_num_rule
          > {};
  struct cmp_gt_rule:
  pegtl::seq<
      var_num_rule,
      seps,
      str_gt,
      seps,
      var_num_rule
  > {};
  struct cmp_ge_rule:
  pegtl::seq<
      var_num_rule,
              seps,
      str_ge,
      seps,
      var_num_rule
          > {};
  struct cmp_eq_rule:
  pegtl::seq<
      var_num_rule,
      seps,
      str_eq,
      seps,
      var_num_rule
  > {};
  struct cmp_rule: 
    pegtl::sor<
        pegtl::seq< pegtl::at<cmp_lt_rule>        , cmp_lt_rule         >,
        pegtl::seq< pegtl::at<cmp_le_rule>        , cmp_le_rule         >,
        pegtl::seq< pegtl::at<cmp_eq_rule>        , cmp_eq_rule         >, 
        pegtl::seq< pegtl::at<cmp_gt_rule>        , cmp_gt_rule         >,
        pegtl::seq< pegtl::at<cmp_ge_rule>        , cmp_ge_rule         >
    > {};

/*
  Instruction rules
*/
   
  //return Instruction Rule
   struct Instruction_void_ret_rule:
    pegtl::seq<
        str_return
    > {};
  
   struct Instruction_var_ret_rule:
    pegtl::seq<
        str_return,
        seps,
        var_num_rule
    > {};
   struct Instruction_return_rule:
    pegtl::sor<
        pegtl::seq< pegtl::at<Instruction_var_ret_rule>        , Instruction_var_ret_rule            >,
        pegtl::seq< pegtl::at<Instruction_void_ret_rule>         , Instruction_void_ret_rule             >
    > {};

  // Instruction assignment
  struct var_common_rule :
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      var_num_label_rule
  > {};
   
  struct var_aop_rule :
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      aop_rule
  > {};
   
  struct var_cmp_rule :
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      cmp_rule
  > {};
   
  struct var_load_rule :
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      load_rule
  > {};
   
  struct store_all_rule :
  pegtl::seq<
      store_rule,
      seps,
      str_arrow,
      seps,
      var_num_label_rule
  > {}; 

  struct Instruction_assignment_rule:
  pegtl::sor<
      pegtl::seq< pegtl::at<var_aop_rule>            , var_aop_rule             >,
      pegtl::seq< pegtl::at<var_cmp_rule>            , var_cmp_rule             >,
      pegtl::seq< pegtl::at<var_load_rule>           , var_load_rule            >,
      pegtl::seq< pegtl::at<store_all_rule>       , store_all_rule        >,
      pegtl::seq< pegtl::at<var_common_rule>         , var_common_rule          >
  > {};
  //Instruction_label

  struct  Instruction_label_rule:
  pegtl::seq<
      Label_rule
  > {};
  
  //Instruction branch
  struct Instruction_conditional_branch_rule: 
      pegtl::seq<
      str_br,
      seps,
      var_num_rule,
      seps,
      Label_rule
  > {};
  struct Instruction_unconditional_branch_rule: 
      pegtl::seq<
      str_br,
      seps,
      Label_rule
  > {};
  struct Instruction_branch_rule:
      pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_conditional_branch_rule>,Instruction_conditional_branch_rule>,  
      pegtl::seq< pegtl::at<Instruction_unconditional_branch_rule>,Instruction_unconditional_branch_rule>
      >{};

//Instruction call rule    
  struct  call_runtime_rule:
  pegtl::seq<
      str_call, 
      seps,
      runtime_rule,seps,
      pegtl::one< '(' >,
      seps,
      args_rule,seps,
      pegtl::one< ')' >,
      seps   
  > {};            
  struct  call_user_rule:
  pegtl::seq<
      str_call,
      seps,
      var_label_rule,
      seps,
      pegtl::one< '(' >,
      seps,
      args_rule,
      seps,
      pegtl::one< ')' >,
      seps
  > {}; 
  struct  Instruction_call_void_rule:      
  pegtl::seq<
      pegtl::sor<
      call_runtime_rule, 
      call_user_rule
      >
  > {}; 
  struct  Instruction_call_ret_rule:     
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      pegtl::sor<call_runtime_rule, call_user_rule>
  > {}; 
  struct Instruction_call_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_call_void_rule>,Instruction_call_void_rule>,  
      pegtl::seq< pegtl::at<Instruction_call_ret_rule>,Instruction_call_ret_rule>      
    > {};

   //all Instruction rule
   struct Instruction_rule:
    pegtl::sor<
        pegtl::seq< pegtl::at<Instruction_return_rule>                  , Instruction_return_rule               >,
        pegtl::seq< pegtl::at<Instruction_assignment_rule>              , Instruction_assignment_rule           >,
        pegtl::seq< pegtl::at<Instruction_call_rule>                    , Instruction_call_rule            >,
        pegtl::seq< pegtl::at<Instruction_label_rule>                   , Instruction_label_rule                >,
        pegtl::seq< pegtl::at<Instruction_branch_rule>                  , Instruction_branch_rule              >   
    > {};
   struct Instructions_rule:
    pegtl::plus<
        pegtl::seq<
        seps,
        Instruction_rule,
        seps
        >
    > {};

   //Function Rule!
  struct Function_rule:
  pegtl::seq<
      seps,
      str_define,
      seps,
      function_name,
      seps,
      pegtl::one< '(' >,
      seps,
      func_variables_rule,
      seps,
      pegtl::one< ')' >,
      seps,
      pegtl::one< '{' >,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< '}' >
  > {};
  struct Entry_rule:
  pegtl::plus<
      seps,
      Function_rule,
      seps
  > {};

  //L3 grammar!
  struct L3_grammar : 
  pegtl::must< 
      Entry_rule
  > {};
   
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

 

  /*
    basic things action
  */
  template <>struct action<variable_rule>{
  template <typename Input>
    static void apply(const Input &in, Program &p)
    {
          Function *F = p.functions.back();
          auto var_name=in.string();
          if (F->name_var_map.find(var_name)!=F->name_var_map.end())
          {
              parsed_items.push_back(F->name_var_map[var_name]);
              return;
          }
          auto *var = new Var_item(var_name);
          F->name_var_map[var_name] = var;
          F->var_set.insert(var);
          parsed_items.push_back(var);
    }
  };
  
  template<> struct action < constant_number > {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        auto cons = new Constant_item();
        cons->value = std::stoll(in.string());
        parsed_items.push_back(cons);
    }
  };

  template<> struct action < Label_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      Function *F = p.functions.back();
      auto label_name=in.string();
      if (F->label_ptr_map.find(label_name)!=F->label_ptr_map.end())
      {
          parsed_items.push_back(F->label_ptr_map[label_name]);
          return;
      }
      Label_item *label = new Label_item(label_name);
      F->label_ptr_map[label_name] = label;
      parsed_items.push_back(label);
    }
  };
  template<> struct action < Label_call_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      Function *F = p.functions.back();
      auto label_name=in.string();
      if (F->label_ptr_map.find(label_name)!=F->label_ptr_map.end())
      {
          parsed_items.push_back(F->label_ptr_map[label_name]);
          return;
      }
      Label_item *label = new Label_item(label_name);
      F->label_ptr_map[label_name] = label;
      parsed_items.push_back(label);
    }
  };
  template<> struct action < function_name > {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        auto func_name=in.string();
        auto F = new Function();
        F->name = func_name;
        p.functions.push_back(F);
        if (func_name == "@main") 
        {
            p.mainF = F;
        }
    }
  };

  template <> struct action<variable_func_rule>{
  template <typename Input>
  static void apply(const Input &in, Program &p)
    {
        Function * F = p.functions.back();
        auto func_name=in.string();
        if (F->name_var_map.find(func_name)!=F->name_var_map.end())
        {
            F->args.push_back(F->name_var_map[func_name]);
            return;
        }
        Var_item *var = new Var_item(func_name);
        F->name_var_map[func_name] = var;
        F->args.push_back(var);
    }
  };

  /*
  
    Item actions
  
  */
  template<> struct action < runtime_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto func_name=in.string();
      if (func_name==runtime_print.label_name) 
      {
          parsed_items.push_back(&runtime_print);
      } 
      else if (func_name == runtime_allocate.label_name) 
      {
          parsed_items.push_back(&runtime_allocate);
      } 
      else if (func_name == runtime_input.label_name) 
      {
          parsed_items.push_back(&runtime_input);
      } 
      else if (func_name == runtime_tensor.label_name) 
      {
          parsed_items.push_back(&runtime_tensor);
      }
      else
      {
        std::cout<<"error runtime type!\n";
      }
      return;
    }
  };


  //load and store items
  template<> struct action < load_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto ptr=parsed_items.back();
      parsed_items.pop_back();
      auto load=new Load_item(ptr);
      parsed_items.push_back(load);
  }
  };
  template<> struct action < store_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p) {
      auto ptr=parsed_items.back();
      parsed_items.pop_back();
      auto store=new Store_item(ptr);
      parsed_items.push_back(store);
  }};

  //used for AOP

  template<> struct action < plus_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      Aop_item* aop = new Aop_item(op1,op2,AopType::plus);
      parsed_items.push_back(aop);
  }
  };

  template<> struct action < minus_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      Aop_item* aop = new Aop_item(op1,op2,AopType::minus);
      parsed_items.push_back(aop);
  }
  };
  template<> struct action < multy_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      Aop_item* aop = new Aop_item(op1,op2,AopType::multy);
      parsed_items.push_back(aop);
  }
  };
  template<> struct action < bitand_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      Aop_item* aop = new Aop_item(op1,op2,AopType::bit_and);
      parsed_items.push_back(aop);
  }
  };
  template<> struct action < shift_left_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){     
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      Aop_item* aop = new Aop_item(op1,op2,AopType::left_shift);
      parsed_items.push_back(aop);
  }
  };
  template<> struct action < shift_right_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      Aop_item* aop = new Aop_item(op1,op2,AopType::right_shift);
      parsed_items.push_back(aop);
  }
  };

  //used for Cmp items

  template<> struct action < cmp_le_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){               
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      auto cmp = new Cmp_item(op1,op2,CmpType::le);
      parsed_items.push_back(cmp);
  }
  };
  template<> struct action < cmp_lt_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      auto cmp = new Cmp_item(op1,op2,CmpType::lt);
      parsed_items.push_back(cmp);
  }
  };
  template<> struct action < cmp_ge_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      auto cmp = new Cmp_item(op1,op2,CmpType::ge);
      parsed_items.push_back(cmp);
    }
  };
  template<> struct action < cmp_gt_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      auto cmp = new Cmp_item(op1,op2,CmpType::gt);
      parsed_items.push_back(cmp);
  }
  };
  template<> struct action < cmp_eq_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      auto cmp = new Cmp_item(op1,op2,CmpType::eq);
      parsed_items.push_back(cmp);
  }
  };

  /*
    Instruction rules
  */
  //void return
  template<> struct action < Instruction_void_ret_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {        
        auto F = p.functions.back();
        auto ins = new Instruction_ret();
        F->instructions.push_back(ins);
        if(isPrint)std::cout<<ins->print();
    }
  };
  //return value return
  template<> struct action < Instruction_var_ret_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p)
  {         
      auto F = p.functions.back();
      auto ret=parsed_items.back();
      parsed_items.pop_back();
      auto ins=new Instruction_var_ret(ret);
      F->instructions.push_back(ins);
  }
  };
  //assignment
  template<> struct action < Instruction_assignment_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p)
  {
    auto F = p.functions.back();
    auto src=parsed_items.back();
    parsed_items.pop_back();
    auto dst=parsed_items.back();
    parsed_items.pop_back();
    auto ins = new Instruction_assignment(src,dst);
    F->instructions.push_back(ins);
    if(isPrint)std::cout<<ins->print();
  }
  };
  //label inst
  template<> struct action < Instruction_label_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F = p.functions.back();
      auto label=parsed_items.back();
      parsed_items.pop_back();
      auto ins=new Instruction_label(label);
      F->label_set.insert(label);
      F->instructions.push_back(ins);
  }
  };

  //branch
  template <> struct action<Instruction_unconditional_branch_rule>{
  template <typename Input>
  static void apply(const Input &in, Program &p)
  {
      auto F = p.functions.back();
      Item* dst = parsed_items.back();
      parsed_items.pop_back();
      auto ins = new Instruction_branch(dst,nullptr);
      F->instructions.push_back(ins);
      if(isPrint)std::cout<<ins->print();
  }
};
  template <>struct action<Instruction_conditional_branch_rule>{
  template <typename Input>
  static void apply(const Input &in, Program &p)
  {        
    auto F = p.functions.back();
    Item *dst = parsed_items.back();
    parsed_items.pop_back();
    Item *condition = parsed_items.back();
    parsed_items.pop_back();
    auto i = new Instruction_branch(dst, condition);
    F->instructions.push_back(i);
  }
  };
  //call function rule

  template<> struct action < Instruction_call_void_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){       
      auto F = p.functions.back();
      auto ins=new Instruction_call();
      auto callee = parsed_items.front();
      parsed_items.pop_front();
      bool isRuntime = check_runtime_call(callee);
      std::vector<Item*> args (parsed_items.begin(),parsed_items.end());
      ins->isRuntime=isRuntime;
      parsed_items.clear();
      ins->caller = new Call_item(isRuntime,args,callee);
      ins->var_ret = nullptr;
      F->instructions.push_back(ins);
      if(isPrint)std::cout<<ins->print();
  }
  };
  template<> struct action < Instruction_call_ret_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F = p.functions.back();
      auto ins=new Instruction_call();
      auto ret=parsed_items.front();
      parsed_items.pop_front();
      auto callee=parsed_items.front();
      parsed_items.pop_front();
      std::vector<Item*>args(parsed_items.begin(),parsed_items.end());
      bool isRuntime = check_runtime_call(callee);
      ins->isRuntime=isRuntime;
      ins->var_ret=ret;
      parsed_items.clear();
      ins->caller = new Call_item(isRuntime,args,callee);
      F->instructions.push_back(ins);
  }
  };
  Program parse_file (char *fileName){
      Program p;
      pegtl::analyze< L3_grammar >();
      file_input< > fileInput(fileName);
      parse< L3_grammar, action >(fileInput, p);
      return p;
  }
}
