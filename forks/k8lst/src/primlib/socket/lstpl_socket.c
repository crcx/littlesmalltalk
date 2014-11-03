/*
 * The socket primitives of the LittleSmalltalk system
 *
 * ---------------------------------------------------------------
 * Little Smalltalk, Version 5
 *
 * Copyright (C) 1987-2005 by Timothy A. Budd
 * Copyright (C) 2007 by Charles R. Childers
 * Copyright (C) 2005-2007 by Danny Reinhold
 * Copyright (C) 2010 by Ketmar // Vampire Avalon
 *
 * ============================================================================
 * This license applies to the virtual machine and to the initial image of
 * the Little Smalltalk system and to all files in the Little Smalltalk
 * packages except the files explicitly licensed with another license(s).
 * ============================================================================
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "lstcore/k8lst.h"

#if defined(LST_USE_SOCKET) && 1 == LST_USE_SOCKET

#ifndef LST_ON_WINDOWS
# include <errno.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netdb.h>
# include <signal.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/tcp.h>
#else
# include <winsock.h>
# define SHUT_RD    0
# define SHUT_WR    1
# define SHUT_RDWR  2
# define MSG_NOSIGNAL  0
#endif

#define LST_WAIT_R 1
#define LST_WAIT_W 2


static const char *socketId = "SocketHandle";

typedef struct {
  const char *type;
  int fd;
} SocketInfo;


static void closeIt (int fd) {
  if (fd >= 0) {
    shutdown(fd, 2);
#ifdef LST_ON_WINDOWS
    closesocket(fd);
#else
    close(fd);
#endif
  }
}


LST_FINALIZER(lpSocketHandleFinalizer) {
  SocketInfo *fi = (SocketInfo *)udata;
  if (fi) {
    closeIt(fi->fd);
    free(fi);
  }
}


static lstObject *newSocketHandle (int fd) {
  SocketInfo *fi;
  if (fd < 0) return lstNilObj;
  lstObject *res = lstNewBinary(NULL, 0);
  fi = malloc(sizeof(SocketInfo));
  if (!fi) { closeIt(fd); return lstNilObj; }
  fi->type = socketId;
  fi->fd = fd;
  lstSetFinalizer(res, lpSocketHandleFinalizer, fi);
  return res;
}


static int getSocket (lstObject *o) {
  if (LST_IS_SMALLINT(o) || !LST_IS_BYTES(o) || LST_SIZE(o) || !o->fin || !o->fin->udata) return -1;
  SocketInfo *fi = (SocketInfo *)o->fin->udata;
  if (fi->type != socketId) return -2;
  return fi->fd;
}


LST_PRIMFN(lpSocketSocket) {
  int family, socketType, protocol, sockfd = -1, newsockfd, rc, how, backlog, n, waitfor;
  lstByteObject *addrObj, *bufObj;
  lstObject *res = lstNilObj;
  int op;
  if (LST_PRIMARGC < 1 || !LST_IS_SMALLINT(LST_PRIMARG(0))) return NULL;
  op = lstIntValue(LST_PRIMARG(0));
  if (op >= 20) {
    if (LST_PRIMARGC < 2) return NULL;
    if ((sockfd = getSocket(LST_PRIMARG(1))) < -1) return NULL;
    if (sockfd < 0 && (op != 28 && op != 29 && op != 95)) return NULL;
  }
  switch (op) {
    case 0: /* constants */
      if (LST_PRIMARGC < 2) return NULL;
      switch (lstIntValue(LST_PRIMARG(1))) {
        case 0: res = lstNewInt(AF_INET); break;
        case 1: res = lstNewInt(AF_INET6); break;
#ifndef LST_ON_WINDOWS
        case 2: res = lstNewInt(AF_LOCAL); break;
#endif
        case 10: res = lstNewInt(SOCK_STREAM); break;
        case 11: res = lstNewInt(SOCK_DGRAM); break;
        case 12: res = lstNewInt(SOCK_RAW); break;
        case 20: res = lstNewInt(IPPROTO_TCP); break;
        case 21: res = lstNewInt(IPPROTO_UDP); break;
#ifdef IPPROTO_SCTP
        case 22: res = lstNewInt(IPPROTO_SCTP); break;
#endif
        case 30: res = lstNewInt(SHUT_RD); break;
        case 31: res = lstNewInt(SHUT_WR); break;
        case 32: res = lstNewInt(SHUT_RDWR); break;
      }
      break;
    case 10: /* create socket */
      if (LST_PRIMARGC != 4) return NULL;
      family = lstIntValue(LST_PRIMARG(1));
      socketType = lstIntValue(LST_PRIMARG(2));
      protocol = lstIntValue(LST_PRIMARG(3));
      sockfd = socket(family, socketType, protocol);
      res = newSocketHandle(sockfd);
      break;
    case 11: /* create TCP socket */
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      res = newSocketHandle(sockfd);
      break;
    case 12: /* create UDP socket */
      sockfd = socket(AF_INET, SOCK_DGRAM, 0);
      res = newSocketHandle(sockfd);
      break;
    case 20: /* close */
      if (LST_PRIMARGC != 2) return NULL;
      closeIt(sockfd);
      ((SocketInfo *)(LST_PRIMARG(1)->fin->udata))->fd = -1;
      res = lstNewInt(0);
      break;
    case 21: /* shutdown */
      if (LST_PRIMARGC != 3) return NULL;
      how = lstIntValue(LST_PRIMARG(2));
      rc = shutdown(sockfd, how);
      res = lstNewInt(rc);
      break;
    case 28: /* getfd */
      if (LST_PRIMARGC != 2) return NULL;
      res = lstNewInt(sockfd);
      break;
    case 29: /* setfd */
      if (LST_PRIMARGC != 3) return NULL;
      if (!LST_IS_SMALLINT(LST_PRIMARG(2))) return NULL;
      newsockfd = lstIntValue(LST_PRIMARG(2));
      ((SocketInfo *)(LST_PRIMARG(1)->fin->udata))->fd = newsockfd;
      res = lstNewInt(sockfd);
      break;
    case 30: /* bind */
      if (LST_PRIMARGC != 3) return NULL;
      addrObj = (lstByteObject *)LST_PRIMARG(2);
      rc = bind(sockfd, (struct sockaddr *)addrObj->bytes, LST_SIZE(addrObj));
      res = lstNewInt(rc);
      break;
    case 40: /* listen */
      if (LST_PRIMARGC < 2) return NULL;
      if (LST_PRIMARGC >= 3) {
        backlog = lstIntValue(LST_PRIMARG(2));
      } else {
        backlog = SOMAXCONN;
      }
      rc = listen(sockfd, backlog);
      res = lstNewInt(rc);
      break;
    case 50: /* accept */
      if (LST_PRIMARGC != 2) return NULL;
      newsockfd = accept(sockfd, NULL, NULL);
      /*printf("ACCEPT:\n  fd=%d\n  nfd=%d\n", sockfd, newsockfd);*/
      res = lstNewInt(newsockfd);
      break;
    case 60: /* connect */
      if (LST_PRIMARGC != 3) return NULL;
      addrObj = (lstByteObject *)LST_PRIMARG(2);
