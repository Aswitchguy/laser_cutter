# laser_cutter
The teensy code for my DIY arduino laser cutter.

The main controller is the teensy 4.1. This vould be substituted for other things, but having a really good microcontroller
is good for these applications because it allows for more accurate step timings as well as increased ability for look-ahead and
g-code processing.

The secondary controller in this case is a raspberry pi pico. This can also be substituted for other microcontrollers,
but the decent clock speed and larger pinout than most Arduino's allows for an increased accuracy with step times.

When running gcode, the teensy processes it, and then sends commands to the pico in raw binary to increase speed. The code on
the pico takes this binary, and converts it to a command for the motor sets. In my configuration, I have 1 X-axis motor, 2 Y-axis
motors, and 4 Z-axis motors. Each of these is controlled by an Axis class on the pico that can pulse each motor individually in
a given axis, but enable and direction are controlled for the Axis, as having motors on the same axis go in different directions is
a recipe for disaster. 

