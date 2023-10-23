# Spark2020-2021
~~Ice Cold Beer~~ 
Remy's Quest :mouse: :cheese:

#### How to simulate this project
See [Getting Started with Wokwi for VS Code](https://docs.google.com/document/d/1XQ6oEajXWh64AKem3sj6RQL7GPWH-MVa7xeWXK2pB6A/edit#heading=h.9a71xmgofye9) for a detailed guide.

TL;DR version:
1. Install VSCode
2. Install python3
3. Install the following VSCode extensions: "Wokwi Simulator", "PlatformIO IDE"
4. Follow the setup steps to request a free wokwi license
5. Linux: Install python3-venv as a pre-req for platformio (`sudo apt install python3-venv`)
6. PlatformIO will look for files in the `src` folder to compile. Click on `src/main.cpp`, then click the checkmark at the bottom of the screen to verify your code (next to the house button)
7. If you get red squiggles under the `#include` directives, go to PlatformIO Home (house button) > Libraries > search for and install the library to the Spark2020-2021 project
8. When the compiler shows `SUCCESS`, run the simulation with `ctrl-shift-p` `Wokwi: Start Simulator`
