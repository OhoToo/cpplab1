#include <iostream>
#include "matrix.hpp"
#include "activation.hpp"
#include "layer.hpp"
#include "neural_network.hpp"
#include <fstream>


void normalize(Matrix& X, float mean0, float std0, float mean1, float std1) {
    int cols = X.getCols();
    for (int j = 0; j < cols; ++j) {
        X[0][j] = (X[0][j] - mean0) / std0;
        X[1][j] = (X[1][j] - mean1) / std1;
    }
}


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
        Matrix X_new(2, 1);
        X_new[0][0] = x;
        X_new[1][0] = y;
        normalize(X_new, mean0, std0, mean1, std1);
        Matrix prob = nn.forward(X_new);
        int cls = (prob[0][0] > 0.5f) ? 1 : 0;
        std::cout << "Predicted class: " << cls << " (probability = " << prob[0][0] << ")" << std::endl;
    }
}

int main(){

    std::vector<int> sizes = {2, 5, 1};
    std::vector<float(*)(float)> activations = {relu, sigmoid};
    std::vector<float(*)(float)> derivs = {relu_deriv, sigmoid_deriv_from_z};
    NeuralNetwork nn(sizes, activations, derivs);


    float mean_x, std_x, mean_y, std_y;
    nn.loadModel("my_model.bin", mean_x, std_x, mean_y, std_y);
    std::cout << "Model loaded from my_model.bin" << std::endl;


    interactiveMode(nn, mean_x, std_x, mean_y, std_y);

    return 0;
}