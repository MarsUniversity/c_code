#include <stdio.h>
#include <stddef.h>
#include <netinet/in.h> // for address structs
#include <stdint.h>
#include <sys/socket.h> // socket(), bind(), recvfrom(), ...
#include <unistd.h>     // for close(), usleep()
#include <string.h> // strlen, memset
#include <arpa/inet.h> // inet_sockaddr
#include <netinet/in.h>
#include <sys/time.h> // gettimeofday(), struct timeval

typedef struct sockaddr_in inetaddr_t; // udp or tcp
typedef struct timeval timeval_t;

#define ERROR_LIST \
X(SOCKERROR_SUCCESS, 0, " ") \
X(SOCKERROR_CREATE, -1, " ") \
X(SOCKERROR_REUSE, -2, " ") \
X(SOCKERROR_SET_TIMEOUT, -3, " ") \
X(SOCKERROR_NOT_OPENED, -4, " ") \
X(SOCKERROR_BIND, -5, " ") \
X(SOCKERROR_IN_USE, -6, " ") \
X(SOCKERROR_RECV, -7, " ") \
X(SOCKERROR_SEND, -8, " ") \
X(SOCKERROR_NULL, -9, " ")

typedef enum {
  #define X(id, val, label) id = val,  // X just extracts the ID 
  ERROR_LIST
  #undef X
  ERR_MAX_COUNT
} ErrorCode;

const char* get_error_string(ErrorCode code) {
  switch (code) {
    #define X(id, val, label) case id: return label; // X extracts the label
    ERROR_LIST
    #undef X
    default: return "Unknown Error";
  }
}

#define UDP_BUFFER_SIZE 1024

typedef struct {
  int32_t fd;
  uint8_t buffer[UDP_BUFFER_SIZE];
  int flags;
} udp_t;

// level: IPPROTO_TCP, IPPROTO_UDP, IPPROTO_IP, SOL_SOCKET
int32_t udp_setsockopt(udp_t *udp, int level, int name, int val) {
  if (udp == nullptr || udp == NULL) return SOCKERROR_NULL;
  int err = setsockopt(udp->fd, level, name, (char *)&val, sizeof(val));
  return err == 0 ? true : false; // FIXME: return type
}

// Set timeout in miliseconds for socket read. The default
// is blocking until data arrives.
int32_t udp_settimeout(udp_t *udp, long timeout_msec) {
  if (udp == nullptr || udp == NULL) return SOCKERROR_NULL;
  timeval_t tv;
  tv.tv_sec  = (long)(timeout_msec / 1000L);
  tv.tv_usec = ((timeout_msec - tv.tv_sec * 1000L) % 1000L) * 1000L;

  // printf("udp_settimeout(): %d sec %d usec\n", (int)tv.tv_sec, (int)tv.tv_usec);

  int err = setsockopt(udp->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  return err < 0 ? false : true; // FIXME: return type
}

int32_t udp_open(udp_t *udp, uint32_t timeout_ms) {
  if (udp == nullptr || udp == NULL) return SOCKERROR_NULL;
  if (udp->fd != 0) return SOCKERROR_IN_USE;

  udp->fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp->fd < 0) return SOCKERROR_CREATE;

  // allow multiple sockets to re-use the same address and port
  bool ok = udp_setsockopt(udp, SOL_SOCKET, SO_REUSEPORT, 1);
  ok |= udp_setsockopt(udp, SOL_SOCKET, SO_REUSEADDR, 1);
  if (!ok) return SOCKERROR_REUSE;

  if (timeout_ms > 0) ok = udp_settimeout(udp, timeout_ms);
  if (!ok) return SOCKERROR_SET_TIMEOUT;

  return 0;
}

void udp_close(udp_t *udp) {
  if (udp == nullptr || udp == NULL) return;
  if (udp->fd == 0) return;
  close(udp->fd);
  udp->fd = 0;
}

// Bind socket to an address and port. You can pass in
// an address with port = 0 (select any port), not totally
// sure why you would do that, but this returns the
// port number being used.
//
// return: port > 0 or error < 0
//
// WARNING: Only have one socket bind to a port, usually
//          the server ... clients should not bind or
//          connect since this is UDP only.
int32_t udp_bind(udp_t *udp, const inetaddr_t *addr) {
  if (udp == nullptr || udp == NULL) return SOCKERROR_NULL;
  if (addr == nullptr || addr == NULL) return SOCKERROR_NULL;
  if (udp->fd == 0) return SOCKERROR_NOT_OPENED;
  
  int err = bind(udp->fd, (const struct sockaddr *)addr, sizeof(*addr)); // FIXME: sizeof(addr) is pointer not struct
  if (err < 0) return SOCKERROR_BIND;

  return 0;
}

int32_t udp_recvfrom(udp_t *udp, size_t size, inetaddr_t *from_addr) {
  if (udp == nullptr || udp == NULL) return SOCKERROR_NULL;
  if (from_addr == nullptr || from_addr == NULL) return SOCKERROR_NULL;
  if (size > UDP_BUFFER_SIZE) size = UDP_BUFFER_SIZE;

  socklen_t from_len = sizeof(*from_addr);
  memset(from_addr, 0, from_len);

  int32_t num = recvfrom(
    udp->fd,
    udp->buffer,
    size,
    udp->flags,
    (struct sockaddr *)from_addr,
    &from_len);
  return num;
}

int32_t udp_sendto(udp_t *udp, size_t size, const inetaddr_t *addr) {
  if (udp == nullptr || udp == NULL) return SOCKERROR_NULL;
  if (size > UDP_BUFFER_SIZE) size = UDP_BUFFER_SIZE;
  int32_t num = sendto(
    udp->fd,
    udp->buffer,
    size,
    udp->flags,
    (struct sockaddr *)addr,
    sizeof(*addr)); // FIXME: sizeof(addr) is pointer not struct
  return num;
}


int main() {
  udp_t sock = {};
  printf("sock.fd = %d, flags: %d\n", sock.fd, sock.flags);
  printf("This needs to be FIXED ... it is broken\n");
  return 0;
}


// int32_t udp_recvfrom(udp_t *udp, uint8_t* buffer, size_t msg_size, inetaddr_t *from_addr) {
//   // if (msg_size > buffer.size) msg_size = buffer.size;
//   // if (from_addr == nullptr || from_addr == NULL) from_addr = &last_client;
//   if (from_addr == nullptr || from_addr == NULL) return SOCKERROR_NULL;
//   if (buffer == nullptr || buffer == NULL) return SOCKERROR_NULL;

//   socklen_t from_len = sizeof(*from_addr);
//   memset(from_addr, 0, from_len);

//   int32_t num = recvfrom(udp->fd, buffer, msg_size, udp->flags,
//                     (struct sockaddr *)from_addr, &from_len);
//   return num;
// }

// int32_t udp_sendto(udp_t *udp, const uint8_t *buffer, size_t size, const inetaddr_t *addr) {
//   int32_t num = sendto(udp->fd, buffer, size, udp->flags,
//                       (struct sockaddr *)addr, sizeof(*addr)); // FIXME: sizeof(addr) is pointer not struct
//   return num;
  
// }