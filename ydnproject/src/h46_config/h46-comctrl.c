#include <h46-comctrl.h>

// 超时检查
int time_out(struct timeval tpstart)
{
	float timeuse;
	int timer;
	struct timeval tpend;
	gettimeofday(&tpend,NULL);
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+ (tpend.tv_usec-tpstart.tv_usec);
	timer = timeuse/=1000;
	return timer;
}

// control
int set_uart(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) {
		perror("Get setupSerial error !\n");
		return -1;
	}

	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch( nBits ) {
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch(nEvent) {
	case 'O':                     //奇校验
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E':                     //偶校验
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':                    //无校验
		//newtio.c_cflag &= ~PARENB;
		newtio.c_cflag &= ~PARENB;
		break;
	}

	switch( nSpeed ) {
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}

	if( nStop == 1 ) {
		newtio.c_cflag &=  ~CSTOPB;
	} else if ( nStop == 2 ) {
		newtio.c_cflag |=  CSTOPB;
	}

	newtio.c_cc[VTIME]  = 0;
	newtio.c_cc[VMIN] = 0;

	tcflush(fd,TCIFLUSH);

	if((tcsetattr(fd,TCSANOW,&newtio))!=0) {
		perror("com set error\n");
		return -1;
	}

	printf("set end!\n");
	return 0;
}

//open
int uart_open(int fd,int comport)
{

	if (comport==1) {
		fd = open( "/dev/ttyAL0", O_RDWR|O_NOCTTY|O_NDELAY|O_SYNC);
		if (-1 == fd) {
			perror("Can't Open Serial Port\n");
			return(-1);
		}

	} else if(comport==2) { //O_NOCTTY|O_NDELAY
		fd = open( "/dev/ttyAL1", O_RDWR|O_NOCTTY|O_NDELAY/*|O_NONBLOCK*/);
		if (-1 == fd) {
			perror("Can't Open Serial Port\n");
			return(-1);
		}
	}


	if(fcntl(fd, F_SETFL, 0)<0) {
		printf("fcntl failed!\n");
	}


//is uart open status
	if(isatty(STDIN_FILENO)==0) {
		printf("standard input is not a terminal device\n");
	}

	return fd;
}

// rcv
int uart_rcv_ok(int fd)
{

	int i;
	char rcv_buf;

	for(i = 0; i<1; i++) {
		read(fd,&rcv_buf,1);

	}
	if(rcv_buf == 75) {
		return 1;
	} else if(rcv_buf == 71) {
		return 2;
	}

	return 0;
}

// send
int uart_send(int fd,char *buf)
{
	int len = 0,i;

	len = strlen(buf);

	for(i = 0; i < len; i++) {

		if(write(fd,&buf[i],1)) {

		} else {

			printf("uart_send error \t\n");
			return -1;
		}
	}

	return 1;
}


int uart_rcv_str(int fd)
{

	int i;

	char *rcv_buf;

	rcv_buf = malloc(20*sizeof(char));

	read(fd,rcv_buf,20);

	for(i=0; i<20; i++)
		printf("%c \n",rcv_buf[i]);

	return 1;
}


