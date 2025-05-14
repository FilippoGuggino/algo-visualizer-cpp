#ifndef LOG_H
#define LOG_H

#include <spdlog/spdlog.h>

#ifdef __EMSCRIPTEN__
#define log_console(S) emscripten_log(EM_LOG_CONSOLE, S)
#else
#define log_console(S) spdlog::info(S)
#endif

#endif /* LOG_H */
