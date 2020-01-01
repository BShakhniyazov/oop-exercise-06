#pragma once

#include <iostream>
#include <vector>
#include <cmath>

#define PI 3.141592653589793
#define mkPT(v, a, b) v.first = a; v.second = b

//square
template<class T>
class Square
{
public:
    using vertex = std::pair<T, T>;
    vertex v[4]; //array of vertecies

    Square() : Square(0, 0, 0, 0) {}
    Square(T Ox, T Oy, T Ax, T Ay) //define square by 2 points
    {
        mkPT(v[0], Ax, Ay);
        mkPT(v[1], 2 * Ox - Ax, Ay);
        mkPT(v[2], 2 * Ox - Ax, 2 * Oy - Ay);
        mkPT(v[3], Ax, 2 * Oy - Ay);
    }
};

//show vertex
template<class T>
std::ostream& operator<<(std::ostream &os, const std::pair<T, T> &p)
{
    os << "{ " << p.first << ", " << p.second << " }";
    return os;
}

//show vertecies of square
template<class T>
std::ostream& operator<<(std::ostream &os, const Square<T> &p)
{
    for (int i = 0; i < 4; i++)
    {
        os << p.v[i];
        if (i < 3) os << ", ";
    }
    return os;
}

//get area of square
template<class T>
double area(Square<T> p)
{
    return ((p.v[0].first - p.v[1].first) + (p.v[1].first - p.v[2].first)) *
            (p.v[0].first - p.v[1].first) + (p.v[1].first - p.v[2].first);
}
