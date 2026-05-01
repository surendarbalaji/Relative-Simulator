# relative simulator

a small 2D physics simulator which allows you to interact with concepts of special relativity.

![](https://github.com/surendarbalaji/Relative-Simulator/blob/main/resources/oscilloscope.gif)

## features
* **the ship** - you move around a ship
* **bunch of cool (in my opinion) physics displays** - as you approach the speed of light you can observe the way different computed values change. this includes the Lorentz factor, and subsequent length contraction. 
* **time dilation visualisation** - the ship is also equipped with a stopwatch that syncs to a, for all our purposes, stationary reference frame. this means you can see how someone from that reference frame would observe the passing of time in relation to you. essentially, the faster you go through space, the slower you go through time (you would age less than a stationary observer (look up the twin paradox))
* **oscilloscope** - the ship has a little wave source that it can deploy and receieve a signal from. with the ability to both read and alter the signal with accurately (i admitedly use that term loosely) simulated pulses (i just mean that it takes real time for your signal to reach the source and for the update to reach you once more), you can interact with the relativistic doppler effects and visualise frequency shifts.
* **terminal** - there's also the mini terminal i built for some reason, it can control most things in the simulation, like ship movement, and you can also customise your display panels by showing or hiding different bits of information.

## controls

* left click and hold to accelerate the ship towards the cursor
* scroll wheel to zoom up, scroll wheel when clicked on the terminal to scroll up and down
* to send an update pulse with the oscilliscope, turn on config (CFG) mode and turn the timebase dial until you get the frequency you want to update the source to. then, hit the dispatch (DSP) button and wait for the signal to be recieved and for your oscilloscope to be updated. it has a cooldown of 3.0s (like a video game ability) and if you're at all close to the source, the update will happen practically instantly.

###  terminal controls 
* enter to send command
* up and down arrows to copy previously sent commands
* scroll wheel (after clicking on the terminal) to scroll up and down
* ctrl + backspace to delete whole words

### terminal commands

* 'set speed [value]' or 'set speed [value]c' - set absolute speed to a number or fraction of c
* 'set acceleration [value]' - set ship acceleration
* 'set thrust [value]' - multiplies the acceleration of mouse presses
* 'move [x] [y]' - move coordinates relative to the ship while stationary
* 'deploy source' - deploy a wave source from the ship
* 'show/hide [position/velocity/speed/acceleration/angle]' - show/hide property from the info panel
* 'toggle [info/grid/oscilloscope]' - show/hide UI panel

## optimisations

* while scanning my code for optimisations i could make, i realised i had a memory leak in oscilloscope. i'm using malloc to allocate memory on the heap for my wavePoints and dispatchWavePoints, however i totally didn't free these up, which is indeed one of those dreaded c memory leaks. i fixed this with a closeOscilloscope() function that triggers at shutdown which frees the memory.
* the other thing i spotted was my calculation to limit the ship's speed. i even made a comment that i thought i was doing it rather inneficiently, but that i couldn't think of any other method. originally, i didn't know how to limit it while preserving the velocity of the ship, so i made an two expensive trig calculations and had a repeated line with a square root. however, when i looked into it, turns out this is a classic problem online, and the way to do it efficiently is to just normalise the vector (thus preserving its direction) and then scale it to your desired magnitude. this saves huge on a square root and unecessary trigonometry. 
