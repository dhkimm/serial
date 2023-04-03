#include "serial_port.h"
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/ioctl.h>


#define printf(...)    { printf(__VA_ARGS__); fflush(stdout); }

Serial_port::Serial_port(char *dev_name_, int baud_) :
	time_to_exit(0),
	dev_name(dev_name_),
	baud(baud_),
	fd_r(-1), fd_w(-1)
{
}

Serial_port::~Serial_port()
{
	handle_quit();
}

bool Serial_port::init()
{
	printf("trying to open serial %s , %d\n", dev_name, baud);
	bool result = open_serial();

	if(result == false)
	{
		printf("serial open error\n");
		handle_quit();
		return result;
	}

	usleep(1000);

	tcflush(fd_r,TCIFLUSH);
	tcflush(fd_w,TCOFLUSH);

	return result;
}

void Serial_port::handle_quit()
{
	time_to_exit = 1;
	int err;
	if (fd_r > 0)
	{
		err = close(fd_r); fd_r = -1;
		if(err != 0) printf("%s:: serial READ fd close err code = %d\n\n", dev_name, err);
	}
	if (fd_w > 0)
	{
		err = close(fd_w); fd_w = -1;
		if(err != 0) printf("%s:: serial WRITE fd close err code = %d\n\n", dev_name, err);
	}

	return;
}

bool Serial_port::open_serial()
{
	fd_r = open(dev_name, O_RDONLY | O_NOCTTY );

	int cnt = 0;

	while ( fd_r == -1 && cnt <3 )
	{
		printf("failure, could not open port. %s %d \n", dev_name,errno);
		usleep(1000000);
		fd_r = open(dev_name, O_RDONLY | O_NOCTTY );
		cnt++;
	}
	if(cnt==3) return false;

	fcntl(fd_r, F_SETFL, 0);

	bool success = _setup_port(fd_r);

	while (!success && !time_to_exit)
	{
		printf("failure, could not configure port.\n");
		usleep(1000000);
		success = _setup_port(fd_r);
	}

	printf("\nConnected to READ %s with %d baud, 8 data bits, no parity, 1 stop bit (8N1)\n", dev_name, baud);

	fd_w = open(dev_name, O_WRONLY | O_NOCTTY | O_NONBLOCK | O_SYNC );

	cnt = 0;
	while ( fd_w == -1 && cnt <3 )
	{
		printf("failure, could not open port. %s %d \n", dev_name,errno);
		usleep(1000000);
		fd_w = open(dev_name, O_WRONLY | O_NOCTTY | O_NONBLOCK | O_SYNC );
		cnt++;
	}
	if(cnt==3) return false;

	fcntl(fd_w, F_SETFL, 0);

	success = _setup_port(fd_w);

	while (!success && !time_to_exit)
	{
		printf("failure, could not configure port.\n");
		usleep(1000000);
		success = _setup_port(fd_w);
	}

	printf("\nConnected to WRITE %s with %d baud, 8 data bits, no parity, 1 stop bit (8N1)\n\n", dev_name, baud);

	return true;
}

bool Serial_port::_setup_port(int &fd)
{
	if (!isatty(fd))
	{
		printf("\nERROR: file descriptor %d is NOT a serial port\n", fd);
		return false;
	}

	struct termios config;
	if (tcgetattr(fd, &config) < 0)
	{
		printf("\nERROR: could not read configuration of fd %d\n", fd);
		return false;
	}

	config.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | ICRNL | IXON | IGNCR );
	//config.c_iflag &= ~(IGNBRK | BRKINT | PARMRK| ISTRIP | INLCR | ICRNL | IXON | INPCK );
	config.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= (CLOCAL | CREAD);
	config.c_cflag |= CS8;
	config.c_cc[VMIN] = 0;
	config.c_cc[VTIME] = 5;

	switch (baud)
	{
	case 1200:
		if (cfsetispeed(&config, B1200) < 0 || cfsetospeed(&config, B1200) < 0)
		{
			printf("\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 1800:
		cfsetispeed(&config, B1800);
		cfsetospeed(&config, B1800);
		break;
	case 9600:
		cfsetispeed(&config, B9600);
		cfsetospeed(&config, B9600);
		break;
	case 19200:
		cfsetispeed(&config, B19200);
		cfsetospeed(&config, B19200);
		break;
	case 38400:
		if (cfsetispeed(&config, B38400) < 0 || cfsetospeed(&config, B38400) < 0)
		{
			printf("\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 57600:
		if (cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0)
		{
			printf("\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 115200:
		if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0)
		{
			printf("\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 230400:
		if (cfsetispeed(&config, B230400) < 0 || cfsetospeed(&config, B230400) < 0)
		{
			printf("\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;

		// These two non-standard (by the 70'ties ) rates are fully supported on
		// current Debian and Mac OS versions (tested since 2010).
	case 460800:
		if (cfsetispeed(&config, 460800) < 0 || cfsetospeed(&config, 460800) < 0)
		{
			printf("\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	case 921600:
		if (cfsetispeed(&config, 921600) < 0 || cfsetospeed(&config, 921600) < 0)
		{
			printf("\nERROR: Could not set desired baud rate of %d Baud\n", baud);
			return false;
		}
		break;
	default:
		printf("ERROR: Desired baud rate %d could not be set, aborting.\n", baud);
		return false;
		break;
	}

	if (tcsetattr(fd, TCSAFLUSH, &config) < 0)
	{
		printf("\nERROR: could not set configuration of fd %d\n", fd);
		return false;
	}

	return true;
}


