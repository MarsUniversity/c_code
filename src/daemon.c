#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#define PID_FILE "/tmp/my_daemon.pid"
 
void create_pid_file() {
  FILE *f = fopen(PID_FILE, "w");
  if (f == NULL) {
    // Handle error (e.g., log to syslog)
    return;
  }
  // Get current process ID and write it to the file
  fprintf(f, "%d\n", getpid());
  fclose(f);
}

void daemonize() {
  pid_t pid;

  // 1. Reset file creation mask, ensures user’s initial
  // shell defaults doesn’t affect the permission of
  // system calls
  umask(0);

  // 2. First Fork: Fork the parent process (parent closes)
  // and shell returns to user
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE); // Fork failed
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS); // Parent exits, child continues
  }

  // 3. Create a new SID (Session ID) for the child process.
  // This prevents original terminal closing from killing process
  if (setsid() < 0) {
    exit(EXIT_FAILURE); // Session creation failed
  }

  // 4. Catch/Ignore Signals
  signal(SIGCHLD, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  // 5. Second Fork: Fork again to relinquish session leadership (step 3)
  // and ensures it doesn’t accidentally attach to a new terminal and the
  // session is isolated
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS); // First child exits, second child continues
  }

  // 6. Change the working directory to root or another safe place
  // that won’t get unmounted by an admin (which will hang because the
  // daemon is running there)
  if ((chdir("/")) < 0) {
    exit(EXIT_FAILURE);
  }
  
  create_pid_file();

  // 7. Close out the standard file descriptors
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // 8. (Optional) Redirect standard streams to /dev/null
  // This prevents third-party libraries from crashing if they try to write to stdout
  // stdin becomes fd 0, stdout becomes fd 1, stderr becomes fd 2
  open("/dev/null", O_RDONLY); // stdin
  open("/dev/null", O_WRONLY); // stdout
  open("/dev/null", O_WRONLY); // stderr
}

int main() {
  // Open system log
  openlog("my_daemon", LOG_PID, LOG_DAEMON);

  // Call the daemonization procedure
  daemonize();

  syslog(LOG_NOTICE, "Daemon successfully started.");

  // The Big Loop (The daemon's actual work)
  while (1) {
    syslog(LOG_INFO, "Daemon is running smoothly...");
    sleep(30); // Do work every 30 seconds
  }

  // Clean up (Though practically unreachable in this infinite loop)
  closelog();
  return EXIT_SUCCESS;
}