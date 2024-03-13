Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.*

1. Provide screen shot verifying the Advertising period matches the values required for the assignment.  
   Answer: 252 ms (milli seconds)  
   <br>Screenshot:  
   ![advertising_period][advertising_period]  

2. What is the average current between advertisements?  
   Answer: 3.16 uA (micro ampere)  
   <br>Screenshot:  
   ![avg_current_between_advertisements][avg_current_between_advertisements]  

3. What is the peak current of an advertisement?  
   Answer: 10.70 mA (milli ampere)  
   <br>Screenshot:  
   ![peak_current_of_advertisement][peak_current_of_advertisement]  

4. Provide screen shot showing the connection interval settings. Do they match the values you set in your slave(server) code or the master's(client) values?.  
   Answer: 42.60 ms (milli seconds). The connection interval that we set in the function is 75 ms and it did not match with the slave value.  
           But in the master we got 39 ms interval, which actually matches with the client value. (Client value screenshot attached in the 6th question)  
   <br>Screenshot: 
   ![connection_interval][connection_interval]  

5. What is the average current between connection intervals?  
   Answer: 257.17 uA (micro amps)
   <br>Screenshot:  
   ![avg_current_between_connection_intervals][avg_current_between_connection_intervals]  

6. If possible, provide screen shot verifying the slave latency matches what was reported when you logged the values from event = gecko_evt_le_connection_parameters_id.  
   Answer: I gave slave latency as 300 ms(4x75). But since the slave latency returned from the master's(client) [gecko_evt_le_connection_parameters_id] is zero, and the actual interval is also reduced to 42.60 ms.   
           Now the actual interval is 39 ms and the latency is zero, which means connection interval would be at 39 milli seconds with zero latency.  
   <br>Screenshot:
   ![slave_latency][slave_latency]  

7. What is the peak current of a data transmission when the phone is connected and placed next to the Blue Gecko?   
   Answer: 12.23 mA (milli amps)  
   <br>Screenshot:  
   ![peak_current_phone_next_to][peak_current_phone_next_to]  
   
8. What is the peak current of a data transmission when the phone is connected and placed approximately 20 feet away from the Blue Gecko?  
   Answer: 27.82 mA (milli amps)  
   <br>Screenshot:  
   ![peak_current_phone_20ft_away][peak_current_phone_20ft_away]

  
[advertising_period]: screenshots/assignment_5/advertising_period.png "advertising_period"  
[avg_current_between_advertisements]: screenshots/assignment_5/avg_current_between_advertisements.jpg "avg_current_between_advertisements"  
[peak_current_of_advertisement]: screenshots/assignment_5/peak_current_of_advertisement.jpg "peak_current_of_advertisement"  
[connection_interval]: screenshots/assignment_5/connection_interval.jpg "connection_interval"  
[avg_current_between_connection_intervals]: screenshots/assignment_5/avg_current_between_connection_intervals.jpg "avg_current_between_connection_intervals"  
[slave_latency]: screenshots/assignment_5/slave_latency.jpg "slave_latency"  
[peak_current_phone_next_to]: screenshots/assignment_5/peak_current_phone_next_to.jpg "peak_current_phone_next_to"  
[peak_current_phone_20ft_away]: screenshots/assignment_5/peak_current_phone_20ft_away.jpg "peak_current_phone_20ft_away"  