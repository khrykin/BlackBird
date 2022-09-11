# BlackBird

Simple, yet peculiar VST3/AU Synth

BlackBird is an analog-inspired software synthesizer. It features two oscillators, each with 3 waveform choices. It has one envelope that can be used to control velocity as well as the cutoff and resonance of the built-in low-pass filter. BlackBird also has a built-in reverb effect, and mod wheel functionality.

If you just want to use BlackBird in your own compositions, please visit the [website](https://khrykin.github.io/BlackBird). Sound examples are available [here](https://soundcloud.com/khrykin/sets/BlackBird).

![Plugin window screenshot](https://raw.githubusercontent.com/khrykin/BlackBird/master/Promo/Screenshot.png)

## Development
BlackBird makes use of JUCE, a development framework for audio applications. More information about JUCE can be found on their [website](https://juce.com/). Fortunately, JUCE provides a utility called “the projucer” that makes setup of projects for development easy. This can be downloaded on the JUCE website [here](https://juce.com/get-juce/download). Once you have downloaded this, extract the folder and place it in a convenient location. Run the “projucer” program, and open BlackBird.jucer to begin building a project. Use “File->Global paths…” to ensure that the module and JUCE paths are set correctly based on where you extracted your JUCE directory. Once this has been established, choose your exporter at the top of Projucer (Xcode or Visual Studio), and then click the button to the right of this selection to build your project. You can now begin development on BlackBird in your chosen IDE.
