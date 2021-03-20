//
// Created by tly on 20.03.2021.
//
#include "Factory.h"
#include "CL/CLFactory.h"
#include "CPU/CPUFactory.h"

const std::vector<std::shared_ptr<Factory>> &Factory::getAllFactories() {
    static auto factories = [] {
        std::vector<std::shared_ptr<Factory>> factories;
        for (uint32_t i = 0; i < CLFactory::getNumberOfDevices(); i++) {
            factories.push_back(std::make_shared<CLFactory>(i));
        }

        factories.push_back(std::make_shared<CPUFactory>());
        return factories;
    }();
    return factories;
}
