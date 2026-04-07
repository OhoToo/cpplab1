#include <iostream>
#include "matrix.hpp"
#include "activation.hpp"
#include "layer.hpp"
#include "neural_network.hpp"
#include <fstream>

// Скопируй сюда функцию normalize (из main.cpp) – она понадобится для интерактивного режима
void normalize(Matrix& X, float mean0, float std0, float mean1, float std1) {
    int cols = X.getCols();
    for (int j = 0; j < cols; ++j) {
        X[0][j] = (X[0][j] - mean0) / std0;
        X[1][j] = (X[1][j] - mean1) / std1;
    }
}

// Скопируй сюда функцию interactiveMode (из main.cpp) – можешь сразу целиком
void interactiveMode(NeuralNetwork& nn,
                     float mean0, float std0, float mean1, float std1) {
    // ... (весь код из твоего main.cpp, без изменений)
}

int main() {
    // Создаём сеть с ТОЙ ЖЕ архитектурой (2 входа, 5 скрытых, 1 выход)
    std::vector<int> sizes = {2, 5, 1};
    std::vector<float(*)(float)> activations = {relu, sigmoid};
    std::vector<float(*)(float)> derivs = {relu_deriv, sigmoid_deriv_from_z};
    NeuralNetwork nn(sizes, activations, derivs);

    // Загружаем модель и параметры нормализации
    float mean_x, std_x, mean_y, std_y;
    nn.loadModel("my_model.bin", mean_x, std_x, mean_y, std_y);
    std::cout << "Model loaded from my_model.bin" << std::endl;

    // Запускаем интерактивный режим (можно также тестировать на любых данных)
    interactiveMode(nn, mean_x, std_x, mean_y, std_y);

    return 0;
}