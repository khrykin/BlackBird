/*
  ==============================================================================

    Synth.h
    Created: 12 Jan 2020 6:07:10pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Voice.h"
#include "LookupTablesBank.h"

class Synth : public Synthesiser {
public:

#pragma mark - Static Properties

    static constexpr auto minCutoff = Voice::minCutoff;
    static constexpr auto maxCutoff = Voice::maxCutoff;

    static constexpr auto minEnvelopeDurationSeconds = Voice::minEnvelopeDurationSeconds;
    static constexpr auto maxEnvelopeDurationSeconds = Voice::maxEnvelopeDurationSeconds;

#pragma mark - Default Properties Values

    static constexpr auto defaultCutoff = Voice::defaultCutoff;
    static constexpr auto defaultResonance = Voice::defaultResonance;

    static constexpr auto defaultAttack = Voice::defaultAttack;
    static constexpr auto defaultDecay = Voice::defaultDecay;
    static constexpr auto defaultSustain = Voice::defaultSustain;
    static constexpr auto defaultRelease = Voice::defaultRelease;

    static constexpr auto defaultCutoffEnvelopeAmount = Voice::defaultCutoffEnvelopeAmount;
    static constexpr auto defaultResonanceEnvelopeAmount = Voice::defaultResonanceEnvelopeAmount;
    static constexpr auto defaultVelocityEnvelopeAmount = Voice::defaultVelocityEnvelopeAmount;

    static constexpr auto defaultReverb = 0.0f;
    static constexpr auto defaultMasterGain = 1.0f;

#pragma mark - Construction

    explicit Synth(DSPParameters &parameters) : parameters(parameters) {
        for (auto i = 0; i < maxNumVoices; ++i) {
            addVoice(new Voice(parameters));
        }

        addSound(new Sound());
    }

#pragma mark - Preparing for Operation

    void prepare(const dsp::ProcessSpec &spec) noexcept {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        lookupTablesBank.initialize(spec.sampleRate);

        for (auto *genericVoice : voices) {
            auto *voice = dynamic_cast<Voice *>(genericVoice);

            voice->prepare(spec, lookupTablesBank);
        }

        tempBlock = dsp::AudioBlock<float>(heapBlock,
                                           spec.numChannels,
                                           spec.maximumBlockSize);

        fxChain.prepare(spec);
    }

#pragma mark - Reverb

    inline bool reverbIsOn() const {
        return *parameters.reverb != 0.0f;
    }

private:
    static constexpr auto maxNumVoices = 5;

    DSPParameters &parameters;

    enum {
        reverbIndex,
        reverbGainIndex,
    };

    HeapBlock<char> heapBlock;
    dsp::AudioBlock<float> tempBlock;

    float lastMasterGain = *parameters.masterGain;
    float lastReverbGain = *parameters.reverb;

    LookupTablesBank<float> lookupTablesBank;

    dsp::ProcessorChain<
        dsp::Reverb,
        dsp::Gain<float>
    > fxChain;

#pragma mark - Rendering Audio Output

    void renderVoices(AudioBuffer<float> &outputBuffer, int startSampleIndex, int numSamples) override {
        Synthesiser::renderVoices(outputBuffer, startSampleIndex, numSamples);

        if (reverbIsOn())
            applyMasterFxChain(outputBuffer, startSampleIndex, numSamples);

        outputBuffer.applyGainRamp(startSampleIndex,
                                   numSamples,
                                   lastMasterGain,
                                   *parameters.masterGain);

        lastMasterGain = *parameters.masterGain;
    }

    void applyMasterFxChain(AudioBuffer<float> &outputBuffer, int startSampleIndex, int numSamples) {
        auto fxBlock = tempBlock.getSubBlock(0, (size_t) numSamples);
        fxBlock.copyFrom(outputBuffer, startSampleIndex, 0, numSamples);

        auto contextToUse = dsp::ProcessContextReplacing<float>(fxBlock);

        auto &reverbGain = fxChain.get<reverbGainIndex>();
        reverbGain.setGainLinear(*parameters.reverb);

        fxChain.process(contextToUse);

        outputBuffer.applyGainRamp(startSampleIndex,
                                   numSamples,
                                   1.0 - lastReverbGain,
                                   1.0 - *parameters.reverb);

        lastReverbGain = *parameters.reverb;

        juce::dsp::AudioBlock<float>(outputBuffer)
            .getSubBlock((size_t) startSampleIndex, (size_t) numSamples)
            .add(tempBlock);
    }
};
