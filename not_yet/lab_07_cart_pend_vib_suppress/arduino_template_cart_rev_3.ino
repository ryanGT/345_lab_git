// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69

//MPU6050 accelgyro(0x69); // <-- use for AD0 high
//MPU6050 accelgyro(0x68, &Wire1); // <-- use for AD0 low, but 2nd Wire (TWI/I2C) object


//int16_t gx, gy, gz;

#include <kraussserial.h>
#include <rtblockdiagram.h>
#include <DualMAX14870MotorShield.h>

// both the libraries above need to be installed on student computers
// - what is the most efficient way to accomplish that?
//     - install from zip using the Arduino IDE?

#define encoderPinA 2
#define encoderPinB 3

#define squarewave_pin 52

// define custom sensor class to work with MPU6050 az:
// - from my rtblockdiagram library
/* class sensor{ */
/*  public: */
/*   // pure virtual function */
/*   virtual int get_reading() = 0; */
/* }; */


// this will not be auto-generated
//class azaccel6050: public sensor{
//  // a valid sensor class must have a get_reading method with no
//  // inputs that returns an int (the sensor reading)
// public:
//  int16_t ax, ay, az;
//  MPU6050* accelgyro;
//
//  azaccel6050(MPU6050* myaccel){
//    accelgyro = myaccel;
//  }
//
//  int get_reading(){
//    accelgyro->getAcceleration(&ax, &ay, &az);
//    return(az);
//  }
//};
//
DualMAX14870MotorShield motors;

class cartmotors: public double_actuator{
  public:
   void stop_motors(){
      motors.setM1Speed(0);
      motors.setM2Speed(0);
   }

 
  void send_commands(int speed1, int speed2){
      motors.setM1Speed(speed1);
      motors.setM2Speed(speed2);
  }
};

// this might be auto-generated:
//azaccel6050 myaccel = azaccel6050(&accelgyro);

// this is the code I am seeking to autogenerate:
/* step_input u = step_input(0.5, 150); */
/* h_bridge_actuator HB = h_bridge_actuator(6, 4, 9);//in1, in2, pwm_pin */
/* encoder enc = encoder(11); */

/* void enc_isr_wrapper() { */
/*   enc.encoderISR(); */
/* } */

/* plant G = plant(&HB, &enc); */
/* summing_junction sum1 = summing_junction(&u, &G); */
/* PD_control_block PD = PD_control_block(3, 0.1, &sum1); */
/* saturation_block sat_block = saturation_block(&PD); */

//bdsysinitcode

//attachInterrupt(0, isr, FALLING);
int nISR;
int nIn;
int ISR_Happened;
int mypause = 1;
int ISRstate = 0;
int motor_speed, raw_motor_speed, vOut,error;
bool send_ser;

unsigned long t0;
unsigned long t;
unsigned long t1;
unsigned long cur_t;
float t_ms, t_sec, prev_t, dt;
float stop_t = 5.0;

void setup(){
   Serial.begin(230400);
   //bdsyswelcomecode
   Serial.println("using rtblockdiagram library");
   
   mynewline();

   motors.enableDrivers();
   motors.flipM1(true);

   
   pinMode(squarewave_pin, OUTPUT);

   // should any of this be auto-generated, or just included in the template?
   //!// encoder pin on interrupt 0 (pin 2)

   // here is the setup code I should autogenerate:
   //attachInterrupt(0, enc_isr_wrapper, RISING);
   //HB.setup();

   //bdsyssetupcode

   //=======================================================
   // set up the Timer1 interrupt
   //=======================================================
   TCCR1A = 0;     // set entire TCCR1A register to 0
   TCCR1B = 0;     // same for TCCR1B

   // set compare match register to desired timer count:
   //OCR1A = 15624;
   //OCR1A = 50;//100 Hz
   OCR1A = 125;//150ish - seems to work
   //OCR1A = 77;//200 Hz <-- seems very borderline (might be 184 Hz)
   //OCR1A = 30;//500 Hz
   //OCR1A = 15;//1000 Hz
   //OCR1A = 7;//2000 Hz


   // turn on CTC mode:
   TCCR1B |= (1 << WGM12);

   // Set CS10 and CS12 bits for 1024 prescaler:
   // or just CS12 to get 256
   //TCCR1B |= (1 << CS10);
   TCCR1B |= (1 << CS12);
 
   // enable timer compare interrupt:
   TIMSK1 |= (1 << OCIE1A);


   //sei();// re-enable global interrupts
   //=======================================================
   menu();
}


void menu(){
   // go into a menu loop where only `s` gets you out
   char mychar ='r';//default to reading the encoder once
  
   while (mychar != 's'){
      if (mychar == 'r'){
         //bdsysmenu3
      }
      else if (mychar == 'z'){
         //bdsysmenu2
      }
      Serial.println("enter r, s, or z:");
      Serial.println("- r: read the encoder");
      Serial.println("- s: start a test");
      Serial.println("- z: zero the encoder");
      mychar = get_char();
   }
   // we should only get here if the user
   // entered 's'
   //bdsysmenucode
    
   t0 = micros();
   prev_t = t0;
   ISR_Happened = 0;// clear flag and wait for next time step
   nISR = -1;
   Serial.print("t0 =");
   Serial.println(t0);
   //bdsyscsvlabels
   // reset encoders and t0 at the start of a test
   //enc.encoder_count = 0;
}

void loop(){
  if (ISR_Happened == 1){
    ISR_Happened = 0;
    cur_t = micros();
    t = cur_t-t0;
    if (t < 0){
      t += 65536;
    }
    t_ms = t/1000.0;
    t_sec = t_ms/1000.0;
    dt = t_sec - prev_t;

    if (t_sec > stop_t){
      G.send_commands(0,0);
      menu();
    }
    else{
      // here is the loop code I want to autogenerate:
      /* u.find_output(t_sec); */
      /* error = sum1.find_output(t_sec); */
      /* raw_motor_speed = PD.get_output(t_sec); */
      /* motor_speed = sat_block.get_output(t_sec); */
      /* G.send_command(motor_speed); */
  
      //bdsysloopcode
     
  
      //HB.send_command(motor_speed);
      // print data
      Serial.print(t_ms);
  
      //bdsysprintcode
  
      //Serial.print(",");
      //Serial.print(dt,8);
      //print_comma_then_int(u.get_output(t_sec));
      //print_comma_then_int(G.get_output(t_sec));
  
      // how do I decide what to print?
      // - all blocks or only some?
      //     - printing all can slow things down
      //     - printing all might be confusing
      // - call the get_output method?
      //     - do I ever need float output?
  
      /* print_comma_then_int(motor_speed); */
      /* print_comma_then_int(raw_motor_speed); */
      /* print_comma_then_int(PD.input_value); */
      /* print_comma_then_float(PD.din_dt); */
      /* print_comma_then_int(G.read_output()); */
  
  
      //Serial.print(",");
      //Serial.print(PD.prev_t,8);
      //Serial.print(",");
      //Serial.print(PD.dt,8);
  
      //print_comma_then_int(enc.get_reading());
      mynewline();
  
      prev_t = t_sec;
      //PD.save_values(t_sec);
     }
  }
}


ISR(TIMER1_COMPA_vect)
{
  ISR_Happened = 1;
  nISR++;

  //G.send_command(motor_speed);

  if (ISRstate == 1){
    ISRstate = 0;
    digitalWrite(squarewave_pin, LOW);
  }
  else{
    ISRstate = 1;
    digitalWrite(squarewave_pin, HIGH);
  }
}
