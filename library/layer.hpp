#ifndef LAYER_HPP
#define LAYER_HPP

#include "matrix.hpp"
#include "activation.hpp"

class Layer {
private:
    // Параметры слоя
    Matrix W;        // веса
    Matrix b;        // смещения (столбец)

    // Промежуточные значения (сохраняются после forward)
    Matrix Z;        // линейная часть (до активации)
    Matrix A;        // выход после активации

    // Градиенты (вычисляются в backward)
    Matrix dW;       // градиент по весам
    Matrix db;       // градиент по смещениям

    // Функции активации (указатели)
    float (*activation_func)(float);      // сама функция
    float (*activation_deriv)(float);     // её производная

    // Размерности
    int input_size;   // количество входов
    int output_size;  // количество нейронов

public:
    // Конструктор
    Layer(int input_size, int output_size,
          float (*act_func)(float),
          float (*act_deriv)(float));

    // Прямой проход
    Matrix forward(const Matrix& A_prev);

    // Обратный проход
    Matrix backward(const Matrix& A_prev, const Matrix& dA, float learning_rate);

    // Геттеры для доступа к параметрам (если понадобятся)
    const Matrix& getW() const { return W; }
    const Matrix& getb() const { return b; }


    Layer(int input_size, int output_size,
      float (*act_func)(float),
      float (*act_deriv)(float))
    : input_size(input_size), output_size(output_size),
      activation_func(act_func), activation_deriv(act_deriv),
      W(output_size, input_size), b(output_size, 1) {

        // Инициализация весов маленькими случайными числами в диапазоне [-0.1, 0.1]
        W.fill_random(-0.1f, 0.1f);

        // Инициализация смещений нулями (можно тоже небольшими случайными, но обычно нули)
        // Матрица b уже заполнена нулями при создании, так что дополнительно ничего не делаем
    }


    Matrix forward(const Matrix& A_prev) {
        // 1. Вычисляем Z = W * A_prev
        Z = W * A_prev;   // размер (output_size, количество_примеров)

        // 2. Прибавляем смещения к каждому столбцу
        // Используем метод addBias, который мы добавили в Matrix
        Z.addBias(b);

        // 3. Применяем функцию активации поэлементно
        A = Z.map(activation_func);

        // 4. Возвращаем выход слоя
        return A;
    }


    Matrix backward(const Matrix& A_prev, const Matrix& dA, float learning_rate) {
        int m = A_prev.getCols();  // количество примеров в пакете

        // 1. Вычисляем производную активации в точках Z
        // Используем map с функцией производной
        Matrix deriv = Z.map(activation_deriv);  // размер (output_size, m)

        // 2. dZ = dA * deriv (поэлементно)
        Matrix dZ = dA.elbyel(deriv);  // размер (output_size, m)

        // 3. Градиент по весам: dW = (dZ * A_prev^T) / m
        Matrix A_prev_T = A_prev.get_T();  // транспонируем вход (m, input_size)
        dW = (dZ * A_prev_T) * (1.0f / m);  // размер (output_size, input_size)

        // 4. Градиент по смещениям: db = сумма по столбцам dZ, делённая на m
        Matrix db_sum = dZ.sumRows();  // результат (output_size, 1)
        db = db_sum * (1.0f / m);      // делим каждый элемент на m

        // 5. Градиент для предыдущего слоя: dA_prev = W^T * dZ
        Matrix W_T = W.get_T();  // транспонируем веса (input_size, output_size)
        Matrix dA_prev = W_T * dZ;  // размер (input_size, m)

        // 6. Обновляем веса и смещения
        W = W - dW * learning_rate;
        b = b - db * learning_rate;

        // 7. Возвращаем dA_prev
        return dA_prev;
    }


};

#endif