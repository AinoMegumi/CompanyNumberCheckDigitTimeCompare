#pragma once
#include <string>
#ifdef CALCULATORDLL_EXPORTS
#define CALCDLL __declspec(dllexport)
#else
#define CALCDLL __declspec(dllimport)
#endif

CALCDLL unsigned long calc(const std::string& val);
