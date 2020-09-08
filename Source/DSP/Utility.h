/*
  ==============================================================================

    Utility.h
    Created: 8 Sep 2020 8:55:07am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include <array>

namespace std {

    namespace detail {

        template<class T, std::size_t N, std::size_t... I>
        constexpr std::array<std::remove_cv_t<T>, N>
        to_array_impl(T (&&a)[N], std::index_sequence<I...>) {
            return {{std::move(a[I])...}};
        }

    }

    /** C++20 std::to_array() implementation */
    template<class T, std::size_t N>
    constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N]) {
        return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
    }

}