#include <string.h>

#include "globs.h"
#include "memory.h"

#ifdef LST_ON_WINDOWS

/****************************** Windows *******************************/

/* WinSock include files */
#include <winsock.h>

static int winsockStarted = 0;
extern void lstSocketRelease();

void lstSocketInit()
{
   WSADATA wsaData;
   WORD wVerReq = MAKEWORD(1, 1);      /* version 1.1, for 2.0, use (2,0) */

   if (winsockStarted)
     return;

   if (WSAStartup(wVerReq, &wsaData) != 0)
     return;

   winsockStarted = 1;
   atexit(lstSocketRelease);
}

void lstSocketRelease()
{
   if (winsockStarted == 0)
     return;
   WSACancelBlockingCall();	       /* cancel any blocking calls */
   WSACleanup();
   winsockStarted = 0;
}

struct object *
lstSocketPrimitive(int primitiveNumber, struct object *args, int *failed)
{
    struct object *returnedValue = nilObject;
    /* TODO to implement */
    return returnedValue;
}

struct object *
lstInetPrimitive(int primitiveNumber, struct object *args, int *failed)
{
    struct object *returnedValue = nilObject;
    /* TODO to implement */
    return returnedValue;
}
#else /* LST_ON_WINDOWS */

/****************************** Unix  *******************************/

/* BSD include files */

/* error codes */
#include <errno.h>
/* close function */
#include <unistd.h>
/* fnctnl function and associated constants */
#include <fcntl.h>
/* struct sockaddr */
#include <sys/types.h>
/* socket function */
#include <sys/socket.h>
/* struct timeval */
#include <sys/time.h>
/* gethostbyname and gethostbyaddr functions */
#include <netdb.h>
/* sigpipe handling */
#include <signal.h>
/* IP stuff*/
#include <netinet/in.h>
#include <arpa/inet.h>
/* TCP options (nagle algorithm disable) */
#include <netinet/tcp.h>

void lstSocketInit()
{
}

void lstSocketRelease()
{
}

