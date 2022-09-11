# BlackBird

Simple, yet peculiar VST3/AU Synth

## Summary
BlackBird is an analog-inspired software synthesizer. It features two oscillators, each with a choice of sine, saw, or pulse waveforms. It has one envelope that can be used to control velocity as well as the cutoff and resonance of the built-in low-pass filter. BlackBird also has a built-in reverb effect, and mod wheel functionality. Blackbird is available in AU and VST3 forms. 

![Plugin window screenshot](https://raw.githubusercontent.com/khrykin/BlackBird/master/Promo/Screenshot.png) 

Blackbird uses JUCE, a C++ framework  that aids in the creation of audio  plugins. More information about JUCE can be found on their website [here](https://juce.com/). 

## Installation
If you just want to use BlackBird in your own compositions, please visit the [website](https://khrykin.github.io/BlackBird), where you can find installers for Windows. Once you have installed your AU or VST3 plugin, navigate from within your audio software to begin using the plugin. Sound examples are available [here](https://soundcloud.com/khrykin/sets/BlackBird).

## Setup
JUCE provides a utility called “the projucer” that makes setup of projects for development easy. This can be downloaded on the JUCE website [here](https://juce.com/get-juce/download). Once you have downloaded this, extract the folder and place it in a convenient location. Within this folder is an application called Projucer, which will allow you to open and manipulate JUCE projects.

Inside Projucer, open BlackBird.jucer to begin building a project for your IDE. Use “File->Global paths…” to ensure that the JUCE and module paths are set correctly based on where you extracted your JUCE directory. If you wish to use different versions of JUCE modules, you can set individual paths in the Modules tab on the left. Once you have your settings selected, choose your exporter at the top of Projucer (Xcode or Visual Studio), and then click the button to the right of this selection to build your project. You can now begin development on BlackBird in your chosen IDE.

This process should create three projects, BlackBird_SharedCode, BlackBird_StandalonePlugin, and BlackBird_VST3. BlackBird_SharedCode contains the bulk of the BlackBird source, while the other two contain information for specific builds of the synthesizer.

If you wish to change any settings in the Projucer, make sure to use the File->Save Project option inside the Projucer. This is a crucial step, as several code files within JUCE projects are auto-generated based on your unique configuration, meaning that failing to save from within the Projucer could result in issues when trying to build the project. Your IDE, if open at this time, may warn you about external changes to the project you are working on. Be sure to accept these changes.

To begin modifying BlackBird, look in BlackBird_SharedCode/BlackBird/Source. Here you will find PluginProcessor.cpp and PluginProcessor.h. You will also find a folder of code related to the UI, and another folder with code related to DSP, or Digital Signal Processing.

## Testing
To build your own version of BlackBird, simply press build from within your chosen IDE. If working on a macOS device, your audio plugin will automatically be copied to 
~/Library/Audio/Plug-Ins/VST3, but on Windows you will have to manually copy the generate .vst3 file. You may then test the synthesizer in an audio plugin, or you can use the Audio Plugin Host built in to JUCE. To activate this, use Projucer and open the .jucer file located in extras/AudioPluginHost/, then use the same method as above to open in your IDE. From here, you can build the Audio Plugin Host which will provide a testing environment. Click Options-> Edit The List of Available Plug-ins, which will provide you with a tool to scan for new plugins. At this point you can load Blackbird into the testing environment and try it out.
