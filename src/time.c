#include <stdint.h>
#include <stdio.h>
#include <unistd.h>   // usleep
#include <sys/time.h> // gettimeofday()
#include <time.h>     // linux clock_gettime()

// Pi Pico returns time in micro seconds (usec, 1E-6 sec) so keep everything
// in the same range.
// 
// milli: 1E-3
// micro: 1E-6
// nano:  1E-9
// 
// |            | `uint64_t` | `uint32_t` |
// |------------|------------|------------|
// | nsec [hrs] | 5124095    | 0.00119    |
// | usec [hrs] | 5124095576 | 1.193      |

constexpr uint64_t U1E3 = 1'000;
constexpr uint64_t U1E6 = 1'000'000;
constexpr uint64_t U1E9 = 1'000'000'000;

typedef struct timeval timeval_t;
typedef struct timespec timespec_t;


// clock_id: Specifies the clock to query. Common options 
// include:
// - CLOCK_REALTIME: System-wide real-time clock (wall-clock
//   time, subject to system adjustments).
// - CLOCK_MONOTONIC: Monotonic clock (counts time since an
//   unspecified point, not affected by system clock changes, 
//   ideal for measuring intervals).
// - CLOCK_PROCESS_CPUTIME_ID: CPU time used by the process.
// - CLOCK_THREAD_CPUTIME_ID: CPU time used by the thread.
// 
// milisec  m -3
// microsec u -6
// nanosec  n -9
uint64_t time_ns() {
  uint64_t time;

  timespec_t tv; // {sec, nsec}
  clock_gettime(CLOCK_MONOTONIC, &tv);
  time = tv.tv_sec * U1E9 + tv.tv_nsec;

  return time;
}

uint64_t time_us() {
  return time_ns() / 1'000;
}

uint64_t time_ms() {
  return time_ns() / 1'000'000;
}

int main() {
  printf("time[nsec]: %llu\n", time_ns());
  printf("time[msec]: %llu\n", time_ms());
  return 0;
}