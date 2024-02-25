/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "dsp/Synth.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

using namespace juce;

class BlackBirdAudioProcessor : public AudioProcessor {
public:
#pragma mark - Listening to Changes

  std::function<void(int)> onProgramChange = nullptr;

#pragma mark - Construction & Destruction

  BlackBirdAudioProcessor();
  ~BlackBirdAudioProcessor() override;

#pragma mark - Lifecycle

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#pragma mark - Capatibilites

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  bool hasEditor() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;

#pragma mark - Processing Input

  void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

#pragma mark - Getting Basic Properties

  const String getName() const override;
  const String getProgramName(int index) override;
  double getTailLengthSeconds() const override;

#pragma mark - Handling Programs

  int getNumPrograms() override;
  int getCurrentProgram() override;

  void setCurrentProgram(int index) override;
  void silentlySetCurrentProgram(int index);

  void changeProgramName(int index, const String &newName) override;

#pragma mark - Accessing State Information

  void getStateInformation(MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

#pragma mark - Handling Presets

  File getPresetsDirectory();
  StringArray getPresetsNames();
  void loadPreset(const String &presetName);

#pragma mark - Creating Editor Instance

  AudioProcessorEditor *createEditor() override;

#pragma mark - Accessing Synth Instance

  Synth &synth();

private:
  AudioProcessorValueTreeState valueTreeState{
      *this, nullptr, Identifier("BlackBird"), DSPParameters::makeLayout()};

  DSPParameters parameters{valueTreeState};
  Synth _synth{parameters};

  int currentProgram = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlackBirdAudioProcessor)
};
