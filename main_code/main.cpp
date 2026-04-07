#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>

#include "matrix.hpp"
#include "activation.hpp"
#include "layer.hpp"
#include "neural_network.hpp"
#include "data_create.cpp"


// Подключаем генератор данных из первой лабораторной
// Предполагается, что там определены struct Point и функция create_data
// Если нет – реализуем простую версию прямо здесь (см. комментарий в конце)

// ---------------------------------------------------------------------
// Вспомогательные функции для работы с данными
// ---------------------------------------------------------------------

// Преобразование вектора точек в матрицы X (2 x N) и Y (1 x N)
void pointsToMatrices(const std::vector<Point>& points, Matrix& X, Matrix& Y) {
    int N = points.size();
    X = Matrix(2, N);
    Y = Matrix(1, N);
    for (int i = 0; i < N; ++i) {
        X[0][i] = points[i].x;
        X[1][i] = points[i].y;
        Y[0][i] = static_cast<float>(points[i].label);
    }
}

// Разделение на обучающую (train_ratio) и тестовую выборки
void trainTestSplit(const Matrix& X, const Matrix& Y,
                    Matrix& X_train, Matrix& Y_train,
                    Matrix& X_test, Matrix& Y_test,
                    float train_ratio) {
    int N = X.getCols();
    int train_size = static_cast<int>(N * train_ratio);

    // Индексы
    std::vector<int> idx(N);
    for (int i = 0; i < N; ++i) idx[i] = i;
    std::shuffle(idx.begin(), idx.end(), std::mt19937(std::random_device{}()));

    // Обучающая выборка
    X_train = Matrix(2, train_size);
    Y_train = Matrix(1, train_size);
    for (int i = 0; i < train_size; ++i) {
        int j = idx[i];
        X_train[0][i] = X[0][j];
        X_train[1][i] = X[1][j];
        Y_train[0][i] = Y[0][j];
    }

    // Тестовая выборка
    int test_size = N - train_size;
    X_test = Matrix(2, test_size);
    Y_test = Matrix(1, test_size);
    for (int i = 0; i < test_size; ++i) {
        int j = idx[train_size + i];
        X_test[0][i] = X[0][j];
        X_test[1][i] = X[1][j];
        Y_test[0][i] = Y[0][j];
    }
}

// Вычисление среднего и стандартного отклонения для каждой строки (признака)
void computeNormParams(const Matrix& X, float& mean0, float& std0, float& mean1, float& std1) {
    int rows = X.getLines();
    int cols = X.getCols();
    if (rows != 2) throw std::runtime_error("Expected 2 rows for X");

    mean0 = 0; mean1 = 0;
    for (int j = 0; j < cols; ++j) {
        mean0 += X[0][j];
        mean1 += X[1][j];
    }
    mean0 /= cols;
    mean1 /= cols;

    std0 = 0; std1 = 0;
    for (int j = 0; j < cols; ++j) {
        std0 += (X[0][j] - mean0) * (X[0][j] - mean0);
        std1 += (X[1][j] - mean1) * (X[1][j] - mean1);
    }
    std0 = std::sqrt(std0 / cols);
    std1 = std::sqrt(std1 / cols);

    // Защита от нулевого стандартного отклонения
    if (std0 < 1e-6) std0 = 1.0f;
    if (std1 < 1e-6) std1 = 1.0f;
}

// Нормализация матрицы X (2 строки) с заданными параметрами
void normalize(Matrix& X, float mean0, float std0, float mean1, float std1) {
    int cols = X.getCols();
    for (int j = 0; j < cols; ++j) {
        X[0][j] = (X[0][j] - mean0) / std0;
        X[1][j] = (X[1][j] - mean1) / std1;
    }
}

// ---------------------------------------------------------------------
// Метрики качества
// ---------------------------------------------------------------------
void printMetrics(const Matrix& Y_pred, const Matrix& Y_true) {
    int m = Y_true.getCols();
    int TP = 0, FP = 0, FN = 0, TN = 0;
    for (int j = 0; j < m; ++j) {
        int pred = static_cast<int>(Y_pred[0][j] + 0.5f); // округление 0/1
        int trueVal = static_cast<int>(Y_true[0][j]);
        if (pred == 1 && trueVal == 1) TP++;
        else if (pred == 1 && trueVal == 0) FP++;
        else if (pred == 0 && trueVal == 1) FN++;
        else if (pred == 0 && trueVal == 0) TN++;
    }
    float accuracy = (TP + TN) / static_cast<float>(m);
    float precision = (TP + FP) > 0 ? TP / static_cast<float>(TP + FP) : 0.0f;
    float recall = (TP + FN) > 0 ? TP / static_cast<float>(TP + FN) : 0.0f;
    float f1 = (precision + recall) > 0 ? 2 * precision * recall / (precision + recall) : 0.0f;

    std::cout << "\n===== Test Metrics =====" << std::endl;
    std::cout << "Accuracy:  " << accuracy << std::endl;
    std::cout << "Precision: " << precision << std::endl;
    std::cout << "Recall:    " << recall << std::endl;
    std::cout << "F1-score:  " << f1 << std::endl;
    std::cout << "========================" << std::endl;
}

