/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "ui/PluginEditor.h"

#pragma mark - Construction & Destruction

BlackBirdAudioProcessor::BlackBirdAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

BlackBirdAudioProcessor::~BlackBirdAudioProcessor() {}

#pragma mark - Lifecycle

void BlackBirdAudioProcessor::prepareToPlay(double sampleRate,
                                            int samplesPerBlock) {
  _synth.prepare({sampleRate, (uint32_t)samplesPerBlock,
                  (uint32_t)getTotalNumOutputChannels()});
}

void BlackBirdAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#pragma mark - Capatibilites

#ifndef JucePlugin_PreferredChannelConfigurations

bool BlackBirdAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

#endif

bool BlackBirdAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

bool BlackBirdAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool BlackBirdAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool BlackBirdAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

#pragma mark - Processing Input

void BlackBirdAudioProcessor::processBlock(AudioBuffer<float> &buffer,
                                           MidiBuffer &midiMessages) {
  ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  _synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

#pragma mark - Getting Basic Properties

const String BlackBirdAudioProcessor::getName() const {
  return JucePlugin_Name;
}

const String BlackBirdAudioProcessor::getProgramName(int index) {
  return getPresetsNames()[index];
}

double BlackBirdAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int BlackBirdAudioProcessor::getNumPrograms() {
  auto numberOfPresets = getPresetsNames().size();
  return numberOfPresets != 0 ? numberOfPresets : 1;
  // NB: some hosts don't cope very well if you tell them there are 0 programs,
  // so this should be at least 1, even if you're not really implementing
  // programs.
}

int BlackBirdAudioProcessor::getCurrentProgram() { return currentProgram; }

void BlackBirdAudioProcessor::setCurrentProgram(int index) {
  silentlySetCurrentProgram(index);

  if (onProgramChange) {
    onProgramChange(index);
  }
}

void BlackBirdAudioProcessor::silentlySetCurrentProgram(int index) {
  currentProgram = index;

  auto presetName = getPresetsNames()[index];

  loadPreset(presetName);
}

void BlackBirdAudioProcessor::changeProgramName(int index,
                                                const String &newName) {}

#pragma mark - Accessing State Information

void BlackBirdAudioProcessor::getStateInformation(MemoryBlock &destData) {
  auto state = valueTreeState.copyState();
  std::unique_ptr<XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void BlackBirdAudioProcessor::setStateInformation(const void *data,
                                                  int sizeInBytes) {
  std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

  if (xmlState != nullptr)
    if (xmlState->hasTagName(valueTreeState.state.getType()))
      valueTreeState.replaceState(ValueTree::fromXml(*xmlState));
}

#pragma mark - Handling Presets

File BlackBirdAudioProcessor::getPresetsDirectory() {
  auto userDataFolder =
      File::getSpecialLocation(
          File::SpecialLocationType::commonApplicationDataDirectory)
          .getChildFile(getName());

  if (!userDataFolder.exists()) {
    userDataFolder.createDirectory();
  }

  auto presetsFolder = userDataFolder.getChildFile("Presets");

  if (!presetsFolder.exists()) {
    presetsFolder.createDirectory();
  }

  std::cout << "Presets directory: " << presetsFolder.getFullPathName() << "\n";

  return presetsFolder;
}

StringArray BlackBirdAudioProcessor::getPresetsNames() {
  auto it = DirectoryIterator(getPresetsDirectory(), false, "*.blackBird");

  auto result = StringArray();

  while (it.next()) {
    result.add(it.getFile().getFileNameWithoutExtension());
  }

  return result;
}

void BlackBirdAudioProcessor::loadPreset(const String &presetName) {
  auto presetPath =
      getPresetsDirectory().getChildFile(presetName + ".blackBird");
  auto presetFile = File(presetPath);
  MemoryBlock data;

  if (presetFile.loadFileAsData(data)) {
    setStateInformation(data.getData(), (int)data.getSize());
  } else {
    AlertWindow::showMessageBoxAsync(
        AlertWindow::WarningIcon, TRANS("Error whilst loading"),
        TRANS("Couldn't read from the specified file!"));
  };
}

#pragma mark - Creating Editor Instance

AudioProcessorEditor *BlackBirdAudioProcessor::createEditor() {
  return new BlackBirdAudioProcessorEditor(*this, valueTreeState);
}

#pragma mark - Accessing Synth Instance

Synth &BlackBirdAudioProcessor::synth() { return _synth; }

#pragma mark - Creating Plugin Filter

// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new BlackBirdAudioProcessor();
}
