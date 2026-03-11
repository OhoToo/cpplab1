#pragma once

#include<cmath>

//!  Сигмоид - превращает любое число в число от 0 до 1

//!-----------------------------------Просто полезные функции-----------------------------------

inline float sigmoid(float num) {
    return 1.0f / (1.0f + std::exp(-num));
}

inline float step(float num) {

    if(num > 0) {
        return 1.0f;
    }
    else {
        return 0.0f;
    }
}

//!-------------------------------------Необходимые функции---------------------------------------

/**
 * @brief Сама функция обучения по условию
 * @param x - х
 * @param y - y
 * @param w1 - веса для x
 * @param w2 - веса для y
 * @param b - b. Вы что, не знаете что такое лин. функция?)
 */
inline float linear(float x, float y, float w1, float w2, float b) {
    return w1 * x + w2 * y + b;
}

/**
 * @brief Функция предсказания
 * @param x - х
 * @param y - y
 * @param w1 - веса для x
 * @param w2 - веса для y
 * @param b - b.
 */
inline float predict(float x, float y, float w1, float w2, float b) {
    return step(linear(x, y, w1, w2, b));
}