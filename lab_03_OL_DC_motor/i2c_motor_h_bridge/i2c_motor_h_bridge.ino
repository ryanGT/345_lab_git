#include <kraussserial.h>
#include <math.h>
#include <Wire.h>
#include <rtblockdiagram.h>
// both the libraries above need to be installed on student computers
// - what is the most efficient way to accomplish that?
//     - install from zip using the Arduino IDE?

#define encoderPinA 2

int state;
int ISR_state;
int position;

bool print_data = false;

bool sent_data = false;
bool send_ser = false;
byte calibrated = 0;
int n, n_i2c;
int nISR=30000;
byte n_loop;
int amp;
float m=0.1;
int width;
int n_start = 10;
int v;
int stop_n = 200;// hard code this if you want to be lame

int i_received;
int myresponse;
byte myr_lsb, myr_msb;

h_bridge_actuator HB = h_bridge_actuator(6, 4, 9);//in1, in2, pwm_pin
encoder enc = encoder(11);

void enc_isr_wrapper() {
  enc.encoderISR();
}



unsigned long t0;
unsigned long prev_t_micros;
unsigned long t;
unsigned long t1;
unsigned long cur_t;
float t_ms, t_sec, prev_t, dt;

bool new_data;

int squarewave_pin = 12;
#define sendPin 11
#define receivePin 10
#define controlPin 9
#define isrPin 8

byte mypause;

//----------------------------
// i2c protocol
// inArray from RPi:
// 0: command byte
// 1: n_msb
// 2: n_lsb
// 3: cmd_msb
// 4: cmd_lsb
// 
// outArray to RPi:
// 0: sensor_msb
// 1: sensor_lsb
// 2: n_msb
// 3: n_lsb
// 4: dt_micro_msb
// 5: dt_micro_lsb


// Note: use sensor_msb and lsb for echo comm check
//----------------------------

#define in_bytes 5
#define out_bytes 6
byte inArray[in_bytes];
byte outArray[out_bytes];


int cmd;
byte cmd_msb, cmd_lsb, n_msb, n_lsb;

int sensor_reading;
byte sensor_msb, sensor_lsb;

int dt_micro;
byte dt_micro_msb, dt_micro_lsb;

int mydelay;
unsigned long prevt;
float dt_ms;
float dt_sec;
float t_ms_prev;

int tempbyte;
int inByte;
int ISR_Happened;

void setup()
{
  Wire.begin(0x07);                // join i2c bus with address 7
  Wire.onRequest(sendEvent); // register event
  Wire.onReceive(receiveEvent);

  Serial.begin(115200);

  attachInterrupt(0, enc_isr_wrapper, RISING);
  HB.setup();


  //bdsyswelcomecode
  Serial.println("Uno DC Motor i2c code");
  Serial.println("i2c addr = 7");
  Serial.println("version 1.0.0");

  pinMode(squarewave_pin, OUTPUT);

  pinMode(sendPin, OUTPUT);
  pinMode(receivePin, OUTPUT);
  pinMode(controlPin, OUTPUT);
  pinMode(isrPin, OUTPUT);  
  pinMode(ao_pin, OUTPUT);  
    

  digitalWrite(receivePin, LOW);
  digitalWrite(sendPin, LOW);
  digitalWrite(controlPin, LOW);
  digitalWrite(isrPin, LOW);  
  analogWrite(ao_pin, 0);


  for(int i=0;i<out_bytes;i++){
    outArray[i] = i;
  }
}

unsigned char getsecondbyte(int input){
    unsigned char output;
    output = (unsigned char)(input >> 8);
    return output;
}


void sendEvent() {
  digitalWrite(sendPin, HIGH);  
  Wire.write(outArray,sizeof(outArray));
  //if (ISR_Happened == 1){
  //    digitalWrite(handshake_pin, HIGH);
  //}
  mypause = 0;
  sent_data = true;
  digitalWrite(sendPin, LOW);    
}

void receiveEvent(int numBytes){
  digitalWrite(receivePin, HIGH);  
  for(int i=0;i<numBytes;i++){
    inArray[i] = Wire.read();
  }
  new_data = true;
  //small delays here shouldn't be terrible because the data is already read
  // - measuring the time between receive events to detect glitches on the
  //   micropython side
  //     - as long as the Arduino receives data before the next time step,
  //       everything is assumed to be working correctly
  prev_t_micros = t;
  t = micros();
  dt_micro = t-prev_t_micros;
  digitalWrite(receivePin, LOW);
}

