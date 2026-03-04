#include <iostream>
#include <vector>


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
            throw invalid_argument("Size!");
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
            throw invalid_argument("Size!");
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

    //!---------------------------------------------------------------------------------------

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
        // Создаем новую матрицу с переставленными размерами
        Matrix new_matrix(columns, lines);
        
        // Заполняем
        for(int i = 0; i < lines; i++) {
            for(int j = 0; j < columns; j++) {
                new_matrix[j][i] = matrix[i][j];
            }
        }
        
        // Заменяем
        matrix = new_matrix.matrix;
        
        // Меняем размеры
        swap(lines, columns);
    }










    //!---------------------------------------------------------------------------------------


    void print() {
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
        /*
        if(columns*lines != nums.size()) {
            throw invalid_argument("Size!");
        }*/
        for(int i = 0; i < lines; i++){
            for(int j = 0; j < columns; j++) {
                matrix[i][j] = nums[i*columns+j];
            }
        } 
    }

    void fill_random() {

        for(int i=0; i < lines; i++) {
            for(int j=0; j < columns; j++) {
                matrix[i][j] = (float)rand() / RAND_MAX; //todo Чёт кринж с этим (float) надо думать
            }
        }

    }
};

/*
std::vector<std::vector<float>> matrix(int lines, int columns) {
    
    std::vector<std::vector<float>> mat(lines, std::vector<float>(columns, 0));

    return mat;
}
*/

int main() {

    Matrix mat1(3, 3);
    Matrix mat2(3, 5);

    mat1.fill_random();
    mat2.fill_random();

    mat1.print();
    mat2.print();

    (mat1*mat2).print();




    return 0;
}