/* I2C example to read the temperature and humidity from DHT20 module
* Written by Jeff Haddow using the book "Exploring Raspberry Pi" */

#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#include<time.h>
#define BUFFER_SIZE 7
#define WRITE_BUF_SIZE 4

int main(){
   int file, i;
	unsigned long dataTemp=7;
	unsigned long dataHumd=5;
	float temp, humd;
   if((file=open("/dev/i2c-1", O_RDWR)) < 0){
      printf("failed to open the bus\n");
      return 1;
   }
   if(ioctl(file, I2C_SLAVE, 0x38) < 0){
      printf("Failed to connect to the sensor\n");
      return 1;
   }
   char writeBuffer[WRITE_BUF_SIZE] = {0x18, 0xAC, 0x33, 0x00};
   char buf[BUFFER_SIZE];
   if(read(file, buf, 1)!=1){
      printf("Failed to read in the buffer\n");
      return 1;
   }
	i = 0;
	while(buf[0] != 0x18){
//		printf("The initial data is not 0x18\n");
		write(file, writeBuffer, 1);
		sleep(0.01);
		read(file, buf, 1);
		i++;
                if (i > 10){
			 printf("Failed to read in buffer ten times\n");
                         return 1;
                }

	}
	sleep(0.01);
   writeBuffer[0] = 0x70;
   if(write(file, writeBuffer, WRITE_BUF_SIZE)!=WRITE_BUF_SIZE){
      printf("Failed to request a reading of data\n");
      return 1;
   }
	sleep(0.08);
    if(read(file, buf, 1)!=1){
       printf("Failed to read in state byte\n");
       return 1;
    }
	i = 0;
	while((buf[0] & 0x01) != 0x00){
              read(file, buf, 1);
		i++;
		if (i > 10){
			printf("Failed to read in state byte ten times\n");
			return 1;
		}
              sleep(0.01);
	}
   if(read(file, buf, BUFFER_SIZE)!=BUFFER_SIZE){
      printf("Failed to read in the buffer\n");
      return 1;
   }
//   printf("The Data Bytes are %2X-%2X-%2X-%2X-%2X-%2X-%2X\n", 
//		buf[0], buf[1], buf[2], buf[3],	buf[4], buf[5], buf[6]);
	dataTemp = (buf[3] & 0x0F)*65536 + buf[4]*256 + buf[5];
	dataHumd = buf[1]*4096 + buf[2]*16 +(buf[3] & 0xF0)/16;
//	printf("DataTemp: %d \tDataHumd: %d\n",dataTemp, dataHumd);
	temp = (((float)dataTemp / 1048576) * 200) - 50;
	humd = (((float)dataHumd / 1048576) * 100);
	printf("Temperature: %4.4g\tRelative Humidity %4.4g%%\n", temp, humd);
   close(file);
   return 0;
}
