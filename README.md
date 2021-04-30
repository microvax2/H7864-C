# H7864-C
A modern replacement for DEC's H7864-A/H7864-B (Astec AA12131)

Non-commercial license to do anything you want with the stuff in this repository.
For commecial licensing please contact me.

Project Folders:
Software - PIC12F675 firmware

Metal Parts

Looms

PCB

Assembly

**IMPORTANT**

There are several "blocks" marked on the power supply control PCB.

A. A 74AC14 TTL gate - to generate POK, DCOK signals on power-up
B. An analog line monitoring circuitry that feeds the PIC microcontroller with a square wave signal and generates a "True" LTC clock
C. A PIC microcontroller - to generate POK, DCOK signals on power-up and an artificial LTC clock

Blocks A and C are mutually exclusive
Block B, if assembled together with Block C, enables the PIC to generate a correct sequence for POK, DCOK upon power-off.

Why this is important: ONLY OPTION B + C WILL PRESERVE THE TOY (Time of Year) CLOCK OF THE KA630.
This would probably be true for other KA6xx CPU Boards that keep time, boot parameters etc in battery-backed RAM.
The reason being, correct hardware shutdown must be performned by the CPU board upon power-off detection.

So your assembly options are:
**Option C: if you are lazy and don't care about preserving TOY clock after power-off, or have a PDP-11 and need an artificial LTC clock signal**
**Option A: if you are lazy, don't need an LTC clock, and don't even want to use a microcontroller**
**Option A+B: if you are lazy, have a PDP-11 system, and need a working, real LTC clock**
**Option B+C: if you are lazy and want to keep the TOY clock functioning when power is removed**


