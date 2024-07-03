#pragma once

#include <juce_dsp/juce_dsp.h>

namespace blackbird::dsp {

using namespace juce;
using namespace juce::dsp;

/**
    Multi-mode filter based on the Moog ladder filter.

    @tags{DSP}
*/
template <typename SampleType> class LadderFilter {
public:
  //==============================================================================
  using Mode = LadderFilterMode;

  //==============================================================================
  /** Creates an uninitialised filter. Call prepare() before first use. */
  LadderFilter();

  /** Enables or disables the filter. If disabled it will simply pass through
   * the input signal. */
  void setEnabled(bool isEnabled) noexcept { enabled = isEnabled; }

  /** Sets filter mode. */
  void setMode(Mode newMode) noexcept;

  /** Initialises the filter. */
  void prepare(const ProcessSpec &spec);

  /** Returns the current number of channels. */
  size_t getNumChannels() const noexcept { return state.size(); }

  /** Resets the internal state variables of the filter. */
  void reset() noexcept;

  /** Sets the cutoff frequency of the filter.

      @param newCutoff cutoff frequency in Hz
  */
  void setCutoffFrequencyHz(SampleType newCutoff) noexcept;

  /** Sets the resonance of the filter.

      @param newResonance a value between 0 and 1; higher values increase the
     resonance and can result in self oscillation!
  */
  void setResonance(SampleType newResonance) noexcept;

  /** Sets the amount of saturation in the filter.

      @param newDrive saturation amount; it can be any number greater than or
     equal to one. Higher values result in more distortion.
  */
  void setDrive(SampleType newDrive) noexcept;

  //==============================================================================
  template <typename ProcessContext>
  void process(const ProcessContext &context) noexcept {
    const auto &inputBlock = context.getInputBlock();
    auto &outputBlock = context.getOutputBlock();
    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    jassert(inputBlock.getNumChannels() <= getNumChannels());
    jassert(inputBlock.getNumChannels() == numChannels);
    jassert(inputBlock.getNumSamples() == numSamples);

    if (!enabled || context.isBypassed) {
      outputBlock.copyFrom(inputBlock);
      return;
    }

    for (size_t n = 0; n < numSamples; ++n) {
      updateSmoothers();

      for (size_t ch = 0; ch < numChannels; ++ch)
        outputBlock.getChannelPointer(ch)[n] =
            processSample(inputBlock.getChannelPointer(ch)[n], ch);
    }
  }

protected:
  //==============================================================================
  SampleType processSample(SampleType inputValue, size_t channelToUse) noexcept;
  void updateSmoothers() noexcept;

private:
  //==============================================================================
  void setSampleRate(SampleType newValue) noexcept;
  void setNumChannels(size_t newValue) { state.resize(newValue); }
  void updateCutoffFreq() noexcept {
    cutoffTransformSmoother.setTargetValue(
        std::exp(cutoffFreqHz * cutoffFreqScaler));
  }
  void updateResonance() noexcept {
    scaledResonanceSmoother.setTargetValue(
        jmap(resonance, SampleType(0.1), SampleType(1.0)));
  }

  //==============================================================================
  SampleType drive, drive2, gain, gain2, comp;

  static constexpr size_t numStates = 5;
  std::vector<std::array<SampleType, numStates>> state;
  std::array<SampleType, numStates> A;

  SmoothedValue<SampleType> cutoffTransformSmoother, scaledResonanceSmoother;
  SampleType cutoffTransformValue, scaledResonanceValue;

  LookupTableTransform<SampleType> saturationLUT{
      [](SampleType x) { return std::tanh(x); }, SampleType(-5), SampleType(5),
      128};

  SampleType cutoffFreqHz{SampleType(200)};
  SampleType resonance;

  SampleType cutoffFreqScaler;

  Mode mode;
  bool enabled = true;
};

} // namespace blackbird::dsp
