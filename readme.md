# Hallway Light Strip (Version 1)
***IOT Motion detecting hallway lights***<br>
Still a work in progress, but it's behaving how it's supposed to.
See here for the [lastest version](https://github.com/T800Fox/IOT-Light-Strip-V2).
## Features
- RGB colour control via. MQTT.
- Detects light level within the space, only coming on if it's dark.
- Queries Sunrise Sunset API to ensure lights only come on during the evening.
- Adjustable keep-on-time.
## Components
- WS2812 RGB LED Strip
- Wemos D1 Mini
- 3x PIR Sensors --> Motion Sensing
- Push button --> Manual ON/OFF Control
- LDR --> Light Level Sensing
## The Code
- Developed for a ESP8266 Chip with Platformio.
- Setup to accomadate extra motion sensors.
- Functionalities broken into local libraries, following OOP as much as possible.
