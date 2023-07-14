#include "twilioLib.hpp"
TwilioLib *twilio;

int commandLoopCounter = 0;  //so we don't ask for commands everytime.

//requested notifs
bool notificationHelp = false;
bool notificationReset = false;
bool notificationStatus = false;

//automated notifs
bool notificationLowBatt = false;
bool notificationCritBatt = false;
bool notificationNoBatt = false;

//Wifi Details
const char *ssid = " ";
const char *password = " ";

// Twilio Account Details
static const char *account_sid = " ";
static const char *auth_token = " ";

// SMS Message Details
static const char *from_number = " ";
static const char *to_number = "  ";

//Security
String deactivationCode = "xxxxx";
String mostRecentMessage = "";
bool armed = false;

//Pin Variables
const int trigPin = 13;
const int echoPin = A0;
const int MosfetPin = 12;

//Definitions
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
const int MAX_ANALOG_VAL = 4095;
const float MAX_BATTERY_VOLTAGE = 4.2;  // Max LiPoly voltage of a 3.7 battery is 4.2

//Global Variables
long duration;
float distanceInch;
float AverageDistanceInch;
float SafeDistanceInch;
float SafeDistanceInchMinus;
float SafeDistanceInchPositive;
bool Safe;
int SafeCount = 0;


//Adjustable Variables
int alpha = 5;        //average over alpha number of distance measurements
int alphaDelta = 0;  //milliseconds between each distance measurement
int betaDelta = 250;  //milliseconds between each average distance measurement
float SafeWindow = 15;
int SafeCountTrigger = 3;

void clearNotificationBlocks() {
  notificationHelp = false;
  notificationReset = false;
  notificationStatus = false;
}

//Measure alpha iterations of distance. Return in inches.
String status() {
  String stat = "ABZ box status:\n";

  if (armed){
    stat = stat + "Armed\n";
  }
  else{
    stat = stat + "Disarmed\n";
  }
  stat = stat + measureBattery() + "% Charge\n";
  return stat;
}

//Measure alpha iterations of distance. Return in inches.
float measureAverageDistance() {
  AverageDistanceInch = 0;
  for (int i = 0; i < alpha; i++) {
    AverageDistanceInch = AverageDistanceInch + measureDistance();
    delay(alphaDelta);
  }
  AverageDistanceInch = AverageDistanceInch / alpha;
  delay(betaDelta);
  return (AverageDistanceInch);
}

void lowBatteryNotification() {
  int batteryLevel = measureBattery();
  if (batteryLevel <= 0 && !notificationNoBatt) {
    sendText("No Battery!\nShutting down your ABZ box.");
    beeper(1);
    notificationNoBatt = true;
  } else if (batteryLevel < 10 && !notificationCritBatt) {
    sendText("Critical Battery!\nYou ABZ box will be shutting down soon.");
    beeper(1);
    notificationCritBatt = true;
  } else if (batteryLevel < 30 && !notificationLowBatt) {
    sendText("Low Battery!\nPlease plug in your ABZ box.");
    beeper(1);
    notificationLowBatt = true;
  }

  //clear blocks
  if (batteryLevel > 50) {
    notificationNoBatt = false;
    notificationCritBatt = false;
    notificationLowBatt = false;
  }
}

int measureBattery() {
  float voltageLevel = (analogRead(A13) / 4095.0) * 2 * 1.1 * 3.3;  // calculate voltage level
  float batteryLevel = (voltageLevel - 3.3) / (4.2 - 3.3) * 100;
  batteryLevel = round(batteryLevel / 5.0) * 5.0;
  if (batteryLevel > 100) {
    batteryLevel = 100;
  } else if (batteryLevel <= 0) {
    batteryLevel = 0;
  }
  return batteryLevel;
}

//Measure one iteration of distance. Return in inches.
float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return (pulseIn(echoPin, HIGH) * SOUND_SPEED / 2 * CM_TO_INCH);
}

void setSafeDistance() {
  float additiveDistance = 0;
  for (int i = 0; i < 10; i++) {
    additiveDistance = additiveDistance + measureDistance();
    delay(50);
  }
  SafeDistanceInch = additiveDistance / 10;
  SafeDistanceInchMinus = SafeDistanceInch - SafeWindow;
  SafeDistanceInchPositive = SafeDistanceInch + SafeWindow;
}

bool questionSafety(float AverageDistanceInch) {
  if ((AverageDistanceInch > SafeDistanceInchPositive) || (AverageDistanceInch < SafeDistanceInchMinus)) {
    Serial.println("NOT SAFE");
    return false;
  } else {
    Serial.println("Safe");
    return true;
  }
}

void setAlarmHigh() {
  digitalWrite(MosfetPin, HIGH);
}

void setAlarmLow() {
  digitalWrite(MosfetPin, LOW);
}

void beeper(int beep1) {
  setAlarmHigh();
  delay(beep1);
  setAlarmLow();
  delay(beep1);
}


void sendText(String messageBody) {
  if (WiFi.status() == WL_CONNECTED) {
    //Serial.println("WiFi Connected.");
    int idk = 1;
  } else {
    //Serial.print("Connecting to WiFi network (");
    //Serial.print(ssid);
    //Serial.println(") ...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      //Serial.println("Connecting...");
      delay(100);
    }

    //Serial.println("WiFi Connected.");
  }

  twilio = new TwilioLib(account_sid, auth_token);
  delay(100); //changed from 500 to 100 ms
  String response;
  bool success = twilio->send_message(to_number, from_number, messageBody, response);
  if (success) {
    int idk = 1;
    //Serial.println(response);
  } else {
    Serial.println("Message Send Failure!");
  }
}

