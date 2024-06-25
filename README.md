# M030GBSP_I2C0_Slave_I2C1_Master
 M030GBSP_I2C0_Slave_I2C1_Master

udpate @ 2024/06/25

1. scenario : 

	- use UART0 (PA1 : TX , PB12 : RX) for printf

	- initial I2C0 as slave , PF3 : SCL , PF2 : SDA 
	
		- slave will return previous data if master send read command
	
	- initial I2C1 as master , PA3 : SCL , PA2 : SDA
	
2. if enable define : DEBUG_LOG_SLAVE_LV2 , will display more slave log message , however will spend more time between each byte
	
	![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/log_DEBUG_LOG_SLAVE_LV2.jpg)
	
	
3. press digit 1 , to send I2C master write command , 

press digit 2 , to read I2C master data from I2C slave


![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/log_digit_1_digit_2.jpg)


![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_1_digit_2.jpg)


4. when press digit 1 , below is LA capture 

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_1_0.jpg)

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_1_1.jpg)


5. when press digit 3 (send I2C master write command) , below is LA capture and log 

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_3.jpg)


6. when press digit 4 (send I2C master write command) , below is LA capture and log 

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_4.jpg)


7. when press digit 5 (send I2C master write command) , below is LA capture and log 

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_5.jpg)

	
8. press digit 6 , to send I2C master write command , 

press digit 7 , to read I2C master data from I2C slave


![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/log_digit_6_digit_7.jpg)


![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_6_digit_7.jpg)


9. when press digit 8 (send I2C master write command and read command) , below is LA capture and log

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_8_0.jpg)

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_8_1.jpg)

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/log_digit_8.jpg)

if enable define : DEBUG_LOG_SLAVE_LV2 ,

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/log_digit_8_DEBUG_LOG_SLAVE_LV2.jpg)


10. when press digit 9 (send I2C master write command and read command) , below is LA capture and log

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_9_0.jpg)

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/LA_digit_9_1.jpg)

![image](https://github.com/released/M030GBSP_I2C0_Slave_I2C1_Master/blob/main/log_digit_9.jpg)

