## System Overview of Car
The computer architecture of the car consists of 4 "nodes", and each node communicates to eachother by sending messages along the CAN bus. CAN, or controller area network, is an industry standard communication protocol, especially used for large system applications, such as automotive and robotics. Think of the car's version of LAN (Local area network). The hardware of the CAN bus is two lines, a CAN High line and a CAN Low line with 120 ohm resistors at the end of the lines and each . This is why some people call it two wire communication. A simple diagram of the system can be seen in the UML diagram below. [See here for a more detailed explanation of CAN](https://www.allaboutcircuits.com/technical-articles/introduction-to-can-controller-area-network/).

![Testpic](https://github.com/Northeastern-Electric-Racing/Embedded_Code/blob/ndepatie-dev/system.drawio.svg)

### Master Processing Unit (MPU)
Handles all essential processes for normal car functionality, from handling button presses to monitoring the integrity of the CAN line. There is a lot of logic and states for this, so it might be best to just jump straight into the code for how it works on a basic level.

### Telemetry Control Unit (TCU)
Handles logging and offboarding of all CAN data, is being improved to connect wirelessly, giving wireless access to the CAN network
