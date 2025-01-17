#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include <pthread.h>

class Serial_port
{

public:

	bool time_to_exit;

	Serial_port(char *dev_name, int baudrate);
	~Serial_port();

	bool init();

	bool debug;
	char *dev_name;
	int baud;

	bool open_serial();
	bool _setup_port(int &fd);

	void handle_quit();

	int  fd_r, fd_w;
};


#endif // SERIAL_PORT_H_
