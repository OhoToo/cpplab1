#include "matrix.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

bool approx_equal(float a, float b, float eps = 1e-5) {
    return std::fabs(a - b) < eps;
}

const float sigmoid_const(float x) {
    return sigmoid(x);
}

void test_creation() {
    std::cout << "Тест создания матрицы...";
    Matrix m(3, 2);
    m.fill({1, 2, 3, 4, 5, 6});
    assert(approx_equal(m[0][0], 1));
    assert(approx_equal(m[0][1], 2));
    assert(approx_equal(m[1][0], 3));
    assert(approx_equal(m[1][1], 4));
    assert(approx_equal(m[2][0], 5));
    assert(approx_equal(m[2][1], 6));
    std::cout << " OK\n";
}

void test_addition() {
    std::cout << "Тест сложения...";
    Matrix A(2, 2);
    A.fill({1, 2, 3, 4});
    Matrix B(2, 2);
    B.fill({5, 6, 7, 8});
    Matrix C = A + B;
    assert(approx_equal(C[0][0], 6));
    assert(approx_equal(C[0][1], 8));
    assert(approx_equal(C[1][0], 10));
    assert(approx_equal(C[1][1], 12));

    try {
        Matrix D(2, 3);
        Matrix E = A + D;
        assert(false && "Исключение не было выброшено");
    } catch (const std::invalid_argument&) {
        // OK, исключение поймано
    }
    std::cout << " OK\n";
}

void test_subtraction() {
    std::cout << "Тест вычитания...";
    Matrix A(2, 2);
    A.fill({5, 6, 7, 8});
    Matrix B(2, 2);
    B.fill({1, 2, 3, 4});
    Matrix C = A - B;
    assert(approx_equal(C[0][0], 4));
    assert(approx_equal(C[0][1], 4));
    assert(approx_equal(C[1][0], 4));
    assert(approx_equal(C[1][1], 4));
    std::cout << " OK\n";
}

void test_multiplication_matrix() {
    std::cout << "Тест умножения матриц...\n";

    Matrix A(2, 2);
    A.fill({1, 2, 3, 4});
    Matrix B(2, 2);
    B.fill({2, 0, 1, 2});
    Matrix C = A * B;
    assert(approx_equal(C[0][0], 4));
    assert(approx_equal(C[0][1], 4));
    assert(approx_equal(C[1][0], 10));
    assert(approx_equal(C[1][1], 8));
    std::cout << "  Квадратные 2x2: OK\n";

    Matrix D(2, 3);
    D.fill({1, 2, 3, 4, 5, 6});
    Matrix E(3, 2);
    E.fill({7, 8, 9, 10, 11, 12});
    Matrix F = D * E;
    assert(approx_equal(F[0][0], 58));
    assert(approx_equal(F[0][1], 64));
    assert(approx_equal(F[1][0], 139));
    assert(approx_equal(F[1][1], 154));
    std::cout << "  Прямоугольные 2x3 * 3x2: OK\n";

    Matrix I(3, 3);
    I.fill({1, 0, 0, 0, 1, 0, 0, 0, 1});
    Matrix M(3, 3);
    M.fill_random();
    Matrix M2 = M * I;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            assert(approx_equal(M[i][j], M2[i][j]));
    std::cout << "  Умножение на единичную: OK\n";

    Matrix Z(3, 3);
    Z.fill({0, 0, 0, 0, 0, 0, 0, 0, 0});
    Matrix M3 = M * Z;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            assert(approx_equal(M3[i][j], 0.0f));
    std::cout << "  Умножение на нулевую: OK\n";

    try {
        Matrix X(2, 3);
        Matrix Y(2, 2);
        Matrix Z2 = X * Y;
        assert(false && "Исключение не выброшено");
    } catch (const std::invalid_argument&) {
        std::cout << "  Несовместимые размеры: OK (исключение)\n";
    }
}

void test_scalar_multiplication() {
    std::cout << "Тест умножения на скаляр...";
    Matrix A(2, 2);
    A.fill({1, 2, 3, 4});
    Matrix B = A * 2.5f;
    assert(approx_equal(B[0][0], 2.5f));
    assert(approx_equal(B[0][1], 5.0f));
    assert(approx_equal(B[1][0], 7.5f));
    assert(approx_equal(B[1][1], 10.0f));
    std::cout << " OK\n";
}

void test_elementwise_multiplication() {
    std::cout << "Тест поэлементного умножения (elbyel)...";
    Matrix A(2, 2);
    A.fill({1, 2, 3, 4});
    Matrix B(2, 2);
    B.fill({2, 3, 4, 5});
    Matrix C = A.elbyel(B);
    assert(approx_equal(C[0][0], 2));
    assert(approx_equal(C[0][1], 6));
    assert(approx_equal(C[1][0], 12));
    assert(approx_equal(C[1][1], 20));
    std::cout << " OK\n";
}

void test_transpose() {
    std::cout << "Тест транспонирования...";
    Matrix A(2, 3);
    A.fill({1, 2, 3, 4, 5, 6});
    Matrix B = A.get_T();
    assert(approx_equal(B[0][0], 1));
    assert(approx_equal(B[0][1], 4));
    assert(approx_equal(B[1][0], 2));
    assert(approx_equal(B[1][1], 5));
    assert(approx_equal(B[2][0], 3));
    assert(approx_equal(B[2][1], 6));

    A.T();
    assert(approx_equal(A[0][0], 1));
    assert(approx_equal(A[0][1], 4));
    assert(approx_equal(A[1][0], 2));
    assert(approx_equal(A[1][1], 5));
    assert(approx_equal(A[2][0], 3));
    assert(approx_equal(A[2][1], 6));
    std::cout << " OK\n";
}

void test_minor() {
    std::cout << "Тест минора...";
    Matrix A(3, 3);
    A.fill({1, 2, 3, 4, 5, 6, 7, 8, 9});
    Matrix M = A.minor(0, 0);
    assert(approx_equal(M[0][0], 5));
    assert(approx_equal(M[0][1], 6));
    assert(approx_equal(M[1][0], 8));
    assert(approx_equal(M[1][1], 9));

    Matrix N = A.minor(1, 2);
    assert(approx_equal(N[0][0], 1));
    assert(approx_equal(N[0][1], 2));
    assert(approx_equal(N[1][0], 7));
    assert(approx_equal(N[1][1], 8));
    std::cout << " OK\n";
}

void test_map() {
    std::cout << "Тест map с сигмоидой...";
    Matrix A(1, 2);
    A.fill({0.0f, 1.0f});

    Matrix B = A.map(sigmoid_const);   // ← исправлено
    assert(approx_equal(B[0][0], 0.5f, 1e-4));
    assert(approx_equal(B[0][1], 1.0f / (1.0f + std::exp(-1.0f)), 1e-4));

    std::cout << " OK\n";
}

void test_fill_random() {
    std::cout << "Тест fill_random (проверка диапазона)...";
    Matrix A(10, 10);
    A.fill_random(2.0f, 5.0f);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            float val = A[i][j];
            assert(val >= 2.0f && val <= 5.0f);
        }
    }
    std::cout << " OK\n";
}

int main() {
    std::cout << "Запуск тестов для matrix.hpp\n";
    test_creation();
    test_addition();
    test_subtraction();
    test_multiplication_matrix();
    test_scalar_multiplication();
    test_elementwise_multiplication();
    test_transpose();
    test_minor();
    test_map();
    test_fill_random();
    std::cout << "Все тесты пройдены успешно!\n";
    return 0;
}