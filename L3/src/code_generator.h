#pragma once
#include "L3.h"
#include "tilling.h"
#include <fstream>
namespace L3
{
    using CODE_SET=std::vector<std::vector<InsForest*>>;
    void generate_all(Program& p);
    void generate_code(Program& p,CODE_SET& cs);
}