void pinISR()
{     
  // this is the main control event
  digitalWrite(isrPin, HIGH);
  ISR_Happened = 1;
    
  if (inArray[0] == 3){
    // - process the data if it is there
    // - probably need to send [3,0,0,0,0] to keep the motors stopped, just to be safe
    digitalWrite(controlPin, HIGH);
    // sensor reading here
    sensor_reading = enc.get_reading();
    sensor_lsb = (byte)sensor_reading;
    sensor_msb = getsecondbyte(sensor_reading);

    n_msb = inArray[1];
    n_lsb = inArray[2];
    n_i2c = 256*n_msb + n_lsb;
    cmd_msb = inArray[3];
    cmd_lsb = inArray[4];
    cmd = reassemblebytes(cmd_msb,cmd_lsb);
    HB.send_command(cmd);
    // load up outArray to acknowledge what we have received
    // - is it better to have the Arduino respond by echoing the 
    //   n it received or its own n_loop?

    // - where and when does outArray get loaded?
    // - when and where do we read the sensor?
    // - when and where do we send the cmd to the plant?
    outArray[0] = sensor_msb;
    outArray[1] = sensor_lsb;
    outArray[2] = n_msb;
    outArray[3] = n_lsb;
    dt_micro_lsb = (byte)dt_micro;
    dt_micro_msb = getsecondbyte(dt_micro);
    outArray[4] = dt_micro_msb;
    outArray[5] = dt_micro_lsb;
    digitalWrite(controlPin, LOW);
  }
  //set_speeds(v1, v2);
  //analogWrite(pwmA, v1);
  //v_out = v1*v1;
  //accel = analogRead(analogPin);
  digitalWrite(isrPin, LOW);
}


void loop()
{
  /* if (ISR_Happened > 0 ){ */
  /*   ISR_Happened = 0; */
  /*   Serial.print(1); */
  /*   Serial.print(","); */
  /*   Serial.print(v1); */
  /*   Serial.print(","); */
  /*   Serial.print(v2); */
  /*   mynewline(); */
  /* } */
  if ( sent_data ){
    sent_data = false;
    //Serial.println("I sent data");
  }
  if ( new_data ){
    /* Serial.print("new data, inArray = "); */
    /* for (int k=0; k<7; k++){ */
    /*   Serial.print(inArray[k]); */
    /*   if (k<6){ */
    /* 	Serial.print(","); */
    /*   } */
    /*   else{ */
    /* 	Serial.print('\n'); */
    /*   } */
    /* } */

    new_data = false;
    //digitalWrite(i2cprocessPin, HIGH);  

    //Serial.println("new data");
    if (inArray[0] == 1){
      // start new test
      // - what needs to happen here?
      nISR = 0;
      n_loop = 0;
      enc.output = 0;
      // - anything else?
    }
    else if (inArray[0] == 2){
      // end test
      // probably send zero to actuator
      HB.send_command(0);
    }

    else if (inArray[0] == 3){
      pinISR();
      if (print_data){
	  //n_i2c, v1, v2, position, dt_micro
      	Serial.print(n_i2c);
      	print_comma_then_int(cmd);
      	print_comma_then_int(sensor_reading);
      	mynewline();
      }
    }
    else if (inArray[0] == 5){
        // i2c communication test
        // - bytes 0-3 are loaded else where
        outArray[2] = inArray[1];
        outArray[3] = inArray[2];
        i_received = 256*inArray[3] + inArray[4];
        myresponse = 10*i_received + 1;
        myr_lsb = (byte)myresponse;
        myr_msb = getsecondbyte(myresponse);
        outArray[0] = myr_msb;
        outArray[1] = myr_lsb;
        //Serial.print("i = ");
    
    }
    /* else if (inArray[0] == 3){ */
    /*   // main control case */
    /*   digitalWrite(controlPin, HIGH);   */
    /*   v1_msb = inArray[1]; */
    /*   v1_lsb = inArray[2]; */
    /*   v1 = reassemblebytes(v1_msb,v1_lsb); */
    /*   v2_msb = inArray[3]; */
    /*   v2_lsb = inArray[4]; */
    /*   v2 = reassemblebytes(v2_msb,v2_lsb); */
    /*   digitalWrite(controlPin, LOW);   */
    /* } */
  }
  n_loop++;
  // load data into array to send to RPi
  //outArray[1] = n_loop;
  //pos_lsb = (byte)position;
  //pos_msb = getsecondbyte(position);
  //outArray[2] = pos_msb;
  //outArray[3] = pos_lsb;
}
  
