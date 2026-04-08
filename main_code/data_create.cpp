#include <tuple>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <cmath>

struct Point {
    float x;
    float y;
    int label;

    Point(float x_, float y_, int l_) { //todo Надо уточнить чё это... типо да, это конструктор, но...
        x = x_;
        y = y_;
        label = l_;
    }
};

struct Line {
    float A;
    float B;
    float C;
    Line(float A_, float B_, float C_) { //todo Надо уточнить чё это... типо да, это конструктор, но...
        A = A_;
        B = B_;
        C = C_;
    }

    std::vector<float> get_kb() {
        return {-1*(A/B), -1*(C/B)};
    }
};


float randnum(const std::vector<float>& range, bool with_zero = true) {
    float num = range[0] + (float)rand() / RAND_MAX * (range[1] - range[0]);

    if(with_zero == false) {
        while(num == 0.0f) {
            num = range[0] + (float)rand() / RAND_MAX * (range[1] - range[0]);
        }
    }

    return num;
}






std::tuple<std::vector<Point>, Line> create_data(int amount = 40, float range_ = 10.0f) {


    //std::vector<float> range = {-1*range_, range_};
    std::vector<int> count = {0, 0}; // количество точек в каждом label
    std::vector<Point> result;

    
    //! Ax + By + C = 0,  y = -(A/B)x - (C/B)
    Line line(randnum({-1000.0f, 1000.0f}),randnum({-1*range_, range_}, false), randnum({-1*range_, range_}));



    float norm = std::sqrt(line.A * line.A + line.B * line.B);
    float min_dist = range_ / 5.0f;

    while (count[0] < amount / 2 or count[1] < amount / 2) {
        float x = randnum({-1*range_, range_});
        float y = randnum({-1*range_, range_});

        float side = line.A * x + line.B * y + line.C;
        float dist = std::fabs(side) / norm;

        if (dist <= min_dist) {
            continue;
        }

        if (side < 0 and count[0] < amount / 2) {
            result.push_back({x, y, 0});
            count[0]++;
        } else if (side > 0 and count[1] < amount / 2) {
            result.push_back({x, y, 1});
            count[1]++;
        }
    }

    return {result, line};
    
}







/*
//! Пока что просто вывод 
int main() {
    srand(time(0));

    auto [points, line] = create_data();

    std::cout << "Line:\n";
    std::cout << "A = " << line.A << ", B = " << line.B << ", C = " << line.C << "\n\n";

    std::cout << "Points:\n";
    for (const auto& p : points) {
        std::cout << "x = " << p.x
                  << ", y = " << p.y
                  << ", label = " << p.label << '\n';
    }

    return 0;
}
*/