struct object *
lstSocketPrimitive(int primitiveNumber, struct object *args, int *failed)
{
    struct object *returnedValue = nilObject;
    switch (integerValue(args->data[0])) {

        case 0:     /* constants */
            switch (integerValue(args->data[1])) {
            case 0:
                returnedValue = newInteger(AF_INET); break;
            case 1:
                returnedValue = newInteger(AF_INET6); break;
            case 2:
                returnedValue = newInteger(AF_LOCAL); break;
            case 10:
                returnedValue = newInteger(SOCK_STREAM); break;
            case 11:
                returnedValue = newInteger(SOCK_DGRAM); break;
            case 12:
                returnedValue = newInteger(SOCK_RAW); break;
            case 20:
                returnedValue = newInteger(IPPROTO_TCP); break;
            case 21:
                returnedValue = newInteger(IPPROTO_UDP); break;
#ifdef IPPROTO_SCTP
            case 22:
                returnedValue = newInteger(IPPROTO_SCTP); break;
#endif
            case 30:
                returnedValue = newInteger(SHUT_RD); break;
            case 31:
                returnedValue = newInteger(SHUT_WR); break;
            case 32:
                returnedValue = newInteger(SHUT_RDWR); break;
            }
            break;

        case 10: {   /* create socket */
            int family = integerValue(args->data[1]);
            int socketType = integerValue(args->data[2]);
            int protocol = integerValue(args->data[3]);
            int sockfd = socket(family, socketType, protocol);
            if (sockfd >= 0)
                returnedValue = newInteger(sockfd);
            break;
        }

        case 11: {   /* create TCP socket */
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd >= 0)
                returnedValue = newInteger(sockfd);
            break;
        }

        case 12: {   /* create UDP socket */
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd >= 0)
                returnedValue = newInteger(sockfd);
            break;
        }

        case 20: {   /* close */
            int sockfd = integerValue(args->data[1]);
            int rc = close(sockfd);
            returnedValue = newInteger(rc);
            break;
        }

        case 21: {   /* shutdown */
            int sockfd = integerValue(args->data[1]);
            int how = integerValue(args->data[2]);
            int rc = shutdown(sockfd, how);
            returnedValue = newInteger(rc);
            break;
        }

        case 30: {   /* bind */
            int sockfd = integerValue(args->data[1]);
            struct byteObject *addrObj = (struct byteObject *) args->data[2];
            int rc = bind(sockfd, (struct sockaddr *) addrObj->bytes, SIZE(addrObj));
            returnedValue = newInteger(rc);
            break;
        }

        case 40: {   /* listen */
            int sockfd = integerValue(args->data[1]);
            int backlog = integerValue(args->data[2]);
            int rc = listen(sockfd, backlog);
            returnedValue = newInteger(rc);
            break;
        }

        case 50: {   /* accept */
            int sockfd = integerValue(args->data[1]);
            int newsockfd = accept(sockfd, NULL, NULL);
            returnedValue = newInteger(newsockfd);
            break;
        }

        case 60: {   /* connect */
            int sockfd = integerValue(args->data[1]);
            struct byteObject *addrObj = (struct byteObject *) args->data[2];
#if defined(DEBUG) && defined(TRACE)
            struct sockaddr_in *p = (struct sockaddr_in*) addrObj->bytes;
printf("before connect()\n");
printf("    sockfd=%d\n", sockfd);
printf("    family=%d\n", p->sin_family);
printf("    port=%d\n", ntohs(p->sin_port));
printf("    addr=%s\n", inet_ntoa(p->sin_addr));
#endif
            int rc = connect(sockfd, (struct sockaddr *) addrObj->bytes, SIZE(addrObj));
#if defined(DEBUG) && defined(TRACE)
printf("connect() returns %d\n", rc);
#endif
            returnedValue = newInteger(rc);
            break;
        }

        case 70: {   /* send */
            int sockfd = integerValue(args->data[1]);
            struct byteObject *bufObj = (struct byteObject *) args->data[2];
            int n = integerValue(args->data[3]);
            if ((LstUInt) n > SIZE(bufObj)) {
                *failed = 1;
                break;
            }
            ssize_t sent = send(sockfd, bufObj->bytes, n, 0);
            returnedValue = newInteger(sent);
            break;
        }

        case 71: {   /* sendto */
            int sockfd = integerValue(args->data[1]);
            struct byteObject *bufObj = (struct byteObject *) args->data[2];
            int n = integerValue(args->data[3]);
            if ((LstUInt) n > SIZE(bufObj)) {
                *failed = 1;
                break;
            }
            struct byteObject *addrObj = (struct byteObject *) args->data[4];
            ssize_t sent = sendto(sockfd, bufObj->bytes, n, 0,
                    (struct sockaddr *) addrObj->bytes, SIZE(addrObj));
            returnedValue = newInteger(sent);
            break;
        }

        case 80: {   /* recv */
            int sockfd = integerValue(args->data[1]);
            struct byteObject *bufObj = (struct byteObject *) args->data[2];
            int n = integerValue(args->data[3]);
            if ((LstUInt) n > SIZE(bufObj)) {
                *failed = 1;
                break;
            }
            ssize_t got = recv(sockfd, bufObj->bytes, n, 0);
            returnedValue = newInteger(got);
            break;
        }

        case 81: {  /* recvfrom */
            int sockfd = integerValue(args->data[1]);
            struct byteObject *bufObj = (struct byteObject *) args->data[2];
            int n = integerValue(args->data[3]);
            if ((LstUInt) n > SIZE(bufObj)) {
                *failed = 1;
                break;
            }
            struct sockaddr_storage ss;
            socklen_t len = sizeof(ss);
            ssize_t got = recvfrom(sockfd, bufObj->bytes, n, 0,
                    (struct sockaddr *) &ss, &len);
            /* TODO , how to return both ssize_t and peer address? */
            returnedValue = newInteger(got);
            break;
        }

        case 90: {   /* getsockopt */
            /*
            int sockfd = integerValue(args->data[1]);
            int level = integerValue(args->data[2]);
            int optname = integerValue(args->data[3]);
            */
            // TODO
            break;
        }

        case 91: {   /* setsockopt */
            // TODO
            break;
        }

        case 92: {  /* select on a single fd */
#define LST_WAIT_R 1
#define LST_WAIT_W 2
            int fd = integerValue(args->data[1]);
            int waitfor = integerValue(args->data[2]);
            struct timeval timeout, *p = NULL;
            if (SIZE(args) > 3) {
                int timeout_in_milliseconds = integerValue(args->data[3]);
                timeout.tv_sec = (timeout_in_milliseconds) / 1000;
                timeout.tv_usec = (timeout_in_milliseconds) % 1000 * 1000;
                p = &timeout;
            }
            fd_set rfds, wfds, *rp = NULL, *wp = NULL;
            if (waitfor & LST_WAIT_R) { FD_ZERO(&rfds); FD_SET(fd, &rfds); rp = &rfds; }
            if (waitfor & LST_WAIT_W) { FD_ZERO(&wfds); FD_SET(fd, &wfds); wp = &wfds; }
            int rc = select(fd + 1, rp, wp, NULL, p);
            if (rc > 0) {
                rc = 0;
                if ((waitfor & LST_WAIT_R) && FD_ISSET(fd, &rfds)) rc |= LST_WAIT_R;
                if ((waitfor & LST_WAIT_W) && FD_ISSET(fd, &wfds)) rc |= LST_WAIT_W;
            }
#undef LST_WAIT_R
#undef LST_WAIT_W
            returnedValue = newInteger(rc);
            break;
        }

        case 93: {  /* getsockname */
            struct sockaddr_storage ss;
            socklen_t len = sizeof(ss);
            int sockfd = integerValue(args->data[1]);
            if (getsockname(sockfd, (struct sockaddr *) &ss, &len) >= 0)
                returnedValue = lstNewBinary(&ss, len);
            break;
        }

        case 94: {  /* getpeername */
            struct sockaddr_storage ss;
            socklen_t len = sizeof(ss);
            int sockfd = integerValue(args->data[1]);
            if (getpeername(sockfd, (struct sockaddr *) &ss, &len) >= 0)
                returnedValue = lstNewBinary(&ss, len);
            break;
        }

    }
    return returnedValue;
}

