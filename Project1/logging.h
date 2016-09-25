#pragma once

#ifdef _DEBUG
#include <iostream>
#include <string.h>
#define MY_PRINT(x) do{ std::cout << #x"=" << x << "\nAt Line: " << __LINE__ << "\nIn File: " << __FILE__ << std::endl; gl_log_err(#x);}while(0)
#endif

extern char * GL_LOG_FILE;
bool restart_gl_log();
bool gl_log(const char* message, ...);
bool gl_log_err(const char* message, ...);