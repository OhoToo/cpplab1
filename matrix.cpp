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



    void print() {
        for(const auto& line : matrix) {
            cout << "| ";
            for(const float& num : line) {
                cout << num << " ";
            }
            cout << "|" << endl;
        }
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

    void fill( const std::vector<float>& nums ) {
        /*if(columns*lines != nums.size()) {
            throw invalid_argument("Size!");
        }*/
        for(int i = 0; i < lines; i++){
            for(int j = 0; j < columns; j++) {
                matrix[i][j] = nums[i*columns+j];
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

    srand(time(nullptr));


    Matrix mat(5, 5);
    
    std::vector<float> nums(25, 0);

    for(int i=0; i < nums.size(); i++) {
        nums[i] = rand() % 11;
        cout << nums[i] << " ";
    }
    cout << endl;

    
    mat.fill(nums);


    mat.print();

    cout << endl;


    Matrix mat1(5, 5);


    for(int i=0; i < nums.size(); i++) {
        nums[i] = rand() % 11;
        cout << nums[i] << " ";
    }
    cout << endl;

    mat1.fill(nums);

    mat1.print();

    cout << endl << endl;

    mat = mat+mat1;
    mat.print();

    return 0;
}