struct object *
lstInetPrimitive(int primitiveNumber, struct object *args, int *failed)
{
    struct object *returnedValue = nilObject;
    switch (integerValue(args->data[0])) {

        case 0: {   /* Inet firstIpStringFromHostname: hostName -> IPv4 address string */
            char hostname[256];
            lstGetString(hostname, sizeof(hostname), args->data[1]);
            struct hostent *h = gethostbyname(hostname);
            if (h && h->h_addrtype == AF_INET) {
                char addr[16];
                inet_ntop(h->h_addrtype, h->h_addr_list[0], addr, sizeof(addr));
                returnedValue = lstNewString(addr);
            }
            break;
        }

        case 1: {   /* Inet firstIpAddressFromHostname: hostName -> IPv4 in_addr binary */
            char hostname[256];
            lstGetString(hostname, sizeof(hostname), args->data[1]);
            struct hostent *h = gethostbyname(hostname);
            if (h && h->h_addrtype == AF_INET)
                returnedValue = lstNewBinary(h->h_addr_list[0], h->h_length);
            break;
        }

        case 2: {   /* Inet newIpAddressFromIpString: ipStr -> IPv4 in_addr binary */
            char ipStr[16];
            lstGetString(ipStr, sizeof(ipStr), args->data[1]);
            struct in_addr addr;
            int rc = inet_aton(ipStr, &addr);
            if (1 == rc)
                returnedValue = lstNewBinary(&addr, sizeof(addr));
            break;
        }

        case 3: {   /* Inet newSocketAddressFromIpAddress: in_addr port: port -> socketaddr_in binary */
            struct byteObject *addrObj = (struct byteObject *) args->data[1];
            struct sockaddr_in in;
            in.sin_family = AF_INET;
            in.sin_port = htons(integerValue(args->data[2]));
            memcpy(&in.sin_addr, addrObj->bytes, SIZE(addrObj));
            returnedValue = lstNewBinary(&in, sizeof(in));
            break;
        }

        case 4: {   /* Inet newSocketAddressFromIpString: ipStr port: port -> socketaddr_in binary */
            char ipStr[16];
            lstGetString(ipStr, sizeof(ipStr), args->data[1]);
            struct in_addr addr;
            int rc = inet_aton(ipStr, &addr);
            if (1 == rc) {
                struct sockaddr_in in;
                in.sin_family = AF_INET;
                in.sin_port = htons(integerValue(args->data[2]));
                memcpy(&in.sin_addr, &addr, sizeof(addr));
                returnedValue = lstNewBinary(&in, sizeof(in));
            }
            break;
        }

        case 5: {   /* Inet newSocketAddressFromHostname: host port: port -> socketaddr_in binary */
            char hostname[256];
            lstGetString(hostname, sizeof(hostname), args->data[1]);
            struct hostent *h = gethostbyname(hostname);
            if (h && h->h_addrtype == AF_INET) {
                struct sockaddr_in in;
                in.sin_family = AF_INET;
                in.sin_port = htons(integerValue(args->data[2]));
                memcpy(&in.sin_addr, h->h_addr_list[0], h->h_length);
                returnedValue = lstNewBinary(&in, sizeof(in));
            }
            break;
        }

    }
    return returnedValue;
}

#endif /* LST_ON_WINDOWS */
