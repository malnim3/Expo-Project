#include <Vector.h>   // for vectors
#include <Adafruit_NeoPixel.h>
#include <stdio.h>
//#include <DS1302.h>
#include <IRremote.h>
#include <TimeLib.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h> //For the bluetooth
LiquidCrystal LCD(12, 11, 5, 4, 3, 2);

using namespace std;

//These are the HEX values of the buttons being used
#define ONE_DAY   0xB946FF00 // VOL+ button (One occurrence of the alarm it will be deleted once used)
#define EVERYDAY    0xB847FF00 // FUNCTION/STOP button (Everyday occurrence of the alarm, will need to be deleted manually)
#define AM 0xE619FF00 // EQ button
#define PM 0xF20DFF00 // ST/REPT button
#define SLASH 0xBA45FF00 // SLASH button (for seperating time)
#define ZERO 0xE916FF00
#define ONE 0xF30CFF00
#define TWO 0xE718FF00
#define THREE 0xA15EFF00
#define FOUR 0xF708FF00
#define FIVE 0xE31CFF00
#define SIX 0xA55AFF00
#define SEVEN 0xBD42FF00
#define EIGHT 0xAD52FF00
#define NINE 0xB54AFF00
  

  const int RECV_PIN = 11;       // IR Sensor pin
  int ledPin = 13;                // LED 
  int pirPin = 12;                 // PIR Out pin 
  int buzzer = 10;                  // buzzer pin
  int pirStat = 0;                   // PIR status

  //Setting variables needed for bluetooth
  uint8_t RXPin = 3;
  uint8_t TXPin = 2;
  SoftwareSerial bluetooth(TXPin, RXPin);

  //String alarmsSet[10]; //An array that will hold all the alarms (max of 10 alarms at once)
  Vector<String> alarmsSet;  //An vector that will hold all the alarms (max of 10 alarms at once)
  Vector<int> alarmHours;    //vector of parsed hours for alarm
  Vector<int> alarmMinutes;  //vector of parsed minutes for alarm
  Vector<String> alarmOc;  //vector of parsed occurence for alarm
  //Vector<int, int, String> conAlarm;
  int alarmNum = 0;
  int amPm = 0; //If 0 then not set yet, if 1 then am, if 2 then pm
  String totalClicked; //All the buttons clicked before am/pm clicked 
  String curClick; //The button that just got clicked

  //------------------------------------------------------------------------------------------------------------------------------------

  //Initialize global variables for time and date of the clock
  int months = 0;  // for settime
  int days = 0;   // for settime
  int years = 0;  // for settime
  int hours = 0;
  int minutes = 0;
  int seconds = 0; // for settime
  String occurrence = "";
  // Need to change these to vectors
  int parseAlarmsHours[10];
  int parseAlarmsMinutes[10];
  int parseAlarmsOccurence[10];
  
  
  // Prompt for user to enter the correct format of the string
  String Time = "Enter the time in this format:  hh/mm.";     // Will be manually entered into Arduino ide everytime the clock needs to be turned on
  String Alarm = "Enter the alarm in this format: occurence/hh/mm. (OD = one day, ED = everyday)"; // EX: OD/hh/mm       ED/hh/mm
  
  // For reading the strings and parsing it 
  String myTime;
  String myAlarm;

  //Keeps track of what type of zone we are currently in and the offset we need to add to current time
  //Default is central
  String currTimeZone = "CENTRAL";
  int timeZoneOffset = 0; //difference in hour

//------------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
  
   pinMode(ledPin, OUTPUT);     
   pinMode(pirPin, INPUT);     
   LCD.begin(16, 2);
   Serial.begin(9600);
   IrReceiver.begin(RECV_PIN);
   pinMode(buzzer, OUTPUT);
   
  //Starting bluetooth
  bluetooth.begin(9600);

   
}

