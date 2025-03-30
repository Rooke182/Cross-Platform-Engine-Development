#include "RandomNum.h"

std::mt19937 RandomNum::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> RandomNum::s_Distribution;