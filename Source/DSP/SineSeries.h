/*
  ==============================================================================

    SineSeries.h
    Created: 7 Sep 2020 10:27:34am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include <cassert>
#include <cmath>
#include <functional>

template <typename ValueType>
auto sineSeries(int order,
                const std::function<ValueType(ValueType)> &getKFactor) {
  assert(order > 0);

  return [=](ValueType value) {
    ValueType result = 0;

    for (auto k = 1; k <= order; k++) {
      const ValueType kFactor = getKFactor(k);

      result += ValueType(1) / kFactor * std::sin(kFactor * value);
    }

    return result;
  };
}

template <typename ValueType> auto sawSeries(int order) {
  return sineSeries<ValueType>(order, [](auto k) { return k; });
}

template <typename ValueType> auto squareSeries(int order) {
  return sineSeries<ValueType>(order, [](auto k) { return 2 * k - 1; });
}