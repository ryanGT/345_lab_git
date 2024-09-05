#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <unistd.h>

// make device ID a commandline argument to 
// test each device
#define DEVICE_ID 0x07

uint32_t t0 = micros();
uint32_t t, t1, t2, t3, prev_t;
uint16_t dt, dt_send, dt_receive;
uint32_t send_total=0;
float ave_send;
int N=100;

#define in_bytes 8
#define out_bytes 7
char inArray[in_bytes];
char outArray[out_bytes];

uint8_t lsb, msb;
int myint, echo_int, expected_resp;
// compile cmd:
// g++ -o wiringpi_multi_byte_attempt1.o wiringpi_multi_byte_attempt1.c -lwiringPi -li2c

// performance cpu comand:
// echo performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor


//uint32_t t1, t2, dt;

void send_int(int fd, int int_in){
    msb = (uint8_t)(int_in/256);
    lsb = (uint8_t)int_in;
        
    outArray[0] = 5;//the i2c test case
    outArray[1] = msb;
    outArray[2] = lsb;
    // Send data to arduino
    //t1 = micros();
    write(fd, outArray, out_bytes);
    t2 = micros();
    //dt_send = t2-t1;
}

int read_int(int fd){
    read(fd, inArray, in_bytes);
    int my_echo_int = 256*inArray[6] + inArray[7];
    return(my_echo_int); 
}


int main (int argc, char **argv)
{
  printf("at top of main\n");
  // Setup I2C communication
    int fd = wiringPiI2CSetup(DEVICE_ID);
    if (fd == -1) {
        std::cout << "Failed to init I2C communication.\n";
        return -1;
    }
    std::cout << "I2C communication successfully setup.\n";

    //FILE * fp;

    //fp = fopen ("data.txt", "w");
    //fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);

    wiringPiSetup();
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(3, OUTPUT);
    printf("sending data\n");

    int q;
    int any_fail = 0;
    int num_fail = 0;
    // send 5, msb, lsb
    // get back msb, lsb, msb2, lsb2 in spots 4-7
    for (q=302; q< 500; q+=7){
        t1 = micros();
	send_int(fd, q);
        delay(100);
        echo_int = read_int(fd);
        expected_resp = q*10+1;

        if (expected_resp == echo_int){
            printf("echo int passed, q = %i\n", q);
        }
        else{
            printf("problem with echo int; expected %i, recieved %i\n", expected_resp, echo_int);
	    any_fail = 1;
	    num_fail +=1;
        }
        
	t2 = micros();
        dt_send = t2-t1;
        printf("dt_send (micros): %i\n", dt_send);
    }
    printf("any_fail = %i\n", any_fail);
    printf("num_fail = %i\n", num_fail);

   //fclose(fp);
    return 0;
}