#if 0 || (defined(DEBUG) && defined(TRACE))
      {
        struct sockaddr_in *p = (struct sockaddr_in *)addrObj->bytes;
        printf("before connect()\n");
        printf("    sockfd=%d\n", sockfd);
        printf("    family=%d\n", p->sin_family);
        printf("    port=%d\n", ntohs(p->sin_port));
        printf("    addr=%s\n", inet_ntoa(p->sin_addr));
      }
#endif
      rc = connect(sockfd, (struct sockaddr *)addrObj->bytes, LST_SIZE(addrObj));
#if 0 || (defined(DEBUG) && defined(TRACE))
      printf("connect() returns %d\n", rc);
      if (rc < 0) printf("errno=%d\n", errno);
#endif
      res = lstNewInt(rc);
      break;
    case 70: /* send */
      if (LST_PRIMARGC != 4) return NULL;
      bufObj = (lstByteObject *)LST_PRIMARG(2);
      n = lstIntValue(LST_PRIMARG(3));
      if ((LstUInt)n > LST_SIZE(bufObj)) {
        return NULL;
      } else {
        ssize_t sent = send(sockfd, (void *)bufObj->bytes, n, MSG_NOSIGNAL);
        res = lstNewInt(sent);
      }
      break;
    case 71: /* sendto */
      if (LST_PRIMARGC != 5) return NULL;
      bufObj = (lstByteObject *)LST_PRIMARG(2);
      n = lstIntValue(LST_PRIMARG(3));
      if ((LstUInt) n > LST_SIZE(bufObj)) {
        return NULL;
      } else {
        addrObj = (lstByteObject *)LST_PRIMARG(4);
        ssize_t sent = sendto(sockfd, (void *)bufObj->bytes, n, MSG_NOSIGNAL, (struct sockaddr *)addrObj->bytes, LST_SIZE(addrObj));
        res = lstNewInt(sent);
      }
      break;
    case 80: /* recv */
      if (LST_PRIMARGC != 4) return NULL;
      bufObj = (lstByteObject *)LST_PRIMARG(2);
      n = lstIntValue(LST_PRIMARG(3));
      if ((LstUInt) n > LST_SIZE(bufObj)) {
        res = lstNewInt(-1);
      } else {
        ssize_t got = recv(sockfd, (void *)bufObj->bytes, n, 0);
        res = lstNewInt(got);
      }
      break;
    case 81: /* recvfrom */
      if (LST_PRIMARGC != 4) return NULL;
      bufObj = (lstByteObject *)LST_PRIMARG(2);
      n = lstIntValue(LST_PRIMARG(3));
      if ((LstUInt) n > LST_SIZE(bufObj)) {
        return NULL;
      } else {
#ifndef LST_ON_WINDOWS
        struct sockaddr_storage ss;
        socklen_t len = sizeof(ss);
        ssize_t got = recvfrom(sockfd, (void *)bufObj->bytes, n, 0, (struct sockaddr *)&ss, &len);
#else
        struct sockaddr ss;
        int len = sizeof(ss);
        ssize_t got = recvfrom(sockfd, (void *)bufObj->bytes, n, 0, &ss, &len);
#endif
        /*TODO: how to return both ssize_t and peer address? */
        res = lstNewInt(got);
      }
      break;
    case 90: /* getsockopt */
      /*
      int sockfd = lstIntValue(LST_PRIMARG(1));
      int level = lstIntValue(LST_PRIMARG(2));
      int optname = lstIntValue(LST_PRIMARG(3));
      */
      /*TODO*/
      return NULL;
      break;
    case 91: /* setsockopt */
      /*TODO*/
      return NULL;
      break;
    case 92: /* select on a single fd */
      if (LST_PRIMARGC >= 3) {
        /* fd cond [timeoutMs] */
        /* cond: 1: read; 2: write; 3: both */
        if (!LST_IS_SMALLINT(LST_PRIMARG(2))) return NULL;
        waitfor = lstIntValue(LST_PRIMARG(2));
        struct timeval timeout, *p = NULL;
        if (LST_PRIMARGC > 3) {
          if (!LST_IS_SMALLINT(LST_PRIMARG(3))) return NULL;
          int msec = lstIntValue(LST_PRIMARG(3));
          if (msec >= 0) {
            timeout.tv_sec = (msec)/1000;
            timeout.tv_usec = (msec)%1000*1000;
            p = &timeout;
          }
        }
        fd_set rfds, wfds, *rp = NULL, *wp = NULL;
        if (waitfor & LST_WAIT_R) { FD_ZERO(&rfds); FD_SET(sockfd, &rfds); rp = &rfds; }
        if (waitfor & LST_WAIT_W) { FD_ZERO(&wfds); FD_SET(sockfd, &wfds); wp = &wfds; }
        /*fprintf(stderr, "SELECT: fd=%d; to=%p\n", sockfd+1, p);*/
        int rc = select(sockfd+1, rp, wp, NULL, p);
        if (rc > 0) {
          rc = 0;
          if ((waitfor & LST_WAIT_R) && FD_ISSET(sockfd, &rfds)) rc |= LST_WAIT_R;
          if ((waitfor & LST_WAIT_W) && FD_ISSET(sockfd, &wfds)) rc |= LST_WAIT_W;
        }
        res = lstNewInt(rc);
      } else return NULL;
      break;
    case 93: /* getsockname */
      {
#ifndef LST_ON_WINDOWS
        struct sockaddr_storage ss;
        socklen_t len = sizeof(ss);
        if (getsockname(sockfd, (struct sockaddr *)&ss, &len) >= 0) res = lstNewBinary(&ss, len);
#else
        struct sockaddr ss;
        int len = sizeof(ss);
        if (getsockname(sockfd, &ss, &len) >= 0) res = lstNewBinary(&ss, len);
#endif
      }
      break;
    case 94: /* getpeername */
      {
#ifndef LST_ON_WINDOWS
        struct sockaddr_storage ss;
        socklen_t len = sizeof(ss);
        if (getpeername(sockfd, (struct sockaddr *)&ss, &len) >= 0) res = lstNewBinary(&ss, len);
#else
        struct sockaddr ss;
        int len = sizeof(ss);
        if (getpeername(sockfd, &ss, &len) >= 0) res = lstNewBinary(&ss, len);
#endif
      }
      break;
    case 95: /* select on a multiple fd */
      /* args: rArray wArray eArray [timeoutMs] */
      /* returns: patched arrays; for each item: boolean */
      /* return nil on error */
      /* return false if there is timeout */
      /* return true if anything happens */
      if (LST_PRIMARGC >= 4) {
        int maxFD = -1, f, c, any[3] = {0};
        lstObject *aa[3];
        for (c = 0; c < 3; ++c) {
          aa[c] = LST_PRIMARG(c+1);
          if (LST_CLASS(aa[c]) != lstArrayClass) aa[c] = NULL;
        }
        struct timeval timeout, *p = NULL;
        if (LST_PRIMARGC > 4) {
          if (!LST_IS_SMALLINT(LST_PRIMARG(4))) return NULL;
          int msec = lstIntValue(LST_PRIMARG(4));
          if (msec >= 0) {
            timeout.tv_sec = (msec)/1000;
            timeout.tv_usec = (msec)%1000*1000;
            p = &timeout;
          }
        }
        fd_set rfds, wfds, efds, *rp[3];
        rp[0] = &rfds; rp[1] = &wfds; rp[2] = &efds;
        for (c = 0; c < 3; ++c) {
          FD_ZERO(rp[c]);
          if (aa[c]) {
            for (f = 0; f < LST_SIZE(aa[c]); ++f) {
              int xfd = getSocket(aa[c]->data[f]);
              if (xfd >= 0) {
                /*fprintf(stderr, "c=%d; f=%d; fd=%d\n", c, f, xfd);*/
                if (xfd > maxFD) maxFD = xfd;
                FD_SET(xfd, rp[c]);
                any[c] = 1;
              }
            }
          }
          if (!any[c]) rp[c] = NULL;
        }
        if (maxFD < 0 && !p) return lstFalseObj;
        int rc = select(maxFD+1, rp[0], rp[1], rp[2], p);
        if (rc == 0) return lstFalseObj;
        if (rc < 0) return lstNilObj;
        for (c = 0; c < 3; ++c) {
          if (!aa[c]) continue;
          for (f = 0; f < LST_SIZE(aa[c]); ++f) {
            int xfd = getSocket(aa[c]->data[f]);
            aa[c]->data[f] = (xfd < 0 || !FD_ISSET(xfd, rp[c])) ? lstFalseObj : lstTrueObj;
          }
        }
        res = lstTrueObj;
      } else return NULL;
      break;
  }
  return res;
}


