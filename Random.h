//
// Created by Guhan on 4/11/2023.
//

#ifndef PROJECT3_RANDOM_H
#define PROJECT3_RANDOM_H

#endif //PROJECT3_RANDOM_H


#pragma once
#include <random>

class Random
{
public:
    static int Int(int min, int max);
    static float Float(float min, float max);
private:
    static std::mt19937 random;
};