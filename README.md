# ArduinoBombDefusal game

This project aims to create a defusable bomb with an Arduino Uno.
It was realised in french engeneering school EPSI.

Do not hesitate to get in touch in case you'd like to contribute!

# Objectives

Create a defusable bomb game with an Arduino.

# Hardware :
- Arduino uno
- 2 * 16 screen
- 4 * 3 keypad
- buzzer
- miscellaneous parts (leds, resistor...)

see bomb_defusal/bomb_scheme.png to have a complete hardware overview

# Repository content

- bomb_scheme/bomb_scheme.png to get an overview of the bomb's hardware
- bomb_defusal/bomb_defusal.ino  : contains the main code
- libraries : the third part libraries


# Main bomb features

Play this game in two parts :
- configuration :
- - select game difficulty (change timer duration and defuse tries)
- - define the wire that powers the bomb
- defuse it! :
- - find the bomb code
- - cut the wire that powers the bomb! (you can cut it before finding the code)

# usefull links :
- http://fritzing.org/home/
- http://www.arduino.cc/en/Guide/Libraries

# Other similar projects :
- http://www.instructables.com/id/Arduino-defuseable-bomb-perfect-for-airsoft-games-/
