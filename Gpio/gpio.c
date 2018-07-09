#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/fcntl.h>
#include<sys/poll.h>
#include<sys/types.h>
#include<sys/stat.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio" /**< Gpio Path **/
#define MAX_BUF 64						 /**< Road Value buff size */

#define GPIOEXPORT 0

/**
  @struct PIN_DIRCTION
  @brief InPut | OutPut Value
  */
typedef enum {
	INPUT_PIN = 0,
	OUTPUT_PIN = 1
} PIN_DIRECTION;

/**
  @struct PIN_VALUE
  @brief Low | High Value
  */
typedef enum {
	LOW = 0,
	HIGH = 1
} PIN_VALUE;

static int gpio_export(unsigned int gpio);
static int gpio_unexport(unsigned int gpio);
static int gpio_set_dir(unsigned int gpio, PIN_DIRECTION out_flag);
static int gpio_set_value(unsigned int gpio, PIN_VALUE value);
static int gpio_get_value(unsigned int gpio, unsigned int *value);
static int gpio_get_value_fd(int fd, unsigned int *value);
static int gpio_set_edge(unsigned int gpio, char *edge);
static int gpio_fd_open(unsigned int gpio);
static int gpio_fd_close(int fd);
static int gpio_no(int bank, int n);
static int gpio_read(int fd, unsigned int *value) ;

static char* kPollEdge_rising = "rising";
static char* kPollEdge_falling = "falling";
static char* kPollEdge_both = "both";

int main() {
	int f, n, i;
	struct pollfd poll_fds[1];
	char value[4];
	unsigned int gpio = gpio_no(0, 49);

#if GPIOEXPORT
	gpio_export(gpio);
#endif

	f = gpio_fd_open(gpio);
	if(f == -1){
		perror("Can`t Open GPIO");
		return 1;
	} else {
		printf("Open GPIO File ds\n");
	}
	
	if(gpio_set_edge(gpio, kPollEdge_rising) == -1){
		perror("Can`t Change edge");
		return 1;
	} else {
		printf("Change edge\n");
	}
	
	sleep(2);

	poll_fds[0].fd = f;
	poll_fds[0].events = POLLPRI | POLLERR;

	while(1){
		if(poll(poll_fds, 1, 1) > 0){
			n = read(f, &value, sizeof(value));
			printf("Interrupt read %d bytes, %c Value\n", n, value[0]);
		}
	}

	if(gpio_fd_close(f) == -1){
		perror("Can`t Close GPIO");
		return 1;
	}

#if GPIOEXPORT
	gpio_unexport(gpio);	
#endif

	return 0;
}

/**
  @fn int gpio_export(unsigned int gpio)
  @param gpio input number
  @return fd | 0
  */
int gpio_export(unsigned int gpio) {
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	printf("\n%d\n",fd);
	if(fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	
	return 0;
}

/**
  @fn int gpio_unexport(unsigned int gpio)
  @param gpio intput number
  @return fd | 0
  */
int gpio_unexport(unsigned int gpio) {
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if(fd < 0){
		perror("gpio/unexport");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

/**
  @fn int gpio_set_dir(unsigned int gpio, PIN_DIRECTION out_flag)
  @param gpio
  @param out_flag
  @retrun fd | 0
  */
int gpio_set_dir(unsigned int gpio, PIN_DIRECTION out_flag) {
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
	
	fd = open(buf, O_WRONLY);
	if(fd < 0) {
		perror("gpio/direction`");
		return fd;
	}

	if(out_flag == OUTPUT_PIN) {
		write(fd, "out", 4);
	} else {
		write(fd, "in", 3);
	}

	return 0;
}

/**
  @fn int gpio_set_value(unsigned int gpio, PIN_VALUE value)
  @param gpio
  @param value
  @return fd | 0
  */
int gpio_set_value(unsigned int gpio, PIN_VALUE value) {
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);
	if(fd < 0) {
		perror("gpio/set-vlaue");
		return fd;
	}

	if(value == LOW) {
		write(fd, "0", 2);
	} else {
		write(fd, "1", 2);
	}

	close(fd);

	return 0;
}

/**
  @fn int gpio_get_value(unsigned int gpio, unsigned int *value)
  @param gpio
  @param out_flag
  @return fd | 0
  */
int gpio_get_value(unsigned int gpio, unsigned int *value) {
	int fd;
	char buf[MAX_BUF];
	char ch;

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	
	fd = open(buf, O_RDONLY);
	if(fd < 0) {
		perror("gpio/get-vlaue");
		return fd;
	}

	read(fd, &ch, 1);
	
	if(ch != '0') {
		*value = HIGH;
	} else {
		*value = LOW;
	}
	
	close(fd);

	return 0;
}

/**
  @fn int gpio_get_value_fd(int fd, unsigned int *value)
  @param fd
  @param *value
  @return 0
  */
int gpio_get_value_fd(int fd, unsigned int *value) {
    char ch;
 
    read(fd, &ch, 1);
 
    if (ch != '0') {
        *value = 1;
    } else {
        *value = 0;
    }
 
    return 0;
}

/**
  @fn int gpio_set_edge(unsigned int gpio, char *edge)
  @param gpio
  @param *edge
  @retrun fd | 0
  */
int gpio_set_edge(unsigned int gpio, char *edge) {
	int fd;
	char buf[MAX_BUF];


	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);
	
	fd = open(buf, O_WRONLY);
	if(fd < 0) {
		perror("gpio/set-edge");
		return fd;
	}
	
	write(fd, edge, strlen(edge) + 1);
	close(fd);

	return 0;
}

/**
  @fn int gpio_fd_open(unsigend int gpio)
  @faram gpio
  @return fd
  */
int gpio_fd_open(unsigned int gpio)	{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	fd = open(buf, O_RDONLY | O_NONBLOCK);
	if(fd < 0) {
		perror("gpio/fd_open");
	}

	return fd;
}

/**
  @fn int gpio_fd_close(int fd)
  @return close(fd)
  */
int gpio_fd_close(int fd) {
	return close(fd);
}

/**
  @fn int gpio_no(int bank, int n)
  @param bank
  @param n
  @retrun bank*32+n
  */
int gpio_no(int bank, int n) {
	return bank * 32 + n;
}

/**
  @fn int gpio_read(int fd, unsigned int *value)
  @param fd
  @param *value
  @return ret | 0
  */
int gpio_read(int fd, unsigned int *value) {
    int ret;
    char ch; 

    lseek(fd, 0, SEEK_SET);

    ret = read(fd, &ch, 1);

    if (ret != 1) {
        fprintf(stderr, "Can't read GPIO %s\n", strerror(errno));
        return ret;
    }

    if (ch != '0') {
        *value = HIGH;
    } else {
        *value = LOW;
    }

    return 0;
}
