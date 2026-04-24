#ifndef CH32TEST_EFFECT_H
#define CH32TEST_EFFECT_H

#include <Arduino.h>
#include "matrix/IS31FL3731.h"
#include "matrix/LedMapper.h"

class Effect {
protected:
    // Protected means ONLY this class and its children can see these variables.
    // We store pointers to your matrix and mapper so every effect can access them
    // without needing global variables!
    IS31FL3731* matrix;
    LedMapper* mapper;

public:
    // Constructor: When an effect is created, we hand it the hardware and the map
    Effect(IS31FL3731* displayMatrix, LedMapper* ledMapper) {
        this->matrix = displayMatrix;
        this->mapper = ledMapper;
    }

    // A virtual destructor is required in C++ when using inheritance.
    // It ensures that if an effect is destroyed, the child class's memory is cleaned up.
    virtual ~Effect() {}

    // ---------------------------------------------------------
    // THE PURE VIRTUAL METHODS
    // The "= 0" tells the C++ compiler: "This function has no code here.
    // Any class that inherits from 'Effect' MUST write their own version of this."
    // ---------------------------------------------------------

    // Called once when the effect is first selected/started
    virtual void setupEffect() = 0;

    // Called repeatedly every frame to draw the animation
    virtual void loopEffect() = 0;
};

#endif // CH32TEST_EFFECT_H