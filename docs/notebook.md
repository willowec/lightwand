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

Both versions of the software have been attempted now, and while not bothering with 3D space proved to be closer to a reasonable result, it is still wildly innacurate and off-base. Perhaps it would be better to use the accelerometer simply to detect when the wand is being moved, and wether it is being moved left or right, and then display the text at a hard-coded speed. That seems to be what most Persistance of Vision wands do, anyway.