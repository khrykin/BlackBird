/*
  ==============================================================================

    Voices.h
    Created: 11 Jan 2020 8:40:25pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "DSPParameters.h"
#include "VCAOscillator.h"
#include "LookupTablesBank.h"

//class LookupTablesBank;

#pragma mark - Sound Class

class Sound : public SynthesiserSound {
public:
    bool appliesToNote(int /* midiNoteNumber */) override {
        return true;
    }

    bool appliesToChannel(int /* channelNumber */) override {
        return true;
    }
};

#pragma mark - Voice Class

class Voice : public SynthesiserVoice, private Timer {
public:

#pragma mark - Static Settings

    static constexpr auto gainHeadroom = 0.9f;

    static constexpr auto minCutoff = 50.0f;
    static constexpr auto maxCutoff = 22000.0f;

    static constexpr auto minEnvelopeDurationSeconds = 0.01f;
    static constexpr auto maxEnvelopeDurationSeconds = 15.0f;

    static constexpr auto pitchWheelSemitonesRange = 2.0f;

    static constexpr auto maxDetuningFactor = 6.0;
    static constexpr auto maxAnalogFactor = 0.0025f;

#pragma mark - Default Properties Values

    static constexpr auto defaultCutoff = maxCutoff;
    static constexpr auto defaultResonance = 0.0f;

    static constexpr auto defaultAttack = minEnvelopeDurationSeconds;
    static constexpr auto defaultDecay = minEnvelopeDurationSeconds;
    static constexpr auto defaultSustain = 1.0f;
    static constexpr auto defaultRelease = minEnvelopeDurationSeconds;

    static constexpr auto defaultCutoffEnvelopeAmount = 0.0;
    static constexpr auto defaultResonanceEnvelopeAmount = 0.0;
    static constexpr auto defaultVelocityEnvelopeAmount = 1.0;

    static constexpr auto defaultDetuningFactor = 2.0;

#pragma mark - Type Aliases

    using Waveform = VCAOscillator<float>::Waveform;

#pragma mark - Construction

    explicit Voice(DSPParameters &parameters) : parameters(parameters) {
        filter().setCutoffFrequencyHz(*parameters.cutoff);
        filter().setResonance(*parameters.resonance);
    }

#pragma mark - Preparing Voice For Operation

    void prepare(const dsp::ProcessSpec &spec, const LookupTablesBank<float> &lookupTable) noexcept {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        tempBlock = dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);

        // Initialize Oscillators

        firstOscillator().initialize(lookupTable);
        secondOscillator().initialize(lookupTable);

        processorChain.prepare(spec);

        // Initialize LFO & VCAs Ramps

        auto lfoSampleRate = spec.sampleRate / lfoSubBlockSize;
        auto lfoSampleDuration = 1 / lfoSampleRate;

        adsr.setSampleRate(lfoSampleRate);

        firstOscillator().setRampDurationSeconds(lfoSampleDuration);
        secondOscillator().setRampDurationSeconds(lfoSampleDuration);

        gainProcessor().setGainLinear(1.0 - gainHeadroom);

        lfo.initialise([](float x) { return std::sin(x); }, 1024);
        lfo.prepare({lfoSampleRate, spec.maximumBlockSize, spec.numChannels});

        clearCurrentNote();
    }

#pragma mark - Juce SynthesiserVoice Methods Overrides

    bool canPlaySound(SynthesiserSound *sound) override {
        return dynamic_cast<Sound *>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber,
                   float velocity,
                   SynthesiserSound *,
                   int currentPitchWheelPosition) override {
        currentNoteFrequency = getBendedFrequencyForWheel(currentPitchWheelPosition,
                                                          getCurrentlyPlayingNote());

        updateFilterDrive();
        updateOscillatorsFrequency();

        lfo.setFrequency(currentNoteFrequency / std::pow(2, 5));

        currentVelocity = 1.0f - (*parameters.velocityEnvelopeAmount) * (1.0f - velocity);

        firstOscillator().setLevel(currentVelocity);
        firstOscillator().setLevel(currentVelocity);

        if (modulationAmount > 0.0f) {
            updateModulation();
        }

        updateADSRParameters();

        noteWillStartAttack();
        adsr.noteOn();
    }

    void stopNote(float /* velocity */, bool allowTailOff) override {
        if (allowTailOff) {
            adsr.noteOff();
        } else {
            noteDidFade();
            adsr.reset();
        }
    }

    void pitchWheelMoved(int newPitchWheelValue) override {
        currentNoteFrequency = getBendedFrequencyForWheel(newPitchWheelValue,
                                                          getCurrentlyPlayingNote());

        updateOscillatorsFrequency();
    }

    void controllerMoved(int controllerNumber, int newControllerValue) override {
        if (controllerNumber == 1) {
            auto percentage = (float) newControllerValue / 127;
            modulationAmount = percentage;
        }
    }

