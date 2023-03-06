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
#include <stack>
#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>
#include "parser.h"
namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
namespace LB {
    std::deque<Item *> parsed_items;
    std::map<std::string,Fname_item*>str_fname_mp;
    std::stack<Instruction_range*>scope_st;
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
        //pegtl::one<'%'>,
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
    //struct str_call : TAOCPP_PEGTL_STRING( "call" ) {};

    //some IR str
    struct str_new:TAOCPP_PEGTL_STRING("new"){};
    struct str_tuple:TAOCPP_PEGTL_STRING("tuple"){};
    struct str_Tuple:TAOCPP_PEGTL_STRING("Tuple"){};
    struct str_array:TAOCPP_PEGTL_STRING("Array"){};
    struct str_length:TAOCPP_PEGTL_STRING("length"){};
    struct str_left_brac:TAOCPP_PEGTL_STRING("["){};
    struct str_right_brac:TAOCPP_PEGTL_STRING("]"){};
    struct str_full_brac:TAOCPP_PEGTL_STRING("[]"){};
    struct str_int64_rule:TAOCPP_PEGTL_STRING("int64"){};
    struct str_void:TAOCPP_PEGTL_STRING("void"){};
    struct str_code:TAOCPP_PEGTL_STRING("code"){};
    struct str_bleft_brac:
      pegtl::seq<
        pegtl::one<'{'>
      >{};
    struct str_bright_brac:
      pegtl::seq<
        pegtl::one<'}'>
      >{};
    struct str_br: TAOCPP_PEGTL_STRING( "br" ) {};
    // struct str_load : TAOCPP_PEGTL_STRING( "load" ) {};
    // struct str_store: TAOCPP_PEGTL_STRING( "store" ) {};
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
    
    //new str in LB
    struct str_while: TAOCPP_PEGTL_STRING( "while" ) {};
    struct str_continue: TAOCPP_PEGTL_STRING( "continue" ) {};
    struct str_break: TAOCPP_PEGTL_STRING( "break" ) {};
    struct str_if: TAOCPP_PEGTL_STRING( "if" ) {};
    struct str_goto: TAOCPP_PEGTL_STRING( "goto" ) {};
  /*
    item rules
  */
 //constant item
  struct constant_number:
    number {} ;
   //label Item
   struct Label_rule:
    label {};
  //source var
  struct var_src_rule:
    variable{};
  //combination of var num and label
  struct var_num_rule:
  pegtl::sor<
      variable_rule,
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

  //runtime item
   struct runtime_rule:
    pegtl::sor<
        str_print,
        str_input,
        //str_allocate,
        str_tensor
        >{};
  
  //anno item rule
  struct int64_anno_rule:str_int64_rule {};
  struct tuple_anno_rule:str_tuple {};
  struct void_anno_rule:str_void {};
  struct code_anno_rule:str_code {};
  struct tensor_anno_rule:
    pegtl::seq<
        str_int64_rule,
        seps,
        pegtl::plus<
            str_full_brac
        >
    >{};
    //new array and new tuple
    struct new_array_rule:
    pegtl::seq<
        str_new,
        seps,
        str_array,
        seps,
        pegtl::one <'('>,
        seps,
        args_rule,
        seps,
        pegtl::one <')'>,
        seps
    > {};

    struct new_tuple_rule:
    pegtl::seq<
        str_new,
        seps,
        str_Tuple,
        seps,
        pegtl::one <'('>,
        seps,
        var_num_rule,
        seps,
        pegtl::one <')'>,
        seps
    >{};
    
