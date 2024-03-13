# Bluetooth based Health Thermometer
For the ECEN 5823 Internet of Things and Embedded Firmware (IoTEF) course project, I have chosen to implement a Bluetooth Smart with a server and a client model. The project focuses on addressing the growing need for remote patient monitoring in healthcare, leveraging Bluetooth technology to transmit vital health data from patients to healthcare practitioners in real-time.

## Motivation
Remote patient monitoring has emerged as a crucial aspect of modern healthcare, enabling continuous monitoring of vital signs, symptoms, and medical conditions. One significant aspect of patient health monitoring is the relationship between body temperature and heart rate, particularly in children. Studies have shown that abnormal fluctuations in body temperature can significantly impact heart rate, potentially leading to severe medical complications if left unattended. Thus, our project aims to provide an effective solution for continuous temperature monitoring, coupled with immediate heart rate assessment, to facilitate timely medical intervention.

## Project Objectives
- Continuous Temperature Monitoring: Implement a system capable of continuously monitoring patient temperature and detecting abnormal fluctuations.

- Immediate Heart Rate Assessment: Integrate a heart rate monitoring device to provide immediate assessment in response to abnormal temperature readings.

- Efficiency and Security: Develop a system that ensures efficient data acquisition, conserves power, and prioritizes data security and privacy.

## High-Level Requirements

- System Components: The project will consist of a BLE server and a BLE client, both running on Blue Gecko Boards.

- Encrypted Link: The server will establish an encrypted link with the client via bonding to ensure data security.

- Temperature Monitoring: The server will periodically read temperature values from an external sensor using LETIMER0.

- Custom GATT Service: The server will implement a custom GATT service with a characteristic to send temperature values.

- Data Transmission: Temperature measurements will be sent to the client only if they exceed a predefined threshold.

- Power Efficiency: The server code will maintain the lowest possible energy mode for optimal power consumption.

- Heart Rate Monitoring: The client will receive temperature values from the server and read heart rate values from a sensor.

- Data Exchange: The client will write heart rate values to the GATT database, which the server can read.

- Display and Alert: Both server and client will display temperature and heart rate values on LCD displays, with LEDs indicating abnormal readings.

## High-Level Design

![Hardware block diagram](https://github.com/Shri2401/Bluetooth-Low-Energy-based-Health-Thermometer/blob/main/questions/screenshots/Hardware_Block_Diagram.png)

![Software block diagram](https://github.com/Shri2401/Bluetooth-Low-Energy-based-Health-Thermometer/blob/main/questions/screenshots/Software_Block_Diagram.png)

## Server_Client Desisgn
![GATT_Server block diagram](https://github.com/Shri2401/Bluetooth-Low-Energy-based-Health-Thermometer/blob/main/questions/screenshots/GATT_Client_Block_Diagram.png)

![GATT_Client_block diagram](https://github.com/Shri2401/Bluetooth-Low-Energy-based-Health-Thermometer/blob/main/questions/screenshots/GATT_Server_Block_Diagram.png)

## Wireless Communication Details

![Wireless_block diagram](https://github.com/Shri2401/Bluetooth-Low-Energy-based-Health-Thermometer/blob/main/questions/screenshots/Wireless_Block_Diagram.png)
