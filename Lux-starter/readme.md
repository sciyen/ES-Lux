# Lux-starter

This is a simpler version of the lux project, which was aimed to use
some basic programming and circuit knowledges to highlight the principle
of a POV device. Therefore, I designed this kit, using atmega 328p as 
the controller, handling the leds with two shift registers, lighting
leds with simulated PWM with hardware timer interrupt.

This project was developed by Arduino framework, one can easily compile
it with regular Arduino IDE. The PCB was layouted by Altium Designer:

The schematic:
![Schematic](/Lux-starter/imgs/schematic.jpg)

The pcb layout:
![Layout](/Lux-starter/imgs/pcb_top.jpg)

The final product:
![Product](/Lux-starter/imgs/product.jpg)

## How to use the provided library

### Basic control of the leds
I designed a helping library which simplifies lots of details in the
controlling of the leds. The only things needed to do is to modify the 
`led_map` array which is a 2D array sized in [number of leds, 3], the 
3 channel indicating the 3 colors of R, G, B. For example:

```c++=
// Turn all leds on
for(int i=0; i<NUM_LEDS; i++)
    for(int k=0; k<3; k++)
        led_map[i][k] = 0;
```
 
The smaller value means the higher brightness, and the value ranging 
from 0 to 7.
