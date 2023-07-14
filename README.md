# ABZ Security Box

The ABZ Security Box is a security program created for ESP32-S3. This software is capable of measuring distances using a distance sensor, setting off an alarm if anything comes too close, and sending alerts via SMS. This program is specifically designed to be controlled remotely using SMS commands. The ABZ Security Box is meant to be mounted to a wall where an opening door would intersect the ultrasonic sensor line-of-sight.

This was a final team project at the University of California, San Diego for ECE 196 (Hands-on Engineering Project).

An adjustable DC Boost Converter Workshop is also attached for speaker voltage adjustments. Learn about the LM2577-ADJ IC:
For more details, see [details](ABZ%20Workshop%20LM2577-ADJ.pdf).

Please visit our website at:
https://sites.google.com/view/abz-secure

## Installation and Requirements

- You will need to install the TwilioLib library into your Arduino environment before you can compile and upload the code to your Arduino device.
- You will need a constant and stable WiFi connection to maintain SMS communication.
- This code is compatible with any ESP32 with WiFi capability. Wifi-capable Arduino devices will require code alteration due to differing WiFi connection libraries.

## PCB Design

PCB design and result images are provided on our website for users wishing to design the system from scratch, as opposed to a pre-made ESP32 device. KiCad was used to design the prototype. Since this design uses plenty of small ICs and 0402 footprint passive components, either PCB manufacturer assembly or a reflow oven is required.

If users wish to veer away from a PCB design, this code and the schematics found in the PCB files folder are compatible with any pre-made ESP32 device.

## Variables

- `commandLoopCounter` - Count of the loop iterations to control how frequently we search for new commands.
- `notificationHelp`, `notificationReset`, `notificationStatus`, `notificationLowBatt`, `notificationCritBatt`, `notificationNoBatt` - Boolean flags to track the status of notifications.
- `ssid`, `password` - Your Wifi network's SSID and password.
- `account_sid`, `auth_token`, `from_number`, `to_number` - Your Twilio account SID, Auth Token, sender number, and receiver number.
- `deactivationCode` - The deactivation code to disarm the alarm.
- `mostRecentMessage` - The most recent message sent to the Twilio phone number.
- `armed` - Boolean flag to track if the system is armed.
- `trigPin`, `echoPin`, `MosfetPin` - The pins connected to the ultrasonic sensor and alarm.
- `SOUND_SPEED`, `CM_TO_INCH`, `MAX_ANALOG_VAL`, `MAX_BATTERY_VOLTAGE` - Constants used for distance and battery level calculation.
- `duration`, `distanceInch`, `AverageDistanceInch`, `SafeDistanceInch`, `SafeDistanceInchMinus`, `SafeDistanceInchPositive`, `Safe`, `SafeCount` - Variables used for distance calculation and safety checking.
- `alpha`, `alphaDelta`, `betaDelta`, `SafeWindow`, `SafeCountTrigger` - Variables used for controlling the distance measurement and checking routines.

## SMS Commands

The following commands are available for controlling the ABZ Security System:

- `Help` - The system will reply with a list of available commands.
- `Arm` - The system will arm the alarm and start checking for intrusions.
- `Disarm` - The system will disarm the alarm.
- `Reset` - The system will ask for confirmation, then reset the device.
- `Status` - The system will send back its current status including the current battery level and whether it's armed.

  These commands are sent by replying to the system's messages.


## Key Functions

- `clearNotificationBlocks` - Clear the flags used to track if notifications have been sent.
- `status` - Get the current status of the system as a string.
- `measureAverageDistance` - Measure the average distance over a number of iterations.
- `lowBatteryNotification` - Check the battery level and send notifications if it's too low.
- `measureBattery` - Measure the current battery level.
- `measureDistance` - Measure the distance using the ultrasonic sensor once.
- `setSafeDistance` - Measure and set the safe distance.
- `questionSafety` - Check if a given distance is within the safe range.
- `setAlarmHigh` - Set the alarm to a high state (i.e., turn it on).
- `setAlarmLow` - Set the alarm to a low state (i.e., turn it off).
- `beeper` - Beep the alarm for a certain duration.
- `sendText` - Send an SMS message using Twilio.
- `getConversation` - Get the most recent message from the Twilio phone number.
- `generateMessage` - Generate a break-in alert message.
- `searchDisarmCode` - Check if the most recent message is the disarm code.
- `searchCommands` - Check if the most recent message is one of the command words.
- `alertBreakIn` - Send a break-in alert and start the alarm.
- `setup` - Set up the device. This function is called once when the device is turned on.
- `loop` - The main loop of the device. This function is called repeatedly.


## Battery Monitoring

The system includes battery level monitoring. It sends a text notification when the battery level falls below certain thresholds:

- Below 30%: A warning is sent to charge the battery
- Below 10%: A critical battery warning is sent, indicating that the system will soon shut down
- Battery level reaches 0: The system sends a final notification before shutting down

  These commands are sent by replying to the system's messages.

  The system is charged by plugging in a USB C.


## Contributing

Contributions to the ABZ Security System are welcome. Please create a pull request with your changes or enhancements, ensuring that the code is well-formatted and includes comments where necessary.
