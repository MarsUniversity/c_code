#include <stdio.h>
#include "common.h"

// This should work in BASH on Linux and macOS
#if __C23__
// Foreground
constexpr char FG_BLACK[]   = "\x1b[30m";
constexpr char FG_RED[]     = "\x1b[31m";
constexpr char FG_GREEN[]   = "\x1b[32m";
constexpr char FG_YELLOW[]  = "\x1b[33m";
constexpr char FG_BLUE[]    = "\x1b[34m";
constexpr char FG_MAGENTA[] = "\x1b[35m";
constexpr char FG_CYAN[]    = "\x1b[36m";
constexpr char FG_WHITE[]   = "\x1b[37m";
constexpr char FG_RESET[]   = "\x1b[0m";

// background
constexpr char BG_BLACK[]   = "\x1b[40m";
constexpr char BG_RED[]     = "\x1b[41m";
constexpr char BG_GREEN[]   = "\x1b[42m";
constexpr char BG_YELLOW[]  = "\x1b[43m";
constexpr char BG_BLUE[]    = "\x1b[44m";
constexpr char BG_MAGENTA[] = "\x1b[45m";
constexpr char BG_CYAN[]    = "\x1b[46m";
constexpr char BG_WHITE[]   = "\x1b[47m";
constexpr char BG_RESET[]   = "\x1b[0m";

#else
// Foreground
#define FG_BLACK   "\x1b[30m"
#define FG_RED     "\x1b[31m"
#define FG_GREEN   "\x1b[32m"
#define FG_YELLOW  "\x1b[33m"
#define FG_BLUE    "\x1b[34m"
#define FG_MAGENTA "\x1b[35m"
#define FG_CYAN    "\x1b[36m"
#define FG_WHITE   "\x1b[37m"
#define FG_RESET   "\x1b[0m"

// background
#define BG_BLACK   "\x1b[40m"
#define BG_RED     "\x1b[41m"
#define BG_GREEN   "\x1b[42m"
#define BG_YELLOW  "\x1b[43m"
#define BG_BLUE    "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN    "\x1b[46m"
#define BG_WHITE   "\x1b[47m"
#define BG_RESET   "\x1b[0m"
#endif

int main() {
  printf("%sColor%s\n", FG_RED, FG_RESET);
  printf("%sColor%s\n", BG_RED, BG_RESET);

  return 0;
}