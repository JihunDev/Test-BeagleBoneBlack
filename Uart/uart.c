#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>

#define BAUDRATE B9600     
#define MODEMDEVICE "/dev/ttyO4"
   
int main()
{
  int fd, ret;
  struct termios oldtio,newtio;
  char *temp = "123.28281 ";
  ret = system ("echo config-pin overlay BB-UART4"); /* System BB-UART4 Uesd Script */
	if(ret < 0){
		perror("BB-UART4 pin Fail");
    return 0;
	} 
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd <0) {
    perror(MODEMDEVICE); 
    return 0;
  }

  fcntl(fd, F_SETOWN, getpid());
  fcntl(fd, F_SETFL, FASYNC);

  tcgetattr(fd,&oldtio); 
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;

  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);

  int i;
  while(1){
    i = write(fd, temp, strlen( temp));
    printf("\tWrite %d",i );
    usleep(1000);
  }
  
  tcsetattr(fd,TCSANOW,&oldtio);
  
  return 0;
}