  //arr element item
  struct arr_ele_rule:
    pegtl::seq<
        variable_rule,
        seps,
        pegtl::plus<
            seps,
            str_left_brac,
            seps,
            var_num_rule,
            seps,
            str_right_brac,
            seps
        >
    >{};
  struct all_anno_rule:
    pegtl::sor<
        pegtl::seq< pegtl::at<tensor_anno_rule>       , tensor_anno_rule    >,
        pegtl::seq< pegtl::at<int64_anno_rule>        , int64_anno_rule    >,
        pegtl::seq< pegtl::at<tuple_anno_rule>        , tuple_anno_rule    >, 
        pegtl::seq< pegtl::at<code_anno_rule>         , code_anno_rule      >,
        pegtl::seq< pegtl::at<void_anno_rule>         , void_anno_rule      >
    > {};
  struct function_name:
    pegtl::seq<
        //pegtl::one<'@'>,
        name
    > {};
    struct func_variables_rule:
    pegtl::opt<
        pegtl::seq<
            seps,
            all_anno_rule,
            seps,
            variable_func_rule,
            seps, 
        pegtl::star<
          pegtl::seq<
            seps,
            pegtl::one<','>,
            seps,
            all_anno_rule,
            seps,
            variable_func_rule,
            seps
          >
        >
      >   
    >{};
  
   
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
      var_src_rule,
      Label_rule,
      Label_call_rule,
      constant_number
  > {};

  
  struct tensor_item_rule:
    pegtl::seq<
        str_int64_rule,
        seps,
        pegtl::plus<
            str_full_brac
        > 
    >{};
  //length get rule
  struct length_get_rule:
    pegtl::seq<
        str_length,
        seps,
        variable_rule,
        seps,
        constant_number,
        seps
    > {};
  struct length_get_one_rule:
  pegtl::seq<
      str_length,
      seps,
      variable_rule,
      seps
  > {};
  //used for op items
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
  struct op_rule: 
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
   
  struct var_op_rule :
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      op_rule
  > {};
   
  struct var_cmp_rule :
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      cmp_rule
  > {};

  struct var_arr_ele_rule:
    pegtl::seq<
    variable_rule,
    seps,
    str_arrow,
    seps,
    arr_ele_rule
    > {};
  struct arr_ele_all_rule:
    pegtl::seq<
    arr_ele_rule,
    seps,
    str_arrow,
    seps,
    var_num_label_rule
    > {};
  struct var_new_tuple_rule:
    pegtl::seq<
    variable_rule,
    seps,
    str_arrow,
    seps,
    new_tuple_rule
    > {};
  struct var_new_array_rule:
    pegtl::seq<
    variable_rule,
    seps,
    str_arrow,
    seps,
    new_array_rule
    > {};
  struct var_length_rule:
    pegtl::seq<
    variable_rule,
    seps,
    str_arrow,
    seps,
    pegtl::sor<
      length_get_rule,
      length_get_one_rule
    >
    > {};    
  struct Instruction_assignment_rule:
  pegtl::sor<
      pegtl::seq< pegtl::at<var_op_rule>             , var_op_rule              >,
      pegtl::seq< pegtl::at<var_cmp_rule>            , var_cmp_rule             >,
      pegtl::seq< pegtl::at<var_arr_ele_rule>        , var_arr_ele_rule         >,
      pegtl::seq< pegtl::at<arr_ele_all_rule>        , arr_ele_all_rule         >,
      pegtl::seq< pegtl::at<var_length_rule>         , var_length_rule          >,
      pegtl::seq< pegtl::at<var_new_array_rule>      , var_new_array_rule       >,
      pegtl::seq< pegtl::at<var_new_tuple_rule>      , var_new_tuple_rule       >,
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
      Label_rule,
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

  struct callee_rule:variable{};
  struct call_full_rule:
  pegtl::seq<
    seps,
    callee_rule,
    seps,
    pegtl::one<'('>,
    seps,
    args_rule,
    seps,
    pegtl::one<')'>,
    seps
  >{};
  struct  Instruction_call_void_rule:      
  pegtl::seq<
      pegtl::sor<
      call_full_rule
      >
  > {}; 
  struct  Instruction_call_ret_rule:     
  pegtl::seq<
      variable_rule,
      seps,
      str_arrow,
      seps,
      call_full_rule
  > {}; 
  struct Instruction_call_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_call_void_rule>,Instruction_call_void_rule>,  
      pegtl::seq< pegtl::at<Instruction_call_ret_rule>,Instruction_call_ret_rule>      
    > {};
  struct variable_decalre:variable_rule{};
  struct declare_one_more:
    pegtl::seq<
        seps,
        variable_decalre,
        seps,
        pegtl::star<
            pegtl::seq<
                seps,
                pegtl::one<','>,
                seps,
                variable_decalre,
                seps
            >
        >
    >{};
  struct Instruction_declare_rule:
    pegtl::seq<
       all_anno_rule,
       seps,
       declare_one_more
    >{};  

