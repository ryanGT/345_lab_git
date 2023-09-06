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
#include <rpiblockdiagram.h>

void alarmWakeup(int sig_num);

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
int uno_fd;
int position;
#define in_bytes 6
#define out_bytes 5
uint8_t inArray[in_bytes];
uint8_t outArray[out_bytes];

uint8_t ilsb, imsb;
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

// mega i2c
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
    // bytes 1 and 2 are n_msb and n_lst
    outArray[3] = msb;
    outArray[4] = lsb;
    // Send data to arduino
    //t1 = micros();
    write(fd, outArray, out_bytes);
    t2 = micros();
    //dt_send = t2-t1;
}

int read_int(int fd){
    read(fd, inArray, in_bytes);
    int my_echo_int = 256*inArray[0] + inArray[1];
    return(my_echo_int); 
}




//bdsysinitcode


//uint32_t t1, t2, dt;
uint8_t servo_angle;

int main (int argc, char **argv)
{
  printf("at top of main\n");

   /*---------------------
   if (argc > 1){
	servo_angle = atoi(argv[1]);
   	printf("servo_angle = %d\n", servo_angle);
   }
   else{
	printf("defaulting to servo_angle = 0");
	servo_angle = 0;
   }
   -------------------------*/


  // Setup I2C communication
    uno_fd = wiringPiI2CSetup(MEGA_ID);
    if (uno_fd == -1) {
        std::cout << "Mega failed to init I2C communication.\n";
        return -1;
    }
    std::cout << "Mega I2C communication successfully setup.\n";
    printf("uno_fd: %i\n", uno_fd);
    //G_cart.set_fd(uno_fd);

    // i2c comm check
    int q;
    int any_fail = 0;
    int num_fail = 0;
 
    printf("i2c comm chec:k\n");

    for (q=302; q< 500; q+=175){
    	send_int(uno_fd, q);
        delay(1);
        echo_int = read_int(uno_fd);
        expected_resp = q*10+1;

        if (expected_resp == echo_int){
            printf("echo int passed, q = %i\n", q);
        }
        else{
            printf("problem with echo int; expected %i, recieved %i\n", expected_resp, echo_int);
	    any_fail = 1;
	    num_fail +=1;
        }
     }

    if (any_fail > 0){
	printf("i2c comm test failed, exiting\n");
	printf("you probably need to power cycle your Arduino\n");
        printf("or execute the command echo_performance.sh\n");
	return(-1);
    }

    FILE * fp;

    fp = fopen ("data.txt", "w");
    //bdsyscsvlabels

    wiringPiSetup();
//    // what are the wiringPi pin #'s?  in what "mode"?
//    // - using the command `gpio readall` leads me to the following
//    //   conclusions on wiringPi pin #'s:
//    //   - 0 ==> BCM 17, physical 11
//    //   - 1 ==> BCM 18, physical 12
//    //   - 3 ==> BCM 22, physical 15
//    pinMode(isr_sw_pin, OUTPUT);
//    pinMode(loop_sw_pin, OUTPUT);
//    pinMode(3, OUTPUT);
//    printf("sending data\n");
//
//
  //bdsyssetupcode


  signal(SIGALRM, alarmWakeup);   
  ualarm(2000, 2000);//500 



  //bdsysmenucode

  N = (int)(stop_t*500);

  printf("at top of for loop\n");

  t0 = micros();

  for (i=0;i<N;i++){
     // main control loop for test
     //
     // Adapt this for a for loop where we wait until
     // the flag has been set

     // wait for timer ISR to set flag"
     while (ISRHappened == 0){
         delayMicroseconds(50);
     }

     // timing pin high
     //digitalWrite(loop_sw_pin, 1);
     // clear flag
     ISRHappened = 0;
     cur_t = micros();
     //Serial.println(cur_t);
     t = cur_t-t0;
     if (t < 0){
       t += 65536;
     }
     t_ms = t/1000.0;
     t_sec = t_ms/1000.0;
 
     //bdsysloopcode
     
     	
     //bdsysprintcode
         
     
     //digitalWrite(loop_sw_pin, 0);
  }

  tf = micros();
  total_dt = tf - t0;
  loop_time_ms = total_dt/1000.0;
  printf("loop time (ms) = %0.2f\n", loop_time_ms);
      
  
//  G_cart.stop_motors();
//  delay(100);
//  G_cart.stop_motors();
 
  close(uno_fd);
  //fclose(fp);
  return 0;
}





void alarmWakeup(int sig_num)
{
  if(sig_num == SIGALRM)
    {
      ISRHappened = 1;
      nISR++;
      if (ISRstate == 1){
        ISRstate = 0;
      }
      else if (ISRstate == 0){
        ISRstate = 1;
      }
      digitalWrite(isr_sw_pin, ISRstate);
      //for(i=0; i<65535; i++); //do something
    }
}
