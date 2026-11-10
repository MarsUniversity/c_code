#include <stdint.h>
#include <stdio.h>
#include <sys/time.h> // gettimeofday()
#include <time.h>     // linux clock_gettime()
#include <unistd.h>   // usleep
#include <errno.h>
#include "common.h"

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

#if __C23__
  constexpr uint64_t U1E3 = 1'000;         // msec
  constexpr uint64_t U1E6 = 1'000'000;     // usec
  constexpr uint64_t U1E9 = 1'000'000'000; // nsec
#else
  #define U1E3 1000
  #define U1E6 1000000
  #define U1E9 1000000000
#endif

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
  timespec_t tv; // {sec, nsec}
  clock_gettime(CLOCK_MONOTONIC, &tv);
  uint64_t time = tv.tv_sec * U1E9 + tv.tv_nsec;

  return time;
}

uint64_t time_us() { return time_ns() / U1E3; }
uint64_t time_ms() { return time_ns() / U1E6; }

// Sleep =========================================
static void sleep_base(const uint32_t usec) {
  // doesn't handle values over 1 second
  if (usec >= U1E6) {
    const uint32_t remainder = usec % U1E6;
    const uint32_t whole_sec = (usec - remainder) / U1E6;
    sleep(whole_sec);  // whole seconds
    usleep(remainder); // fractional msec left over
    return;
  }
  usleep(usec);
}

static void sleep_base2(uint32_t sec, uint32_t nsec){
  // const uint32_t subsec = usec % U1E6;
  // const uint32_t sec = (usec - subsec) / U1E6;

  struct timespec req = {sec, nsec}; // second, nanoseconds
  struct timespec rem;

  if (nanosleep(&req, &rem) == -1) {
    if (errno == EINTR) {
        printf("Interrupted. Remaining time: %ld seconds, %ld nanoseconds\n", rem.tv_sec, rem.tv_nsec);
    }
    else {
        perror("nanosleep failed");
    }
  }
}

void sleep_ms(uint32_t msec) {
  // sleep_base2(U1E3 * msec);
  // sleep_base2((msec / U1E3), (U1E6 * msec) % U1E9);

  uint32_t sec = msec / U1E3;
  uint32_t nsec = (U1E6 * msec) % U1E9;
  sleep_base2(sec, nsec);
}

void sleep_us(uint32_t usec) {
  sleep_base(usec);
}

void sleep_sec(uint32_t sec) {
  sleep(sec);
}

void sleep_test(const uint32_t duration) {
  uint64_t start, stop;
  start = time_ms();
  sleep_ms(duration);
  stop = time_ms();
  printf("%u diff[msec]: %llu\n", duration, stop - start);
}

int main() {
  printf("time[nsec]: %llu\n", time_ns());

  sleep_test(1000);
  sleep_test(5432);
  return 0;
}