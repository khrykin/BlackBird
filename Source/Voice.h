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

#include <cmath>
#include <chrono>

template<typename ValueType>
class CustomOscillator {
public:
    enum Type {
        sin,
        saw,
        square
    };

    explicit CustomOscillator(Type type = Type()) : _type(type) {
        updateWaveTable();
    }

    const std::atomic<Type> &type() const {
        return _type;
    }

    void setTypeForFrequency(Type type, float noteFrequency = 440.0) {
        _type = type;
        updateWaveTable(noteFrequency);
    }

    void updateWaveTable(float noteFrequency = 440.0) {
        auto &osc = processorChain.template get<oscIndex>();
        auto tableSize = 2048; //(int) (sampleRate / noteFrequency);

        switch (_type) {
            case sin:
                osc.initialise([](ValueType x) { return std::sin(x); }, tableSize);
                break;
            case saw:
                osc.initialise([](ValueType x) {
//                    return jmap(x,
//                                ValueType(-MathConstants<double>::pi),
//                                ValueType(MathConstants<double>::pi),
//                                ValueType(-1),
//                                ValueType(1));
                    return x / (2 * MathConstants<double>::pi) + 0.5;
                }, tableSize);
                break;
            case square:
                osc.initialise([](ValueType x) {
                    return x < 0 ? -1 : 1;
                }, tableSize);
                break;
        }
    }

    void setFrequency(ValueType newValue, bool force = false) {
        auto &osc = processorChain.template get<oscIndex>();
        osc.setFrequency(newValue, force);
    }

    void setLevel(ValueType newValue) {
        auto &gain = processorChain.template get<gainIndex>();
        gain.setGainLinear(newValue);
    }

    void setRampDurationSeconds(double duration) {
        auto &gain = processorChain.template get<gainIndex>();
        gain.setRampDurationSeconds(duration);
    }

    void reset() noexcept {
        processorChain.reset();
    }

    template<typename ProcessContext>
    void process(const ProcessContext &context) noexcept {
        processorChain.process(context);
    }

    void prepare(const dsp::ProcessSpec &spec) {
        processorChain.prepare(spec);
        sampleRate = spec.sampleRate;
    }

private:
    enum {
        oscIndex,
        gainIndex
    };

    float sampleRate = 0.0;

    std::atomic<Type> _type = Type();

    dsp::ProcessorChain<
            dsp::Oscillator<ValueType>,
            dsp::Gain<ValueType>
    > processorChain;
};


class SineSound : public SynthesiserSound {
public:
    bool appliesToNote(int midiNoteNumber) override {
        return true;
    };

    bool appliesToChannel(int channelNumber) override {
        return true;
    };
};


class SineVoice : public SynthesiserVoice {
public:
    static constexpr auto gainHeadroom = 0.9f;
    static constexpr auto minCutoff = 50.0f;
    static constexpr auto maxCutoff = 4000.0f;

    static constexpr auto minEnvelopeDurationSeconds = 0.01f;
    static constexpr auto maxEnvelopeDurationSeconds = 15.0f;

    static constexpr auto defaultCutoff = maxCutoff;
    static constexpr auto defaultResonance = 0.0f;

    static constexpr auto defaultAttack = minEnvelopeDurationSeconds;
    static constexpr auto defaultDecay = minEnvelopeDurationSeconds;
    static constexpr auto defaultSustain = 1.0f;
    static constexpr auto defaultRelease = minEnvelopeDurationSeconds;

    static constexpr auto defaultCutoffEnvelopeAmount = 0.0;
    static constexpr auto defaultResonanceEnvelopeAmount = 0.0;
    static constexpr auto defaultVelocityEnvelopeAmount = 1.0;

    static constexpr auto pitchWheelSemitonesRange = 2.0f;

    static constexpr auto defaultDetuningFactor = 2.0;
    static constexpr auto maxDetuningFactor = 6.0;
    static constexpr auto maxAnalogFactor = 0.0025f;

    using OscillatorType = CustomOscillator<float>::Type;

    explicit SineVoice(DSPParameters &parameters) : parameters(parameters) {
        auto &filter = processorChain.get<filterIndex>();
        filter.setCutoffFrequencyHz(*parameters.cutoff);
        filter.setResonance(*parameters.resonance);
    }

