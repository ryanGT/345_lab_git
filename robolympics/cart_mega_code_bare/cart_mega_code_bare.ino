#include <math.h>
#include <QTRSensors.h>


volatile int v_left, v_right;

int test_case;

int state;
int ISR_state;
int squarewave_pin = A7;
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

bool debug=true;


unsigned long t0;
unsigned long prev_t_micros;
unsigned long t;
unsigned long t1;
unsigned long cur_t;
float t_ms, t_sec, prev_t, dt;

bool new_data;

#define SensorCount 7
uint16_t sensorValues[SensorCount];

byte mypause;


int v1, v2, vnom, mydir;
byte v1_msb, v1_lsb, v2_msb, v2_lsb, pos_msb, pos_lsb, n_msb, n_lsb;

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

QTRSensors qtrF;
//QTRSensors qtrR;

void set_speeds(int v_right, int v_left){
  motors.setM1Speed(v_right);
  motors.setM2Speed(v_left);
}

void print_sensor_values(){
    for (uint8_t q=0; q< SensorCount; q++){
        if ( q > 0){
            Serial.print(" ");
        }
        Serial.print(sensorValues[q]);
    }
}

void print_cal_res(QTRSensors qtr){
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
 

}


void calibrate_line_sensor(){
  Serial.println("calibrating 200 times");
  int j = 10;//counter to change rotation direction
  int sign = 1;
  for (uint16_t i = 0; i < 200; i++)
  {
    // using j to alternate the rotation of the sensor over the line
    // for autocalibration
    //set_speeds(sign*150,-150*sign);
    j++;
    if ( j >= 20 ){
      sign *= -1;
      j = 0;
    }
    qtrF.calibrate();
    //qtrR.calibrate();
    Serial.println(i);
  }
    set_speeds(0,0);

  Serial.println("calibration results:");
  // print the calibration minimum values measured when emitters were on
  Serial.println("Front cal:");
  print_cal_res(qtrF);
  //Serial.println("Rear cal:");
  //print_cal_res(qtrR);
  Serial.println();
  Serial.println();
  calibrated = 1;
  delay(500);
}



void menu(){
  Serial.println("enter any character to start a test");
  char mychar = get_char();
  // reset encoders and t0 at the start of a test
  //enc.encoder_count = 0;
  //bdsysmenucode

  t0 = micros();
  ISR_Happened = 0;// clear flag and wait for next time step
  Serial.print("t0 =");
  Serial.println(t0);
}

void setup()
{
  Serial.begin(115200);

  //bdsyswelcomecode
  Serial.println("Mega i2c code for RPI WiringPi C");
  Serial.println("version 1.0.1");
  Serial.println("- emitter pin 27");
  Serial.println("- skip line sensor reading if not calibrated");
  Serial.println("    - hard code position to 10,000 if not calibrated");
  Serial.println("");
  Serial.println("debug - bare");
  
  //!// encoder pin on interrupt 0 (pin 2)

  //Serial.print("pendulum/cart v. 1.1.0 RT Serial");
  //Serial.print("\n");

  //attachInterrupt(digitalPinToInterrupt(18), pinISR, CHANGE);

  qtrF.setTypeRC();
  //////qtr.setSensorPins((const uint8_t[]){27,29,31,33,35,37,39}, SensorCount);
  qtrF.setSensorPins((const uint8_t[]){32,34,36,38,40,42,44}, SensorCount);
  //qtrF.setSensorPins((const uint8_t[]){44,42,40,38,36,34,32}, SensorCount);
  qtrF.setEmitterPin(46);
 
  //qtrR.setTypeRC();
  //qtrR.setSensorPins((const uint8_t[]){45,43,41,39,37,35,33}, SensorCount);
  ////qtr.setSensorPins((const uint8_t[]){27,29,31,33,35,37,39}, SensorCount);
  //qtrR.setEmitterPin(31);

  calibrated = 0;

  /* delay(500); */

  /* Serial.println("press c to begin calibration (s to skip)"); */
  /* tempbyte = get_char(); */
  /* if (tempbyte == 'c'){ */
  /*   calibrate_line_sensor(); */
  /* } */

  //calibrate_line_sensor();

  /* // encoder */
  /* pinMode(encoderPinA, INPUT);  */
  /* pinMode(encoderPinB, INPUT);  */
  /* // turn on pullup resistors */
  /* digitalWrite(encoderPinA, HIGH); */
  /* digitalWrite(encoderPinB, HIGH); */

  /* // encoder pin on interrupt 0 (pin 2) */
  /* attachInterrupt(0, doEncoderB, CHANGE); */
  /* attachInterrupt(1, doEncoderA, CHANGE);   */

  // need to switch to Timer3 for Arduino Mega

  Serial.println("enter any character to start calibration");
  char mychar = get_char();
  calibrate_line_sensor();

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
  if (calibrated>0){
    mydir = 1;
    if (mydir > 0){
  	position = qtrF.readLineBlack(sensorValues);//<--- this will block the reading of new data
    }
    else{
        position = 7;
        //position = qtrR.readLineBlack(sensorValues);//<--- this will block the reading of new data
    }
    if (debug){
	Serial.print("mydir: ");
        Serial.println(mydir);
	print_sensor_values();
	Serial.print(" : ");
        Serial.println(position);
	delay(500);
    }
  }
  else{
	position = 10000;
  }
  n_loop++;
  // load data into array to send to upy
}
  
