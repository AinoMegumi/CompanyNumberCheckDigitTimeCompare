/*=============================================================================
Copyright (C) 2022 yumetodo <yume-wikijp@live.jp>
Distributed under the Boost Software License, Version 1.0.
(See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <smmintrin.h>
#include <chrono>

namespace check_digit_calc {
    namespace {
        static inline const auto zero = _mm_setzero_si128();
        static inline const auto mask_get_value = _mm_set1_epi8(0b0000'1111);
        namespace bits_for_validate {
            static inline const auto mask_extract_significant_bit = _mm_set1_epi8(0b1111'0000);
            static inline const auto valid_pattern_alphabetic = _mm_set1_epi8(0b0011'0000);
            static inline const auto valid_pattern_full_width = _mm_set1_epi8(0b1001'0000);
        }
        static inline const auto mask_for_pack = _mm_set1_epi16(0xFF);
        //16要素詰め込めるところを12要素しか詰め込んでいないので残りの部分は判定からはずすためのmask
        static inline const auto mask_valid_bits = _mm_setr_epi32(-1, -1, -1, 0);

        /**
         * 入力文字列から必要な部分を抜き出して`__m128i`に格納する
         *
         * ０-９の場合、`EF BC 9x`となるので3byte目の`9x`の部分を格納する
         * 0-9の場合、`3x`となるので1byte目の`3x`の部分を格納する
         * @param s 入力文字列
         */
        inline __m128i gather_target_bytes(const std::string& s) noexcept
        {
            if (s.length() > 12 * 3) return zero;
            alignas(16) char ret[16]{};
            std::size_t i, j;
            for (i = 0, j = 0; i < 12 && j < s.length(); ++i, ++j) {
                constexpr std::byte expected[] = { std::byte(0xEF), std::byte(0xBC) };
                const bool b = (j + 2 >= s.length() || std::memcmp(s.data() + j, expected, 2) != 0);
                ret[i] = b ? s[j] : s[j += 2];
            }
            return (j != s.length()) ? zero : _mm_load_si128(reinterpret_cast<__m128i*>(ret));
        }
    /**
     * 各byteの上位bitが0-9もしくは０-９のものであることを検証する
     * @param x 8bit整数が格納された`__m128i`
     */
    inline bool validate_phase1(const __m128i& x) noexcept
    {
        using namespace bits_for_validate;
        // 下位桁(0x0-0xF)はvalidate_phase2で見るので一旦mask
        const auto masked = _mm_and_si128(x, _mm_set1_epi8(0b1111'0000));
        // 0-9が格納されたbyteでは0になる
        const auto distance_alphabetic = _mm_sub_epi8(masked, _mm_set1_epi8(0b0011'0000));
        // ０-９が格納されたbyteでは0になる
        const auto distance_full_width = _mm_sub_epi8(masked, _mm_set1_epi8(0b1001'0000));
        // AND演算すれば0-9もしくは０-９が格納されたbyteでは0になる
        const auto distance = _mm_and_si128(distance_alphabetic, distance_full_width);
        // 12要素分全部0なら正しい
        return _mm_test_all_zeros(distance, _mm_setr_epi32(-1, -1, -1, 0)) == 1;
    }
    /**
     * 各byteで9以下であることを検証する
     * @param x 8bit整数が格納された`__m128i`
     */
    inline bool validate_phase2(const __m128i& x) noexcept
    {
        const __m128i t = _mm_sub_epi8(x, _mm_min_epu8(x, _mm_set1_epi8(9)));
        return _mm_test_all_zeros(t, _mm_setr_epi32(-1, -1, -1, 0)) == 1;
    }
    /**
     * 奇数byte * 2 + 偶数byte
     * @param x input
     */
    inline int accumlate(const __m128i& x) noexcept
    {
        const auto mask_for_pack = _mm_set1_epi16(0xFF);
        // x:    o  e  o  e  | o  e  o  e  | o  e  o  e  | o  e  o  e
        // AND
        // mask: FF 00 FF 00 | FF 00 FF 00 | FF 00 FF 00 | FF 00 FF 00
        // ----> o  00 o  00 | o  00 o  00 | o  00 o  00 | o  00 o  00
        const auto odd = _mm_and_si128(x, mask_for_pack);
        // x:    o  e  o  e  | o  e  o  e  | o  e  o  e  | o  e  o  e
        // RSHIFT
        // ----> e  o  e  o  | e  o  e  o  | e  o  e  o  | e  o  e  00
        // AND
        // mask: FF 00 FF 00 | FF 00 FF 00 | FF 00 FF 00 | FF 00 FF 00
        // ----> e  00 e  00 | e  00 e  00 | e  00 e  00 | e  00 e  00
        const auto even = _mm_and_si128(_mm_srli_si128(x, 1), mask_for_pack);
        // odd * 2 + even
        // sum_pre:  s  00 s  00 | s  00 s  00 | s  00 s  00 | s  00 s  00
        const auto sum_pre = _mm_add_epi8(_mm_add_epi8(odd, odd), even);
        //ref: https://qiita.com/YSRKEN/items/17097b26ddf0754c5d34#%E5%90%88%E8%A8%88%E5%87%A6%E7%90%86%E3%81%A8%E6%A4%9C%E6%9F%BB%E7%94%A8%E6%95%B0%E5%AD%97%E3%81%AE%E8%A8%88%E7%AE%97%E5%87%A6%E7%90%86
        //zeroは当て馬、_mm_sad_epu8はa-bの総和を求めるため0を引く
        const auto sum_tmp = _mm_sad_epu8(_mm_and_si128(sum_pre, _mm_setr_epi32(-1, -1, -1, 0)), _mm_setzero_si128());
        //_mm_sad_epu8が8byteずつ足し算の結果をまとめるのでbyte shiftして足す
        const auto sum = _mm_add_epi16(sum_tmp, _mm_srli_si128(sum_tmp, 8));
        //先頭
        return _mm_cvtsi128_si32(sum);
    }
    }
    inline std::uint64_t gater, ph1,ph2, acc;
    inline int calc(const std::string& s) noexcept
    {
        namespace ch = std::chrono;
        using ch::duration_cast;
        const auto t0 = std::chrono::system_clock::now();
        const auto gathered = gather_target_bytes(s);
        const auto t1= std::chrono::system_clock::now();
        if (!validate_phase1(gathered)) return 0xFF;
        const auto t2 = std::chrono::system_clock::now();
        // 下位桁(0x0-0xF)をとってくる
        const auto value = _mm_and_si128(gathered, mask_get_value);
        if (!validate_phase2(value)) return 0xFE;
        const auto t3 = std::chrono::system_clock::now();
        const auto sum = accumlate(value);
        const auto t4 = std::chrono::system_clock::now();
        gater += duration_cast<ch::nanoseconds>(t1 - t0).count();
        ph1 += duration_cast<ch::nanoseconds>(t2 - t1).count();
        ph2 += duration_cast<ch::nanoseconds>(t3 - t2).count();
        acc += duration_cast<ch::nanoseconds>(t4 - t3).count();
        return 9 - (sum % 9);
    }
};
