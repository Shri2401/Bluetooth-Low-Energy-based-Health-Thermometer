Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

**1. How much current does the system draw (instantaneous measurement) when a single LED is on with the GPIO pin set to StrongAlternateStrong?**
   Answer: 5.27 mA - Screenshot_Path : screenshots/Strong_Alternate_Strong.png 


**2. How much current does the system draw (instantaneous measurement) when a single LED is on with the GPIO pin set to WeakAlternateWeak?**
   Answer: 5.22 mA - Screenshot_Path : screenshots/Weak_Alternate_Weak.png


**3. Is there a meaningful difference in current between the answers for question 1 and 2? Please explain your answer, referencing the main board schematic, WSTK-Main-BRD4001A-A01-schematic.pdf, and AEM Accuracy in the ug279-brd4104a-user-guide.pdf. Both of these PDF files are available in the ECEN 5823 Student Public Folder in Google drive at: https://drive.google.com/drive/folders/1ACI8sUKakgpOLzwsGZkns3CQtc7r35bB?usp=sharing . Extra credit is available for this question and depends on your answer.**
   Answer: There was NOT much difference in current between the two cases. The reasons are described below.
1. The energy monitor has an accuracy of 0.1 mA when measuring current beyond 250 micro amps. Hence any difference in current should be within that range and that's why in our case, we got an difference of 0.05 mA. (Ref - https://www.silabs.com/documents/public/user-guides/ug279-brd4104a-user-guide.pdf) - [Page Number - 31 Last Paragraph].
2. GPIO pins in this case are of active high configuration and are also set in push-pull mode. Hence, when the output becomes high, it is actively pushed to Vcc. This could possibly remove much difference in current between WeakAlternateWeak and StrongAlternateStrong case. (Ref- https://embeddedartistry.com/blog/2018/06/04/demystifying-microcontroller-gpio-settings).

**4. With the WeakAlternateWeak drive strength setting, what is the average current for 1 complete on-off cycle for 1 LED with an on-off duty cycle of 50% (approximately 1 sec on, 1 sec off)?**
   Answer: 5.34 mA - Screenshot_Path : screenshots/one_Led_avg_Current.png


**5. With the WeakAlternateWeak drive strength setting, what is the average current for 1 complete on-off cycle for 2 LEDs (both on at the time same and both off at the same time) with an on-off duty cycle of 50% (approximately 1 sec on, 1 sec off)?**
   Answer: 5.58 mA - Screenshot_Path : screenshots/two_Led_avg_Current.png



