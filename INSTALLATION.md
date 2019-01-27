#TODO :: Add more detailed Windows / VS2017 instructions

ROUGH STEPS FOR WINDOWS / VS2017

1. Download / Clone this repo
2. Download & install/build boost filesystem
3. Download & install JUCE (using ver 5.4.1)
4. Create new project with Projucer for a VST3 plugin
5. Preserve contents of 'JuceLibraryCode' as created by projucer, but delete/move the default PluginProcessor and PluginEditor files 
6. Add VST SDK, Juce, boost paths to VS2017 project
7. Add SSREMOTE_VST path to env variables
8. Copy this repo's folder 'src' into folder 'Source' as created by Projucer
9. Add contents of 'src' to the VS2017 project as created by Projucer 
10. Build
11. Create the network config XML file and save it in the SSREMOTE_VST path folder
10. Load the VST3 file into host DAW or VST3 host

/ROUGH STEPS


Installation / Compilation
The following is a installation guide written for Linux Distributions. The system that was used for testing the guide is Fedora 22. It may help you compiling the SSRemote VST but in no case asserts to claim completness.

The Plugin uses the C++11 Standard which has to be supported by your compiler (e.g. at least GCC 4.7).

Clone the VST Plugin repository from Github:
# HTTPS
git clone https://github.com/QULab/SSRemoteVST.git

# SSH
git clone git@github.com:QULab/SSRemoteVST.git
Install the Steinberg VST SDK
For installing the Steinberg VST SDK checkout the Steinberg VST SDK Wiki Page.

Install several dependencies
The SSRemote VST Plugin uses several external libraries. The following is a list of possible dependencies you may have to solve befor compiling:

Boost Libraries (boost-devel)
OpenGL (freeglut-devel)
Jack Audio Connection Kit (jack-audio-connection-kit-devel)
ALSA Libraries (alsa-lib-devel)
freetype Libraries (freetype-devel)
X11 Libraries (libX11-devel, libXinerama-devel)
Set the environment variables
Please set the following environment variables described as follows:

The directory/location of the Steinberg VST3 SDK:

# Steinberg VST3 Audio Plug-Ins SDK
export VST3_SDK=/path/to/VST3SDK
The directory/location of the SSRemote VST:

# SSRemote VST
export SSREMOTE_VST=/path/to/ssremote_vst
The directory/location of the JUCE Framework located in the SSRemote VST:

# SSRemote VST JUCE Framework
export JUCE_LIB_CODE=$SSREMOTE_VST/JUCE
Compile the SSRemote VST Plugin
Go to the folder of the SSRemote VST and compile as follows:

# go to the SSREmote VST folder
cd $SSREMOTE_VST

# go to the linux build folder
cd builds/linux

# compile
make

# compile multicore
make -j8
Congratulations! You successfully compiled the SSRemote VST Plugin!