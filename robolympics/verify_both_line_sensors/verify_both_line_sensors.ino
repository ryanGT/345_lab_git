#include <kraussserial.h>
#include <QTRSensors.h>

uint16_t positionF, positionR;

const uint8_t SensorCount = 7;

uint16_t sensorValuesF[SensorCount];
uint16_t sensorValuesR[SensorCount];

QTRSensors qtrF;
QTRSensors qtrR;

int calibrated;
char mychar;


void print_sensor_values(uint16_t *myarray){
    for (uint8_t q=0; q< SensorCount; q++){
        if ( q > 0){
            Serial.print(" ");
        }
        Serial.print(myarray[q]);
    }
}

void print_cal_res(QTRSensors &qtr){
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
    qtrR.calibrate();
    Serial.println(i);
  }

  Serial.println("calibration results:");
  // print the calibration minimum values measured when emitters were on
  Serial.println("Front cal:");
  print_cal_res(qtrF);
  //  for (uint8_t i = 0; i < SensorCount; i++)
  //    {
  //      Serial.print(qtrF.calibrationOn.minimum[i]);
  //      Serial.print(' ');
  //    }
  //    Serial.println();
  //  
  //    for (uint8_t i = 0; i < SensorCount; i++)
  //    {
  //      Serial.print(qtrF.calibrationOn.maximum[i]);
  //      Serial.print(' ');
  //    }
  //  
  Serial.println("Rear cal:");
  print_cal_res(qtrR);
  Serial.println();
  Serial.println();
  calibrated = 1;
  delay(500);
}




void setup()
{
  Serial.begin(115200);

  //bdsyswelcomecode
  
  Serial.println("dual line sensor verification");
  Serial.println("- Arduino Mega only (no RPi)");
  Serial.println("v 1.0.0");
  //!// encoder pin on interrupt 0 (pin 2)

  //Serial.print("pendulum/cart v. 1.1.0 RT Serial");
  //Serial.print("\n");

  //attachInterrupt(digitalPinToInterrupt(18), pinISR, CHANGE);

  qtrF.setTypeRC();
  //////qtr.setSensorPins((const uint8_t[]){27,29,31,33,35,37,39}, SensorCount);
  qtrF.setSensorPins((const uint8_t[]){32,34,36,38,40,42,44}, SensorCount);
  //qtrF.setSensorPins((const uint8_t[]){44,42,40,38,36,34,32}, SensorCount);
  qtrF.setEmitterPin(46);
 
  qtrR.setTypeRC();
  qtrR.setSensorPins((const uint8_t[]){45,43,41,39,37,35,33}, SensorCount);
  ////qtr.setSensorPins((const uint8_t[]){27,29,31,33,35,37,39}, SensorCount);
  qtrR.setEmitterPin(31);

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

  mychar = get_char();
  calibrate_line_sensor();

  Serial.println("enter any character to continue");

  mychar = get_char();
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
      positionF = qtrF.readLineBlack(sensorValuesF);//<--- this will block the reading of new data
      Serial.print("F: ");
      print_sensor_values(sensorValuesF);
      Serial.print(" - ");
      Serial.println(positionF);

      positionR = qtrR.readLineBlack(sensorValuesR);//<--- this will block the reading of new data
      Serial.print("R: ");
      print_sensor_values(sensorValuesR);
      Serial.print(" - ");
      Serial.println(positionR);
       	
     delay(750);
  }
  // load data into array to send to upy
}
  
