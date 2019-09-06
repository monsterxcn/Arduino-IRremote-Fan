#include <IRremote.h>
#include <Servo.h>
 
const int motorIn1 = 5;
const int motorIn2 = 6;
const int buttonPin = 8;
const int irReceiverPin = 11;
const int ledPin = 13;
 
int fanspeed = 0;
#define rank1 150
#define rank2 200
#define rank3 250
int btState;
int lastbtState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;
 
Servo myservo;
int servoSwitch = 0;
int angle = 90;
int anglestep = 1;                           // Here is where you need to customize.
 
IRrecv irrecv(irReceiverPin);
decode_results results;
 
void setup() {
    pinMode(buttonPin,INPUT);
    pinMode(ledPin,OUTPUT);
    pinMode(motorIn1,OUTPUT);
    pinMode(motorIn2,OUTPUT);
    myservo.attach(3);
    Serial.begin(9600);
    irrecv.enableIRIn();
}
 
void loop() {
    // Key switch module code
    int reading = digitalRead(buttonPin);
    if (reading != lastbtState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != btState) {
            btState = reading;
            if (btState == HIGH) {
                digitalWrite(ledPin,HIGH);
                fanspeed = fanspeed + 1;
                if (fanspeed >= 4) {
                    fanspeed = 0;
                    servoSwitch = 0;
                    angle = myservo.read();
                }
            }
            else
                digitalWrite(ledPin,LOW);
        }
    }
    lastbtState = reading;
    
    // Infrared remote control module code
    if (irrecv.decode(&results)) {
        /* 
        Serial.print("Detail Information: irCode:");
        Serial.print(results.value, HEX);
        Serial.print(",bits:");
        Serial.println(results.bits);
        */
        switch (results.value) {
            case 0xFF22DD:
                Serial.println("Button |<< had been pressed.");
                fanspeed = fanspeed - 1;
                if (fanspeed <= 0) {
                    fanspeed = 0;
                    servoSwitch = 0;
                    angle = myservo.read();
                }
                Serial.println("Finished.");
                break;
            case 0xFF02FD:
                Serial.println("Button >>| has been pressed.");
                fanspeed = fanspeed + 1;
                if (fanspeed >= 3) {
                    fanspeed = 3;
                }
                Serial.println("Finished.");
                break;
            case 0xFF629D:
                Serial.println("Button CH has been pressed.");
                fanspeed = 0;
                servoSwitch = 0;
                angle = myservo.read();
                Serial.println("Finished.");
                break;
            case 0xFF30CF:
                Serial.println("Button 1 has been pressed.");
                fanspeed = 1;
                Serial.println("Finished.");
                break;
            case 0xFF18E7:
                Serial.println("Button 2 has been pressed.");
                fanspeed = 2;
                Serial.println("Finished.");
                break;
            case 0xFF7A85:
                Serial.println("Button 3 has been pressed.");
                fanspeed = 3;
                Serial.println("Finished.");
                break;
            case 0xFFE21D:
                Serial.println("Button CH+ has been pressed.");
                if (fanspeed != 0) {
                    servoSwitch = 1;
                    Serial.println("Start servo. Finished.");
                }
                else
                    Serial.println("Start failed, start fan firstly.");
                break;
            case 0xFFA25D:
                Serial.println("Button CH- has been pressed.");
                servoSwitch = 0;
                angle = myservo.read();
                Serial.println("Finished.");
                break;
        }
        delay(600);
        /* Serial.println("Delay is end, try to reset the irReceiver."); */
        while (!irrecv.isIdle());
        irrecv.resume();
        /* Serial.println("Reset Success!"); */
    }
    
    // Fan control
    switch (fanspeed) {
        case 1:
            analogWrite(motorIn1, 0);
            analogWrite(motorIn2, rank1);
            break;
        case 2:
            analogWrite(motorIn1, 0);
            analogWrite(motorIn2, rank2);
            break;
        case 3:
            analogWrite(motorIn1, 0);
            analogWrite(motorIn2, rank3);
            break;
        default:
            analogWrite(motorIn1, 0);
            analogWrite(motorIn2, 0);
            break;
    }
    
    // Servo control
    switch (servoSwitch) {
        case 1:
            myservo.write(angle);
            delay(15);                       // Here is where you need to customize.
            angle = angle + anglestep;
            if (angle == 160 || angle == 20) // Here is where you need to customize.
                anglestep = -anglestep;
            break;
        case 0:
            angle = myservo.read();
            break;
    }
}