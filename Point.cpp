//Christina Tsui

#include <cmath>
#include <iostream>
#include "Point.h"
#include "Exceptions.h"
#include "Cluster.h"

using namespace std;

namespace Clustering
{

    unsigned int Point::__idGen = 0;
    const char Point::POINT_VALUE_DELIM = ',';

    void Point::rewindIdGen()
    {
        __idGen--;
    }

    Point::Point(unsigned int dim)
    {
        if (dim == 0)
        {
            throw ZeroDimensionsEx();
        }
        __dim = dim;
        __values = new double[__dim];
        __id = __idGen;
        __idGen++;

        for (int index = 0; index < __dim; ++index)
        {
            __values[index] = 0;
        }
    }

    // Big three: cpy ctor, overloaded operator=, dtor
    Point::Point(const Point &p)
    {
        __dim = p.__dim;
        __values = new double[p.__dim];
        for (int index = 0; index < __dim; ++index)
        {
            __values[index] = p.__values[index];
        }
        __id = p.__id;
    }

    // Big three: cpy ctor, overloaded operator=, dtor
    Point& Point::operator=(const Point &p)
    {
        std::size_t index;
        double *newPoint;

        if (this == &p)
        {
            return *this;
        }
        if (__dim != p.__dim)
        {
            throw DimensionalityMismatchEx(__dim, p.__dim);
        }
        for (index = 0; index < __dim; ++index)
        {
            __values[index] = p.__values[index];
        }
        __id = p.__id;
        return *this;
    }

    // Big three: cpy ctor, overloaded operator=, dtor
    Point::~Point()
    {
        delete [] __values;
    }

    // Accessors & mutators
    int Point::getId() const
    {
        return __id;
    }

    // Accessors & mutators
    unsigned int Point::getDims() const
    {
        return __dim;
    }

    // Accessors & mutators
    void Point::setValue(unsigned int i, double p)
    {
        if (i >= __dim)
        {
            throw OutOfBoundsEx(__dim, i);
        }
        __values[i] = p;
    }

    // Accessors & mutators
    double Point::getValue(unsigned int i) const
    {
        if (i >= __dim)
        {
            throw OutOfBoundsEx(__dim, i);
        }
        return __values[i];
    }


    double Point::distanceTo(const Point &p) const
    {
        if (__dim != p.getDims())
        {
            throw DimensionalityMismatchEx(__dim, p.getDims());
        }
        double distance = 0;
        double deltaX = 0;

        for (int index = 0; index < __dim; ++index)
        {
            deltaX += pow((__values[index] - p[index]), 2);
        }
        distance = sqrt(deltaX);
        return distance;
    }

    //Overloaded Operator Members
    Point& Point::operator*=(double rhs)
    {
        for (int index = 0; index < __dim; ++index)
        {
            __values[index] = __values[index] * rhs;
        }

        return *this;
    }

    Point& Point::operator/=(double rhs)
    {
        for (int i = 0; i < __dim; ++i)
        {
            __values[i] /= rhs;
        }
        return *this;
    }

    const Point Point::operator*(double rhs) const
    {
        Point p(__dim);
        for (int i = 0; i < __dim; ++i)
        {
            p.__values[i] = __values[i] * rhs;
        }
        return p;
    }

    const Point Point::operator/(double rhs) const
    {
        Point p(__dim);
        for (int index = 0; index < __dim; ++index)
        {
            p.__values[index] = __values[index] / rhs;
        }
        return p;
    }

    double& Point::operator[](unsigned int index)
    {
        if (index >= __dim)
        {
            throw OutOfBoundsEx(__dim, index);
        }
        return __values[index];
    }
    const double& Point::operator[](unsigned int index) const
    {
        if (index >= __dim)
        {
            throw OutOfBoundsEx(__dim, index);
        }
        return __values[index];
    }

    //Friends
    Point& operator+=(Point &p, const Point &q)
    {
        if (p.__dim != q.__dim)
        {
            throw DimensionalityMismatchEx(p.__dim, q.__dim);
        }
        for (int index = 0; index < p.__dim; ++index)
        {
            p.__values[index] += q.__values[index];
        }
        return p;
    }

    Point& operator-=(Point &p, const Point &q)
    {
        if (p.__dim != q.__dim)
        {
            throw DimensionalityMismatchEx(p.__dim, q.__dim);
        }
        for (int index = 0; index < p.__dim; ++index)
        {
            p.__values[index] -= q.__values[index];
        }
        return p;
    }

    const Point operator+(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }
        Point p(lhs);
        p += rhs;
        return p;
    }

    const Point operator-(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }
        Point p(lhs);
        p -= rhs;
        return p;
    }

    bool operator==(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }

        for (int index = 0; index < lhs.__dim; ++index)
        {
            if (lhs.__values[index] != rhs.__values[index])
                return false;
        }
        return true;
    }

    bool operator!=(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }

        if (lhs == rhs)
        {
            return false;
        }
        return true;
    }

    bool operator<(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }
        for (int index = 0; index < lhs.__dim; ++index)
        {
            if (lhs.__values[index] < rhs.__values[index])
                return true;
            else if (lhs.__values[index] > rhs.__values[index])
                return false;
        }
        return false;
    }

    bool operator>(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }
        for (int index = 0; index < lhs.__dim; ++index)
        {
            if (lhs.__values[index] > rhs.__values[index])
                return true;
        }
        return false;
    }

    bool operator<=(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }

        if (lhs > rhs)
            return false;
        else
            return true;
    }

    bool operator>=(const Point &lhs, const Point &rhs)
    {
        if (lhs.__dim != rhs.__dim)
        {
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        }

        if (lhs < rhs)
            return false;
        else
            return true;
    }

    std::ostream& operator<<(std::ostream &os, const Point &rhs)
    {
        int index = 0;
        for (; index < rhs.__dim - 1; ++index)
        {
            os << rhs.__values[index] << ", ";
        }
        os << rhs.__values[index];
        return os;
    }

    std::istream& operator>>(std::istream &is, Point &rhs)
    {
        std::string value;
        double d;
        int index = 0;

        while (getline(is, value, Point::POINT_VALUE_DELIM))
        {
            if (index > rhs.__dim - 1)
            {
                Point::rewindIdGen();
                throw DimensionalityMismatchEx(rhs.__dim, index);
            }

            d = stod(value);
            rhs.__values[index++] = d;
        }

        if (index != rhs.__dim )
        {
            Point::rewindIdGen();
            throw DimensionalityMismatchEx(index, rhs.__dim);
        }

        return is;
    }
}