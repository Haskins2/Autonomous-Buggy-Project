#include <Encoder.h>
#include <WiFiNINA.h>

//set wifi name & password
char ssid[] = "Y12";
char pass[] = "buggymobile";

WiFiServer server(5200);
Encoder myEnc(2, 3);

const byte LEYE = A0;
const byte REYE = A3;
const byte encoderL = 3;
const byte encoderR = 2;
const byte US_TRIG = 4;  //4
const byte US_ECHO = 5;  //5

// Motor A connections
const byte enA = 6;   //pwm 6
const byte in1 = 9;   //9 5
const byte in2 = 10;  //10 6

// Motor B connections
const byte enB = 12;  //pwm 12
const byte in3 = 11;  //11
const byte in4 = 8;   //8

//Ultrasonic sensor
int DistanceState = LOW;           // ledState used to set the LED
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 200;         // interval at which to blink (milliseconds)
unsigned long currentMillis;
double distance;
double distance1;
double distance2;
double velocity;
double duration;

//PID variables
double kp = 10;
double ki = 0;
double kd = 0;
double PID;
double Max, Min;
int Speed = 255;
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double input, output;
double setPoint = 15;
double cumError, rateError;

//Encoder variables
double oldPosition = -999;
double speedstart = 0;
double newPosition = 0;
double distancePerStep = 1.25;
int difference;
int encoderSpeed = 0, usSpeed = 0, eyeDist = 0;
String m;
bool drive = false;
void setup() {

  Serial.begin(9600);
  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);
  pinMode(LEYE, INPUT);
  pinMode(REYE, INPUT);

  // Set all the motor control pins to outputs for hbridge
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Turn off motors - Initial state
  analogWrite(enA, 255);
  analogWrite(enB, 255);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  //start WiFi
  WiFi.beginAP(ssid, pass);
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address:");
  Serial.println(ip);
  server.begin();
}
void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (DistanceState == LOW) {
      DistanceState = HIGH;
      distance1 = CalculateDistance();
    } else {
      DistanceState = LOW;
      distance2 = CalculateDistance();
      velocity = (distance2 - distance1) / 0.5f;

      print();
      server.write(encoderSpeed);
      server.write(usSpeed);

      difference = newPosition - speedstart;
      speedstart = newPosition;
    }
  }

  WiFiClient client = server.available();
  if (client.connected()) {
    //client.write("Hello Client");
    char c = client.read();
    if (c != 'o') {
      Serial.print("Received character: ");
      Serial.println(c);
    }
    if (c == 'W') {
      drive = true;
    }
    if (c == 'S') {
      drive = false;
    }
  }
  if (drive) {
    PID = computePID(distance);
    if ((100 + 0.18 * (abs(PID))) > 0 && (100 + 0.18 * (abs(PID))) < 255) {
      Speed = 100 + 0.18 * (abs(PID));
      //Serial.println(PID);
    }
    //Serial.println(Speed);
    if (Speed > 230) {
      Speed = 230;
    }
    if (distance > 20 && distance != 0) {
      if (EyeMonitor() == 1) {
        //Serial.println("GO");
        FORWARD(Speed, Speed);
      } else if (EyeMonitor() == 2) {
        //Serial.println("STOP");
        action(2);
      } else if (EyeMonitor() == 3) {
        //Serial.println("RIGHT");
        client.write("Turning Left");
        action(3);
      } else if (EyeMonitor() == 4) {
        //Serial.println("LEFT");
        client.write("Turning Right");
        action(4);
      }
    } else {
      client.write("Obstacle Seen");
      action(2);
    }
  } else if (!drive) {
    action(2);
  }
  newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
  }
}
double computePID(double inp) {
  currentTime = millis();                              //get current time
  elapsedTime = (double)(currentTime - previousTime);  //compute time elapsed from previous computation
  error = setPoint - inp;  // determine error
  cumError += error * elapsedTime;                         // compute integral
  rateError = (error - lastError) / elapsedTime;             // compute derivative
  double out = kp * error + ki * cumError + kd * rateError;  //PID output
  lastError = error;                                         //remember current error
  previousTime = currentTime;  //remember current time 
  return out;               //have function return the PID output
}

double CalculateDistance() {
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);
  duration = pulseIn(US_ECHO, HIGH);
  distance = (duration * 0.0293) / 2;  //58
  return distance;
}

void action(int i) {
  switch (i) {
    case 1:
      //MOVE FORWARD
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      break;
    case 2:
      //IDLE
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      break;
    case 3:
      //TURN RIGHT
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);  //SET LOW FOR SMOOTH TURNING
      break;
    case 4:
      //TURN RIGHT
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);  //SET LOW FOR SMOOTH TURNING
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      break;
    case 5:
      //MOVE BACKWARD
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      break;
  }
}

int EyeMonitor() {
  const int black = HIGH;
  const int white = LOW;
  if ((digitalRead(LEYE) == black) && (digitalRead(REYE) == black)) {
    //Then GO
    return 1;
  }
  if ((digitalRead(LEYE) == white) && (digitalRead(REYE) == white)) {
    //Then stop
    return 2;
  }
  if ((digitalRead(LEYE) == black) && (digitalRead(REYE) == white)) {
    //Then turn right
    return 3;
  }
  if ((digitalRead(LEYE) == white) && (digitalRead(REYE) == black)) {
    //Then turn left
    return 4;
  }
}

void FORWARD(int speed1, int speed2) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enA, speed1);
  analogWrite(enB, speed2);
}

void print() {
  //Used for Testing
  /*Serial.print("Velocity of Buggy: ");
  Serial.print(difference * distancePerStep);
  Serial.print("cm/s");
  Serial.print("\t");
  encoderSpeed = difference * distancePerStep;
  Serial.print("Velocity of Object in Front: ");
  Serial.print((difference * distancePerStep) - velocity);
  Serial.print("cm/s");
  Serial.print("\t");
  usSpeed = (difference * distancePerStep) - velocity;
  Serial.print("Distance from US Sensor: ");
  Serial.print(eyeDist);
  Serial.println("cm");*/
  usSpeed = (difference * distancePerStep) - velocity;
  
  //Printing Us Distance
  eyeDist = distance;
  server.write(eyeDist);
  Serial.println(eyeDist);
}