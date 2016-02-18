/* A program monitor an Lawnmower robot and send alarm wia Bluetoot to an smartphone App.  
 */ 
 
 
 #include <SoftwareSerial.h> 
 
 
 const int rxPin = 4; //SoftwareSerial RX pin, connect to JY-MCY TX pin 
 const int txPin = 2; //SoftwareSerial TX pin, connect to JY-MCU RX pin 
                      // level shifting to 3.3 volts may be needed 
 
 
  SoftwareSerial mySerial(rxPin, txPin); // RX, TX 

 const int debugLevel = 0; //specifiy printlevel for debug text, higher number=more text. 0= no text
 
 const int ledPin = 13;  // led pin 
 const int robotSwitchPin = 7; // pin were switch that checks when robot is in charging station
 const int alarmPin = 8; //Pin that goes high when an alarm is sent
 
 //const unsigned long alarmTime= 60000; //Variable to hold the time that need to be ellapsed before an alarm is sent, in milli seconds
 //const unsigned long alarmTime= 7200000; //2h Variable to hold the time that need to be ellapsed before an alarm is sent, in milli seconds
 const unsigned long alarmTime= 36000000; //10h
 unsigned long lastTimeInChager=0; //Variable to hold when the robot was last time in the chager 
 unsigned long lastTimeSending=0; //variable to hold the time when we last time sent a time msg to phone 
 int state = 0;        // if state is 1, the LED will turn on and 
                       // if state is 0, the LED will turn off 
 int flag = 0;         // a flag to prevent duplicate messages
 
 
 int alarmState = 0;  //Inital alarm is off
 boolean inChargerMsgSent = false;
 
 int lastRobotSwitchState =LOW;
 int robotInCharger;
  unsigned long lastSwitchDebounceTime = 0;  // the last time the output pin was toggled
  

 
 void setup() { 
     // sets the pins as outputs: 
    pinMode(ledPin, OUTPUT);
    pinMode(alarmPin, OUTPUT);
    pinMode(robotSwitchPin, INPUT); 
    mySerial.begin(9600);
    if (debugLevel >= 1){ 
      Serial.begin(9600);
    }
    digitalWrite(ledPin, LOW); // LED is initially off 
    digitalWrite(alarmPin, LOW); // alarm is initially off 
    
    
    debugPrint(String("Alarm Time: "),1);
    debugPrintln(String(alarmTime),1);
    robotInCharger = digitalRead(robotSwitchPin); //Let define if the robot is in charger depending on switch state
    lastTimeInChager=millis(); //We asume the robot is in the charger for the first time
    lastTimeSending=millis();
 } 
 
 
 void loop() { 
          //read switch to see if robot is at home
     
     
   
     //set alarm if robot is not there, only for test
     if (!isRobotInCharger()){
       inChargerMsgSent=false;
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
        debugPrint(String("Time since last Charging: "),2);
        debugPrintln(String(timeSinceLastCharging),2);

       //No robot in charger, check if time has elapsed, if so send alarm
       if( timeSinceLastCharging >= alarmTime) //Check if alarm time has elapsed 
       {
       setAlarmForRobot();
       }else{ //Send time since last charging
         sendTime(timeSinceLastCharging);
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
           mySerial.println("0"); 
           flag = 1; 
         } 
     } 
     // if the state is '1' the led will turn on 
     else if (state == '1') { 
         digitalWrite(ledPin, HIGH); 
         if(flag == 0){ 
           mySerial.println("1"); 
           flag = 1; 
         } 
     } 
  
    
  }
    
    void sendTime(unsigned long time){
      unsigned long currentTime = millis();
      unsigned long timeSinceLastSending = currentTime-lastTimeSending;  
      //debugPrint(String("timeSinceLastSending: ",1);
      //debugPrintln(String(timeSinceLastSending,1));
      if( timeSinceLastSending >= 10000){
        //debugPrintln(String("Sending time: T " ),1);
        //mySerial.println("T " + time);
        lastTimeSending=millis();
        debugPrint(String("Sending time: T " ),1);
        debugPrintln(String(time),1);
        mySerial.println("T");
        mySerial.println(time);
     } 
    }
    
    void setAlarmForRobot()
    {
       if(alarmState==LOW){
         debugPrintln(String("Set Alarm!"),1);
         digitalWrite(alarmPin,HIGH);
         alarmState=HIGH;
         inChargerMsgSent=false;
         //Send Alarm via BlueTooth
          mySerial.println("A"); 
     }
     //DO nothing
    }
    
    boolean isRobotInCharger(){
      unsigned long debounceDelay = 2000;    // the debounce time; increase if the output flickers
      int robotSwitchState = digitalRead(robotSwitchPin); //variable to read robotSwitchState
      debugPrint(String("Reading robotswitchState: "),2);
      debugPrintln(String(robotSwitchState),2);
      
      //Check if switch has toggeled
      if (robotSwitchState != lastRobotSwitchState){
        lastSwitchDebounceTime=millis();
        debugPrintln(String("Switch Has toggeled"),2);
      }
      
      lastRobotSwitchState = robotSwitchState;
      
      //Update robot in chager if time hase elapsed
      if ((millis()-lastSwitchDebounceTime) > debounceDelay){ 
        debugPrintln(String("Time has been elapsed"),2);  
        robotInCharger = robotSwitchState;
        if (robotInCharger){
          digitalWrite(ledPin, HIGH); 
        }else {
          digitalWrite(ledPin, LOW);
          
        }
        
      }
      debugPrint(String("Return: "),2);
      debugPrintln(String(robotInCharger),2);
      return robotInCharger;      
    }
    
    void clearAlarmForRobot()
    {
      debugPrintln(String("Clear Alarm!"),1);
      if(alarmState==HIGH || !inChargerMsgSent){
       digitalWrite(alarmPin,LOW);
       alarmState=LOW;
        //Send status via BlueTooth
        mySerial.println("C"); 
        inChargerMsgSent=true;
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



