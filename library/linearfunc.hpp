    #pragma once
    #include<cmath>

    float sigmoid(float num) {
        return 1.0f / (1.0f + std::exp(-num));
    }

    float step(float num) {

        if(num > 0) {
            return 1.0f;
        }
        else {
            return 0.0f;
        }
    }

    float linear(float x, float y, float w1, float w2, float b) {
        return w1 * x + w2 * y + b;
    }

    float predict(float x, float y, float w1, float w2, float b) {
        return step(linear(x, y, w1, w2, b));
    }