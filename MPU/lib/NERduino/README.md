# NERduino
Contains the base code for all NERduino applications

Handles board peripherals and onboard sensor processing

#### TODOs
1. CAN ID filtering
2. ~~Finish testing of SHT30~~
3. Finish Implementation of SHT30
4. Rework ADXL312
5. Finish testing of AMC6821
6. Finish Implementation of AMC6821

### NERduino Library Overview of Usable Function
```
NERduino object
|
|───private:
|   |───ADXL312 object
|   |───AMC6821 object
|   └───SHT30 object
|
└───public:
    |───nerduino()      # calls Wire.begin() and Serial.begin(9600)
    |───~nerduino()
    |───begin()         #calls the constructors of each chip (returns false if a chip is not found)
    |───getADXLdata(XYZData_t *xyzbuf)  # fills an XYZDATA_t buffer
    └───...more to be added
```

```
CAN Functions (currently not bound to an object)
|
|───sendMessage(uint32_t id, uint8_t len, const uint8_t *buf)   # sends a CAN Message w/ params
|───initializeCAN(uint8_t canLine)      # initializes the CAN functionality of node
|
|───canHandler_CANMSG_BMSSHUTDOWN(const CAN_message_t &msg)         # specific CAN handle commands
|───...
|───canHandler_CANMSG_MC_BMS_INTEGRATION   (const CAN_message_t &msg)
```

## Code Overview
#### ***Note*** : the NERduino.begin() function must be called to read chip data

### CAN Message Handler
The master CAN handler files allow for all CAN messages on all nodes to be processed using the same function and a large switch statement. It also allows for configurable CAN ID's that are saved in each node's EEPROM.

To send a message on the CAN network, use the ```sendMessage(uint32_t id, uint8_t len, const uint8_t *buf)``` function.

To add fuctionality to a certain received CAN ID, simply implement one of the following functions that correspond to each CAN ID, the compiler will know to use the function you created:
```
void canHandler_CANMSG_BMSSHUTDOWN          (const CAN_message_t &msg)
void canHandler_CANMSG_BMSDTCSTATUS         (const CAN_message_t &msg)
void canHandler_CANMSG_SET_INVERTER         (const CAN_message_t &msg)
void canHandler_CANMSG_SET_CARDIRECTION     (const CAN_message_t &msg)
void canHandler_CANMSG_SET_BRAKELIGHT       (const CAN_message_t &msg)
void canHandler_CANMSG_ERR_BRAKESWITCH      (const CAN_message_t &msg)
void canHandler_CANMSG_ERR_PEDALSENSOR      (const CAN_message_t &msg)
void canHandler_CANMSG_CARACCELERATION      (const CAN_message_t &msg)
void canHandler_CANMSG_BRAKEFLUIDPRESSURE   (const CAN_message_t &msg)
void canHandler_CANMSG_COOLINGFLOWRATE      (const CAN_message_t &msg)
void canHandler_CANMSG_GPSDATA              (const CAN_message_t &msg)
void canHandler_CANMSG_DIFFTEMP             (const CAN_message_t &msg)
void canHandler_CANMSG_ACCELERATIONCTRLINFO (const CAN_message_t &msg)
void canHandler_CANMSG_MOTORTEMP1           (const CAN_message_t &msg)
void canHandler_CANMSG_MOTORTEMP2           (const CAN_message_t &msg)
void canHandler_CANMSG_MOTORETEMP3          (const CAN_message_t &msg)
void canHandler_CANMSG_MOTORMOTION          (const CAN_message_t &msg)
void canHandler_CANMSG_MOTORCURRENT         (const CAN_message_t &msg)
void canHandler_CANMSG_MOTORVOLTAGE         (const CAN_message_t &msg)
void canHandler_CANMSG_MCVEHICLESTATE       (const CAN_message_t &msg)
void canHandler_CANMSG_ERR_MCFAULT          (const CAN_message_t &msg)
void canHandler_CANMSG_MOTORTORQUETIMER     (const CAN_message_t &msg)
void canHandler_CANMSG_BMSSTATUS2           (const CAN_message_t &msg)
void canHandler_CANMSG_BMSCHARGEDISCHARGE   (const CAN_message_t &msg)
void canHandler_CANMSG_MC_BMS_INTEGRATION   (const CAN_message_t &msg)
```


### ADXL312 Accelerometer
[ADXL312 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL312.pdf)

A small, 3 axis accelerometer that we are using to get acceleration data. Currently, the acceleration data is being retrieved through ```getADXLdata(XYZData_t *xyzbuf)``` which retrieves the XYZ register data through the ADXL specific reading and writing code.

##### **XYZData_t** Data type for ADXL312
```
XYZData_t
|───.XData.data  # Each of these are signed 16 bit integers with axis data
|───.YData.data
└───.ZData.data
```


### SHT30 Humidity Sensor
[SHT30 Datasheet](https://www.mouser.com/datasheet/2/682/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital-971521.pdf)

A small temperature and humidity sensor we are using to get the humidity and temperature of each node. The data can be accessed through ```getSHTdata(HumidData_t *humidbuf)```*(WIP)* which retrieves a buffer of humidity and temperature data through SHT specific read and write commands.

##### **HumidData_t** Data type for SHT30
```
HumidData_t
|───.tempC       # Ambient Temp in Celsius float
|───.tempF       # Ambient Temp in Fahrenheit float
└───.relHumid    # Relative Humidity float
```


### AMC6821 PWM Generator
[AMC6821 Datasheet](https://www.ti.com/lit/ds/symlink/amc6821.pdf?ts=1644706226375&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FAMC6821%253Futm_source%253Dgoogle%2526utm_medium%253Dcpc%2526utm_campaign%253Dasc-sens-null-prodfolderdynamic-cpc-pf-google-wwe%2526utm_content%253Dprodfolddynamic%2526ds_k%253DDYNAMIC%2BSEARCH%2BADS%2526DCM%253Dyes%2526gclid%253DCj0KCQiA0p2QBhDvARIsAACSOOPKQVP7tfyxbaC8997ZjeHcQWZiSwAi1yblV-rFrJZ4BQS3xCwo1iYaAjmLEALw_wcB%2526gclsrc%253Daw.ds)

A pulse width modulation chip specifically designed for intelligently controlling fan behavior. In the NERduino, howeveer, all fan-related functions of the chip are ignored, and the chip is solely used for generating a PWM signal. The duty cycle can be set via ```setDutyCycle(uint8_t dutycycle)``` *(WIP)* which sets the PWM Duty Cycle to a value between 0-255. The functionality of this chip may differ per device due to debug flags.