    void prepare(const dsp::ProcessSpec &spec) noexcept {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        tempBlock = dsp::AudioBlock<float>(heapBlock,
                                           spec.numChannels,
                                           spec.maximumBlockSize);
        processorChain.prepare(spec);

        auto lfoSampleRate = spec.sampleRate / lfoSubBlockSize;
        adsr.setSampleRate(lfoSampleRate);

        auto lfoSampleDuration = 1 / lfoSampleRate;

        processorChain.get<osc1Index>().setRampDurationSeconds(lfoSampleDuration);
        processorChain.get<osc2Index>().setRampDurationSeconds(lfoSampleDuration);

        processorChain.get<gainIndex>().setGainLinear(1.0 - gainHeadroom);

        lfo.initialise([](float x) { return std::sin(x); }, 128);
        lfo.prepare({lfoSampleRate,
                     spec.maximumBlockSize,
                     spec.numChannels});
    }

    bool canPlaySound(SynthesiserSound *sound) override {
        return dynamic_cast<SineSound *>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber,
                   float velocity,
                   SynthesiserSound *,
                   int currentPitchWheelPosition) override {
        currentNoteFrequency = getBendedFrequencyForWheel(currentPitchWheelPosition,
                                                          getCurrentlyPlayingNote());

        updateOscillatorsType();
        updateFilterDrive();

        lfo.setFrequency(currentNoteFrequency / std::pow(2, 5));

        currentVelocity = 1.0 - (*parameters.velocityEnvelopeAmount) * (1.0 - velocity);

        processorChain.get<osc1Index>().setLevel(currentVelocity);
        processorChain.get<osc2Index>().setLevel(currentVelocity);

        updateOscillatorsFrequency();

        if (modulationAmount) {
            updateModulation();
        }

        updateADSRParameters();
        adsr.noteOn();

        noteIsPlaying = true;
    }

