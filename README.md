# Switchee Eurorack Module
Switchee is a Eurorack signal switcher using the ATSAMD10D14A. It's a fairly simple design that uses the MCU to control DG447 analog signal switchers. The DG447 a cheap and can handle the full voltage bandwidth of typical Eurorack signals. Assemble is with SMD components, and the PCB falls into budget range of JLCPCB.

The module has:
* A TRIGGER input that changes the output
* A Signal input that is buffered and routed to one of four outputs
* A Reset input that changes the switching back to Output One

[](front_panel.png)

This project includes the PCB, PCB front panel, and firmware. Firmware is downloaded to the ATSAMD10 via ATMEL ICE.

The PCB is done in Kicad. There are fab-ready gerbers in the gerbers/ directory. The main firmware file is in firmware/main.c

