//
// Created by thomas on 11/13/18.
//

#include "Star.h"

Astronomy::Star::Star(std::string name, description_t description) : mName(name), mDescription(description) {
    mSystem = std::make_shared<Astronomy::System>();
}

std::string Astronomy::Star::getName() {
    return mName;
}

float Astronomy::Star::getDiameter() {
    return static_cast<float>(mDescription.diameter);
}

glm::vec2 Astronomy::Star::getCenterDistance() {
    return glm::vec2(mDescription.sunDistance, mDescription.sunDistance);
}

bool Astronomy::Star::hasSystem() {
    return true;
}

std::shared_ptr<Astronomy::System> Astronomy::Star::getSystem() {
    return mSystem;
}

Astronomy::description_t Astronomy::Star::getDescription() {
    return mDescription;
}
