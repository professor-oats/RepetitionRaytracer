#include "rtweekend.h"

#include <iostream>
#include <iomanip>

/* Interestingly, the stratified method is not only better, it converges with a better asymptotic rate! 
    Unfortunately, this advantage decreases with the dimension of the problem 
    (so for example, with the 3D sphere volume version the gap would be less). 
    This is called the Curse of Dimensionality. We are going to be very high dimensional 
    (each reflection adds two dimensions), so I won't stratify in this book, 
    but if you are ever doing single-reflection or shadowing or some strictly 2D problem, 
    you definitely want to stratify. */

int main() {
    int inside_circle = 0;
    int inside_circle_stratified = 0;
    int sqrt_N = 10000;
    for (int i = 0; i < sqrt_N; i++) {
        for (int j = 0; j < sqrt_N; j++) {
            auto x = random_double(-1,1);
            auto y = random_double(-1,1);
            if (x*x + y*y < 1)
                inside_circle++;
            x = 2*((i + random_double()) / sqrt_N) - 1;
            y = 2*((j + random_double()) / sqrt_N) - 1;
            if (x*x + y*y < 1)
                inside_circle_stratified++;
        }
    }

    auto N = static_cast<double>(sqrt_N) * sqrt_N;
    std::cout << std::fixed << std::setprecision(12);
    std::cout
        << "Regular    Estimate of Pi = "
        << 4*double(inside_circle) / (sqrt_N*sqrt_N) << '\n'
        << "Stratified Estimate of Pi = "
        << 4*double(inside_circle_stratified) / (sqrt_N*sqrt_N) << '\n';
}