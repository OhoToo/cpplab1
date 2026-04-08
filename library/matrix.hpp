
#pragma once

#include <iostream>
#include <vector>
#include <cmath>


#include <cstdlib>
#include <ctime>



using namespace std;

class Matrix{
private:
    int lines, columns;
    std::vector<std::vector<float>> matrix;

public:
    Matrix(int l, int c) : lines(l), columns(c) {
        matrix = std::vector<std::vector<float>>(lines, std::vector<float>(columns, 0));
    }



    std::vector<float>& operator[](int i) {
        return matrix[i];
    }

    Matrix operator+(const Matrix& other) const {
        if((matrix.size() != other.matrix.size()) or (matrix[0].size() != other.matrix[0].size())) {
            throw invalid_argument("Size");
        }

        Matrix final(matrix.size(), matrix[0].size());
        

        for(int i = 0; i < matrix.size(); i++){
            for(int j = 0; j < matrix[0].size(); j++) {
                final[i][j] = matrix[i][j] + other.matrix[i][j];
            }
        }

        return final;
    }

    Matrix operator-(const Matrix& other) const {
        if((matrix.size() != other.matrix.size()) or (matrix[0].size() != other.matrix[0].size())) {
            throw invalid_argument("Size");
        }

        Matrix final(matrix.size(), matrix[0].size());
        

        for(int i = 0; i < matrix.size(); i++){
            for(int j = 0; j < matrix[0].size(); j++) {
                final[i][j] = matrix[i][j] - other.matrix[i][j];
            }
        }

        return final;
    }


    Matrix operator*(const Matrix& other) const {
        if(columns == other.matrix.size()) {

            Matrix temporary(lines, other.matrix[0].size());

            for(int i = 0; i < lines; i++) {
                for(int j = 0; j < other.matrix[0].size(); j++) {
                    for(int k = 0; k < columns; k++) {
                        temporary[i][j] += matrix[i][k] * other.matrix[k][j];
                    }
                }
            }
            return temporary;

        }
        else {
            throw invalid_argument("Size!");
        }
    }


    Matrix operator*(float num) const {
        
        Matrix temporary(lines, columns);

        for(int i = 0; i < lines; i++) {
            for(int j = 0; j < columns; j++) {
                temporary[i][j] = matrix[i][j]*num;
            }
        }

        return temporary;
    }

    
    Matrix elbyel(const Matrix& other) const {

        if (lines != other.lines || columns != other.columns) {
            throw invalid_argument("Size!");
        }

        Matrix temporary(lines, columns);

        for(int i = 0; i < lines; i++) {
            for(int j = 0; j < columns; j++) {
                temporary[i][j] = matrix[i][j]*other.matrix[i][j];
            }
        }

        return temporary;
    }


    Matrix get_T() const{
        Matrix trans(columns, lines);
        for(int i = 0; i < lines; i++) {
            for(int j = 0; j < columns; j++) {
                trans[j][i] = matrix[i][j];
            }
        }
        return trans;
    }


    void T() {
        Matrix new_matrix(columns, lines);
        
        for(int i = 0; i < lines; i++) {
            for(int j = 0; j < columns; j++) {
                new_matrix[j][i] = matrix[i][j];
            }
        }
        

        matrix = new_matrix.matrix;
        
        swap(lines, columns);
    }


    Matrix minor(int i=0, int j=0) const {
        if(lines < 3 or lines != columns) {
            throw invalid_argument("Size");
        }

        Matrix small(lines-1, columns-1);

        for (int i1 = 0, i2 = 0; i1 < lines; i1++) {
            if (i1 == i) continue;
        
            for (int j1 = 0, j2 = 0; j1 < columns; j1++) {
                if (j1 == j) continue;
            
            small[i2][j2] = matrix[i1][j1];
            j2++;
            }
        i2++;
        }

        return small;
    }


    Matrix map(float (*func)(float)) const {

        Matrix newM(lines, columns);

        for (int i = 0; i < lines; i++) {
            for (int j = 0; j < columns; j++) {
                newM[i][j] = func(matrix[i][j]);
            }
        }
        return newM;
    }



    void print() const {
        for(const auto& line : matrix) {
            cout << "| ";
            for(const float& num : line) {
                cout << num << " ";
            }
            cout << "|" << endl;
        }
        cout << endl;
    }

    void fill( const std::vector<float>& nums ) {
        if(columns*lines != nums.size()) {
            throw invalid_argument("Size!");
        }
        for(int i = 0; i < lines; i++){
            for(int j = 0; j < columns; j++) {
                matrix[i][j] = nums[i*columns+j];
            }
        } 
    }

    void fill_random(float min = 0.0f, float max = 1.0f) {
        for (int i = 0; i < lines; i++) {
            for (int j = 0; j < columns; j++) {
                float t = static_cast<float>(rand()) / RAND_MAX;
                matrix[i][j] = min + t * (max - min);
            }
        }
    }


    int getLines() const { return lines; }
    int getCols() const { return columns; }


    void addBias(const Matrix& bias) {

        if (bias.getCols() != 1 || bias.getLines() != lines) {
            throw std::invalid_argument("addBias: bias must be a column vector with same number of rows");
        }
        for (int i = 0; i < lines; ++i) {
            float bval = bias[i][0];
            for (int j = 0; j < columns; ++j) {
                matrix[i][j] += bval;
            }
        }
    }

    Matrix sumRows() const {
        Matrix result(lines, 1);
        for (int i = 0; i < lines; ++i) {
            float s = 0;
            for (int j = 0; j < columns; ++j) {
                s += matrix[i][j];
            }
            result[i][0] = s;
        }
        return result;
    }

    Matrix() : lines(0), columns(0) {}

    const std::vector<float>& operator[](int i) const {
        return matrix[i];
    }

};