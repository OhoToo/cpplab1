#ifndef LAYER_HPP
#define LAYER_HPP

#include <fstream>
#include <stdexcept>
#include <string>
#include "matrix.hpp"
#include "activation.hpp"

class Layer {
private:
    Matrix W, b, Z, A, dW, db;
    float (*activation_func)(float);
    float (*activation_deriv)(float);
    int input_size, output_size;

public:
    Layer(int input_size, int output_size,
          float (*act_func)(float),
          float (*act_deriv)(float))
        : input_size(input_size), output_size(output_size),
          activation_func(act_func), activation_deriv(act_deriv),
          W(output_size, input_size), b(output_size, 1)
    {
        W.fill_random(-0.1f, 0.1f);
    }

    Matrix forward(const Matrix& A_prev) {
        Z = W * A_prev;
        Z.addBias(b);
        A = Z.map(activation_func);
        return A;
    }

    Matrix backward(const Matrix& A_prev, const Matrix& dA, float learning_rate) {
        int m = A_prev.getCols();
        Matrix deriv = Z.map(activation_deriv);
        Matrix dZ = dA.elbyel(deriv);
        Matrix A_prev_T = A_prev.get_T();
        dW = (dZ * A_prev_T) * (1.0f / m);
        Matrix db_sum = dZ.sumRows();
        db = db_sum * (1.0f / m);
        Matrix W_T = W.get_T();
        Matrix dA_prev = W_T * dZ;
        W = W - dW * learning_rate;
        b = b - db * learning_rate;
        return dA_prev;
    }

    void save(std::ofstream& out) const {
        int w_lines = W.getLines();
        int w_cols = W.getCols();
        int b_lines = b.getLines();
        int b_cols = b.getCols();
        out.write(reinterpret_cast<const char*>(&w_lines), sizeof(w_lines));
        out.write(reinterpret_cast<const char*>(&w_cols), sizeof(w_cols));
        out.write(reinterpret_cast<const char*>(&b_lines), sizeof(b_lines));
        out.write(reinterpret_cast<const char*>(&b_cols), sizeof(b_cols));
        for (int i = 0; i < w_lines; ++i)
            for (int j = 0; j < w_cols; ++j)
                out.write(reinterpret_cast<const char*>(&W[i][j]), sizeof(float));
        for (int i = 0; i < b_lines; ++i)
            for (int j = 0; j < b_cols; ++j)
                out.write(reinterpret_cast<const char*>(&b[i][j]), sizeof(float));
    }

    void load(std::ifstream& in) {
        int w_lines, w_cols, b_lines, b_cols;
        in.read(reinterpret_cast<char*>(&w_lines), sizeof(w_lines));
        in.read(reinterpret_cast<char*>(&w_cols), sizeof(w_cols));
        in.read(reinterpret_cast<char*>(&b_lines), sizeof(b_lines));
        in.read(reinterpret_cast<char*>(&b_cols), sizeof(b_cols));
        if (w_lines != W.getLines() || w_cols != W.getCols() ||
            b_lines != b.getLines() || b_cols != b.getCols()) {
            throw std::runtime_error("Layer size mismatch when loading");
        }
        for (int i = 0; i < w_lines; ++i)
            for (int j = 0; j < w_cols; ++j)
                in.read(reinterpret_cast<char*>(&W[i][j]), sizeof(float));
        for (int i = 0; i < b_lines; ++i)
            for (int j = 0; j < b_cols; ++j)
                in.read(reinterpret_cast<char*>(&b[i][j]), sizeof(float));
    }

    const Matrix& getW() const { return W; }
    const Matrix& getb() const { return b; }
};

#endif