/* A program monitor an Lawnmower robot and send alarm wia Bluetoot to an smartphone App.  
 */ 
 
 
 #include <SoftwareSerial.h> 
 
 
 const int rxPin = 4; //SoftwareSerial RX pin, connect to JY-MCY TX pin 
 const int txPin = 2; //SoftwareSerial TX pin, connect to JY-MCU RX pin 
                      // level shifting to 3.3 volts may be needed 
 
 
  SoftwareSerial mySerial(rxPin, txPin); // RX, TX 

 const int debugLevel = 1; //specifiy printlevel for debug text, higher number=more text. 0= no text
 
 const int ledPin = 13;  // led pin 
 const int robotSwitchPin = 7; // pin were switch that checks when robot is in charging station
 const int alarmPin = 8; //Pin that goes high when an alarm is sent
 
 const unsigned long alarmTime= 10000; //Variable to hold the time that need to be ellapsed before an alarm is sent, in milli seconds
 unsigned long lastTimeInChager=0; //Variable to hold when the robot was last time in the chager 
 
 int state = 0;        // if state is 1, the LED will turn on and 
                       // if state is 0, the LED will turn off 
 int flag = 0;         // a flag to prevent duplicate messages
 
 int robotSwitchState = 0; //variable to read robotSwitchState
 int alarmState = 0;  //Inital alarm is off
 
 void setup() { 
     // sets the pins as outputs: 
    pinMode(ledPin, OUTPUT);
    pinMode(alarmPin, OUTPUT);
    pinMode(robotSwitchPin, INPUT); 
    if (debugLevel <= 1){
      mySerial.begin(9600); 
      Serial.begin(9600);
    }
    digitalWrite(ledPin, LOW); // LED is initially off 
    digitalWrite(alarmPin, LOW); // alarm is initially off 
    
    
    debugPrint(String("Alarm Time: "),1);
    debugPrintln(String(alarmTime),1);
    
    lastTimeInChager=millis(); //We asume the robot is in the charger for the first time
 } 
 
 
 void loop() { 
          //read switch to see if robot is at home
     robotSwitchState =digitalRead(robotSwitchPin);
     debugPrint(String("Reading robotswitchState: "),2);
     debugPrintln(String(robotSwitchState),2);
     
   
     //set alarm if robot is not there, only for test
     if (robotSwitchState == LOW){
       //Need to check for roll-over 
      unsigned long currentTime = millis();
      //Need to check for roll-over, happends after ~50days
      if (currentTime < lastTimeInChager){
        debugPrintln(String("An Roll-Over Event happend"),1);
        //Let us asume we are in the charger anyway
        lastTimeInChager=currentTime;
      }
       unsigned long timeSinceLastCharging = currentTime-lastTimeInChager;
          //TODO Need to check for roll-over 
        debugPrint(String("Time since last Charging: "),1);
        debugPrintln(String(timeSinceLastCharging),1);

       //No robot in charger, check if time has elapsed, if so send alarm
       if( timeSinceLastCharging >= alarmTime) //Check if alarm time has elapsed 
       {
       setAlarmForRobot();
       }
     }
     else {
       //Robot in chager
       clearAlarmForRobot();
       //Save time
       lastTimeInChager=millis();
       debugPrintln(String("Robot In Chager, saving time"),1);
     }
     
     
     //reads serial input and saves it in the state variable
     if(mySerial.available() > 0){ 
       state = mySerial.read(); 
       flag=0; //clear the flag so we can print the state 
     } 
     // if the state is '0' the LED will turn off 
     if (state == '0') { 
         digitalWrite(ledPin, LOW); 
         if(flag == 0){ 
           mySerial.println("LED: off"); 
           flag = 1; 
         } 
     } 
     // if the state is '1' the led will turn on 
     else if (state == '1') { 
         digitalWrite(ledPin, HIGH); 
         if(flag == 0){ 
           mySerial.println("LED: on"); 
           flag = 1; 
         } 
     } 
  
    
  }
    
    void setAlarmForRobot()
    {
       if(alarmState==LOW){
         debugPrintln(String("Set Alarm!"),1);
         digitalWrite(alarmPin,HIGH);
         alarmState=HIGH;
         //Send Alarm via BlueTooth
          mySerial.println("RobotAlarm_1"); 
     }
     //DO nothing
    }
    
    void clearAlarmForRobot()
    {
      debugPrintln(String("Clear Alarm!"),1);
      if(alarmState==HIGH){
       digitalWrite(alarmPin,LOW);
       alarmState=LOW;
        //Send Alarm via BlueTooth
        mySerial.println("RobotCharging_1"); 
     }
     //DO nothing
    }
    
    
    //Functions to print text to serial using debug level syntax
   void debugPrint(String text, int printLevel)
   {
     //Serial.println(printLevel);
       if ((debugLevel  != 0) && (printLevel <= debugLevel)) { 
         Serial.print(text);
         //delay(200);
       }
   }  
     
      void debugPrintln(String text, int printLevel)
   {
       if ((debugLevel != 0) && (printLevel <= debugLevel)) { 
         Serial.println(text);
         delay(300);
       }
   }  