  struct Instruction_if_rule:
    pegtl::seq<
      str_if,
      seps,
      pegtl::one<'('>,
      seps,
      cmp_rule,
      seps,
      pegtl::one<')'>,
      seps,
      Label_rule,
      seps,
      Label_rule
    >{};
  struct Instruction_while_rule:
    pegtl::seq<
      str_while,
      seps,
      pegtl::one<'('>,
      seps,
      cmp_rule,
      seps,
      pegtl::one<')'>,
      seps,
      Label_rule,
      seps,
      Label_rule
    >{};
  struct Instruction_continue_rule:
    pegtl::seq<
      str_continue
    >{};
  struct Instruction_break_rule:
    pegtl::seq<
      str_break
    >{};
  struct Instruction_goto_rule:
    pegtl::seq<
      str_goto,
      seps,
      Label_rule
    >{};
  struct Instruction_range_rule;
   //all Instruction rule
   struct Instruction_common_rule:
    pegtl::sor<
        pegtl::seq< pegtl::at<Instruction_label_rule>                   , Instruction_label_rule                >,

        pegtl::seq< pegtl::at<Instruction_call_rule>                    , Instruction_call_rule            >,
        pegtl::seq< pegtl::at<Instruction_declare_rule>                 , Instruction_declare_rule            >,
        pegtl::seq< pegtl::at<Instruction_assignment_rule>              , Instruction_assignment_rule           >,
        pegtl::seq< pegtl::at<Instruction_return_rule>                  , Instruction_return_rule               >,
        

        pegtl::seq<pegtl::at<Instruction_while_rule>                      , Instruction_while_rule>,
        pegtl::seq< pegtl::at<Instruction_if_rule>                      , Instruction_if_rule             >,
        
        pegtl::seq<pegtl::at<Instruction_continue_rule>                  , Instruction_continue_rule>,
        pegtl::seq<pegtl::at<Instruction_break_rule>                 , Instruction_break_rule>,
        pegtl::seq< pegtl::at<Instruction_goto_rule>                  , Instruction_goto_rule              >,
        pegtl::seq<pegtl::at<Instruction_range_rule>                   , Instruction_range_rule>
    > {};

