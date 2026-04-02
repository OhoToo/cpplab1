#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

#include <vector>
#include <cmath>
#include "layer.hpp"
#include "matrix.hpp"

class NeuralNetwork {
private:
    std::vector<Layer> layers;   // слои сети
    std::vector<Matrix> layer_inputs;  // входы каждого слоя (A_prev)
    std::vector<Matrix> layer_outputs; // выходы каждого слоя (A)

public:
    // Конструктор: принимает список размеров слоёв и список функций активации
    // Пример: sizes = {2, 4, 1}  (2 входа, 4 скрытых, 1 выход)
    // activations = {relu, sigmoid}, derivs = {relu_deriv, sigmoid_deriv_from_z}
    NeuralNetwork(const std::vector<int>& sizes,
                  const std::vector<float(*)(float)>& activations,
                  const std::vector<float(*)(float)>& derivs);

    // Прямой проход: X - вход (признаки × примеры), возвращает выход сети (1 × примеры)
    Matrix forward(const Matrix& X);

    // Обратный проход: Y - истинные метки (1 × примеры), lr - скорость обучения
    // Вычисляет градиенты и обновляет веса
    void backward(const Matrix& Y, float learning_rate);

    // Обучение на эпохах
    void train(const Matrix& X_train, const Matrix& Y_train,
               int epochs, float learning_rate, int batch_size = 1,
               const Matrix& X_test = Matrix(0,0), const Matrix& Y_test = Matrix(0,0));

    // Предсказание классов (0 или 1) после порога 0.5
    Matrix predict(const Matrix& X);

    // Вычисление бинарной кросс-энтропии
    float binary_cross_entropy(const Matrix& Y_pred, const Matrix& Y_true);
};

#endif


NeuralNetwork::NeuralNetwork(const std::vector<int>& sizes,
                             const std::vector<float(*)(float)>& activations,
                             const std::vector<float(*)(float)>& derivs) {
    if (sizes.size() < 2) {
        throw std::invalid_argument("At least two layers (input and output) required");
    }
    if (activations.size() != sizes.size() - 1 || derivs.size() != sizes.size() - 1) {
        throw std::invalid_argument("Number of activations must equal number of layers - 1");
    }

    for (size_t i = 0; i < sizes.size() - 1; ++i) {
        layers.emplace_back(sizes[i], sizes[i+1], activations[i], derivs[i]);
    }
}


Matrix NeuralNetwork::forward(const Matrix& X) {
    layer_inputs.clear();
    layer_outputs.clear();
    Matrix A = X;
    for (auto& layer : layers) {
        layer_inputs.push_back(A);
        A = layer.forward(A);
        layer_outputs.push_back(A);
    }
    return A;
}

void NeuralNetwork::backward(const Matrix& Y, float learning_rate) {
    int m = Y.getCols();
    // Начинаем с последнего слоя
    Matrix dA = layer_outputs.back() - Y;  // dA для последнего слоя
    for (int i = layers.size() - 1; i >= 0; --i) {
        // Вход для этого слоя — layer_inputs[i]
        dA = layers[i].backward(layer_inputs[i], dA, learning_rate);
    }
}


Matrix NeuralNetwork::predict(const Matrix& X) {
    Matrix probs = forward(X);
    // Применяем порог 0.5: если элемент > 0.5, то 1, иначе 0
    for (int i = 0; i < probs.getLines(); ++i) {
        for (int j = 0; j < probs.getCols(); ++j) {
            probs[i][j] = (probs[i][j] > 0.5f) ? 1.0f : 0.0f;
        }
    }
    return probs;
}


float NeuralNetwork::binary_cross_entropy(const Matrix& Y_pred, const Matrix& Y_true) {
    int m = Y_true.getCols();
    float loss = 0.0f;
    for (int j = 0; j < m; ++j) {
        float y = Y_true[0][j];
        float y_hat = Y_pred[0][j];
        // Добавляем маленькое число (1e-8) чтобы избежать log(0)
        loss -= y * std::log(y_hat + 1e-8f) + (1 - y) * std::log(1 - y_hat + 1e-8f);
    }
    return loss / m;
}


void NeuralNetwork::train(const Matrix& X_train, const Matrix& Y_train,
                          int epochs, float learning_rate, int batch_size,
                          const Matrix& X_test, const Matrix& Y_test) {
    int m = X_train.getCols();
    if (batch_size <= 0) batch_size = m;

    for (int epoch = 0; epoch < epochs; ++epoch) {
        // Перемешивание: создаём индексы и перемешиваем
        std::vector<int> indices(m);
        for (int i = 0; i < m; ++i) indices[i] = i;
        std::random_shuffle(indices.begin(), indices.end());

        float total_loss = 0.0f;
        int num_batches = 0;

        for (int start = 0; start < m; start += batch_size) {
            int end = std::min(start + batch_size, m);
            int batch_m = end - start;

            // Формируем батч X_batch и Y_batch
            Matrix X_batch(X_train.getLines(), batch_m);
            Matrix Y_batch(1, batch_m);
            for (int j = 0; j < batch_m; ++j) {
                int idx = indices[start + j];
                for (int i = 0; i < X_train.getLines(); ++i) {
                    X_batch[i][j] = X_train[i][idx];
                }
                Y_batch[0][j] = Y_train[0][idx];
            }

            // Прямой проход
            Matrix Y_pred = forward(X_batch);
            // Вычисляем loss для батча (для информации)
            float batch_loss = binary_cross_entropy(Y_pred, Y_batch);
            total_loss += batch_loss * batch_m;
            num_batches++;

            // Обратный проход
            backward(Y_batch, learning_rate);
        }

        float avg_loss = total_loss / m;
        std::cout << "Epoch " << epoch+1 << "/" << epochs << " - loss: " << avg_loss;

        // Если переданы тестовые данные, оцениваем точность
        if (X_test.getLines() > 0 && Y_test.getLines() > 0) {
            Matrix Y_pred_test = forward(X_test);
            float test_loss = binary_cross_entropy(Y_pred_test, Y_test);
            Matrix Y_test_classes = predict(X_test);
            // Вычисляем accuracy
            int correct = 0;
            for (int j = 0; j < Y_test.getCols(); ++j) {
                if (Y_test_classes[0][j] == Y_test[0][j]) correct++;
            }
            float acc = (float)correct / Y_test.getCols();
            std::cout << " - test loss: " << test_loss << " - test acc: " << acc;
        }
        std::cout << std::endl;
    }
}

