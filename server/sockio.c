/*
 * we support socket based networking over TCP if the port name
 * is of the form: ":<integer>".  we ignore speed.
 * each client machine gets its own server instance on the same port,
 * which are handled by forking a new child server every time a connection
 * is accepted.
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/select.h>

#include "sockio.h"
#include "main.h"

extern int portnum;
extern int _debug;

/*----------------------------------------------------------------------*/

static int _fdo = -1;
static int _fdi = -1;
static char *_sdev = NULL;
static int _stdio = 0;

void
reaper()
{
    int pid;
    int wstat;

    pid = wait3(&wstat, WNOHANG, (void *)0);
    return;
}

int
socket_open(char *sdev)
{
    struct sockaddr_in laddr, connaddr;
    int cfd;
    int off = 0;
    static char sbuf[30];

    if (*sdev != ':') {
        return -1;
    }
    sdev++;
    portnum = atoi(sdev);

    signal(SIGCHLD, reaper);

	sprintf(sbuf, "socket:%d\n", portnum);
	sdev = sbuf;
    if ((_fdo = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("create stream socket");
        return -1;
    }
    off = 1;
    if (setsockopt(_fdo, IPPROTO_TCP, TCP_NODELAY, (char *) &off, sizeof(int)) < 0) {
        perror("setsockopt TCP_NODELAY");
        return -1;
    }

    off = 1;
    if (setsockopt(_fdo, SOL_SOCKET, SO_REUSEADDR, (char *) &off, sizeof(int)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        return -1;
    }

    /* bind the socket */
    laddr.sin_family = AF_INET;
    laddr.sin_addr.s_addr = htonl(INADDR_ANY);
    laddr.sin_port = htons(portnum);
    if (bind(_fdo, (struct sockaddr *)&laddr, sizeof(laddr)) != 0) {
        perror(sdev);
        return -1;
    }

    /* now listen, and listen good. */
    if (listen(_fdo, 5) != 0) {
        perror("listen");
        return -1;
    }

    /*
     * the parent accept loop sits here forever.
     *
     * only the child process will return from this function, 
     * one for each client connection.
     */
    while (1) {
        int child;
        int salen;

        salen = sizeof(connaddr);

        cfd = accept(_fdo, (struct sockaddr *)&connaddr, &salen);
        if (cfd == -1) {
            perror("accept");
            return -1;
        }
        if (_debug & DEBUG_MISC) {
            printf("connection from %s\n", inet_ntoa(connaddr.sin_addr));
        }
        child = fork();
        if (child == 0) {
            _fdi = _fdo = cfd;
            return 0;
        }
        close(cfd);
    }
    exit(0);
}

int
socket_close()
{
	int retc;

	if (_fdo == -1) {
		errno = EBADF;
		return -1;
	}

	retc = close(_fdo);
	_fdo = -1;

	return retc;
}

int
socket_send(char *buf, int len)
{
    int i;
    unsigned char *ubuf = (unsigned char *)buf;

    if (_fdo < 0)
		return -1;
    if (_debug & DEBUG_LOW) {
        fprintf(stderr, "sockio_send len: %d\n", len);
        fflush(stderr);
    }
	if (len < 0)
		len = strlen(buf);

    if ((ubuf[4] + 6) != len) {
        printf("--------------------------------request %x does not match length %x!\n",
            ubuf[4]+6, len);
        printf("%x %x %x %x %d %d\n", ubuf[0], ubuf[1], ubuf[2], ubuf[3], ubuf[4], len);
    }
	if ((i = send(_fdo, buf, len, 0)) == len) {
		return 0;
    }
    if (_debug & DEBUG_LOW) {
        fprintf(stderr, "sockio_send only sent: %d\n", i);
        fflush(stderr);
    }
	return -1;
}

/*
 * wait until we get at least min bytes
 */
void
sock_wait(int min)
{
    fd_set ins;
    int i, n;

    while (1) {
        FD_ZERO(&ins);
        FD_SET(_fdi, &ins);

        i = select(_fdi+1, &ins, 0, 0, 0);
        if (i < 0) {
            fprintf(stderr, "sockio_select returns %d\n", i);
            perror("select");
            continue;
        }
        i = ioctl(_fdi, FIONREAD, &n);
        if (i < 0) {
            fprintf(stderr, "sockio fionread returns %d\n", i);
            perror("ioctl");
            continue;
        }
        if (n >= min) return;
    }
}

/*
 * we should be able to read the entire request without timeouts.
 * if any of the intermediate characters time out, we have a problem.
 */
int
socket_receive(char *buf, int len)
{
	int i, n, t, need;

	if (_fdo < 0)
		return -1;

    if (_debug & DEBUG_LOW) {
        fprintf(stderr, "sockio_receive len: %d", len);
    }

    /*
     * due to the vagaries of TCP, we could get a short read.
     * we ain't playing that shit. 
     * wait for and read the header. 
     * find out how much payload, and then wait for and read that.
     */
    sock_wait(5);
    n = recv(_fdi, buf, 5, 0);
    if (n != 5) {
            fprintf(stderr, "sockio header read returns %d\n", n);
            return -1;
    }
    need = buf[4] + 1;
    sock_wait(need);
	n = recv(_fdi, &buf[5], need, 0);
    if (n != need) {
            fprintf(stderr, "sockio payload read returns %d expected %d\n", n, need);
            return -1;
	}

    n += 5;

    if (_debug & DEBUG_LOW) {
        fprintf(stderr, " ret %d\n", n);
    }
	return n;
}

/*
 * vim: tabstop=4 shiftwidth=4 expandtab:
 */

