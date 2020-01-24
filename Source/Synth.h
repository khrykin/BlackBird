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

class Synth : public Synthesiser {
public:
    static constexpr auto minCutoff = SineVoice::minCutoff;
    static constexpr auto maxCutoff = SineVoice::maxCutoff;

    static constexpr auto minEnvelopeDurationSeconds = SineVoice::minEnvelopeDurationSeconds;
    static constexpr auto maxEnvelopeDurationSeconds = SineVoice::maxEnvelopeDurationSeconds;

    static constexpr auto defaultCutoff = SineVoice::defaultCutoff;
    static constexpr auto defaultResonance = SineVoice::defaultResonance;

    static constexpr auto defaultAttack = SineVoice::defaultAttack;
    static constexpr auto defaultDecay = SineVoice::defaultDecay;
    static constexpr auto defaultSustain = SineVoice::defaultSustain;
    static constexpr auto defaultRelease = SineVoice::defaultRelease;

    static constexpr auto defaultCutoffEnvelopeAmount = SineVoice::defaultCutoffEnvelopeAmount;
    static constexpr auto defaultResonanceEnvelopeAmount = SineVoice::defaultResonanceEnvelopeAmount;
    static constexpr auto defaultVelocityEnvelopeAmount = SineVoice::defaultVelocityEnvelopeAmount;

    static constexpr auto defaultReverb = 0.0f;
    static constexpr auto defaultMasterGain = 1.0f;

    explicit Synth(DSPParameters &parameters) : parameters(parameters) {
        for (auto i = 0; i < maxNumVoices; ++i)
            addVoice(new SineVoice(parameters));

        addSound(new SineSound());
    }

    void prepare(const dsp::ProcessSpec &spec) noexcept {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        for (auto *voice : voices) {
            dynamic_cast<SineVoice *>(voice)->prepare(spec);
        }

        tempBlock = dsp::AudioBlock<float>(heapBlock,
                                           spec.numChannels,
                                           spec.maximumBlockSize);

        fxChain.prepare(spec);
    }

    bool reverbIsOn() {
        return *parameters.reverb != 0.0f;
    }

private:
    static constexpr auto maxNumVoices = 4;
    DSPParameters &parameters;

    enum {
        reverbIndex,
        reverbGainIndex,
    };

    HeapBlock<char> heapBlock;
    dsp::AudioBlock<float> tempBlock;

    float lastMasterGain = *parameters.masterGain;
    float lastReverbGain = *parameters.reverb;

    dsp::ProcessorChain<
            dsp::Reverb,
            dsp::Gain<float>
    > fxChain;

    void renderVoices(AudioBuffer<float> &outputBuffer,
                      int startSampleIndex,
                      int numSamples) override {
        Synthesiser::renderVoices(outputBuffer, startSampleIndex, numSamples);

        if (reverbIsOn()) {
            // Apply master fx chain
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

        outputBuffer.applyGainRamp(startSampleIndex,
                                   numSamples,
                                   lastMasterGain,
                                   *parameters.masterGain);

        lastMasterGain = *parameters.masterGain;
    }
};
