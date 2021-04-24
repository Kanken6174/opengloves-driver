# OpenGloves Driver - LucidVR X Fngrs X ApexGloves
## 🚨 Please follow instructions in the <a href="https://github.com/LucidVR/opengloves-driver/wiki">wiki</a> for necessary configuration of the driver
The driver won't work unless you configure the SteamVR input bindings and the driver settings.

Developed by:
* Danwillm
* Lucas_VRTech (LucidVR)
* Kanken6174 (𒀇𒃯#2919)

## What is this?
This repository contains the OpenVR(SteamVR) driver for a DIY VR Haptic Glove.

__This Repository is a *very early* work in progress. Many things are likely to change.__
Things in this repo are subject to rapid changes and currently is not in a stable release state.

Pre-Built binaries are available in the releases section.
Instructions for building from source code are in the Wiki.

Releases will be available on Steam once the repo hits a stable release.

That said, we are more than happy to help with issues that may arise, skip to the bottom for how to contact us.

## Officically Compatible Hardware:
* ApexGloves prototype 1.5 and onwards

## Currently supported:
* Finger flexion tracking
* Hardware error correction (finger fallback, ignores nonsensical input or at least tries to average it)
* thumb Y axis
* Positioning from a controller / rough position from the onbaord MPU
* Button and joystick inputs
* Communication Protocols:
  - Serial over USB

## Features that are almost certainly going to be supported:
* Y axis for all fingers (V2)
* Lighter overall glove

### Considered additions:
* Switching from nano to esp8266 or 32
* Wireless mode
* Full XYZ tracking using MPU 9250 instead of 6050 which only does X/Y OR adding a wrist potentiometer

## Games tested:
* none yet, the driver is still WIP 

### What's knuckles emulation?
Some applications are picky with the controllers they support, we've found this to be the case with Boneworks and VRChat. To get around this issue, we "emulate" a knuckle controller and pass in the values expected for the skeleton, buttons, joysticks etc.
This should make the gloves compatible with any game that is able to use an index controller, with finger tracking as an added bonus.
You are welcome to emulate knuckle controllers for all games, but we recommend switching back to using the lucidglove controllers in the games compatible with it listed above.

#### How do I enable knuckles emulation?
To enable knuckle emulation go to `resources/settings/default.vrsettings` and set `device_driver` to `1`. When launching SteamVR you should see the knuckle icons appear, as well as the controllers appear in game.  
To switch back to using the lucidglove controllers, set `device_driver` to `0`.

## Contributions
We are actively welcoming contributions, feel free to open a pull request.
## Issues
If you run into issues, you are more than welcome to open a GitHub issue/discussion, or contact us directly on Discord: 
`danwillm#8254`  
`LucidVR#0001`
`𒀇𒃯#2919` (i'm the one who made this fork, the folks at LucidVR might be able to help but open an issue on this github if you encounter issues with MY fork specifically)
