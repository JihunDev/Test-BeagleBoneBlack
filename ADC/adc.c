#include<stdio.h>
#include<sys/fcntl.h>

int adc_read(unsigned int pin);

#define MAX_BUF 64

int main() {
	int v0,v1,v2,v3,v4,v5;
	int ret;

	ret = system("echo 'BB-ADC' > /sys/devices/platform/bone_capemgr/slots");
	if(ret < 0){
		perror("BB-ADC Fail");
		exit(1);
	}

    while (1) {
	    v0 = adc_read(0); 
	    v1 = adc_read(1); 
	    v2 = adc_read(2);               
	    v3 = adc_read(3); 
	    v4 = adc_read(4); 
	    v5 = adc_read(5); 
   		printf("\n\nv0= %d\nv1= %d\nv2= %d\nv3= %d\nv4= %d\nv5= %d\n", v0, v1, v2, v3, v4, v5);
   		sleep(1);
    }
    return 0;
}

int adc_read(unsigned int pin) {
	int fd, len, j;
	char buf[MAX_BUF];
	char val[3];

	len = snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", pin);

	//printf("Scan of len = %d \n", len);
	//printf("Scan of buf = %s\n", buf);
	fd = open(buf, O_RDONLY);
	if (fd < 0) {
    	perror("adc/get-value");
	}

	read(fd, &val, 4);
	close(fd);
	//printf("value of ADC =%.4s \n", val);

	return atoi(&val);
}