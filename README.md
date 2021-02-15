# Stick-Intercepter

M5 Stick code to intercept traffic between the app and the amp - needs two M5 Sticks and a serial connection in both grove ports.   

The picture below has the yellow and white leads swapped so they make a tx-rx serial connection both ways. You can see the soldered join under the sticky tape in the picture.   

It runs at 1M baud for the serial, and whatever the bluetooth can run at on the Sticks.   

![Spark Setups](https://github.com/paulhamsh/Stick-Intercepter/blob/main/M5Sticks.jpg)

One Stick runs App to Stick, the other runs Stick to Amp   
The app talks to 'App to Stick' (which appears as 'Spark 40 Audio' over bluetooth) and 'Stick to Amp' talks to the amp.   
I only use two Sticks because I don't think you can have two bluetooth connection simultaneously on an ESP32.   

## Installation instructions   

Using Arduino IDE, load the programs onto the two Sticks (one program each).   

## Operating instructions

Turn off both Sticks  
Turn on the Amp  
Turn on the Stick running 'Stick to Amp'. 
Wait until Connected  
Turn on the other Stick  
Turn on the App - pair over bluetooth, then 'Connect' in the App   

Watch the messages scroll by  
``` < ```  is from amp to app    
``` > ``` is from app to amp    

Enjoy
