#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <fcntl.h>

int
main(int argc, char **argv)
{
	int fd;
	struct termios ti;
	int i;
	unsigned char c;
	int col;
	unsigned char ca[64];
	int ln = 16;

	if (argc <= 2) {
		fprintf(stderr, "need tty name\n");
		exit(-1);
	}
	if (argc == 3) {
		ln = atoi(argv[2]);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror(argv[1]);
		exit(-2);
	}
	if (tcgetattr(fd, &ti) < 0) {
		perror("gettermios");
		exit(-2);
	}
	cfsetspeed(&ti, B115200);
	cfmakeraw(&ti);
	if (tcsetattr(fd, TCSANOW, &ti) < 0) {
		perror("settermios");
		exit(-3);
	}

	col = 0;

	while (1) {
		int bytes;
		fd_set infds;

		FD_ZERO(&infds);
		FD_SET(0, &infds);
		FD_SET(fd, &infds);

		if (select(fd+1, &infds, 0, 0, 0) < 0) {
			perror("select");
			exit(-5);
		} 
		if (ioctl(0, FIONREAD, &bytes) < 0) {
			perror("FIONREAD");
			exit(-4);
		}
		if (bytes > 0) {
			char trash[1000];

			read(0, trash, 100);
			system("clear");
		}
		if (ioctl(fd, FIONREAD, &bytes) < 0) {
			perror("FIONREAD");
			exit(-4);
		}
		if (bytes == 0) continue;

		i = read(fd, &c, 1);
		if (i != 1) continue;			
		ca[col] = c;
		col++;
		if ((c == 0x0a) || (col == (ln - 1))) {
			for (i = 0; i < ln; i++) {
				if (i < col) {
					printf("%02x ", ca[i]);
				} else {
					printf("   ");	
				}
			}
			printf("| ");
			for (i = 0; i < ln; i++) {
				if (i < col) {
					c = ca[i];
					if ((c < ' ') || (c > 0x7e)) c = '.';
					printf("%c", c);
				} else {
					printf(" ");	
				}
			}
			col = 0;
			printf("\n");
		}
	}
}

