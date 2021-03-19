//
// Created by tly on 15.03.2021.
//

#include "VariantUtils.h"

VariantNumericType variant_from_numeric_type(const NumericType &type) {
    if (type.getString() == "float") return float();
    if (type.getString() == "double") return double();
    if (type.getString() == "qf128") return qf128();
    if (type.getString() == "Fixed4") return Fixed4();
    if (type.getString() == "Fixed8") return Fixed8();
    if (type.getString() == "Fixed16") return Fixed16();
    else
        throw std::runtime_error("unhandled numeric type in variant_from_numeric_type: " + type.getString());
}

VariantComplexImage make_variant_complex_image(Factory &factory, const NumericType &type, const Range &imageSize) {
    return std::visit(
        [&]<typename T>(T type) -> VariantComplexImage {
            return Image<Vec<2, T>>(factory.createImage(imageSize.x, imageSize.y, sizeof(Vec<2, T>)));
        },
        variant_from_numeric_type(type));
}

VariantNumericType make_variant_number(const NumericType &type, const HighPrecisionType &highPrecisionNumber) {
    VariantNumericType number = variant_from_numeric_type(type);
    std::visit([&]<typename T>(T &number) { number = fromString<T>(toString<HighPrecisionType>(highPrecisionNumber)); },
               number);
    return number;
}
