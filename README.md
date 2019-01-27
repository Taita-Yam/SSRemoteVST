# Introduction

This VST Plugin addresses the need for the automation of SoundScapeRenderer (SSR) scenes from a Digital Audio Workstation (DAW).

This fork is an attempt to adapt the codebase to updated JUCE framework and the VST3 SDK. This VST can run in Nuendo, Cubase, Reaper and other DAW's but it has only been tested on a small number of Win10 machines. It is a 'bare-bones' implementation which bypasses some deprecated JUCE functions and addresses some performance issues that were causing crashes and hangs when multiple instances of the plugin were running in a DAW. Some functionality has been removed compared to the original codebase, notably any references to JACK (although JACK still needs to be present for SSR to run!).

Since this is a experimental open source project, any contributions are very much welcome!

# Known Bugs
This VST3 plugin will cause its host DAW to hang if the SSR is shut down while the VST is 'Connected'.

This VST3's GUI window must remain open or minimised when connected to SSR, otherwise SSR suffers memory leaks


# TODO :: Update this doc for Windows  /#TODO

# Installation
Please read the installation guide.

You need to compile the boost lib's using the same compiler/linker as the VST itself. This fork is compiling and working using VS2017.

All references to jack have been commented out. Consequently, the ability to add new sources to SSR from the VST is disabled.

# Configuration

## Environment Variables
Please set the environment variables as follows, since the following environment variables are not only needed for compilation but also at runtime:


Please set the environment variables as follows, since the following environment variables are not only needed for compilation but also at runtime:

  The directory/location of the Steinberg VST3 SDK (needed at compile time, should already be included if using VS2017 project created by Projucer):
  
  ```bash
  # Steinberg VST3 Audio Plug-Ins SDK
  export VST3_SDK=/path/to/VST3SDK
  ```

  The directory/location of the SSRemote VST (needed at runtime):

  ```bash
  # SSRemote VST
  export SSREMOTE_VST=/path/to/ssremote_vst
  ```

  The directory/location of the JUCE Framework located in the SSRemote VST (needed at compile time, should already be included if using VS2017 project created by Projucer):
  
  ```bash
  # SSRemote VST JUCE Framework
  export JUCE_LIB_CODE=$SSREMOTE_VST/JUCE
  ```

# XML Configuration File
The VST Plugin requires a config file to successfully establish a TCP/IP connection to the SSR. The user has to create a file called ssremote_config.xml with the following content and it must be placed in the "SSREMOTE_VST" path env variable as above:
```XML
<?xml version="1.0" encoding="utf-8"?>
<config>
  <network>
    <host>localhost</host>
    <port>4711</port>
    <timeout>1000</timeout>
  </network>
</config>
```


| Markup | Description |
| ------:|:------------|
|network|the network configuration|
|host   |the host on which the SSR is running|
|port   |the port on which the SSR is listening for incoming TCP/IP requests|
|timeout|the time in milliseconds the VST Plugin waits for an answer by the SSR|


The contents of the markups host, port and timeout are just examples which the user may edit regarding his systems and SSR configuration.

# General Information

## Running the VST Plugin

To remotly control the SSR scene from the Plugin the user has to do the following:

Start the JACK Audio Connection Kit
Start the SSR and load a scene
Start the DAW and load the SSRemote VST Plugin

# Description of the GUI

The VST Plugins GUI is generally split in two sections; <b>General Controls</b> section and the <b>Source Controls</b> section.

### General Controls

|Name       |GUI Element|Description|
|:---------:|:---------:|:---------:|
|Connect    |Button     |connects or disconnects the connection to the SSR|
|New Source **(DISABLED)** |Button     |creates a new source with default values for the parameters|

### Source Controls

|Name       |GUI Element|Description|
|:---------:|:---------:|:---------:|
|Source     |Dropdown   |select a source for manipulation|
|Name       |Textfield  |read and writeable textfield for the name of the source|
|Jackport **(DISABLED)**   |Dropdown   |select the jackport the source shall connect to|
|Model **(NOT ACTIVE)**      |Dropdown   |select the type of model of the source|
|Azimuth **(NOT ACTIVE)**    |Textfield  |readable only textfield for displaying the sources orientation|
|Mute **(NOT ACTIVE)**      |Button     |mutes and unmutes the source|
|Fix **(NOT ACTIVE)**        |Button     |fix and unfix the source position|
|Gain **(NOT ACTIVE)**      |Slider     |adjust the gain of the source in dB|
|Source Control Pad|2D Pad|displayes the sources position|
|Zoom       |Buttons    |zooms in and out the Source Control Pad|
|Scale Reference|-      |displays a scale reference in meter for the Source Control Pad|
|X Position |Slider     |controller for the X position of the source|
|Y Position |Slider     |controller for the Y position of the source|

## Automatable Parameters

The Host (DAW) is able to automate the following parameters:

+ Which Source to manipulate
+ X Position
+ Y Position

# Logfiles (Logging is currently disabled!!)
The Plug-In logs will be written to the file log.txt, located in the root directory.
The logs will look as follows:

[YYYY-MM-DD HH:mm:SS] [<LOGTYPE>] <Log text>
 
 <b><i>Logging is currently disabled!!</b></i>

License
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright © 2015
Quality and Usability Lab Berlin Institute of Technology
http://www.qu.tu-berlin.de/

For more detailed information, please read the license.txt in the root directory
