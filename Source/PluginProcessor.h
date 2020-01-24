/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Synth.h"

class BlackFaceAudioProcessor : public AudioProcessor {
public:
    std::function<void(int)> onProgramChange = nullptr;

    BlackFaceAudioProcessor();
    ~BlackFaceAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

    AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    void silentlySetCurrentProgram(int index);

    const String getProgramName(int index) override;
    void changeProgramName(int index, const String &newName) override;

    void getStateInformation(MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    File getPresetsDirectory();
    StringArray getPresetsNames();
    void loadPreset(const String &presetName);

    Synth &synth();

private:
    AudioProcessorValueTreeState valueTreeState{*this,
                                                nullptr,
                                                Identifier("BlackFace"),
                                                DSPParameters::makeLayout()};

    DSPParameters parameters{valueTreeState};
    Synth _synth{parameters};

    int currentProgram = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlackFaceAudioProcessor)
};