   struct Instructions_rule:
    pegtl::star<
        pegtl::seq<
        seps,
        Instruction_common_rule,
        seps
        >
    > {};
    struct Instruction_range_rule:
    pegtl::seq<
      str_bleft_brac,
      seps,
      Instructions_rule,
      seps,
      str_bright_brac
    >{};
   //Function Rule!
  struct Function_rule:
  pegtl::seq<
      seps,
      all_anno_rule,
      seps,
      function_name,
      seps,
      pegtl::one< '(' >,
      seps,
      func_variables_rule,
      seps,
      pegtl::one< ')' >,
      seps,
      Instruction_range_rule,
      seps
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
          auto var=get_var(var_name,p);
          parsed_items.push_back(var);
    }
  };
  template <>struct action<str_bleft_brac>{
  template <typename Input>
    static void apply(const Input &in, Program &p)
    {
        Instruction_range* scope;
        auto F=p.functions.back();
        if(scope_st.empty())
        {
          scope=new Instruction_range(nullptr);
          F->scope=scope;
        } 
        else
        {
          auto old_scope=scope_st.top();
          scope=new Instruction_range(old_scope);
          old_scope->insts.push_back(scope);
        }
        scope_st.push(scope);
    }
  };
  template <>struct action<str_bright_brac>{
  template <typename Input>
    static void apply(const Input &in, Program &p)
    { 
        scope_st.pop();   
    }
  };
  template<> struct action < constant_number > {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        auto cons = new Constant_item();
        cons->value = std::stoll(in.string());
        parsed_items.push_back(cons);
        auto F=p.functions.back();
       // F->need_encode.insert(cons);
    }
  };
  //annotate types!
  template<> struct action <int64_anno_rule> {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        parsed_items.push_back(&LB::int64_anno_ex);  
    }
  };
  template<> struct action <tuple_anno_rule> {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        parsed_items.push_back(&LB::tuple_anno_ex);
    }
  };
  template<> struct action <void_anno_rule> {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        parsed_items.push_back(&LB::void_anno_ex);
    }
  };
  template<> struct action <code_anno_rule> {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        parsed_items.push_back(&LB::code_anno_ex);
    }
  };
  template<> struct action <tensor_anno_rule> {
  template< typename Input >
  static void apply( const Input & in, Program & p)
    {
        ll min_len=7;//int64[]
        ll str_len=5;//int64
        ll dim=(in.string().size()-str_len)/2;
        auto anno=new TypeAnno_item(AnnoType::tensor_anno,dim);
        parsed_items.push_back(anno);
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
  
  template<> struct action < var_src_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
        auto var_name=in.string();
        auto F=p.functions.back();
        auto var=get_var(var_name,p);
        if(var!=nullptr)
        {
            parsed_items.push_back(var);
        }
        else
        {
          Fname_item* fname;
          if(str_fname_mp.find(var_name)!=str_fname_mp.end())
          {
            fname=str_fname_mp[var_name];
          }
          else
          {
            fname=new Fname_item(var_name);
            str_fname_mp[var_name]=fname;
          }
          parsed_items.push_back(fname);
        }
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
        Fname_item* fname;
        if(str_fname_mp.find(func_name)!=str_fname_mp.end())
        {
            fname=str_fname_mp[func_name];
        }
        else
        {
            fname=new Fname_item(func_name);
            str_fname_mp[func_name]=fname;
        }
        if (func_name == "@main") 
        {
            p.mainF = F;
        }
        F->retType=(TypeAnno_item*)parsed_items.back();
        parsed_items.pop_back();
    }
  };

  template <> struct action<variable_func_rule>{
  template <typename Input>
  static void apply(const Input &in, Program &p)
    {
        Function * F = p.functions.back();
        auto func_name=in.string();
        auto anno=parsed_items.back();
        parsed_items.pop_back();
        Var_item *var = new Var_item(func_name,anno);
        F->var_set.insert(var);
        auto var_name=in.string();
        F->args_ptr_map[var_name]=var;
        F->args.push_back(var);
    }
  };

  /*
  
    Item actions
  
  */
  // template<> struct action < runtime_rule > {
  // template< typename Input >
  // static void apply( const Input & in, Program & p){
  //     auto func_name=in.string();
  //     if (func_name==runtime_print.label_name) 
  //     {
  //         parsed_items.push_back(&runtime_print);
  //     } 
  //   //   else if (func_name == runtime_allocate.label_name) 
  //   //   {
  //   //       parsed_items.push_back(&runtime_allocate);
  //   //   } 
  //     else if (func_name == runtime_input.label_name) 
  //     {
  //         parsed_items.push_back(&runtime_input);
  //     } 
  //     else if (func_name == runtime_tensor.label_name) 
  //     {
  //         parsed_items.push_back(&runtime_tensor);
  //     }
  //     else
  //     {
  //       std::cout<<"error runtime type!\n";
  //     }
  //     return;
  //   }
  // };
  
    template<> struct action < callee_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto var_name=in.string();
      auto F=p.functions.back();
      auto var=get_var(var_name,p);
      if(var!=nullptr)
      {
          parsed_items.push_back(var);
      }
      else 
      {
         if(var_name=="print")
         {
            parsed_items.push_back(&LB::fname_print);
         }
         else if(var_name=="input")
         {
            parsed_items.push_back(&LB::fname_input);
         }
         else
         {
            if(str_fname_mp.find(var_name)!=str_fname_mp.end())
            {
              parsed_items.push_back(str_fname_mp[var_name]);
            }
            else
            {
              auto fname=new Fname_item(var_name);
              str_fname_mp[var_name]=fname;
              parsed_items.push_back(fname);
            }
         }
      }
    }
  };
  //load and store items
