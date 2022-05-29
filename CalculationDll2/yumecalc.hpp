#pragma once
#include <string>
#ifdef CALCULATIONDLL2_EXPORTS
#define CALCDLL2 __declspec(dllexport)
#else
#define CALCDLL2 __declspec(dllimport)
#endif

CALCDLL2 int yumecalc(const std::string& val);
CALCDLL2 std::string discribe();