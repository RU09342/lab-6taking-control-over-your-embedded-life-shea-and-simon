# Open Loop Systems

The goal of this section of lab was to build a feedback loop that would send a corrective signal to the system. The fan from previous sections would change speed based on the temperature sensed by the LM35. 

A 5 V regulator was used to push down an voltage of 20 going into the system. This caused a decent amount of heat to be present on the regulator. 

The circuit used to test this portion of the lab is below

![alt text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-shea-and-simon/blob/master/Images/FanCircuit.JPG)

The graph below shows the tempertaure of the LM35 versus the duty cycle of the fan. 

![alt text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-shea-and-simon/blob/master/Images/DutyCycleTemp.JPG)

And the raw values shown below

![alt text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-shea-and-simon/blob/master/Images/tempdutyexcel.JPG)

The graph shows an expoential relationship between duty cycle and temperture. In order to model this in software an expontial line may be used but some MSP microcontrollers may struggle with the mathematical computation. In order to circumvent this problem, piecewise linear functions should be used instead.

### Code Implantation 

Below is the code that controls the fan speed to adjust to the target temperature of the regulator. It works by running a linear function which sets the duty cycle of the PWM as a fuction of the target temperature. Each temperature range has a different function to control the duty cycle.

```c
	if(RegTemp > Temp + 3)          //Check if the temperature of the regulator is greater than target temperature
        {
                TA0CCR1 = 1000;         //Run fan at 100% Duty Cycle
        }
        else if(RegTemp < Temp - 3)     //Check if the temperature of the regulator is less than target temperature
        {
                TA0CCR1 = 0;            //Run fan at 0% Duty Cycle
        }
        if(Temp >= 380 && Temp < 405)   // Perform if Temperature is 38-40.5 C
        {
                TA0CCR1 = ((Temp/10)*(-24.87)+1035.8) ;     // y = -24.87x + 1035.8
        }
        else if(Temp >= 405 && Temp <= 430)                 // Perform if Temperature is 40.5-43 C
        {
                TA0CCR1 = ((Temp/10)*(-7.1429)+321.43) ;    // y = -7.1429x + 321.4
        }
        else if(Temp >= 430 && Temp <= 480)                 // Perform if Temperature is 43-48 C
        {
                TA0CCR1 = ((Temp/10)*(-2.0833)+108.33) ;    // y = -2.0833x + 108.33
        }
        else if(Temp >= 480 && Temp <= 540)                 // Perform if Temperature is 48-54 C
        {
                TA0CCR1 = ((Temp/10)*(-24.87)+1035.8) ;     // y = -0.8333x + 49
        }
        else if(Temp >= 540 && Temp <= 710)                 // Perform if Temperature is 54-71 C
        {
                TA0CCR1 = ((Temp/10)*(-0.8333)+49) ;        // y = -0.8333x + 49
        }   
```

# Instructions
# Lab 6: Open Loop Systems
Believe it or not, up to this point, any time that you have wanted to control your LED color or brightness so far, you have been attempting to control an Open Loop System. Basically, when in your code you state that you want a certain brightness or even a duty cycle, you are going on blind faith that the output is actually what it is supposed to be. If something seemed off, you probably went back into the code and tweaked some values. In the case of actual Systems and Control Theory, you are the feedback loop, providing some corrective signal to the system to help obtain a closer output, and we will deal with this in the Milestone. For now, we need to focus on system modeling getting a system to a desirable state. For this lab, you will be attempting to keep a voltage regulator within a specific temperature range using a DC fan which you will have control over. For this part to be a success, you need to figure out what is the minimum fan speed you need to cool off the regulator so that is stays operational.

## Voltage Regulator
You will need to take a 5V regulator from the back of the lab and drop the output across a 100 ohm power resistor (1 watt should do the trick). The input to the voltage regulator will be between 15-20V. I am giving you a range because when it is dropping a ton of voltage, it may not be able to cool it off enough with just a fan. Most of the voltage regulators in the back will have a tab on the top which we can place a thermistor to. If provided, you can use that tab, or place a through-hole thermistor making contact to the component on your board.

## Fan Control
It will be up to you, the engineer, to decide which method you want to use to control the DC fan. Most of these fans run off of 5V, and as such can not directly be driven by your microcontroller. Depending on the type of fan you use, some can take in a PWM control signal, others will need to have the voltage be modified. Since we are not providing you with any mechanical mounts, you are free to place the fan in whatever orientation you wish, so long as it is safe to operate.

## Temperature Reading
It would be really useful to see what the temperature of your system is so you can determine the performance of your system. This can be done either by displaying the current temperature over a display, passing the information over UART, or other ways as well. Remember that UART is Asynchronous, meaning that you can send information whenever you would like from your controller back to a PC, it doesn't have to be reactionary. If you used MATLAB in Lab 5, you could even plot the temperature over time which could be extremely useful in figuring out whether your system is actually doing something. 


## System Modeling
For starters, you need to figure out with your fan at MAX what the temperature that the voltage regulator reaches. Your thermistors/PTATs/Whatever else you want to use to measure temperature may not be calibrated, so your results may be consistently off with reality, but for now that is ok. After figuring this out, in increments of about 5C, see what fan speed you need to have to maintain that temperature. Do this until your regulator gets down to about 30C-40C, keeping a record of what your applied Duty Cycles or voltages were. Then using this information, attempt to find a transfer function between the applied input and the resulting temperature to model the system behavior. A simple way to do this is in MATLAB or Excel to plot your applied input on the x-axis, and your steady state temperature on your y-axis and attempt a line fit.

## Open Loop Control System
You then need to use this information to make a final open loop control system where a user can state what temperature they want the regulator to be maintained at, and the microcontroller will basically calculate/look up what it needs to set the fan to. Do not over complicate this and make it some elaborate system. All this needs to do is some math and set a duty cycle or voltage, and display in some capacity the current temperature of the system as you are measuring it.


## Deliverables
Your README needs to contain schematics of your system, the plot of the plot of the temperature and input voltages at the 5C steps, and a brief talk about why you chose the processor you did along with the control technique for the fan. As always, you should include a brief description of the code you generated to run the experiment. You need to also include information on how to use your control software, including what inputs it is expecting and what range of values you are expecting. At this time you are not going to need to user-proof the system, but you will for the milestone, so keep it in the back of your head.

### What your README and code doesn't need
For starters, note the fact I ask you to do this with only one board. You also do not need to give me all of your code in the README, just tell me functionally what is going on along with showing off any functions you may have made.

Your code *DOES NOT* need to perform any sort of closed loop control. Save that for the milestone. This means that your system does not need to try to actively change the fan speed without your help. You are going to essentially make your microcontroller one big Convert-o-Box to turn a desired temperature into a controllable signal, and then be able to read a temperature.