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

## Conclusion
In conclusion, our project aims to address the crucial need for remote patient monitoring in healthcare through the implementation of a Bluetooth Smart system with server and client models. Leveraging Bluetooth Low Energy (BLE) technology, we have designed a system capable of continuously monitoring temperature and heart rate, enabling healthcare practitioners to make informed decisions regarding patient treatment in real-time.

By utilizing Blue Gecko Boards as both the server and client devices, we have successfully established an encrypted link between them, ensuring secure communication and data transfer. The server device periodically reads temperature values from an external sensor and sends them to the client only when abnormal fluctuations are detected, optimizing power consumption and data efficiency. The client device, upon receiving temperature measurements, promptly retrieves heart rate values from a sensor and updates the server with the combined data.

Our project also prioritizes user experience and interface, with LCD displays providing real-time updates on device status, connection status, and measured values. LED indicators further enhance user awareness by signaling abnormal temperature and heart rate readings through visual cues.

Through meticulous hardware and software integration, our system offers a reliable solution for remote patient monitoring, providing healthcare professionals with timely and accurate data to facilitate proactive patient care. With its emphasis on data security, energy efficiency, and user-friendly design, our project represents a significant step forward in the realm of IoT-enabled healthcare applications.







