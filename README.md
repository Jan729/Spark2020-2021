~~Ice Cold Beer~~ 
# Remy's Quest :mouse: :cheese:

## Table of Contents
1. [Intended Gameplay](#intended-gameplay)
2. [Build Tips](#build-tips)
3. [Build Schematics and Tutorials](#build-schematics-and-tutorials)
    - [TODO Power Supply and Pin Count](#todo-power-supply-and-pin-count)
    - [Limit Switches](#limit-switches)
    - [Motors and Motor Drivers](#motors-and-motor-drivers)
    - [Joysticks](#joysticks)
    - [LEDs](#leds)
    - [Hex Displays](#hex-displays)
    - [Start/Reset Button](#startreset-button)
4. [How to Simulate this Project](#how-to-simulate-this-project)
5. [How to Run the Code on a Real Arduino Mega](#how-to-run-the-code-on-a-real-arduino-mega)


## Intended Gameplay

Press the Start button to start the game.

The first “target hole” LED, Hole #1, flashes until the ball is dropped into Hole 1, where the ~~IR sensors~~ limit switches detect if the ball passed through that hole. If the ball is dropped into Hole 1, the bonus score is added to the current player score, the bar automatically lowers to the bottom, the ball is returned to the bar, and Hole #2 flashes, indicating the start of level 2. The order of the target holes remains the same, from Hole 1, Hole 2, etc until Hole 30. Repeat until you reach hole #30. 

You earn points when you drop the ball into the target hole. Points are added to your player score on the hex display. The faster you complete each level, the more points you earn. See Hex Displays for the score calculation.

When the player puts the ball into hole #30 at level 30, they win the game. The hex displays should flash in a 'win' pattern to indicate you won, and the game automatically resets. 

If the ball drops into any hole other than the target hole at ANY time, you lose, the hex displays should flash in an 'oops' pattern, and the game resets.

Press the Start button at any time to reset the game to level 1.

## Build Tips

1) Securely ground ALL your circuit components before turning the power on. Use the "tug test": your wires (soldered or crimped together) should not fall out of their connections if you gently tug on it with your hands.

2) Do not connect the Arduino via programming cable into your computer AT THE SAME TIME AS the power supply. If you ignore this tip and turn on the power supply, it may fry the USB port on your computer.

3) If your circuit is not working as intended, check all your connections with a multimeter set to "continuity" mode, and get a friend to review your circuit placement and/or code. Sometimes the problem is as small as a breakage inside a wire or a loose connection.

4) If you can't find a PCB diagram, try looking for the faint lines in the pcb where the traces are. There are usually a top and a bottom layer of lines. Flip the PCB over to see the second layer of lines. That should show you how the through-holes are connected. You can also use a multimeter to check which holes are connected.

5) To replace/reposition a hot-glued component on the backboard, DO NOT scrape the glue as the wood will peel. Touch a hot glue gun tip to the glue to melt the glue, just like desoldering.

## Build Schematics and Tutorials

### I/O Diagram

[Click here to view I/O diagram](https://drive.google.com/file/d/1cqpRm-2tPJbcKSMdjFogfon1K79lBqss/view?usp=sharing)
As of 2025, we use limit switches instead of IR Sensors so ignore IR related diagram.

### I/O Wiring Explanation

Walk to the back of the display and face the wires on the backboard. Hole numbers are written in pencil on the backboard wood. The LEFT side contains (1) the LED VCC wires, (2) the limit switch VCC wire the IR receiver DATA wires, (3) the IR emitter VCC wires. Some wires that belong to the same node, like the LED VCC wires, are merged together to save wire. The RIGHT side is common ground.

#### TODO Power Supply and Pin Count

The computer power supply (the grey box) is for testing only and does not meet fire safety standards for long-term use. You will need to replace this power supply with the new power supply to the arduino, PCBs, and motor drivers. There is a new 12V power supply reserved for Remy's in the spark garage. Janelle is not sure where the regulator is.

12V goes to the arduino and motors, 3V or 5V goes to everything else depending on the component. For subsystems that cost more than a few dollars or a few minutes of labour to replace, Janelle recommends splicing a fuse with appropriate current rating into the wire between the regulator and the subsystem's VCC. For example, if the datasheet says the circuit will fry at >1A, pick a 1A fuse.

##### Power supply requirements
Class 2 fused 12V power supply. Keep it outside of the display, away from flammable materials. See diagram, where `<--->` are wires.
```
Wall outlet <---------> power supply with exposed 12V port <---> 12V to 3V regulator <---> Remy’s build
```


##### Overall Arduino Mega Pin Count
- Motors – Assuming stepper motor for ball return, 6 digital driver pins (step and dir x3 drivers, right bar, left bar, ball return)
- Limit switches – 1 analog, 6 digital (for muxes)
- LEDs + Shift Register Select – 3 digital
- Reset/Start button - 1 digital interrupt pin
- Left joystick - 2 digital pins (up and down)
- Right joystick - 2 digital pins (up and down)
- Hex displays () - 4 digital pins total, 2 per 4 digits, CLK and DIO

TOTAL PINS: 6 + 7 + 3 + 1 + 2 + 2 + 4 = 25

#### Limit Switches

Below link is viewable by Spark 2020-2021 exec team:

[IR Sensor–Mux Circuit Documentation by Ginny](https://docs.google.com/document/d/12kzbL69qe4iZLc2-gt9O6dBEr0O6Me1dWEzLK4bp4YY/edit?usp=sharing)

The mux chip is CD4051B. If you want to simulate it in wokwi, copy the custom chip implementation from this demo:

[CD4051B Demo](https://wokwi.com/projects/343522915673702994)

#### Motors and motor drivers

There are three motors, one to control the left side of the bar, one to control the right side of the bar, and one to operate the cam mechanism that returns the ball to the bar after the ball falls through a hole.

##### Bar motors and drivers

The bar can move straight up if you hold both joysticks in the 'up' position, and vice versa for down. If the bar is at an angle, the code will prevent the bar from tilting too much to prevent the bar from breaking.

[Stepper motor datasheet](https://www.omc-stepperonline.com/download/17HS16-2004S1.pdf)

[Motor Driver TB6600 datasheet](https://bulkman3d.com/wp-content/uploads/2019/06/TB6600-Stepper-Motor-Driver-BM3D-v1.1.pdf)

###### Wiring

1) Small wire to short DIR- and PUL- together
2) DIR+ green wire goes to arduino DIR_L or DIR_R pin specified in code
3) PUL+ light blue wire goes to arduino STEP_L or STEP_R pin specified in code
4) GND to GND
5) B- to Blue stepper motor wire **CAUTION If you reverse this order, the code will run the motors backwards, with up/down swapped**
6) B+ to Green stepper motor wire
7) A- to Red stepper motor wire
8) A+ to Black stepper motor wire
9) VCC to 12V on power supply
10) As of Mar 19 2024, upload the file bar_motor_test.ino  to the arduino

The lead screws are lubricated. Don't touch them. Myfab has extra lubricant if you need more.

Tip: If you notice a terrible squealing noise after modifying the code, the bar mechanism squeals when the motor is accelerating or decelerating at a *very* slow rate (ie 1 RPM, just before the motor stops to change spin direction). Increase the total acceleration in your code to prevent the AccelStepper library from holding the motor at a low acceleration for more than a fraction of a second. Lubrication does NOT affect the squealing.

###### TO TEST

The bar should lower itself by a few cm automatically if nobody has touched the controls in the last few seconds. When the next person arrives to play the game, the bar is already at the bottom and they can begin playing a little sooner. It also incentivizes the player to keep playing instead of standing idle.

###### Motor position calculations

- clockwise stepper rotation = positive accelstepper position = move bar up

- 30.5" from ball return to last hole = 775mm
- 7" = 178mm from ball return to start of playing area. not sure where visible area is
- 2mm of vertical bar movement per rev
- 178mm / 2 = 89 revs from ball return to start
- 388 revs from ball return to last hole
- Guess: 76mm = 38 rev max height difference between L and R ends of bar

- AccelStepper Library Documentation

  https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html


##### Ball return motor

###### TODO

As of early 2024, the ball return motor depends on the final mechanical design of the ball return mechanism. Please update this section once the motor is chosen (ie servo, stepper, or DC).

#### Joysticks

The two joysticks control the right and left sides of the bar, respectively. The joysticks can move the bar up or down. To prevent the bar from breaking, the joystick code will automatically stop the bar from moving if one side of the bar is too high. The sideways joystick movement does not do anything.

##### Joystick Wiring

Each joystick direction has a switch connected to a pair of wires. One side is always grounded. The up and down signal wires (yellow/green) for each joystick are twisted together. We are using an active low setup for both up and down switches. Active low means one wire is connected to ground, and the other goes to the arduino to read for a `LOW` signal. 

#### Leds

##### LED Wiring

LEDs are connected to the "SPARK" PCB. This PCB has 4 shift registers with 8 output pins each. We have 30 LEDs. Reece designed the PCB, he may be able to answer questions about the design.

#### Hex displays

There are two hex displays: current player score and all-time high score.

These tutorials may help you wire the hex displays:

[Interfacing TM1637 4-Digit 7-Segment Display with Arduino](https://simple-circuit.com/interfacing-tm1637-display-module-with-arduino/)

[Display de 7 Segmentos TM1637 (the best wokwi one I found, but it's in spanish)](https://wokwi.com/projects/378335118444269569)

##### Hex display scoring system

1) Starting from 100 bonus points, for each level, the max bonus increments by 10 points, and drops by 10 incrementally, every 3 seconds
2) At the end of each round, the level bonus is added to the current score
3) At the end of each round, the current score is compared with the highest score such that if the top score has been exceeded, the current player’s score will replace the historic top score

###### TODO - code written but not tested yet

The all-time high score should be stored in non-volatile memory to prevent loss when unplugging the display.

#### Start/Reset button

This button controls the flow of the game. Pressing it while the game is "idle" triggers the variables to reset and begin level 1. Pressing the button during the game play will trigger an interrupt that will reset the game back to level 1. The bar should move such that it will dump any ball remaining on the bar into the backboard. Then the ball return mechanism should return a new ball onto the bar.

[Start button flow diagram as of 2023](https://mermaid.live/edit#pako:eNptkU1zgjAQhv_KupfSGbQIAi2HOipqvXXG9tLSQ4T1Y4TECWFaqvz3RrAdmWkOmey-b57dbI4Yi4QwwHUqPuMtkwpewoiDXiNjkaQEmdZvodt9hFPnkLJyxzdzltEJRhdbo7Wk8ftSadSdpJwUrJg09ZamJrwuTMhjIckEtctIfjSMcc2YGM8NBDaa0hRu9Mk_NSYtqd1aaEx58ke5QMLaOT0-sRzUluqOQJIqJKdkWDWmaYNbEcvGUuyJn2B2PYeWq3NtmxszIeMLVglgvIStSGkIoeA3Cn5noVVOX6p-__DS2rzmzSJ-DtFEPZmM7RL9LcdzJkLdb0YRBvqYMLmPMOKV9rFCiWXJYwyULMjE4pAwReGObSTLMFizNNfZA-NvQrRiDI74hUHXth2r1x-4ju0NPN-yfM_EEgPf8nqO7_iufW97dt91KhO_a4TVe_DtgTXo65uu7VlWv_oBVUWv-Q)

## How to Simulate this Project
See [Getting Started with Wokwi for VS Code](https://docs.google.com/document/d/1XQ6oEajXWh64AKem3sj6RQL7GPWH-MVa7xeWXK2pB6A/edit#heading=h.9a71xmgofye9) for a detailed guide.

TL;DR version:
1. Install VSCode
2. Install python3
3. Install the following VSCode extensions: "Wokwi Simulator", "PlatformIO IDE"
4. Follow the setup steps to request a free wokwi license. If your license expires, you can request a new one with the same email
5. Linux: Install python3-venv as a pre-req for platformio (`sudo apt install python3-venv`)
6. PlatformIO will look for files in the `src` folder to compile. Click on `src/main.cpp`, then click the checkmark at the bottom of the screen to verify your code (next to the house button)
7. If you get red squiggles under the `#include` directives, go to PlatformIO Home (house button) > Libraries > search for and install the library to the Spark2020-2021 project
8. Go to `global-variables.h` and change the directive `IS_WOKWI_TEST` to `true`. This enables print statements and extra buttons for debugging.


```
#define IS_WOKWI_TEST true
```

9. Click the checkmark (PlatformIO:Build) at the bottom of vscode to compile. When the compiler shows `SUCCESS`, run the simulation with `ctrl-shift-p` `Wokwi: Start Simulator`.

### How to modify wokwi diagram.json

You can edit the diagram.json two ways.

Option 1: Manually edit the diagram.json in vscode by right clicking `diagram.json` >  Open With > Text Editor. Parts are listed in `parts`, and wires are listed in `connections`.

Option 2: Go to wokwi.com, make a New Project, then copy and paste the `diagram.json`. You can then use the web GUI to re-position and copy `parts`. To add `parts` and `connections`, you'll need to manually edit the json file.

#### How to add code to this project
Put your code in the src/ folder (otherwise the simulator will not find your code).

To add another file with helper functions:
1. Create a new file in src/ and add the following
  ```
#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"
```
2. Declare any variables in `main.cpp`
3. To make the variable accessible across files, define the variables again with `extern` in `global-variables.h`
4. Declare function prototypes in `functions.h`
5. Write your functions in the new file you created

### How to add libraries to a platformio project

1. Click on the house icon on the bottom of vscode. This is platformio home page.
2. Click on "Libraries" on the left sidebar
3. Search for the name of the library you want to add
4. Click on your desired search result
5. Click "Add to Project" > "Select Project" > "Spark2020-2021"
6. The new library should be listed in the `platformio.ini` file

## How to Run the Code on a Real Arduino Mega

1. Go to `global-variables.h` and change the directive `IS_WOKWI_TEST` to `false`:

```
#define IS_WOKWI_TEST false
```

2. **IMPORTANT** - If you have not stored the bar motors' last positions in non-volatile memory, estimate the position of the left and right bar motors. CEILING is the top of the backboard. FLOOR is just below hole #1. Hardcode the positions into `calibrateBarPosition()`

**If you don't set the motor positions before uploading the code, the motors might break the bar :O**

3. Click the checkmark (PlatformIO: Build) and the arrow (PlatformIO: Upload) icons to upload the code to an arduino. The arduino will remember the code even if you disconnect the power

4. If you need to run the setup() again, press the little rubber button on the arduino. The arduino will stop whatever it was doing and run the setup() then loop() again.

Tip: If you're running out of RAM on the mega, comment out the print statements, or wrap the prints in `#if IS_WOKWI_TEST` and `#endif` directives