    void stopNote(float /* velocity */, bool allowTailOff) override {
        if (allowTailOff) {
            adsr.noteOff();
        } else {
            clearCurrentNote();
            noteIsPlaying = false;
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

    void renderNextBlock(AudioSampleBuffer &outputBuffer,
                         int startSampleIndex,
                         int numSamples) override {
        auto output = tempBlock.getSubBlock(0, (size_t) numSamples);
        output.clear();

        if (noteIsPlaying) {
            bool adsrWasActive = adsr.isActive();

            if (currentOscillatorType != (OscillatorType) parameters.oscillatorType->load()) {
                updateOscillatorsType();
            }

            if (currentDetuningAmount != *parameters.detuningAmount) {
                updateOscillatorsFrequency();
            }

            if (currentFilterDrive != *parameters.filterDrive) {
                updateFilterDrive();
            }

            for (size_t subBlockPosition = 0; subBlockPosition < (size_t) numSamples;) {
                auto subBlockSize = jmin(lfoSubBlockSize, (size_t) numSamples - subBlockPosition);
                auto subBlock = output.getSubBlock(subBlockPosition, subBlockSize);

                juce::dsp::ProcessContextReplacing<float> context(subBlock);

                updateADSRParameters();
                auto nextADSRSample = adsr.getNextSample();
                adsrWasActive = adsr.isActive();

                updateLevelWithADSRSample(nextADSRSample);
                updateFilterWithADSRSample(nextADSRSample);

                updateModulation();

                processorChain.process(context);

                subBlockPosition += subBlockSize;
            }

            if (adsrWasActive && !adsr.isActive()) {
                noteIsPlaying = false;
                clearCurrentNote();
            }
        }

        juce::dsp::AudioBlock<float>(outputBuffer)
                .getSubBlock((size_t) startSampleIndex, (size_t) numSamples)
                .add(tempBlock);
    }

    inline void updateOscillatorsType() {
        currentOscillatorType = (OscillatorType) parameters.oscillatorType->load();

        processorChain.get<osc1Index>().setTypeForFrequency(currentOscillatorType,
                                                            currentNoteFrequency);
        processorChain.get<osc2Index>().setTypeForFrequency(currentOscillatorType,
                                                            currentNoteFrequency);
    }

    inline void updateADSRParameters() {
        adsr.setParameters({*parameters.attack,
                            *parameters.decay,
                            *parameters.sustain,
                            *parameters.release});
    }

    inline void updateOscillatorsFrequency() {
        currentOsc1Frequency = currentNoteFrequency * (1.0f + analogFactor());
        currentOsc2AnalogFactor = analogFactor();
        currentOsc2Frequency = currentNoteFrequency *
                               (1.0f + maxDetuningFactor * (*parameters.detuningAmount) * currentOsc2AnalogFactor);

        currentDetuningAmount = *parameters.detuningAmount;

        processorChain.get<osc1Index>().setFrequency(currentOsc1Frequency, true);
        processorChain.get<osc2Index>().setFrequency(currentOsc2Frequency, true);
    }

    inline void updateFilterDrive() {
        currentFilterDrive = *parameters.filterDrive;
        processorChain.get<filterIndex>().setDrive(currentFilterDrive);
    }

    inline float analogFactor() const {
        return (Random::getSystemRandom().nextBool() ? 1.0f : -1.0f)
               * maxAnalogFactor * Random::getSystemRandom().nextFloat();
    }

    inline double getBendedFrequencyForWheel(int newPitchWheelValue,
                                             int currentlyPlayingNote) {
        auto bendSemitonesValue = pitchWheelSemitonesRange * getBendValueForWheel(newPitchWheelValue);
        auto noteFrequency = MidiMessage::getMidiNoteInHertz(currentlyPlayingNote);

        if (!bendSemitonesValue)
            return noteFrequency;

        return noteFrequency * std::pow(2.0f, 1.0f * bendSemitonesValue / 12);
    }

    /**
     * Returns bend value in a range of -1.0 to 1.0
     */

    inline double getBendValueForWheel(int newPitchWheelValue) const {
        if (newPitchWheelValue >= 0x3FFF)
            newPitchWheelValue = 0x3FFF + 0x1;

        return 2.0 * (double) (newPitchWheelValue - 0x2000) / 0x3FFF;
    }


    inline void updateModulation() {
        processorChain.get<osc2Index>().setFrequency(
                currentOsc2Frequency
                * (1.0
                   + modulationAmount
                     * lfo.processSample(0.0f)
                     * (maxDetuningFactor - defaultDetuningFactor) * maxAnalogFactor),
                true);
    }

    inline void updateFilterWithADSRSample(float nextADSRSample) {
        auto &filter = processorChain.get<filterIndex>();

        auto envelopeCutoffValue = *parameters.cutoffEnvelopeAmount >= 0
                                   ? (1.0 - *parameters.cutoffEnvelopeAmount) +
                                     *parameters.cutoffEnvelopeAmount * nextADSRSample
                                   : 1.0 + *parameters.cutoffEnvelopeAmount * nextADSRSample;

        filter.setCutoffFrequencyHz(envelopeCutoffValue * (*parameters.cutoff - minCutoff) + minCutoff);

        auto envelopeResonanceValue = *parameters.resonanceEnvelopeAmount >= 0
                                      ? (1.0 - *parameters.resonanceEnvelopeAmount) +
                                        *parameters.resonanceEnvelopeAmount * nextADSRSample
                                      : 1.0 + *parameters.resonanceEnvelopeAmount * nextADSRSample;

        filter.setResonance(envelopeResonanceValue * *parameters.resonance);
    }

    inline void updateLevelWithADSRSample(float nextADSRSample) {
        auto envelopeVelocityValue = (1.0 - (*parameters.velocityEnvelopeAmount)
                                            * (1.0 - currentVelocity)) * nextADSRSample;

        processorChain.get<osc1Index>().setLevel(envelopeVelocityValue);
        processorChain.get<osc2Index>().setLevel(envelopeVelocityValue);
    }

private:
    static constexpr size_t lfoSubBlockSize = 50;

    DSPParameters &parameters;

    float currentVelocity = 0.0;
    float currentNoteFrequency = 0.0;
    float modulationAmount = 0.0;
    float currentDetuningAmount = parameters.detuningAmount->load();
    float currentFilterDrive = parameters.filterDrive->load();

    float currentOsc1Frequency = 0.0;
    float currentOsc2Frequency = 0.0;
    float currentOsc2AnalogFactor = 0.0;

    OscillatorType currentOscillatorType = (OscillatorType) parameters.oscillatorType->load();

    bool noteIsPlaying = false;

    HeapBlock<char> heapBlock;
    dsp::AudioBlock<float> tempBlock;

    enum {
        osc1Index,
        osc2Index,
        filterIndex,
        gainIndex,
    };

    dsp::ProcessorChain<
            CustomOscillator<float>,
            CustomOscillator<float>,
            dsp::LadderFilter<float>,
            dsp::Gain<float>
    > processorChain;

    juce::dsp::Oscillator<float> lfo;

    ADSR adsr;
};