//   template<> struct action < load_rule > {
//   template< typename Input >
//   static void apply( const Input & in, Program & p){
//       auto ptr=parsed_items.back();
//       parsed_items.pop_back();
//       auto load=new Load_item(ptr);
//       parsed_items.push_back(load);
//   }
//   };
//   template<> struct action < store_rule > {
//   template< typename Input >
//   static void apply( const Input & in, Program & p) {
//       auto ptr=parsed_items.back();
//       parsed_items.pop_back();
//       auto store=new Store_item(ptr);
//       parsed_items.push_back(store);
//   }};

  //used for AOP

 template<> struct action < variable_decalre > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
     auto F=p.functions.back();
     auto scope=scope_st.top();
     auto type=parsed_items.front();
     auto var_name=in.string();
     auto var=new Var_item(var_name,type);
     scope->var_ptr_map[var_name]=var;
     F->var_set.insert(var);
     parsed_items.push_back(var);
  }
  };

  template<> struct action < plus_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto op2=parsed_items.back();
      parsed_items.pop_back();
      auto op1=parsed_items.back();
      parsed_items.pop_back();
      Op_item* aop = new Op_item(op1,op2,OpType::plus);
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
      Op_item* aop = new Op_item(op1,op2,OpType::minus);
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
      Op_item* aop = new Op_item(op1,op2,OpType::multy);
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
      Op_item* aop = new Op_item(op1,op2,OpType::bit_and);
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
      Op_item* aop = new Op_item(op1,op2,OpType::left_shift);
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
      Op_item* aop = new Op_item(op1,op2,OpType::right_shift);
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
      Op_item* cmp = new Op_item(op1,op2,OpType::le);
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
      Op_item* cmp = new Op_item(op1,op2,OpType::lt);
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
      Op_item* cmp = new Op_item(op1,op2,OpType::ge);
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
      Op_item* cmp = new Op_item(op1,op2,OpType::gt);
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
      Op_item* cmp = new Op_item(op1,op2,OpType::eq);
      parsed_items.push_back(cmp);
  }
  };
   //var <- new array and var<- new tuple
  template<> struct action < var_new_array_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
     std::vector<Item*>args;
     while(parsed_items.size()>1)
     {
        args.push_back(parsed_items.back());
        parsed_items.pop_back();
     }
     std::reverse(args.begin(),args.end());
     parsed_items.push_back(new NewArr_item(args));
  }
  }; 
  
  template<> struct action < arr_ele_all_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
     auto F=p.functions.back();
     std::vector<Item*>offsets;
     Item* addr=parsed_items.front();
     parsed_items.pop_front();
     while(parsed_items.size()>1)
     {
        auto off=parsed_items.front();
        offsets.push_back(off);
        parsed_items.pop_front();
        if(off->type==iType::constant_item)
        {
            //F->need_encode.erase(dynamic_cast<Constant_item*>(off));
        }
     }
     parsed_items.push_front(new Arrele_item(addr,offsets,in.position().line)); 
  }
  }; 
  template<> struct action < var_arr_ele_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
     std::vector<Item*>offsets;
     auto F=p.functions.back();
     while(parsed_items.size()>2)
     {
        auto off=parsed_items.back();
        offsets.push_back(off);
        parsed_items.pop_back();
        if(off->type==iType::constant_item)
        {
          //F->need_encode.erase(dynamic_cast<Constant_item*>(off));
        }
     }
     std::reverse(offsets.begin(),offsets.end());
     Item* addr=parsed_items.back();
     parsed_items.pop_back();
     parsed_items.push_back(new Arrele_item(addr,offsets,in.position().line)); 
  }
  }; 
  template<> struct action < var_new_tuple_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
     auto len=parsed_items.back();
     parsed_items.pop_back();
     parsed_items.push_back(new NewTup_item(len));
  }
  }; 
  template<> struct action < length_get_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
    auto F=p.functions.back();
    auto dim=parsed_items.back();
    parsed_items.pop_back();
    auto arr=parsed_items.back();
    parsed_items.pop_back();
    parsed_items.push_back(new Length_item(arr,dim)); 
    if(dim->type==iType::constant_item)
    {
      //F->need_encode.erase(dynamic_cast<Constant_item*>(dim));
    }
  } 
  }; 
  template<> struct action <length_get_one_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
    auto arr=parsed_items.back();
    parsed_items.pop_back();
    while(parsed_items.back()==arr)parsed_items.pop_back();
    parsed_items.push_back(new Length_item(arr,nullptr)); 
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
        auto scope=scope_st.top();
        auto ins = new Instruction_ret(scope);
        scope->insts.push_back(ins);
        if(isPrint)std::cout<<ins->print();
    }
  };
  //return value return
  template<> struct action < Instruction_var_ret_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p)
  {         
      auto F = p.functions.back();
      auto scope=scope_st.top();
      auto ret=parsed_items.back();
      parsed_items.pop_back();
      auto ins=new Instruction_var_ret(ret,scope);
      scope->insts.push_back(ins);
      scope->insts.push_back(ins);
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
    auto scope=scope_st.top();
    auto ins = new Instruction_assignment(src,dst,scope);
    scope->insts.push_back(ins);
    if(isPrint)std::cout<<ins->print();
  }
  };
  //label inst
  template<> struct action < Instruction_label_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F = p.functions.back();
      auto scope=scope_st.top();
      auto label=parsed_items.back();
      parsed_items.pop_back();
      auto ins=new Instruction_label(label,scope);
      F->label_insts.insert(label);
      scope->insts.push_back(ins);
  }
  };

  //branch
  template <> struct action<Instruction_goto_rule>{
  template <typename Input>
  static void apply(const Input &in, Program &p)
  {
      auto F = p.functions.back();
      auto scope=scope_st.top();
      Item* dst = parsed_items.back();
      parsed_items.pop_back();
      auto ins = new Instruction_goto(dst,scope);
      scope->insts.push_back(ins);
      if(isPrint)std::cout<<ins->print();
  }
};
  //call function rule

  template<> struct action < Instruction_call_void_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){       
      auto F = p.functions.back();
      auto scope=scope_st.top();
      auto callee = parsed_items.front();
      parsed_items.pop_front();
      bool isRuntime = check_runtime_call(callee);
      std::vector<Item*> args (parsed_items.begin(),parsed_items.end());
      parsed_items.clear();
      auto ins=new Instruction_call(new Call_item(isRuntime,args,callee),scope);
      scope->insts.push_back(ins);
      if(isPrint)std::cout<<ins->print();
  }
  };
  template<> struct action < Instruction_call_ret_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F = p.functions.back();
      auto scope=scope_st.top();
      auto ret=parsed_items.front();
      parsed_items.pop_front();
      auto callee=parsed_items.front();
      parsed_items.pop_front();
      std::vector<Item*>args(parsed_items.begin(),parsed_items.end());
      bool isRuntime = check_runtime_call(callee);
      parsed_items.clear();
      auto new_caller=new Call_item(isRuntime,args,callee);
      auto ins=new Instruction_assignment(new_caller,ret,scope);
      scope->insts.push_back(ins);
  }
  };
  template<> struct action < Instruction_declare_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F=p.functions.back();
      auto scope=scope_st.top();
      auto anno=parsed_items.front();
      parsed_items.pop_front();
      auto vars=std::vector<Item*>(parsed_items.begin(),parsed_items.end());
      parsed_items.clear();
      auto declare=new Instruction_declare(anno,vars,scope);
      scope->insts.push_back(declare);
  }
  };
  template<> struct action < Instruction_while_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F=p.functions.back();
      auto scope=scope_st.top();
      auto loc2=parsed_items.back();
      parsed_items.pop_back();
      auto loc1=parsed_items.back();
      parsed_items.pop_back();
      auto condition=parsed_items.back();
      parsed_items.pop_back();
      auto ins=new Instruction_while(loc1,loc2,condition,scope);
      scope->insts.push_back(ins);
  }
  };
    template<> struct action < Instruction_if_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F=p.functions.back();
      auto scope=scope_st.top();
      auto loc2=parsed_items.back();
      parsed_items.pop_back();
      auto loc1=parsed_items.back();
      parsed_items.pop_back();
      auto condition=parsed_items.back();
      parsed_items.pop_back();
      auto ins=new Instruction_if(loc1,loc2,condition,scope);
      scope->insts.push_back(ins);
  }
  };
  template<> struct action < Instruction_continue_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F=p.functions.back();
      auto scope=scope_st.top();
      auto ins=new Instruction_continue(scope);
      scope->insts.push_back(ins);
  }
  };
  template<> struct action < Instruction_break_rule > {
  template< typename Input >
  static void apply( const Input & in, Program & p){
      auto F=p.functions.back();
      auto scope=scope_st.top();
      auto ins=new Instruction_break(scope);
      scope->insts.push_back(ins);
  }
  };
  Var_item* get_var(std::string& var_str,Program& p)
  {
     Instruction_range* now_scope=nullptr;
     auto F=p.functions.back();
     if(!scope_st.empty())
     {
        now_scope=scope_st.top();
     }
     while(now_scope!=nullptr)
     {
        if(now_scope->var_ptr_map.find(var_str)!=now_scope->var_ptr_map.end())
        {
          return now_scope->var_ptr_map[var_str];
        }
        else
        {
          now_scope=now_scope->scope;
        }
     }
     if(F->args_ptr_map.find(var_str)!=F->args_ptr_map.end())
     {
        return F->args_ptr_map[var_str];
     }
     return nullptr;
  }
  Program parse_file (char *fileName){
      Program p;
      pegtl::analyze< L3_grammar >();
      file_input< > fileInput(fileName);
      parse< L3_grammar, action >(fileInput, p);
      // for(auto F:p.functions)
      // {
      //   for(auto ins:F->insts)
      //   {
      //       std::cout<<ins->print();
      //   }
      // }
      return p;
  }
}
