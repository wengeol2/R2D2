# R2D2
Code for Arduino in R2D2

The model for 3D Print can be found here: https://www.cgtrader.com/3d-print-models/miniatures/figurines/r2d2-hq-print-3d-new-hope-42cm

The model download (code part) includes the sound files needed for this project, although you will have to convert them from .wav to .mp3, remove the ones you don't want (I removed the ones with spoken or too much background noise) and renumber the files to be in 0-nn.mp3 sequence

When pickip up an Arduino Nano get one with the ATmega328P chip. I had to set Arduino Studio to use the old bootloader to get the app to load
- For the lights I am NeoPixel compatible Smart-Pixels from Blink Stick: https://shop.blinkstick.com/collections/blinkstick-pro-accessories/products/smart-pixels
- Servo is a standard High Torque MG996R Servo (10km/cm). This servo is pretty loud, so if you know of a better one to use please send me a quick message
- Audio Conroller: DFRobot DFPlayer mini MP3 for Arduino 
- SD Card to hold the MP3 files
- Speaker is 50mm 40Ohm, 3W and fits perfectly into the holder
- 2x 1k resistors between Arduino and MP3 to remove clicking sounds
- small breadboard to make wiring from Arduino and MP3 player easier

For Power I am using a small powerbank and soldered new connectors to a USB cable to power lights, servo and Arduino. The USB cable needed to have a 90° angle to fit into the head when plugged into the power-bank
