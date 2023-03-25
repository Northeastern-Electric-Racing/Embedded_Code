# Master Processing Unit (MPU)
Handles all essential processes for normal car functionality


### Overview of Architecture

![TestPic](https://github.com/Northeastern-Electric-Racing/Embedded_Code/blob/ndepatie-dev/docs/mpu.drawio.svg)

```main.cpp``` is the entry point for the program. From here, the ```gpioProcess()```, ```driverioProcess()```, and ```pedalsProcess()``` is called to handle each respective group of 
functionalities of the car. Inside the ```mpu``` object, there is a ```bms``` and ```motorController``` object that is defined and then passed to the ```pedals```, ```gpio```, and ```driverio```
objects. This means that all the functions of the car can modify and retrieve the current state of the motor controller and bms (i.e. all parts of the car can retrieve if the car is moving forward
and can modify it as necessary). This is utilized to send the acceleration message to the motor controller, where the ```sendMCMsg()``` is called on each loop in ```main.cpp``` to send the message that
each process modifies (all the functions "load" the message with the desired info, and then this function sends the message).

CAN messages are dealt with in a master CAN message handler defined in the NERduino library, which uses a large switch statement to correctly route the correct ID to the correct function. The way to create a new CAN message can be found [here](https://github.com/Northeastern-Electric-Racing/NERduino/tree/main).

The grouping of functionality into multiple objects was a design choice made to organize the code, make it more distributable, and easier to understand on a high level. This approach also gives newer members an insight into a higher level of C++ that is used in industry embedded applications and firmware. 

***USEFUL TIP*** In newer versions of VSCode, you can ```Ctrl+click``` or left click on a function or object to go to it's definition or references, which makes navigating the code a lot easier.

***USEFUL TIP*** The code is organized so that it gets less abstract as you go into each function, so start in ```main.cpp``` for the highest level of what the code does, and then go into the ```gpioProcess()```, ```driverioProcess()```, and ```pedalsProcess()``` to see what they do, and then go into their respective functions.

***USEFUL TIP*** If you are looking for documentation on what each function does in the code, the header files (```.h``` files in the ```/include``` directory) contain a lot of comments and an overview of what is in each class 


## TODO:
1. Update Documentation
2. Fix faulting logic
3. Radiator Fan PWM control circuit