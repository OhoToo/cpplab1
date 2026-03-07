#include <iostream>
#include <vector>
#include <cmath>


#include <cstdlib>
#include <ctime>



using namespace std;

/**
 * @brief Операции над матрицами на основе std::vector<std::vector<float>> . 
 * На данный момент есть транспонирование (.T | .get_T()), сложение вычитание умножение (скалярное), вывод .print() и 
 * заполнение .fill() нулевыми и fill_random() случайными значениями от 0 до 1
 * 
 * 
 * @param lines число строк
 * @param columns число столбцов
 * 
 * 
 * 
 * 
 */
class Matrix{
private:
    int lines, columns;
    std::vector<std::vector<float>> matrix;
    


public:
    //!------------------------------------Base-----------------------------------------------------
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

    //!---------------------------------------Действия в------------------------------------------------

    /**
     * @brief Транспонирование с созданием новой матрицы
     * 
     * 
     */
    Matrix get_T() const{
        Matrix trans(columns, lines);
        for(int i = 0; i < lines; i++) {
            for(int j = 0; j < columns; j++) {
                trans[j][i] = matrix[i][j];
            }
        }
        return trans;
    }


    /**
     * @brief Транспонирование без создания новой
     */
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


    /**
     * @brief Нахождение минора без указанного i j
     */
    Matrix minor(int i=0, int j=0) {
        if(lines < 3 or lines != columns) {
            throw invalid_argument("Size");
        }

        Matrix small(lines-1, columns-1);

        for (int i1 = 0, i2 = 0; i1 < lines; i1++) {
            if (i1 == i) continue;  // пропускаем строку i
        
            for (int j1 = 0, j2 = 0; j1 < columns; j1++) {
                if (j1 == j) continue;  // пропускаем столбец j
            
            small[i2][j2] = matrix[i1][j1];
            j2++;
            }
        i2++;
        }

        return small;
    }

    //** Короч с детерминантом и обратной матрицей жуть, а нейрослоп добавлять не хочу, пока без них





    //!------------------------------------------Действия вне---------------------------------------------


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

int main() {

    Matrix mat1(3, 3);
    Matrix mat2(3, 5);

    mat1.fill_random();
    mat2.fill_random();

    mat1.print();
    //mat2.print();

    //(mat1*mat2).print();

    mat1.minor().print();

    return 0;
}