#pragma mark - Rendering Audio Output

    void renderNextBlock(AudioSampleBuffer &outputBuffer,
                         int startSampleIndex,
                         int numSamples) override {
        auto output = tempBlock.getSubBlock(0, (size_t) numSamples);
        output.clear();

        if (noteIsPlaying) {
            for (size_t subBlockPosition = 0; subBlockPosition < (size_t) numSamples;) {
                auto subBlockSize = jmin(lfoSubBlockSize, (size_t) numSamples - subBlockPosition);
                auto subBlock = output.getSubBlock(subBlockPosition, subBlockSize);

                renderLFOSubBlock(subBlock);

                subBlockPosition += subBlockSize;
            }

            if (!adsr.isActive() && !isTimerRunning()) {
                noteDidFade();
            }
        }

        dsp::AudioBlock<float>(outputBuffer)
            .getSubBlock(startSampleIndex, numSamples)
            .add(tempBlock);
    }

private:
    static constexpr size_t lfoSubBlockSize = 50;

    HeapBlock<char> heapBlock;
    dsp::AudioBlock<float> tempBlock;

    DSPParameters &parameters;

    float currentVelocity = 0.0;
    float currentNoteFrequency = 0.0;
    float modulationAmount = 0.0;
    float currentDetuningAmount = *parameters.detuningAmount;
    float currentFilterDrive = *parameters.filterDrive;

    float currentOsc1Frequency = 0.0;
    float currentOsc2Frequency = 0.0;
    float currentOsc2AnalogFactor = 0.0;

    Waveform currentWaveform = static_cast<Waveform>(static_cast<int>(*parameters.oscillatorWaveform));

    bool noteIsPlaying = false;

    enum {
        osc1Index,
        osc2Index,
        filterIndex,
        gainIndex,
    };

    dsp::ProcessorChain<
        VCAOscillator<float>,
        VCAOscillator<float>,
        dsp::LadderFilter<float>,
        dsp::Gain<float>
    > processorChain;

    dsp::Oscillator<float> lfo;

    ADSR adsr;

#pragma mark - Accessing Processors

    VCAOscillator<float> &firstOscillator() {
        return processorChain.get<osc1Index>();
    }

    VCAOscillator<float> &secondOscillator() {
        return processorChain.get<osc2Index>();
    }

    dsp::Gain<float> &gainProcessor() {
        return processorChain.get<gainIndex>();
    }

    dsp::LadderFilter<float> &filter() {
        return processorChain.get<filterIndex>();
    }


#pragma mark - Helper Functions

    void renderLFOSubBlock(dsp::AudioBlock<float> &subBlock) {
        dsp::ProcessContextReplacing<float> context(subBlock);

        updateCurrentDSPState();
        updateADSRParameters();

        auto nextADSRSample = adsr.getNextSample();

        updateLevelWithADSRSample(nextADSRSample);
        updateFilterWithADSRSample(nextADSRSample);

        updateModulation();

        processorChain.process(context);
    }

    static float analogFactor() {
        return (Random::getSystemRandom().nextBool() ? 1.0f : -1.0f)
               * maxAnalogFactor * Random::getSystemRandom().nextFloat();
    }

    static double getBendedFrequencyForWheel(int newPitchWheelValue, int currentlyPlayingNote) {
        auto bendSemitonesValue = pitchWheelSemitonesRange * getBendValueForWheel(newPitchWheelValue);
        auto noteFrequency = MidiMessage::getMidiNoteInHertz(currentlyPlayingNote);

        if (!bendSemitonesValue)
            return noteFrequency;

        return noteFrequency * std::pow(2.0f, 1.0f * bendSemitonesValue / 12);
    }

    /** Returns bend value in a range of -1.0 to 1.0 */
    static double getBendValueForWheel(int newPitchWheelValue) {
        if (newPitchWheelValue >= 0x3fff)
            newPitchWheelValue = 0x3fff + 0x1;

        return 2.0 * (double) (newPitchWheelValue - 0x2000) / 0x3fff;
    }

