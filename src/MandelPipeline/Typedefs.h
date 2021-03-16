//
// Created by tly on 14.03.2021.
//

#pragma once

#include "../Platform/Factory.h"
#include "../Type/Vec.h"
#include "../Type/TypeHelper.h"
#include <variant>

using UInt2Buffer = Buffer<Vec<2,uint32_t>>;
using UIntBuffer = Buffer<uint32_t>;
using UCharBuffer = Buffer<uint8_t>;
using Float3Buffer = Buffer<Vec<3,float>>;
using FloatImage = Image<float>;
using Float3Image = Image<Vec<3,float>>;
using Float4Image = Image<Vec<4,float>>;
using RGBAImage = Image<uint32_t>;

using HighPrecisionType = Fixed16;
using VariantNumericType = std::variant<float, double, qf128, Fixed4, Fixed8, Fixed16>;

namespace detail {
    template<typename> struct VariantTypes;
    template<typename... Types> struct VariantTypes<std::variant<Types...>>{
        using ComplexImage = std::variant<Image<Vec<2, Types>>...>;
        using Vec2 = std::variant<Vec<2, Types>...>;
    };
}

using VariantComplexImage = typename detail::VariantTypes<VariantNumericType>::ComplexImage;
using VariantVec2 = typename detail::VariantTypes<VariantNumericType>::Vec2;
