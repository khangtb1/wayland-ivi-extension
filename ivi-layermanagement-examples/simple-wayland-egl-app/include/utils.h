#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef LOG_ERROR
#define LOG_ERROR(...) {                     \
    fprintf(stderr, "ERROR : " __VA_ARGS__); \
}
#endif

#ifndef LOG_INFO
#define LOG_INFO(...) {                      \
    fprintf(stderr, "INFO  : " __VA_ARGS__); \
}
#endif

#ifndef LOG_WARNING
#define LOG_WARNING(...) {                   \
    fprintf(stderr, "WARN  : " __VA_ARGS__); \
}
#endif

#endif