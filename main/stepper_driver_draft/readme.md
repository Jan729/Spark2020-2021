Contains code for bar mechanism
(Will copy over to main.ino when finished testing)

# How to setup the bar motors

CAUTION: ECE doesn’t know which motor “direction” actually corresponds to “up” or “down” - so please check the motor rotation (CW or CCW) before attaching it to your bar to prevent the motors driving the bar down into the floor when the code thinks the bar is going up

## Stuff you’ll need

- Arduino
- Power supply
- Use the one that has an arduino charger thing spliced onto it
- It may not have a power button - that’s ok, u can plug the power supply into a power bar and use that as an on/off switch
- Jumper cables
- Male - male wires
- Masking tape or something to keep the power supply’s ground, red 5V, and yellow 12V exposed wires from touching each other
- Laptop with arduino installed
- Programming cable
- 2 joysticks OR some wires and a breadboard connected to the arudino ground to mimic the joystick circuit
- The joysticks work by leaving the up or down pins floating by default. When you push the joystick up, the “up” pin is connected to ground. When the arduino code sees the pin is LOW (ground), it will move the motor in that direction
- 2 stepper motors (in the remy’s box)
- Make sure it does NOT say “this is the ball return motor” - ball return motor has different specs than the ones I adjusted the motor drivers with
- 2 stepper drivers
- **Careful**: there are 3 drivers in the box. One is labelled “broken” because I haven’t finished debugging that one. The other two worked when tested
- Don’t solder the wires onto the motor drivers yet until we’re ready for final assembly. In the past, moving around drivers/motors in a box leads to soldered wire joints breaking
- You can hook a little “J” shaped piece of solidcore wire (see picture) around the connections and attach jumper cables to the wire

## Code to run bar motors

https://github.com/Jan729/Spark2020-2021/blob/master/main/stepper_driver_draft/stepper_driver_draft.ino 

#### Circuit setup

The #define variables define your arudino pins.

Either attach these pins to your joystick wires or attach a wire that you can touch ground with to mimic a joystick press:
If L UP and L DOWN are both ground, i think the code will just tell the motors to do nothing
L_UP 6
L_DOWN 7
R_UP 8
R_DOWN 9

The arduino needs two pins for each motor: step and direction. Read the PCB label to find the correct wire.

Right motor
DIR_R 2
STEP_R 3

Left motor
DIR_L 4
STEP_L 5

**Important: to connect the motors to the motor drivers, check the color of the wires in the pictures below.**

- Red and green go together
- Blue and yellow go together

- It doesn’t matter if the red/green goes to A1/A2 or yellow/blue goes to A1/A2 as long as those colors paired together

- I don’t know what will happen if you accidentally pair the wrong colors together…i hope we don’t find out

- You can ignore the SWAP variable. That’s used for commented out debug code

#### Code constraints to stop the bar from tilting too much

- You can only move one motor at a time. Release the other joystick to move the other motor
- If the code thinks the bar is at the bottom or top of the playing area, it will stop the motors from continuing to move in one direction even if you hold the joysticks down
- If the motors seem to randomly stop moving you can try tilting the joystick in the opposite direction
- See comments in code for number of revolutions I estimated to reach the top of the playing area
- I also guess that to prevent the bar from tilting too much and snapping, if the difference between the L and R motor positions is >= 38 revolutions, the motors will stop moving towards a more tilted bar position unless you make the bar more “horizontal” again

Pictures in doc here: https://docs.google.com/document/d/1C26doHakzTLT27pgkO1CtdtIsKdsgBkVAXNBJU7FRAE/edit?usp=sharing