//------------------------------------------------------------------------------------------------------------------------------------
// Start of expoCode and my modified Lab6 Code
// loop also acts as a main function
void loop()
{ 
  //Checks to see if something was clicked on webpage
  readBluetooth();
  
  //Setting up alarm
  if(IrReceiver.decode()){
   if(alarmNum == 10){
    Serial.println("Sorry you already set 10 alarms!");
    IrReceiver.resume();
    }else{
      //Checks and converts the HEX to string
      switch(IrReceiver.decodedIRData.decodedRawData){
        case ONE_DAY:
        amPm = 1;
        curClick = "OD";
          //Serial.println(curClick);
          break;
        case EVERYDAY:
        amPm = 2;
        curClick = "ED";
          //Serial.println(curClick);
          break;  
        case ZERO:
        curClick = "0";
          //Serial.println(curClick);
          break;
        case ONE:
        curClick = "1";
          //Serial.println(curClick);
          break;
        case TWO:
        curClick = "2";
          //Serial.println(curClick);
          break;
        case THREE:
        curClick = "3";
          //Serial.println(curClick);
          break;
        case FOUR:
        curClick = "4";
          //Serial.println(curClick);
          break;
        case FIVE:
        curClick = "5";
          //Serial.println(curClick);
          break;
        case SIX:
        curClick = "6";
          //Serial.println(curClick);
          break;
        case SEVEN:
        curClick = "7";
          //Serial.println(curClick);
          break;
        case EIGHT:
        curClick = "8";
          //Serial.println(curClick);
          break;
        case NINE:
        curClick = "9";
          //Serial.println(curClick);
          break;
        case SLASH:
          curClick = "/";
          //Serial.println("PM");
          break;
        }
      totalClicked.concat(curClick);  
      Serial.println(totalClicked);

      //If the user selects ED/OD then it stores the time in the array
      if(amPm != 0){
        Serial.println("Adding to vector");  
        alarmsSet.PushBack(totalClicked);
        Serial.println(totalClicked);
        amPm=0;
        totalClicked="";
        ++alarmNum;
      }
      IrReceiver.resume();
      }
    }else{
        Serial.println("Nothing was clicked");
        IrReceiver.resume();
      }
   //End of alarm function
   delay(1000);

    // If alarm size > 0 check it's validity 
  if (alarmsSet.Size() > 0)
  {  

    //Store user inputted Alarm, and check it's validity
    int i = 0;
    int n = alarmsSet.Size();
    while(i < n)
    {
      myAlarm = alarmsSet[i];
      int sizeAlarm = myAlarm.length();
      // Invalid Alarm entered 
      if (sizeAlarm > 8)
      {
        Serial.println("Not A Valid Alarm");
        //LCD.setCursor(3, 0);
        //LCD.print("Invalid Alarm. Try Again! ");
        alarmsSet.Erase(i);
      }
      else
      {
        checkAlarm(myAlarm, i);
      }
        Serial.println(myAlarm);
        i++;
    }  
  }

  // Wait for input
  Serial.println(Time);
  while (1)
  {
    if (Serial.available() == 1)
    {
      break;
    }
  }

  // If input recieved for time
  if (Serial.available() == 1)
  {  

    //Store user inputted Time, and check it's validity
    myTime = Serial.readString();
    int sizeTime = myTime.length();
    checkTime(myTime);
    Serial.println(myTime);


     // Invalid Time entered 
    if (sizeTime > 9)
    {
      Serial.println("Not A Valid Time");
      LCD.setCursor(3, 0);
      LCD.print("Invalid Time. Try Again! ");
    }

    // Set the time using the setTime function
    setTime(hours, minutes, seconds, days, months, years);
    clock();
    //alarm();
  }


    
}
//------------------------------------------------------------------------------------------------------------------------------------
//Conversion function
//Based on the current time zone and the new time zone, we have functions which tell us the difference between
//of the 2 time zones(in hours) so we can correctly change the time
void timeZoneConversion(String newZone){
  if(currTimeZone == "EASTERN"){
    if(newZone == "CENTRAL"){
      timeZoneOffset = -1;
    }else if(newZone == "MOUNTAIN"){
      timeZoneOffset = -2;
    }else if(newZone == "PACIFIC"){
      timeZoneOffset = -3;
    }
  }else if(currTimeZone == "CENTRAL"){
    if(newZone == "EASTERN"){
      timeZoneOffset = 1;
    }else if(newZone == "MOUNTAIN"){
      timeZoneOffset = -1;
    }else if(newZone == "PACIFIC"){
      timeZoneOffset = -2;
    }
  }else if(currTimeZone == "MOUNTAIN"){
    if(newZone == "CENTRAL"){
      timeZoneOffset = 1;
    }else if(newZone == "EASTERN"){
      timeZoneOffset = 2;
    }else if(newZone == "PACIFIC"){
      timeZoneOffset = -1;
    }
  }else if(currTimeZone == "PACIFIC"){
    if(newZone == "CENTRAL"){
      timeZoneOffset = 2;
    }else if(newZone == "MOUNTAIN"){
      timeZoneOffset = 1;
    }else if(newZone == "EASTERN"){
      timeZoneOffset = 3;
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------
//Bluetooth function
void readBluetooth(){
  char buff[100];
  if(bluetooth.avaliable() > 0){
    int bytesGiven = bluetooth.readyBytesUntil('\n', buff, 99);
    buff[bytesGiven] = NULL;

    if (strstr(buff, "PACIFIC") == &buff[0]) {
      //Need to add the offset of pacific zone to current time zone
      timeZoneConversion("PACIFIC");
      currTimeZone = "PACIFIC";
    }else if (strstr(buff, "MOUNTAIN") == &buff[0]) {
      //Needs to add offset of mountain zone to current time zone
      timeZoneConversion("MOUNTAIN");
      currTimeZone = "MOUNTAIN";
    }else if (strstr(buff, "CENTRAL") == &buff[0]) {
      //Need to add offset of central to current time zone
      timeZoneConversion("CENTRAL");
      currTimeZone = "CENTRAL";
    }else if(strstr(buff, "EASTERN") == &buff[0]){
      //Needs to add offset of eastern to current time zone
      timeZoneConversion("EASTERN");
      currTimeZone = "EASTERN";
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------
//Function for Alarm validation.
void checkAlarm(String Alarm, int index)
{

  //Local String variables for parsing
  String parseOccurrence;
  String parseMinutes;
  String parseHours;
  

  //Parse through Alarm for Occurrence, minutes, and hours
  parseOccurrence = Alarm.substring(0, Alarm.indexOf("/"));
  
  //Parse through Time for Minutes
  parseMinutes = Alarm.substring(5, Alarm.indexOf("/") + 1);
  minutes = parseMinutes.toInt();
  
  //Parse through Time for Seconds
  parseHours = Alarm.substring(6);
  hours = parseHours.toInt();
  
  // INVALID VALUES for HOURS AND MINUTES
  if ((hours < 0) || (hours > 24) || (minutes < 0) || (minutes > 59) || occurrence != "OD" || occurrence != "ED")
  {
    Serial.println("Invalid Alarm. Removing it. Try Again !");
    Serial.println(Alarm);
    alarmsSet.Erase(index);
    //LCD.setCursor(3, 0);
    //LCD.print("Invalid Alarm. Try Again! ");
    /*
    while (1)
    {
      if (Serial.available() == 1)
      {
        break;
      }
    }
    if (Serial.available() == 1)
    {
      myAlarm = Serial.readString();
      checkAlarm(myAlarm);
      Serial.print(hours);
      Serial.print(":");
      Serial.print(minutes);
      Serial.print(":");
      Serial.println(occurrence);
    }
    */
  }
  else
  {
    alarmHours.PushBack(hours);    
    alarmMinutes.PushBack(minutes); 
    alarmOc.PushBack(parseOccurrence);
  }
    
}

//------------------------------------------------------------------------------------------------------------------------------------
//Function for Time validation.
void checkTime(String Time)
{

  //Local String variables for parsing
  String parseHours;
  String parseMinutes;

  //Parse through Time for Hours
  parseHours = Time.substring(0, Time.indexOf("/"));
  hours = parseHours.toInt();
  
  //Parse through Time for Minutes
  parseMinutes = Time.substring(5, Time.indexOf("/") + 1);
  minutes = parseMinutes.toInt();

  
  // INVALID VALUES for DATE AND TIME
  if ((hours < 0) || (hours > 23) || (minutes < 0) || (minutes > 59))
  {
    Serial.println("Invalid Time. Try Again !");
    Serial.println(Time);
    LCD.setCursor(3, 0);
    LCD.print("Invalid Date. Try Again! ");
    while (1)
    {
      if (Serial.available() == 1)
      {
        break;
      }
    }
    if (Serial.available() == 1)
    {
      myTime = Serial.readString();
      checkTime(myTime);
      Serial.print(hours);
      Serial.print(":");
      Serial.print(minutes);

    }
  }
}




//------------------------------------------------------------------------------------------------------------------------------------
// Updates Time

void clock()
{
  while (1)
  {
    time_t t = now();
    // delay is 1 second like a real clock
    delay(1000);

    //printClock(minute(t), second(t));
    // add alarm here once you implement the vector global variable  // Wait for input
    // Alarm code
    int i = 0;
    int n = alarmHours.Size();
    while(i < n)
    {
      int myHour = alarmHours[i];
      int myMinutes = alarmMinutes[i];
      String myOc = alarmOc[i];
      if(myHour == hour(t) && myMinutes == minute(t))
      {
          pirStat = digitalRead(pirPin); 
          if (pirStat == LOW) 
          {            
            tone(buzzer, 1);               // if Alarm detected
          } 
          else if (pirStat == HIGH)
          {
            noTone(buzzer);                 // turn off buzzer
          }
      }
 
    }
  }
    
}


//------------------------------------------------------------------------------------------------------------------------------------
// Once Alarm and time has been validated check if any of the current alarms match current time
/*
void alarm()
{
    Vector<int> alarmHours;    //vector of parsed hours for alarm
  Vector<int> alarmMinutes;  //vector of parsed minutes for alarm
  Vector<String> alarmOc
  

}
*/
//------------------------------------------------------------------------------------------------------------------------------------

