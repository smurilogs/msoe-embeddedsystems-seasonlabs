# MSOE | EE2920 Embedded Systems Labs | AVR ATmega328P (C programming language)

### Lab 6: A/D Conversion, Interrupts and Analog Sensors

The system described in this report uses the ATmega328P microcontroller analog-to-digital converter module sweep over three ADC channels in order to get samples from them and take some decisions. The system is driven by the use of interruptions from ADC conversion completion event.

 The system gets samples from three channels ADC channels, each one of them is interfaced to analog sensors. The system uses a reflectance sensor module QTR-1A to get data relative to proximity or reflectance from objects. The variations of voltage in the module output is read by the system, and a group of three LEDs is used to indicate the current level of reflectance. The more it reflects, the more LEDs will be on. 
In addition, the system interfaces two structures of voltage dividers that use LDRs to change the output voltage, and get ADC samples from it to show the percentage of light intensity in the place they are. As LDRs change its resistance according to such changes of light intensity in a non-linear way, the output voltage is not linear either. So math manipulations are needed to make such values get the linear percent representation.