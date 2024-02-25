/*
  ==============================================================================

    MultibandLookupTable.h
    Created: 7 Sep 2020 2:08:06pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "Utility.h"
#include <array>
#include <cassert>
#include <cmath>
#include <functional>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

using namespace juce;

/**
 * This class combines lookup tables for several frequency bands into one
 * callable entity.
 */
template <typename FloatType> class MultibandLookupTable {
public:
  using TableGenerator =
      std::function<FloatType(FloatType maxFrequency, FloatType value)>;

  static constexpr auto pi = MathConstants<FloatType>::pi;

#pragma mark - Initialization

  MultibandLookupTable() = default;

  void setTable(const TableGenerator &tableGenerator, int tableSize) {
    forEachBand([=](int bandIndex) {
      auto bandTableGenerator = [=](FloatType phase) {
        auto maxFrequency = bandMaxFrequencies[bandIndex];
        return tableGenerator(maxFrequency, phase);
      };

      tables[bandIndex] =
          std::make_unique<dsp::LookupTableTransform<FloatType>>(
              bandTableGenerator, -pi, pi, tableSize);
    });
  }

#pragma mark - Call Operator

  FloatType operator()(FloatType phase, FloatType frequency) const {
    assert(tables[0] != nullptr &&
           "setTable() must be called before operator()");

    auto bandIndex = bandForFrequency(frequency);

    return (*tables[bandIndex])(phase);
  }

private:
  using SingleBandLookupTable = dsp::LookupTableTransform<FloatType>;

#pragma mark - Bands Frequencies

  static constexpr std::array bandMaxFrequencies = std::to_array<FloatType>({
      60,   // Sub
      250,  // Bass
      500,  // LowMid
      1000, //
      2000, // Mid
      3000, //
      4000, // UpperMid
      5000, //
      6000, // Presence
      20000 // Brilliance
  });

  static constexpr auto numberOfBands = bandMaxFrequencies.size();

#pragma mark - Private Members

  std::array<std::unique_ptr<SingleBandLookupTable>, numberOfBands> tables;

#pragma mark - Resolving Frequency Band

  static constexpr int bandForFrequency(FloatType frequency) {
    for (auto &maxFrequency : bandMaxFrequencies) {
      auto bandIndex = &maxFrequency - &bandMaxFrequencies[0];
      auto band = static_cast<int>(bandIndex);

      if (frequency < maxFrequency) {
        return band;
      }
    }

    return numberOfBands - 1;
  }

#pragma mark - Iterating Frequency Bands

  void forEachBand(const std::function<void(int)> &fn) {
    for (int band = 0; band < numberOfBands; band++) {
      fn(band);
    }
  }
};
