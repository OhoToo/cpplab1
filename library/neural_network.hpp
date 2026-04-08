#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <cmath>
#include "layer.hpp"
#include "matrix.hpp"
#include <fstream>
#include <stdexcept>
#include <string>
#include <random>


class NeuralNetwork {
private:
    std::vector<Layer> layers;
    std::vector<Matrix> layer_inputs;
    std::vector<Matrix> layer_outputs;

public:
    NeuralNetwork(const std::vector<int>& sizes,
                  const std::vector<float(*)(float)>& activations,
                  const std::vector<float(*)(float)>& derivs);


    Matrix forward(const Matrix& X);


    void backward(const Matrix& Y, float learning_rate);


    void train(const Matrix& X_train, const Matrix& Y_train,
               int epochs, float learning_rate, int batch_size = 1,
               const Matrix& X_test = Matrix(0,0), const Matrix& Y_test = Matrix(0,0));


    Matrix predict(const Matrix& X);


    float binary_cross_entropy(const Matrix& Y_pred, const Matrix& Y_true);

    void saveModel(const std::string& filename,
               float mean_x, float std_x,
               float mean_y, float std_y) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) throw std::runtime_error("Cannot open file for writing: " + filename);


        out.write(reinterpret_cast<const char*>(&mean_x), sizeof(mean_x));
        out.write(reinterpret_cast<const char*>(&std_x), sizeof(std_x));
        out.write(reinterpret_cast<const char*>(&mean_y), sizeof(mean_y));
        out.write(reinterpret_cast<const char*>(&std_y), sizeof(std_y));


        size_t num_layers = layers.size();
        out.write(reinterpret_cast<const char*>(&num_layers), sizeof(num_layers));


        for (const auto& layer : layers) {
            layer.save(out);
        }
        out.close();
    }


    void loadModel(const std::string& filename,
               float& mean_x, float& std_x,
               float& mean_y, float& std_y) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) throw std::runtime_error("Cannot open file for reading: " + filename);


        in.read(reinterpret_cast<char*>(&mean_x), sizeof(mean_x));
        in.read(reinterpret_cast<char*>(&std_x), sizeof(std_x));
        in.read(reinterpret_cast<char*>(&mean_y), sizeof(mean_y));
        in.read(reinterpret_cast<char*>(&std_y), sizeof(std_y));

        size_t num_layers;
        in.read(reinterpret_cast<char*>(&num_layers), sizeof(num_layers));
        if (num_layers != layers.size()) {
            throw std::runtime_error("Layer count mismatch: saved " +
                                     std::to_string(num_layers) +
                                     " but current has " +
                                     std::to_string(layers.size()));
        }

        for (auto& layer : layers) {
            layer.load(in);
        }
        in.close();
    }
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
    Matrix dA = layer_outputs.back() - Y;
    for (int i = layers.size() - 1; i >= 0; --i) {
        // Вход для этого слоя — layer_inputs[i]
        dA = layers[i].backward(layer_inputs[i], dA, learning_rate);
    }
}


Matrix NeuralNetwork::predict(const Matrix& X) {
    Matrix probs = forward(X);
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
        std::vector<int> indices(m);
        for (int i = 0; i < m; ++i) indices[i] = i;
        std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device{}()));

        float total_loss = 0.0f;
        int num_batches = 0;

        for (int start = 0; start < m; start += batch_size) {
            int end = std::min(start + batch_size, m);
            int batch_m = end - start;

            Matrix X_batch(X_train.getLines(), batch_m);
            Matrix Y_batch(1, batch_m);
            for (int j = 0; j < batch_m; ++j) {
                int idx = indices[start + j];
                for (int i = 0; i < X_train.getLines(); ++i) {
                    X_batch[i][j] = X_train[i][idx];
                }
                Y_batch[0][j] = Y_train[0][idx];
            }


            Matrix Y_pred = forward(X_batch);

            float batch_loss = binary_cross_entropy(Y_pred, Y_batch);
            total_loss += batch_loss * batch_m;
            num_batches++;


            backward(Y_batch, learning_rate);
        }

        float avg_loss = total_loss / m;
        std::cout << "Epoch " << epoch+1 << "/" << epochs << " - loss: " << avg_loss;


        if (X_test.getLines() > 0 && Y_test.getLines() > 0) {
            Matrix Y_pred_test = forward(X_test);
            float test_loss = binary_cross_entropy(Y_pred_test, Y_test);
            Matrix Y_test_classes = predict(X_test);
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

    