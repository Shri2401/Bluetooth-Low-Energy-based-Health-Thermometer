Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.* 

1. What is the average current per period?
   Answer: 20.02 uA (micro amps)
   <br>Screenshot:  
   ![Avg_current_per_period][Avg_current_per_period]

2. What is the average current when the Si7021 is Powered Off?
   Answer: 2.49 uA (micro amps)
   <br>Screenshot:  
   ![Avg_current_LPM_Off][Avg_current_LPM_Off]

3. What is the average current when the Si7021 is Powered On?
   Answer: 522.18 uA (micro amps)
   <br>Screenshot:  
   ![Avg_current_LPM_On][Avg_current_LPM_On]

4. How long is the Si7021 Powered On for 1 temperature reading?
   Answer: 100.10 ms (milli seconds)
   <br>Screenshot:  
   ![duration_lpm_on][duration_lpm_on]

5. Compute what the total operating time of your design for assignment 4 would be in hours, assuming a 1000mAh battery power supply?  
   Answer: For total operating time in hours, the formula that I am going to use is time(in hrs) = mAh/(current in milli amps).  
           In our case, the average current per period is 20.02 uA => 0.02002 mA  
           => Substituting these values in above formula, 1000/(0.02002) = 49950.04995 hours  
           THE TOTAL OPERATING TIME OF THIS DESIGN IS => 49950.04995 hours.  
   
6. How has the power consumption performance of your design changed since the previous assignment?  
   Answer: In my assignment-3 the power consumption for one period is 523.26 uW(micro watt), whereas in my assignment-4 it is 66.36 uW(microwatt).   
           => From the above statement, we can observe that the average power consumption has reduced by 87.32%.  
           => This drastic reduction in average power consumption is mainly because of the use of INTERRUPTS for TIMER wait and I2C transfers.  
           => Also during I2C transactions, current consumption is minimal in EM1 i.e. CPU is OFF and CPU is sleeping in EM3 after reading the temperature from I2C bus. 
           => All these have contributed significantly to lower power consumption.  
   
7. Describe how you tested your code for EM1 during I2C transfers.  
   Answer:  => SLEEP_EnergyMode_t SLEEP_LowestEnergyModeGet (void)  
            => I used the above API to get the lowest possible energy mode that is needed during I2C transfer  
            => Reference:- https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg13/html/group__SLEEP.html#gaeb054dce581519f0f29d46d372a7abb2  
            => Also, I tried to do the I2C transaction in EM3, but it didn't happen as expected.  
            => This is because I2C need RCC clock to do its transaction, but in EM3 we have only ULFRCO clock.  
            => Hence I2C would transactions would happen properly in EM1, or EM2.  

[Avg_current_per_period]: screenshots/assignment_4/Avg_current_per_period.jpg "Avg_current_per_period"
[Avg_current_LPM_On]: screenshots/assignment_4/Avg_current_LPM_On.jpg "Avg_current_LPM_On"
[Avg_current_LPM_Off]: screenshots/assignment_4/Avg_current_LPM_Off.jpg "Avg_current_LPM_Off"
[duration_lpm_on]: screenshots/assignment_4/duration_lpm_on.jpg "duration_lpm_on"
