# Notebook
This document contains notes taken during the design of the software and circuitry of the Light Wand project.

## 2024/02/17

Testing what acceleration level needs to be selected on the ADXL343 for rapidly waving the stick to not max it out.

- Range = 4G. Gravity = 128. MaxVal = 512
- Range = 8G. Gravity = 64. MaxVal = 511
- Range = 16G. Gravity = 32. MaxVal = 275

Because 8G maxxes out, it looks like we will need to use the 16G setting. However, it is not impossible that the 8G setting will turn out to be a better fit. When waving the stick at more reasonable speeds, the 16G max value was more like 200

## 2024/02/18

### Boost Converter Component selection

Lets look at the MT3608 boost converter ([Datasheet](https://www.olimex.com/Products/Breadboarding/BB-PWR-3608/resources/MT3608.pdf)). We need it to step up the battery voltage to the required 5V. The boost converter needs some supporting components: A schotkey diode, an inductor, and some caps/resistors as shown in Figure 1 on the datasheet.

To properly select the inductor and diode, we need to find the peak current through the inductor, also known as the maximum switch current. First, we'll find the duty cycle for our application following the TI guide [Basic Calculation of a Boost Converter's Power Stage](https://www.ti.com/lit/an/slva372d/slva372d.pdf?ts=1708242509978&ref_url=https%253A%252F%252Fwww.google.com%252F).

$$D = 1 - \frac{V_{IN(min)} * \eta}{V_{OUT}}$$

$$\Delta I_L = \frac{V_{IN(min)} * D}{f_S * L}$$

$$I_{OUT(max)} = (I_{LIM(min)} - \frac{\Delta I_L}{2}) * (1 - D)$$

Where $\eta = 0.8$ (worst case efficiency of the converter), $V_{IN(min)} = 2.5V$, $V_{OUT} = 5V$, $f_s = 1.2MHz$ as given by the datasheet, $ L = 22\mu H$ as given by the datasheet, and $I_{LIM(min)} = 4A$ as given by the datasheet.

Unfortunately, this results in a calculated maximum output current of $I_{OUT(max)} = 1.58A$, which is well below the required 2A. This means a different boost converter needs to be used.

Lets look at the PAM2423 ([Datasheet](https://www.mouser.com/datasheet/2/115/PAM2421_22_23-3043389.pdf)). It has a higher $I_{LIM(min)}$ of 5.5A, recommends a $6.8\mu H$ inductor, and switches at 520kHz. $\eta = 0.8$ seems like a reasonable efficiency in this case as well, based on the datasheet's plots. The minimum input voltage for the boost converter is $V_{IN(min)} = 2.7V$, which means we will not be able to discharge the battery to its limit and will waste some capacity (2.5V is the usual LiION minimum voltage).

Using these values, we get $I_{OUT(max)} = 2.28A$, which is within spec!

The datasheet recommends the use of the SS34 Schotkey diode. However, I would prefer to prototype with a through-hole component. The [31DQ05](https://www.smc-diodes.com/propdf/31DQ05(06)%20N0691%20REV.A.pdf) has a Vf of 0.55V and can handle 3A, which is plenty for this application.

As for the inductor, the recommended value seems good. However, we need it to have a higher current rating than the maximum switching current of 5.5A. The [SBC6-6R8-662](https://www.digikey.com/en/products/detail/kemet/SBC6-6R8-662/5819291) satisfies this with a current rating of 6.6A

### Battery Protection Component Selection

The battery protection IC we plan to use is the [S-82K1BAM-I6T1U](https://www.digikey.com/en/products/detail/ablic-inc/S-82K1BAM-I6T1U/15996683). The protection IC has a overdischarge protection voltage of 2.30V and an overcharge protection voltage of 4.59V. While the overcharge protection is dangerously high, and the overdisrcharge is a tiny bit low, this should be fine.

Page 22 of the datasheet contains a typical circuit example. Other than resistors and capacitors, the chip requires two MOSFETS (well, we might be able to get away with only one). The 2N7000s that we have on hand should be sufficient.

### Power Supply Parts Selection Summary

 - [PAM2423](https://www.mouser.com/ProductDetail/Diodes-Incorporated/PAM2423AECADJR?qs=pYVYkI7xuRXROp8n8TyUXw%3D%3D) boost converter
 - [SBC6-6R8-662](https://www.digikey.com/en/products/detail/kemet/SBC6-6R8-662/5819291) inductor
 - [1N5820](https://www.mccsemi.com/pdf/Products/1N5820-1N5822(DO-201AD).pdf) schotkey diode
 - [S-82K1BAM-I6T1U](https://www.digikey.com/en/products/detail/ablic-inc/S-82K1BAM-I6T1U/15996683) protection IC
 - 2N7000 transistor (have on hand)
 - passive components (have on hand)

## 2024/02/21

### Calculations for acquiring the position of the wand in space using the accelerometer

In order to accurately see how far the wand has moved, there are two options as far as I can tell:

1. Calculate the Jerk of the device, and then integrate up to position. This would mean that as long as the accelerometer is not moving at all in physical space, it will not detect movement in digital space. However, it seems likely that this method will accumulate innacuracies rapidly. This can be solved by resetting the wand with a button before waving, or resetting when no motion is detected.
2. Instead of calculating position in 3D space, only measure distance from origin as a magnitude. In this way, gravity could be subtracted from the acceleration as a scalar to eliminate it from the equation. This may even be more useful for our purposes, as it simplifies the user experience greatly.

it would be prudent to try both.

## 2024/02/22

Both versions of the software have been attempted now, and while not bothering with 3D space proved to be closer to a reasonable result, it is still wildly innacurate and off-base. Perhaps it would be better to use the accelerometer simply to detect when the wand is being moved, and wether it is being moved left or right, and then display the text at a hard-coded speed. That seems to be what most Persistance of Vision wands do, anyway. [This hackaday project](https://hackaday.com/2018/08/31/max1000-tutorial-is-quite-persistent/) only bothers to detect the max G's that occur when you switch direction.

[The POV Staff](https://github.com/shurik179/povstaff) seems like it might be a bit more sophisticated. From the user guide, "As soon as rotation speed is high enough, the staff will begin showing your first image, one line at a time, and will continue doing that as long as you are twirling the staff. It will adjust the interval between successive lines depending on the rotation speed, to keep the image ratio close to original regardless of how fast you are rotating the staff". They use the LSM6DS33, which is a 3-axis gyro + 3-axis accelerometer. The gyroscope is also how they find the rotational speed of the staff, and since it is fairly expensive I think this is more or less off the table for now.

## 2024/03/01

When waved at extreme speeds, the wand can now display about onen character at a time in the afterimage. This is not what we are really going for. We need at least three characters to be displayed simultaneously. In order to reach this goal, we need to make it easier to wave the wand at high speeds. Some possible changes:

1. The wand is too long. We can either
	1. Buy denser neopixel strips and make the wand shorter. 
		+: Higher quality product. 
		-: expensive, ~$10 more per unit
	2. Use fewer neopixels per wand.						 
		+: Materials on hand, much cheaper, longer battery life. 
		-: lower quality product
2. The wand is too heavy (currently mounted on a 2x4 more than a meter long). We can
	1. Use a lighter material		
		-: After trying extremely light discarded fiberglass rods, while speed increased control decreased
		-: having a stick this long waved at high speeds is simply dangerous
	2. Make it shorter (see above) 

From this little set of bullet points, it would seem that the best course of action is to reduce the wand from 30 pixels to 15 pixels. This would reduce the unit price, make them much easier to use (less tiring on the arms), increase their battery life, make them safer, etc etc.

## 2024/03/04

### Software issue

After trying to proto-board a 15 pixel version, I have encountered a frustrating issue - communicating to the ADXL343 appears to always result in a timeout. I have tried using a different ADXL343 on a breadboard, and a different Pico on the breadboard. The problem persists. It is difficult to ascertain what has caused this issue - if I had damanged the pico, or damaged the ADXL343, then swapping both out should have fixed it. And yet, there are few other ways to explain the timeout. Perhaps both ADXL343's were damaged?

### Power Supply circuit schematic

Began drawing out the power supply in KiKad. From the PAM2423 datasheet, the output voltage of the boost converter is set by resistors on the feedback path following this equation:

$$R_1 = R_2 * (\frac{V_{OUT}}{1.262} - 1)$$

In their example schematic, both $R_1$ and $R_2$ are on the order of 10k - 100k, so we will follow suit here. As $V_f$ of our shottky diode is 0.55V typical, we want $V_{OUT} = 5.55$. After some trial and error, it was found that with $R_1 = 15k\Omega$ and $R_2 = 51k\Omega$, the output of the boost converter would be $V_{OUT} = 5.5528V$, and after the boost converter a voltage of $V_{SYS} = 5.0028V$ would be supplied to the rest of the circuit.

## 2024/03/05

### Software issue resolved

(it was a hardware issue). The ADXL343 placed on the protoboard has not been salvaged yet, but the issuw was the spare was simply a bad solder connection between Vin and the rest of the board.

## 2024/03/08

### Testing the Boost Converter

The boost converter circuit was constructed and tested following the below schematic:

![KiCad circuit schematic of the boost converter](./notebook_imgs/boost_conv_ckt.jpg)

A photo of the constructed circuit is shown below:

![Constructed boost converter](./notebook_imgs/boost_conv_constructed.jpg)

At no point during testing were any of the components hot to touch. 

The results are as follows:

 - $V_{in} = 4.20V$: $V_{out}$ = 5.38V
 - $V_{in} = 4.00V$: $V_{out}$ = 5.38V
 - $V_{in} = 3.80V$: $V_{out}$ = 5.38V
 - $V_{in} = 3.60V$: $V_{out}$ = 5.38V
 - $V_{in} = 3.40V$: $V_{out}$ = 5.37V
 - $V_{in} = 3.20V$: $V_{out}$ = 5.08V
 - $V_{in} = 3.00V$: $V_{out}$ = 4.57V
 - $V_{in} = 2.80V$: $V_{out}$ = 3.97V

This indicates that the boost converter will be more or less unable to support the circuit at around 3Vin - 3.2Vin. The RPi Pico should be alright with an upper $V_{in}$ of 5.38V as the datasheet claims it can run off 1.8V to 5.5V.

### Wand Rebuild

The wand has been rebuilt in its 15 LED form on a light, flexible fiberglass rod. This allows it to be waved much faster and more easily than previously. However, the results are still extremely unsatisfactory. In a well lit room, the POV effect is not strong enough for onlookers to make out what is being spelled. The same goes for video recordings of the effect. It still needs to be tested in a dark room, however.

## 2024/03/09

### Component Selection for the S-82K1B battery protection IC

On the S-82K1BAM-I6T1U datasheet, page 22, an example is shown. In the example, two N-Channel MOSFETs are used: One for overvoltage circuit disconnect, one for undervoltage circuit disconnect. Additionally, there are three resistors. R1 forms a low-pass filter with C1 across the battery and VDD and VSS terminals of the the chip. R3 connects VSS and INI on the chip, and R2 connects VM to the minus rail of the output.

#### R1 (Power fluctuation prot.)

The datasheet recommends a typical value of $330\Omega$, which we will use

#### C1 (Power fluctuation prot.)

The datasheet recommends a typical value of $0.1\mu F$, which we will use

#### R2 (Reverse voltage prot.)

The datasheet recommends a typical value of $470\Omega$, which we will use

#### R3 (Overcurrent detection)

The datasheet does not explain the value selection for this component, but recommends a typical value of $1.5m\Omega$. On digikey, such resistors are under a dollar each. 

#### MOSFETS

According to the datasheet, the MOSFETS should have a threshold voltage that is less than or equal to the overdischarge protection voltage. We are using the S-82K1BAM-I6T1U, which has an overdischarge detection voltage of 2.300V.

The [SI3900DV-T1-E3](https://www.digikey.com/en/products/detail/vishay-siliconix/SI3900DV-T1-E3/1656399) ([datasheet](ttps://www.vishay.com/docs/71178/si3900dv.pdf)) seems like a good choice for the mosfets. Relatively hand solderable at 1mm pitch, $0.7 a piece, with $V_{GS(th)} = 0.6V$ to $1.5V$. In worst case conditions they can handle continuous $I_{DS} = 1.8A$, and since only 15 LEDS are being used at this stage of the design, that is more than enough current capacity.

## 2024/03/18

### PCB - Pico or RP2040?

There is a decision to be made on whether the rpi pico should be soldered directly onto the PCB, or whether an RP2040 should be pick-and-placed onto the PCB instead.

#### Hand soldering the pico:
 - ~$5
 - larger space requirement
 - hand solderable
 - convenient

#### Pick-and-place assembly and RP2040:
 - pick-and-place would already be helpful for the battery protection IC and other small components
 - would result in a smaller, more professional final design
 - may be cheaper

#### Decision

In order to reduce complexity of the design, avoiding potential unexpected roadblocks, the RPi pico will be soldered directly on the board. The battery protection IC can be stenciled on using tweezers and solder paste.

[This helpful KiCad library](https://github.com/ncarandini/KiCad-RP-Pico) for the Pico will be used in the PCB layout. 

## 2024/03/19

### Boost Converter Testing

The boost converter has not yet been tested under load. Now is the time to do so. 

Initially, the output voltage would read as 5V and then immediately drop down to around 4V when measured with the multimeter, which is very bad. However, upon inspecting the circuit, it was noticed that the wire connecting pin EN to pin VIN of the PAM2423 was broken. Fixing this issue made the boost converter perform as expected when measured by the oscilloscope. However, when powering the RPi Pico and LED lights, things seem off. The system powers on just fine when powered through the boost converter, but the RPi Pico voltage is measured to be 3.3V, when it should be at least 5V. Additionally, the system powers on correctly when connected directly to the battery, at which point the system reads 4.2V.

 - Fully charged battery, no boost converter: VSYS = 4.2V. 	 System works.
 - Fully charged battery, with boost converter: VSYS = 3.3V. System works.
 - Med charge battery, no boost converter: VSYS = 3.86V. 	 System works.
 - Med charge battery, with boost converter: VSYS = 3.47V.	 System works.
 - Low charge vattery, no boost converter: VSYS = 3.6V. IBatt = 0.19A.  System barely works, all leds noticibly dim. 
 - Low charge vattery, with boost converter: VSYS = 3.292V. IBatt = 0.17A.  System barely works, all leds noticibly dim. 

Neither the boost converter nor the system power are behaving as I expected them to. Not only does the boost converter appear to actually introduce a votlage drop instead of a votage rise when under load, but the system works just fine without the boost converter regardless. However, based on the fact that the system starts to break down at around 3.6VBatt, an actually functional boost converter would likely be beneficial. Therefore, I plan to evaluate and then re-design the boost converter.

### Boost Converter - Why didn't it work?

Here is the circuit schematic of the boost converter: ![Boost converter schematic screenshot](./notebook_imgs/2024-03-20-BoostConverterSchematic.png)
And here's a view of the oscilloscope monitoring the boost converter while it powers the system off of a single battery: ![Boost converter oscilloscope screenshot](./notebook_imgs/2024-03-20-BoostConverterOscilloscope.png)

The image shows, once more, that when under load the boost converter just adds a voltage drop and some ripple, although the ripple isn't particularly bad in this case. Additionally, some spikes can be seen from when the system sends communications to the LEDs. Unfortunately, this doesn't explain why it doesn't work.

Possible issues:

 - The PAM232 chip cannot actually handle the load
 - The inductor is undersized
 - The capacitors are undersized
 - The diode has some mismatched spec I am unaware of

Now, I could spend a few hours reading literature and taking measurements to try and figure out what went wrong, OR I could use the TI boost converter designer tool to make sure that the next iteration actually works. Notably, we do not need to design for a 2A continuous load anymore, as half the LED's are being used at this point. Therefore, a boost converter designed for a ~0.6+A continuous load will be well within spec.

### Boost Converter - new design

According to some back of the napkin calculculations, the [TLV61070ADBVR](https://www.digikey.com/en/products/detail/texas-instruments/TLV61070ADBVR/16982069?s=N4IgTCBcDaIAQBUAyA1AbARgAwHYsEEQBdAXyA) should be more than capable of handling our load.

Those calculations are:

$$I_{OUT(CL)} = (1 - D) * (I_{LIM} + \frac{1}{2}\Delta I_{L(P-P)})$$
$$D = 1 - \frac{V_{IN} * 0.9}{V_{OUT}} = 0.46$$
$$\Delta I_{L(P-P)} = \frac{V_{IN} * D}{L * f_{SW}}$$

Where $V_{IN} = 3V$ (minimum), $V_{OUT} = 5V$, $f_{SW} = 1MHz$, $I_{LIM} = 2A$ and $L = 2.2\mu H$ (from datasheet: should be between 2.2 and 4.7, lower seems to grant better results).

The results are: $I_{OUT(CL)} = 1.25A$ of continuous load current! This is more than double the required current.

The example circuit is shown below:

![TI example boost conveter implementation](./notebook_imgs/2024-03-21-BoostConverterExampleTI.png)

#### Inductor selection

The datasheet recommends the $2.2\mu H$ part [74438357022](https://www.digikey.com/en/products/detail/w%C3%BCrth-elektronik/74438357022/6833539?s=N4IgTCBcDaIOwBYEGYAcyCscAMYIF0BfIA)

$I_{L(DC)} = \frac{V_{OUT} * I_{OUT}}{V_{IN} * \eta} = 1.11A $  

$\Delta I_{L(P-P)} = \frac{V_{IN} * D}{L * f_{SW}} = 0.63A$

$I_{L(P)} = I_{L(DC)} + \frac{\Delta I_{L(P-P)}}{2} = 1.43A$


#### Output Capacitor Selection

The absolute minimum value of the output capacitor is selected by:

$V_{RIPPLE(ESR)} = I_{L(P)} * R_{ESR} = 0.056$

$C_{OUT} = \frac{I_{OUT} * D_{MAX}}{f_{SW} * V_{RIPPLE}} = 6.6\mu F$. We can give plenty of space for this by selecting a $47\mu F $ capacitor

Part: [865080543009](https://www.digikey.com/en/products/detail/w%C3%BCrth-elektronik/865080543009/5728103)

#### Resistor selection

The resistors can be selected by the following equation:

$R_1 = (\frac{V_{OUT}}{V_{REF}}-1) * R_2$

where $V_{REF}$ is the internal reference of the chip, $V_{REF} = 0.5V$. The lower $R_2$ is, the less noise there is, and the more quiescent current is wasted. $R_2$ should be below 100k.

By selecting $R_2 = 20k$, we get $R_1 = 180k$. These are both common series 24 resistor values!

#### Loop Stability, Feedforward Capacitor Selection

The feedforward cap is selected by:

$C_3 = \frac{1}{2 * \pi * f_{FFZ} * R_1} = 884pF$

The closest reasonable capacitor value is $C_3 = 820pF$

#### Input Capacitor Selection

The datasheet recommends $C_1 = 10\mu F$

## 2024/03/21

### Power draw test

By powering the 15 LED wand at full brightness (rgb(255, 255, 255)) with a RIGOL DP832 programmable power supply, the power draw is measured.

- At 5.4V, with all led's set to full-bright white, the system draws 5.41V * 0.52A = 2.8W
- At 5V, with all led's set to full-bright white, the system draws 5V * 0.52A = 2.6W
- At 3V, with all led's set to full-bright white, the system draws 3V * 0.28A = 0.84W

Notably, at 5.4V and at 5V the system draws the same current, so it is unlikely that the LED's are any brighter when pushed beyond 5V.

When it comes to low brightness (rgb(1, 1, 1)) the results are unsurprising:

- At 5.4V, with all led's set to min-bright white, the system draws 5.41V * 0.02A = 0.11W
- At 5V, with all led's set to min-bright white, the system draws 5V * 0.03A = 0.15W
- At 3V, with all led's set to min-bright white, the system draws 3V * 0.03A = 0.09W

## 2024/03/25

### PCB ordering

Total component price for the PCB with battery protection: 	$23.36

Total component price for the PCB without battery protection:	$19.86

So, keeping in battery protection increases unit price by $3.50. 

This is not including the price for physical materials, like a 3D printed case or a rod for the led strips to mount to. It also excludes the unit price of the 18650 batteries and their holders. The batteries were $7.49 each, and the holders were $0.83 each. Assuming an additional $5 for physical materials (PCB, dowel), the total unit price for each wand is ~$33.17

## 2024/05/26

### Revisiting wand position

After watching this video [Simple pendulum animation](https://www.youtube.com/watch?v=cekU-08YQj0) I thought of an idea for a new method for finding the direction change points in the swing. While it is true that the peak acceleration magnitude occurs at the ends of each swing, like the video demonstrates, in practice I have found that since the exact peak value is never the same for each swing, the direction change detection can be finniky. So, instead testing if the acceleration of the wand has crossed a certain prerecorded peak value, the jerk of the wand could be recorded and then simply tested if it is greater than or less than 0. That would also allow the jerk of the wand to be treated like a direction of travel.