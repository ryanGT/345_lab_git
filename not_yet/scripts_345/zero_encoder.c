#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <unistd.h>

//void alarmWakeup(int sig_num);

int pin = 21;
int pin2 = 20;//look up a good choice here

int ISRHappened = 0;
int ISRstate = 0;
int nISR = 0;
int i;


#define MEGA_ID 0x07
#define UNO_ID 0x08

uint32_t t0, tf, total_dt;
float loop_time_ms;
uint32_t t, t1, t2, t3, prev_t, cur_t;
float t_ms, t_sec;
uint16_t dt, dt_send, dt_receive;
uint32_t send_total=0;
float ave_send;
int N=1000;
//int i_echo[N];
//int two_byte_response[N];
int enc_fd, mega_fd;
int position;
#define in_bytes 8
#define out_bytes 1
uint8_t inArray[in_bytes];
uint8_t outArray[out_bytes];
#define enc_bytes 2
uint8_t enc_array[enc_bytes];

uint8_t ilsb, imsb;
uint8_t calibrated;
float stop_t=5;
uint8_t lsb, msb;

float get_float(const char* var_name){
  float out;
  printf("enter %s:\n", var_name);
  scanf("%f", &out);
  return out;
}


// pin #'s
int isr_sw_pin = 0;
int loop_sw_pin = 1;

// compile cmd (old):
// compile cmd (old):
// g++ -o wiringpi_multi_byte_attempt1.o wiringpi_multi_byte_attempt1.c -lwiringPi -li2c

// performance cpu comand:
// echo performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

// compiling and linking command with rpi library:
//g++ -o wiringpi_line_following_i2c_control.o wiringpi_line_following_i2c_control.c rpiblockdiagram/rpiblockdiagram.o -lwiringPi -li2c


//uint8_t getsecondbyte(int input){
//  uint8_t output;
//  output = (uint8_t)(input >> 8);
//  return output;
//}
//

int reassemblebytes(uint8_t msb, uint8_t lsb){
  int output;
  output = (int)(msb << 8);
  output += lsb;
  return output;
}

// it feels like the file descriptors need to get assigned
// when main runs; this means that these i2c functions need
// an fd int that isn't constant and doesn't exist until main
// runs
//
// - or can they be global ints that default to zero and then get 
//   set inside main before they are used?
//   - is that better? 

int read_encoder_i2c(){
  int out;

  read(enc_fd, enc_array, enc_bytes);
  out = reassemblebytes(enc_array[0],enc_array[1]);
  if (out > 32000){
     out -= 65536;
  } 
  //out = 7;//not reading the encoder for now
  return(out);
}

// - addr is 0x07
// - byte 0 is always calibrated variable:
//    outArray[0] = calibrated;
//    outArray[1] = n_loop;
//    pos_lsb = (byte)position;
//    pos_msb = getsecondbyte(position);
//    outArray[2] = pos_msb;
//    outArray[3] = pos_lsb;
//    outArray[4] = n_msb;
//    outArray[5] = n_lsb;
//    dt_micro_lsb = (byte)dt_micro;
//    dt_micro_msb = getsecondbyte(dt_micro);
//    outArray[6] = dt_micro_msb;
//    outArray[7] = dt_micro_msb;
// # Approach:
//   - I need a buffer of 8 bytes
//   - a function that reads the sensor value should read to fill the buffer
//     and then use bytes 2 and 3
//   - a check_cal function should read the buffer and return byte 0


void print_comma_then_int(int myint){
    printf(",%i",myint);
}

void mynewline(){
    printf("\n");
}

int echo_int, expected_resp;

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

void read_encoder_N_times(int N){
    int q;
    int myreading;
    
    for (q=0; q<N; q++){
	myreading = read_encoder_i2c();
	printf("%d, %d\n", q, myreading);
        delay(50);
    }
}


//uint32_t t1, t2, dt;

int main (int argc, char **argv)
{
    printf("at top of main\n");

    wiringPiSetup();
  
    // Setup I2C communication
    enc_fd = wiringPiI2CSetup(UNO_ID);
    if (enc_fd == -1) {
        std::cout << "Encoder Uno failed to init I2C communication.\n";
        return -1;
    }
    std::cout << "Encoder Uno I2C communication successfully setup.\n";

    printf("enc_fd: %i\n", enc_fd);   	

    printf("Before zeroing encoder readings:\n");

    read_encoder_N_times(10);

    // sending anything to the Uno over i2c should trigger an
    // encoder zeroing

    outArray[0] = 250;
    //outArray[1] = 51;
    write(enc_fd, outArray, out_bytes);

    delay(100);

    printf("After zeroing encoder readings:\n");

    read_encoder_N_times(10);

   	

    return 0;
}
