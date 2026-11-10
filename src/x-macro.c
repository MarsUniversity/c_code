#include <stdio.h>

// 1. The Master List: Contains the data for all entries
#define ERROR_LIST                                                                                                                                                                                     \
  X(ERR_SUCCESS, "Success")                                                                                                                                                                            \
  X(ERR_NOT_FOUND, "Resource not found")                                                                                                                                                               \
  X(ERR_TIMEOUT, "Request timed out")                                                                                                                                                                  \
  X(ERR_OUT_OF_MEMORY, "Memory allocation failed")

// 2. Generating the Enum
typedef enum {
#define X(id, label) id, // X just extracts the ID
  ERROR_LIST
#undef X
      ERR_MAX_COUNT
} ErrorCode;

// 3. Generating a Function to Convert Enum to String
const char *get_error_string(ErrorCode code) {
  switch (code) {
#define X(id, label)                                                                                                                                                                                   \
  case id:                                                                                                                                                                                             \
    return label; // X extracts the label
    ERROR_LIST
#undef X
  default:
    return "Unknown Error";
  }
}

int main() {
  ErrorCode myErr = 0;

  while (myErr != ERR_MAX_COUNT) {
    printf("Test: %d: %s\n", myErr, get_error_string(myErr));
    myErr += 1;
  }

  printf("Max error count: %d\n", ERR_MAX_COUNT);

  return 0;
}
