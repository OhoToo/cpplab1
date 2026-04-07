#ifndef LAYER_HPP
#define LAYER_HPP

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

    const Matrix& getW() const { return W; }
    const Matrix& getb() const { return b; }
};

#endif