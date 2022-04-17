# Embedded_Code
Contains all the code for the embedded systems found within the car as of the 2022 competition

Built off of the [NERduino](https://github.com/Northeastern-Electric-Racing/NERduino) library

## System Overview of Car
![Testpic](https://github.com/Northeastern-Electric-Racing/Embedded_Code/blob/ndepatie-dev/system.drawio.svg)

### Overview of Repo
```
.
|
|───MPU (Master Processing Unit)
|───TCU (Telemetry Control Unit)
|───Watchdog
|
|───Legacy Code
|   |───Brakelight
|   |───Dashboard
|   └───PedalBox
|
└───README.md
```

### Master Processing Unit (MPU)
Handles all essential processes for normal car functionality

___Found in ```MPU``` directory___

### Telemetry Control Unit (TCU)
