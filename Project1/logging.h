#pragma once

extern char * GL_LOG_FILE;
bool restart_gl_log();
bool gl_log(const char* message, ...);
bool gl_log_err(const char* message, ...);