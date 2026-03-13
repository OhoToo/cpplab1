    #include "matrix.hpp"
    #include "linearfunc.hpp"
    #include "point.hpp"
    #include <iostream>


    int main() {

        
        
        Matrix mat(5, 5);
        Matrix mat1(5,5);

        mat1.fill_random();
        mat.fill_random();

        mat.print();
        mat1.print();

        (mat.minor(0, 0)).print();

        (mat*mat1).print();



        return 0;
}