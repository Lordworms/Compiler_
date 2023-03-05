#pragma once

#include  "LB.h"

namespace LB{
  Program parse_file (char *fileName);
  Var_item* get_var(std::string& var_str,Program& p);
}
