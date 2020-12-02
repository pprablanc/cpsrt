# C-pitch-shifting-RT

This C code allows one to modify the pitch of the voice. It's an old code that would need some debugging (especially for the cracky sound), improvement and a decent command line interface at least.


## Requirements

- gcc
- libjack-dev
- libasound2-dev


## How to use

Just launch the binary file "pitch_shifting" in the commandline. If error occurs, try to set up your audio output configuration.


## Building from source

To build the code from source, in the command line in the main directory:
```
make
```
