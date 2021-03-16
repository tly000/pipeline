//
// Created by tly on 15.03.2021.
//

#pragma once
#include "../Pipeline/StaticPipelineAction.h"
#include "Enums.h"
#include "Typedefs.h"

VariantNumericType variant_from_numeric_type(const NumericType &type);
VariantComplexImage make_variant_complex_image(Factory &factory, const NumericType &type, const Range &imageSize);
VariantNumericType make_variant_number(const NumericType &type, const HighPrecisionType &highPrecisionNumber);

template<typename... Inputs>
struct NumberToVariantConversionAction
    : StaticPipelineAction<Input(KV("numeric type", NumericType), Inputs...),
    Output(std::conditional_t<std::is_same_v<Val<Inputs>, HighPrecisionType>, KeyValuePair<Key<Inputs>, VariantNumericType>, Inputs>...)> {
protected:
    void execute() {
        auto callImpl = [&]<size_t... I>(std::index_sequence<I...>) {
            variadicForEach(executeImpl(this->template getInput<I + 1>(), this->template getOutput<I>()));
        };
        callImpl(std::index_sequence_for<Inputs...>());
    }

    void executeImpl(const StaticInput<HighPrecisionType> &input, StaticOutput<VariantNumericType> &output) {
        output.setValue(make_variant_number(this->getInput(_C("numeric type")).getValue(), input.getValue()));
    }

    template<typename T>
    void executeImpl(const StaticInput<T> &input, StaticOutput<T> &output) {
        output.setValue(input.getValue());
    }
};

template<typename... Inputs, typename... Outputs>
struct NumberToVariantConversionAction<StaticPipelineAction<Input(Inputs...), Output(Outputs...)>>
    : NumberToVariantConversionAction<Outputs...> {};