LST_PRIMFN(lpSocketInet) {
  if (LST_PRIMARGC < 1) return NULL;
  lstObject *res = lstNilObj;
  switch (lstIntValue(LST_PRIMARG(0))) {
    case 0: /* Inet firstIpStringFromHostname: hostName -> IPv4 address string */
      {
        if (LST_PRIMARGC != 2) return NULL;
        char hostname[256];
        lstGetString(hostname, sizeof(hostname), LST_PRIMARG(1));
        struct hostent *h = gethostbyname(hostname);
        if (h && h->h_addrtype == AF_INET) {
#ifndef LST_ON_WINDOWS
          char addr[16];
          inet_ntop(h->h_addrtype, h->h_addr_list[0], addr, sizeof(addr));
          res = lstNewString(addr);
#else
          struct in_addr addr;
          addr.s_addr = *(u_long *)h->h_addr_list[0];
          res = lstNewString(inet_ntoa(addr));
#endif
        }
      }
      break;
    case 1: /* Inet firstIpAddressFromHostname: hostName -> IPv4 in_addr binary */
      {
        if (LST_PRIMARGC != 2) return NULL;
        char hostname[256];
        lstGetString(hostname, sizeof(hostname), LST_PRIMARG(1));
        struct hostent *h = gethostbyname(hostname);
        if (h && h->h_addrtype == AF_INET) res = lstNewBinary(h->h_addr_list[0], h->h_length);
      }
      break;
    case 2: /* Inet newIpAddressFromIpString: ipStr -> IPv4 in_addr binary */
      {
        if (LST_PRIMARGC != 2) return NULL;
        char ipStr[64];
        lstGetString(ipStr, sizeof(ipStr), LST_PRIMARG(1));
        struct in_addr addr;
#ifndef LST_ON_WINDOWS
        int rc = inet_aton(ipStr, &addr);
        if (1 == rc) res = lstNewBinary(&addr, sizeof(addr));
#else
        addr.s_addr = inet_addr(ipStr);
        res = addr.s_addr==INADDR_NONE ? lstNilObj : lstNewBinary(&addr, sizeof(addr));
#endif
      }
      break;
    case 3: /* Inet newSocketAddressFromIpAddress: in_addr port: port -> socketaddr_in binary */
      {
        if (LST_PRIMARGC != 3) return NULL;
        lstByteObject *addrObj = (lstByteObject *)LST_PRIMARG(1);
        struct sockaddr_in in;
        in.sin_family = AF_INET;
        in.sin_port = htons(lstIntValue(LST_PRIMARG(2)));
        memcpy(&in.sin_addr, addrObj->bytes, LST_SIZE(addrObj));
        res = lstNewBinary(&in, sizeof(in));
      }
      break;
    case 4: /* Inet newSocketAddressFromIpString: ipStr port: port -> socketaddr_in binary */
      {
        if (LST_PRIMARGC != 3) return NULL;
        char ipStr[16];
        lstGetString(ipStr, sizeof(ipStr), LST_PRIMARG(1));
        struct in_addr addr;
#ifndef LST_ON_WINDOWS
        int rc = inet_aton(ipStr, &addr);
        if (1 == rc)
#else
        addr.s_addr = inet_addr(ipStr);
        if (addr.s_addr != INADDR_NONE)
#endif
        {
          struct sockaddr_in in;
          in.sin_family = AF_INET;
          in.sin_port = htons(lstIntValue(LST_PRIMARG(2)));
          memcpy(&in.sin_addr, &addr, sizeof(addr));
          res = lstNewBinary(&in, sizeof(in));
        }
      }
      break;
    case 5: /* Inet newSocketAddressFromHostname: host port: port -> socketaddr_in binary */
      {
        if (LST_PRIMARGC != 3) return NULL;
        if (!LST_IS_BYTES_EX(LST_PRIMARG(1))) return NULL;
        char hostname[256];
        lstGetString(hostname, sizeof(hostname), LST_PRIMARG(1));
        struct hostent *h = gethostbyname(lstGetStringPtr(LST_PRIMARG(1)));
        if (h && h->h_addrtype == AF_INET) {
          struct sockaddr_in in;
          if (!LST_IS_SMALLINT(LST_PRIMARG(2))) return NULL;
          in.sin_family = AF_INET;
          in.sin_port = htons(lstIntValue(LST_PRIMARG(2)));
          memcpy(&in.sin_addr, h->h_addr_list[0], h->h_length);
          res = lstNewBinary(&in, sizeof(in));
        }
      }
      break;
  }
  return res;
}


LST_PRIMCLEARFN(lpSocketClear) {
#ifdef LST_ON_WINDOWS
  WSACancelBlockingCall(); /* cancel any blocking calls */
  WSACleanup();
#endif
}


static const LSTExtPrimitiveTable primTbl[] = {
{"SocketSocket", lpSocketSocket, lpSocketClear},
{"SocketInet", lpSocketInet, NULL},
{0}};


void lstInitPrimitivesSocket (void) {
  lstRegisterExtPrimitiveTable(primTbl);
#ifdef LST_ON_WINDOWS
  WSADATA wsaData;
  WORD wVerReq = MAKEWORD(1, 1); /* version 1.1, for 2.0, use (2,0) */
  if (WSAStartup(wVerReq, &wsaData) != 0) return;
  /*winsockStarted = 1;*/
  /*atexit(lstSocketRelease);*/
#endif
}


#else
void lstInitPrimitivesSocket (void) {}
#endif
