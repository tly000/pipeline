//
// Created by tly on 14.03.2021.
//

#include "GridbasedCalculationAction.h"


bool GridbasedCalculationAction::step() {
    Range imageRange = this->getInput(_C("imageRange")).getValue();
    if(currentPosY < imageRange.y){
        uint32_t nextPixelsX = std::min(gridSize,imageRange.x - currentPosX);
        uint32_t nextPixelsY = std::min(gridSize,imageRange.y - currentPosY);

        this->kernelAction.getInput(_C("globalOffset")).setDefaultValue(Range{currentPosX,currentPosY,0});
        this->kernelAction.getInput(_C("globalSize")).setDefaultValue(Range{nextPixelsX,nextPixelsY,1});
        this->kernelAction.run();

        currentPosX += gridSize;
        if(currentPosX >= imageRange.x){
            currentPosX = 0;
            currentPosY += gridSize;
        }
        return false;
    }else{
        return true;
    }
}
void GridbasedCalculationAction::reset() {
    currentPosX = 0;
    currentPosY = 0;
}