// ---------------------------------------------------------------------
// Визуализация разделяющей поверхности (текстовая карта)
// ---------------------------------------------------------------------
void visualizeDecisionSurface(NeuralNetwork& nn,
                              float mean0, float std0, float mean1, float std1,
                              float x_min, float x_max, float y_min, float y_max,
                              float step = 0.2f) {
    // Вычисляем количество точек сетки
    int nx = static_cast<int>((x_max - x_min) / step) + 1;
    int ny = static_cast<int>((y_max - y_min) / step) + 1;
    int total = nx * ny;

    // Создаём матрицу X_grid (2 x total)
    Matrix X_grid(2, total);
    int idx = 0;
    for (float y = y_max; y >= y_min - 1e-6; y -= step) { // сверху вниз
        for (float x = x_min; x <= x_max + 1e-6; x += step) {
            X_grid[0][idx] = x;
            X_grid[1][idx] = y;
            ++idx;
        }
    }

    // Нормализуем сетку теми же параметрами, что и обучающие данные
    normalize(X_grid, mean0, std0, mean1, std1);

    // Получаем предсказания (вероятности) для всей сетки
    Matrix probs = nn.forward(X_grid); // (1 x total)

    // Печатаем карту
    std::cout << "\n===== Decision Surface ('.' = class 0, '#' = class 1) =====" << std::endl;
    idx = 0;
    for (int iy = 0; iy < ny; ++iy) {
        for (int ix = 0; ix < nx; ++ix) {
            float p = probs[0][idx++];
            std::cout << (p > 0.5f ? '#' : '.');
        }
        std::cout << std::endl;
    }
    std::cout << "==========================================================" << std::endl;
}

// ---------------------------------------------------------------------
// Интерактивный режим: пользователь вводит координаты, получает класс
// ---------------------------------------------------------------------
void interactiveMode(NeuralNetwork& nn,
                     float mean0, float std0, float mean1, float std1) {
    std::cout << "\n===== Interactive Mode =====" << std::endl;
    std::cout << "Enter x y (or 'q' to quit):" << std::endl;
    float x, y;
    while (true) {
        std::cout << "> ";
        std::string input;
        std::cin >> input;
        if (input == "q" || input == "Q") break;
        try {
            x = std::stof(input);
            std::cin >> y;
        } catch (...) {
            std::cout << "Invalid input. Try again." << std::endl;
            continue;
        }
        // Создаём матрицу 2x1
        Matrix X_new(2, 1);
        X_new[0][0] = x;
        X_new[1][0] = y;
        // Нормализуем
        normalize(X_new, mean0, std0, mean1, std1);
        // Предсказание
        Matrix prob = nn.forward(X_new);
        int cls = (prob[0][0] > 0.5f) ? 1 : 0;
        std::cout << "Predicted class: " << cls << " (probability = " << prob[0][0] << ")" << std::endl;
    }
}

// ---------------------------------------------------------------------
// Главная функция
// ---------------------------------------------------------------------
int main() {
    // Инициализация генератора случайных чисел (для create_data)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 1. Генерация данных (совместимо с C++11)
    std::cout << "Generating data..." << std::endl;
    std::tuple<std::vector<Point>, Line> data = create_data(400, 12.0f);
    std::vector<Point> points = std::get<0>(data);
    Line line = std::get<1>(data);

    // 2. Преобразование в матрицы (создаём матрицы нужного размера)
    Matrix X(2, points.size());
    Matrix Y(1, points.size());
    pointsToMatrices(points, X, Y);
    std::cout << "Total points: " << X.getCols() << std::endl;

    // 3. Разделение на train/test (80/20)
    Matrix X_train, Y_train, X_test, Y_test;
    trainTestSplit(X, Y, X_train, Y_train, X_test, Y_test, 0.8f);
    std::cout << "Train size: " << X_train.getCols() << ", Test size: " << X_test.getCols() << std::endl;

    // 4. Нормализация (вычисляем параметры по обучающей выборке)
    float mean_x, std_x, mean_y, std_y;
    computeNormParams(X_train, mean_x, std_x, mean_y, std_y);
    std::cout << "Normalization params: mean_x=" << mean_x << " std_x=" << std_x
              << " mean_y=" << mean_y << " std_y=" << std_y << std::endl;
    normalize(X_train, mean_x, std_x, mean_y, std_y);
    normalize(X_test,  mean_x, std_x, mean_y, std_y);

    // 5. Создание нейросети
    // Архитектура: 2 входа -> 5 нейронов (ReLU) -> 1 выход (сигмоида)
    std::vector<int> sizes = {2, 5, 1};
    std::vector<float(*)(float)> activations = {relu, sigmoid};
    std::vector<float(*)(float)> derivs = {relu_deriv, sigmoid_deriv_from_z};
    NeuralNetwork nn(sizes, activations, derivs);
    std::cout << "Neural network created." << std::endl;

    // 6. Обучение
    int epochs = 150;
    float learning_rate = 0.05f;
    int batch_size = 16;   // можно 1 для SGD, но 16 ускоряет
    std::cout << "\nStarting training for " << epochs << " epochs, lr=" << learning_rate
              << ", batch_size=" << batch_size << std::endl;
    nn.train(X_train, Y_train, epochs, learning_rate, batch_size, X_test, Y_test);

    // 7. Оценка на тестовой выборке
    Matrix Y_pred = nn.predict(X_test);
    printMetrics(Y_pred, Y_test);

    // 8. Визуализация разделяющей поверхности
    // Определяем границы области (с запасом) в исходных (ненормализованных) координатах
    float x_min = -13.0f, x_max = 13.0f;
    float y_min = -13.0f, y_max = 13.0f;
    visualizeDecisionSurface(nn, mean_x, std_x, mean_y, std_y, x_min, x_max, y_min, y_max, 0.3f);

    // 9. Интерактивный режим
    interactiveMode(nn, mean_x, std_x, mean_y, std_y);

    nn.saveModel("my_model.bin", mean_x, std_x, mean_y, std_y);
    std::cout << "Model saved to my_model.bin" << std::endl;
    return 0;
}