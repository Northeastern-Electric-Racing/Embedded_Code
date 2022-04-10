### Pedals
1. ~Two sensors for brake and accelerator (error checking)~
2. Do something with accelerator and brake faults
3. ~Sending MC messages~
4. Brakelight
5. ~Take in temp and derate maximum torque based on temp~

### DriverIO
1. ~SS Button(Turn on car by sending MC message)~
2. ~Direction Switch (switch direction by sending MC message)~
3. ~SS Button LED (On when car on)~
4. Two programmable LEDs (Low Battery and Noncritical fault __blink code__)
5. ~Speaker (Buzzes when car turns on)~
6. SS button controls charging
7. While charging, turn SS button green
8. ~SS sen must be high to turn on motor controller (Still can turn on)~ 

### GPIO
1. PWM Out?
2. ~SS Sensor (Clear fault)~
3. ~CAN network~

### BMS
1. ~Get SOC via accumulator message~
2. Get AVG Temp via BMS Status