#pragma mark - Updating DSP-Related State

    void updateCurrentDSPState() {
        if (currentWaveform != Waveform(static_cast<int>(*parameters.oscillatorWaveform))) {
            updateOscillatorsWaveform();
        }

        if (currentDetuningAmount != *parameters.detuningAmount) {
            updateOscillatorsFrequency();
        }

        if (currentFilterDrive != *parameters.filterDrive) {
            updateFilterDrive();
        }
    }

    void updateOscillatorsWaveform() {
        currentWaveform = static_cast<Waveform>(static_cast<int>(*parameters.oscillatorWaveform));

        firstOscillator().setWaveform(currentWaveform);
        secondOscillator().setWaveform(currentWaveform);
    }

    void updateADSRParameters() {
        adsr.setParameters({*parameters.attack,
                            *parameters.decay,
                            *parameters.sustain,
                            *parameters.release});
    }

    void updateOscillatorsFrequency() {
        currentOsc1Frequency = currentNoteFrequency * (1.0f + analogFactor());
        currentOsc2AnalogFactor = analogFactor();
        currentOsc2Frequency = currentNoteFrequency *
                               (1.0f + maxDetuningFactor * (*parameters.detuningAmount) * currentOsc2AnalogFactor);

        currentDetuningAmount = *parameters.detuningAmount;

        firstOscillator().setFrequency(currentOsc1Frequency);
        secondOscillator().setFrequency(currentOsc2Frequency);
    }

    void updateFilterDrive() {
        currentFilterDrive = *parameters.filterDrive;

        filter().setDrive(currentFilterDrive);
    }

    void updateModulation() {
        secondOscillator().setFrequency(currentOsc2Frequency
                                        * (1.0 + modulationAmount
                                                 * lfo.processSample(0.0f)
                                                 * (maxDetuningFactor - defaultDetuningFactor) * maxAnalogFactor));
    }

    void updateFilterWithADSRSample(float nextADSRSample) {
        const auto envelopeCutoffValue = *parameters.cutoffEnvelopeAmount >= 0
                                         ? (1.0 - *parameters.cutoffEnvelopeAmount) +
                                           *parameters.cutoffEnvelopeAmount * nextADSRSample
                                         : 1.0 + *parameters.cutoffEnvelopeAmount * nextADSRSample;

        filter().setCutoffFrequencyHz(envelopeCutoffValue * (*parameters.cutoff - minCutoff) + minCutoff);

        const auto envelopeResonanceValue = *parameters.resonanceEnvelopeAmount >= 0
                                            ? (1.0 - *parameters.resonanceEnvelopeAmount) +
                                              *parameters.resonanceEnvelopeAmount * nextADSRSample
                                            : 1.0 + *parameters.resonanceEnvelopeAmount * nextADSRSample;

        filter().setResonance(envelopeResonanceValue * *parameters.resonance);
    }

    void updateLevelWithADSRSample(float nextADSRSample) {
        auto envelopeVelocityValue = (1.0 - (*parameters.velocityEnvelopeAmount)
                                            * (1.0 - currentVelocity)) * nextADSRSample;

        firstOscillator().setLevel(envelopeVelocityValue);
        secondOscillator().setLevel(envelopeVelocityValue);
    }

#pragma mark - Note Lifecycle

    void noteWillStartAttack() {
        noteIsPlaying = true;

        stopTimer();
    }

    void noteDidFade() {
        startTimer(1000);
    }

#pragma mark - Bypassing processing

    void setProcessorsBypassed(bool bypassed) {
        processorChain.template setBypassed<osc1Index>(bypassed);
        processorChain.template setBypassed<osc2Index>(bypassed);

        filter().setEnabled(!bypassed);
    }

#pragma mark - Timer

    /** Timer callback is called some time after note tails off. */
    void timerCallback() override {
        stopTimer();

        noteIsPlaying = false;
        clearCurrentNote();
    }
};