String getConversation() {
  if (WiFi.status() == WL_CONNECTED) {
    //Serial.println("WiFi Connected.");
    int idk = 1;
  } else {
    //Serial.print("Connecting to WiFi network (");
    //Serial.print(ssid);
    //Serial.println(") ...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      //Serial.println("Connecting...");
      delay(100);
    }

    //Serial.println("WiFi Connected.");
  }

  twilio = new TwilioLib(account_sid, auth_token);
  delay(100); //changed from 500ms to 100ms
  bool success = twilio->get_recentMessage(to_number, from_number, mostRecentMessage);
  if (success) {
    int idk = 1;
    //Serial.println(mostRecentMessage);
  } else {
    Serial.println("Message Send Failure!");
  }

  for (size_t i = 0; i < mostRecentMessage.length(); i++) {
    mostRecentMessage[i] = tolower(mostRecentMessage[i]);
  }

  if (!mostRecentMessage.isEmpty() && isspace(mostRecentMessage[0])) {
    mostRecentMessage.remove(0, 1);  // Remove the leading space
  }

  // Check if last character is a space
  if (!mostRecentMessage.isEmpty() && isspace(mostRecentMessage[mostRecentMessage.length() - 1])) {
    mostRecentMessage.remove(mostRecentMessage.length() - 1);  // Remove the trailing space
  }

  return mostRecentMessage;
}

String generateMessage() {
  String alertmessage = "BREAK IN ALERT!\nReply with deactivation code to disarm: ";
  String randomCode = "";
  char alphanumeric[] = "abcdefghijklmnopqrstuvwxyz0123456789";

  for (int i = 0; i < 5; i++) {
    int randomIndex = random(sizeof(alphanumeric) - 1);  // Subtract 1 to ignore the null-terminating character in the char array
    randomCode += alphanumeric[randomIndex];
  }
  deactivationCode = randomCode;
  return alertmessage + randomCode;
}

bool searchDisarmCode() {
  mostRecentMessage = getConversation();

  if (mostRecentMessage == deactivationCode) {
    return true;
  } else {
    return false;
  }
}

void searchCommands() {
  mostRecentMessage = getConversation();
  if (mostRecentMessage == "help" && !notificationHelp) {
    sendText("Welcome to ABZ Security!\nHere are a list of commands:\n- Arm\n- Disarm\n- Reset\n- Status");
    clearNotificationBlocks();
    notificationHelp = true;
  } else if (mostRecentMessage == "disarm" && armed) {
    armed = false;
    clearNotificationBlocks();
    sendText("System is disarmed.");
    beeper(20);
  } else if (mostRecentMessage == "arm" && !armed) {
    armed = true;
    setSafeDistance();
    clearNotificationBlocks();
    sendText("System is armed.");
    beeper(20);delay(1000);beeper(20);
  } else if (mostRecentMessage == "status" && !notificationStatus){
    clearNotificationBlocks();
    notificationStatus = true;
    sendText(status());
  }
  
  else if (mostRecentMessage == "reset" && !notificationReset) {
    sendText("Are you sure you want to reset your ABZ box?");
    clearNotificationBlocks();
    for (int i = 0; i < 5; i++) {
      delay(1000);
      mostRecentMessage = getConversation();
      if (mostRecentMessage == "yes" || mostRecentMessage == "yeah" || mostRecentMessage == "yep" || mostRecentMessage == "yuh" || mostRecentMessage == "yea" || mostRecentMessage == "ye") {
        sendText("Resetting your ABZ box.");
        ESP.restart();
      }
      else if (mostRecentMessage == "no" || mostRecentMessage == "nope" || mostRecentMessage == "nah") {
        break;
      }
      else{
        sendText("Please reply either Yes or No.");
      }
    }
    notificationReset = true;
    sendText("Resetting cancelled. ABZ box will continue as usual.");
    clearNotificationBlocks();
  }
}


void alertBreakIn() {
  setAlarmHigh();
  sendText(generateMessage());
}

void setup() {
  Serial.begin(115200);        // Starts the serial communication
  pinMode(trigPin, OUTPUT);    // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);     // Sets the echoPin as an Input
  pinMode(MosfetPin, OUTPUT);  // Sets the echoPin as an Input

  setSafeDistance();  //set the safe distance
  sendText(status());
  setAlarmLow();
}

void loop() {
  delay(50);
  if (commandLoopCounter++ == 15) {
    searchCommands();
    commandLoopCounter = 0;
  }
  if (armed) {
    AverageDistanceInch = measureAverageDistance();
    Serial.print("Distance: ");
    Serial.print(SafeDistanceInchMinus);
    Serial.print(" -- ");
    Serial.print(AverageDistanceInch);
    Serial.print(" -- ");
    Serial.println(SafeDistanceInchPositive);

    Serial.print("Safety Result: ");
    Safe = questionSafety(AverageDistanceInch);
    if (Safe) {
      SafeCount = 0;
    } else {
      SafeCount++;
    }

    Serial.print("Safety Count: ");
    Serial.println(SafeCount);

    Serial.print("Alarm Status: ");
    if (SafeCount >= SafeCountTrigger) {
      alertBreakIn();
      Serial.println("SOUND ALARM");
      Serial.print("Searching for Disarm Code (");
      Serial.print(deactivationCode);
      Serial.print(").");
      while (true) {
        delay(1000);
        if (searchDisarmCode()) {
          ESP.restart();
          //sendText("Shutting Down Alarm.");
        } else {
          Serial.print(".");
        }
      }
    } else {
      setAlarmLow();
      Serial.println("None");
    }
    Serial.println("----------------------");
  } else {
    Serial.println("System is disarmed.");
    delay(betaDelta);
  }
}
