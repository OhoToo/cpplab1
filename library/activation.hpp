#ifndef ACTIVATION_HPP
#define ACTIVATION_HPP

#include <cmath>


inline float sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}


inline float sigmoid_deriv_from_a(float a) {
    return a * (1.0f - a);
}


inline float sigmoid_deriv_from_z(float z) {
    float s = sigmoid(z);
    return s * (1.0f - s);
}


inline float relu(float x) {
    return x > 0 ? x : 0.0f;
}

inline float relu_deriv(float x) {
    return x > 0 ? 1.0f : 0.0f;
}

#endif