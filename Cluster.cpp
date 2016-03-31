//Christina Tsui

#include <iostream>
#include <sstream>
#include <cassert>
#include <limits>
#include "Cluster.h"
#include "Exceptions.h"

using namespace std;

namespace Clustering
{

    LNode::LNode(const Point &p, LNodePtr n) : point(p), next(n)
    {
    }

    Cluster::Centroid::Centroid(unsigned int d, const Cluster &c): __dimensions(d), __c(c), __p(d)
    {
        __valid = false;
    }

    const Point Cluster::Centroid::get() const
    {
        return __p;
    }
    void Cluster::Centroid::set(const Point &p)
    {
        __p = p;
        __valid = true;
    }
    bool Cluster::Centroid::isValid() const
    {
        return __valid;
    }
    void Cluster::Centroid::setValid(bool valid)
    {
        __valid = valid;
    }

    void Cluster::Centroid::compute()
    {
        if(__c.__size > 0)
        {
            Point p(__dimensions);
            LNodePtr temp = __c.__points;

            while (temp != nullptr)
            {
                p += temp->point / __c.getSize();

                temp = temp->next;
            }
            set(p);
        }
        else if (__c.__size == 0)
        {
            assert(__c.__points == nullptr);
            __valid = true;
            toInfinity();
        }
    }

    bool Cluster::Centroid::equal(const Point &p) const
    {
        if (p == __p)
            return true;
        return false;
    }
    void Cluster::Centroid::toInfinity()
    {
        for (int index = 0; index < __p.getDims(); ++index)
        {
            __p[index] = numeric_limits<double>::max();
        }
    }

    unsigned int Cluster::__idGenerator = 0;

    void Cluster::__del()
    {
        while (__points != nullptr && __points->next != nullptr)
        {

            LNodePtr temp;
            temp = __points->next;
            delete __points;
            __points = temp;

        }

        if (__points != nullptr && __points->next == nullptr)
        {
            delete __points;
        }
    }


    void Cluster::__cpy(LNodePtr pts)
    {
        if (pts == nullptr)
            return;
        this->__points = new LNode(pts->point, nullptr);
        this->__size++;

        pts = pts->next;
        while (pts != nullptr)
        {
            this->add(pts->point);
            pts = pts->next;
        }
    }

    bool Cluster::__in(const Point &p) const
    {
        if (__dimensionality != p.getDims())
        {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }
        if (__size == 0)
        {
            return false;
        }
        LNodePtr curr = __points;
        while (curr->next != nullptr)
        {
            if (curr->point.getId() == p.getId())
            {
                return true;
            }
            else
            {
                curr = curr->next;
            }
        }
        if (curr->point.getId() == p.getId())
        {
            return true;
        }
        return false;
    }

    const char Cluster::POINT_CLUSTER_ID_DELIM = ':';


    Cluster::Cluster(unsigned int d): __size(0), __points(nullptr), __dimensionality(d), centroid(d, *this)
    {
        __id = __idGenerator;
        __idGenerator++;
    }

    // The big three: cpy ctor, overloaded operator=, dtor
    Cluster::Cluster(const Cluster &c) :__size(0), __dimensionality(c.__dimensionality), centroid(c.__dimensionality, c)
    {
        if (c.__size == 0)
        {
            __points = nullptr;
            __id = c.__id;
        }
        else if (c.__size == 1)
        {
            __points = new LNode(c.__points->point, nullptr);
            __size++;
            __id = c.__id;
        }
        else if (c.__size > 1)
        {
            LNodePtr temp = c.__points;
            __cpy(temp);
            __id = c.__id;
        }
        centroid.compute();
        assert(__size == c.__size);
    }

    // The big three: cpy ctor, overloaded operator=, dtor
    Cluster& Cluster::operator=(const Cluster &c) //Fix this for centroid
    {

        if (this == &c)
        {
            return *this;
        }
        else
        {
            __del();
            LNodePtr temp = c.__points;
            __size = 0;
            __cpy(temp);
            centroid.compute();
            assert(this->__size == c.__size);
            __id = c.__id;
            return *this;
        }
    }

    // The big three: cpy ctor, overloaded operator=, dtor
    Cluster::~Cluster()
    {
        __del();
    }

    // Getters/Setters
    unsigned int Cluster::getSize() const
    {
        return __size;
    }
    unsigned int Cluster::getDimensionality() const
    {
        return __dimensionality;
    }
    unsigned int Cluster::getId() const
    {
        return __id;
    }

    // Add/remove: They allow calling c1.add(c2.remove(p));
    void Cluster::add(const Point &p)
    {
        if (p.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }
        LNodePtr temp = new LNode(p, nullptr);
        LNodePtr prev = __points;
        LNodePtr curr = __points;

        if (__size == 0)
        {
            __points = temp;
            __size++;
        }
        else if (__size == 1)
        {
            if (temp->point < curr->point)
            {
                LNodePtr foo = __points;
                temp->next = foo;
                __points = temp;
                __size++;
            }
            else
            {
                __points->next = temp;
                __size++;
            }
        }
        else if (__size > 1)
        {
            int index = 0;

            while (curr->next != nullptr)
            {
                if (temp->point < curr->point)
                {
                    if (curr == __points)
                    {
                        LNodePtr foo = __points;
                        temp->next = foo;
                        __points = temp;
                        break;
                    }
                    else
                    {
                        temp->next = curr;
                        prev->next = temp;
                        break;
                    }
                }

                curr = curr->next;

                if (index > 0)
                {
                    prev = prev->next;
                }
                index++;
            }
            if (curr->next == nullptr)
            {
                curr->next = temp;
            }
            __size++;
            centroid.setValid(false);
        }
    }

    // Add/remove: They allow calling c1.add(c2.remove(p));
    const Point& Cluster::remove(const Point &p)
    {
        if (p.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }
        int index = 0;
        LNodePtr temp;
        LNodePtr prev = __points;
        LNodePtr curr = __points;

        while (curr != nullptr)
        {
            if (curr->point == p)
            {
                if (curr == __points)
                {
                    __points = __points->next;
                    delete curr;
                    --__size;
                    break;
                }
                temp = curr;
                curr = curr->next;
                prev->next = curr;
                delete temp;
                --__size;
                break;
            }

            curr = curr->next;
            if (index > 0)
            {
                prev = prev->next;
            }
            ++index;
        }
        if (__size == 0)
        {
            centroid.setValid(false);
        }
        return p;
    }

    // Add/remove: They allow calling c1.add(c2.remove(p));
    bool Cluster::contains(const Point &p) const
    {
        if (__dimensionality != p.getDims())
        {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }
        if (__in(p))
        {
            return true;
        }
        return false;
    }

    // Centroid functions
    void Cluster::pickCentroids(unsigned int k, Point **pointArray)
    {
        LNodePtr curr = __points;
        Point inf(__dimensionality);
        for (int index = 0; index < __dimensionality; ++index)
        {
            inf[index] = std::numeric_limits<double>::max();
        }
        if (k >= __size)
        {
            for (int index = 0; index < __size; ++index)
            {
                *pointArray[index] = curr->point;
                curr = curr->next;
            }
            for (int i = __size; i < k; ++i)
            {
                *pointArray[i] = inf;
            }
        }
        else if (k < __size)
        {
            for (int index = 0; index < k; ++index)
            {
                *pointArray[index] = curr->point;
                curr = curr->next;
            }
        }
    }

    // Element access (aka direct access, array operator)
    const Point& Cluster::operator[](unsigned int index) const
    {
        if (__size <= 0)
        {
            throw EmptyClusterEx();
        }
        if (index >= __size)
        {
            throw OutOfBoundsEx(__size, index);
        }

        LNodePtr curr = __points;

        int i = 1;
        if (index == 0)
        {
            return __points->point;
        }
        curr = curr->next;

        while (curr->next != nullptr)
        {
            if (i == index)
            {
                return curr->point;
            }
            curr = curr->next;
            ++i;
        }
        return curr->point;
    }

    // Operators with Point-s
    // - Members
    Cluster& Cluster::operator+=(const Point &p)
    {
        if (p.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }
        this->add(p);
        centroid.setValid(false);
        return *this;
    }

    // Operators with Point-s
    // - Members
    Cluster& Cluster::operator-=(const Point &p)
    {
        if (p.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }
        if (this->contains(p))
        {
            this->remove(p);
            centroid.setValid(false);
        }

        return *this;
    }

    // Operators with Cluster-s
    // - Members
    Cluster& Cluster::operator+=(const Cluster &c) //Union
    {
        LNodePtr temp = c.__points;

        while (temp != nullptr)
        {
            if (!this->__in(temp->point))
            {
                this->add(temp->point);
            }
            temp = temp->next;
        }
        return *this;
    }

    // Operators with Cluster-s
    // - Members
    Cluster& Cluster::operator-=(const Cluster &c)
    {
        LNodePtr temp = c.__points;
        while (temp != nullptr)
        {
            if (__in(temp->point))
            {
                remove(temp->point);
                centroid.setValid(false);
            }
            temp = temp->next;
        }

        if (__size == 0)
        {
            centroid.setValid(false);
        }
        return *this;
    }

    // IO
    // - Friends
    std::ostream& operator<<(std::ostream &os, const Cluster &c)
    {
        LNodePtr curr = c.__points;
        if (c.__size == 0)
        {
            return os;
        }
        else
        {
            while (curr->next != nullptr)
            {
                os << curr->point << ' ' << Cluster::POINT_CLUSTER_ID_DELIM << ' ' << c.__id << endl;
                curr = curr->next;
            }
            os << curr->point;
            os << ' ' << Cluster::POINT_CLUSTER_ID_DELIM << ' ' << c.__id;
            return os;
        }
    }

    std::istream& operator>>(std::istream &is, Cluster &c)
    {

        string wholeLine;

        while (getline(is,wholeLine))
        {
            Point p(c.__dimensionality);
            stringstream lineStream(wholeLine);
            string value;
            int i = 0;
            double d = 0.0;

            while (getline(lineStream, value, Point::POINT_VALUE_DELIM))
            {
                try
                {
                    if (i >= c.__dimensionality)
                    {
                        p.rewindIdGen();
                        throw OutOfBoundsEx(c.__dimensionality, i);
                    }
                    d = stod(value);
                    p.setValue(i, d);
                }
                catch (OutOfBoundsEx &ex)
                {
                    cout << "Exception caught by user:" << ex << endl;
                    p.rewindIdGen();
                }
                ++i;
            }
            try
            {
                if (i != c.__dimensionality)
                {
                    throw DimensionalityMismatchEx(c.__dimensionality, i);
                }
                c.add(p);
            }
            catch (DimensionalityMismatchEx &ex)
            {
                cout << "Exception caught by user: " << ex << endl;

            }
        }

        return is;
    }

    // - Friends
    bool operator==(const Cluster &lhs, const Cluster &rhs)
    {
        if (lhs.__dimensionality != rhs.__dimensionality)
        {
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);
        }

        if (lhs.__size != rhs.__size)
        {
            return false;
        }

        LNodePtr lCurr = lhs.__points;
        LNodePtr rCurr = rhs.__points;

        while (lCurr != nullptr)
        {
            if (lCurr->point != rCurr->point )
            {
                return false;
            }

            lCurr = lCurr->next;
            rCurr = rCurr->next;
        }

        return true;
    }
    // - Friends
    bool operator!=(const Cluster &lhs, const Cluster &rhs)
    {
        if (lhs.__dimensionality != rhs.__dimensionality)
        {
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);
        }

        if (lhs == rhs)
        {
            return false;
        }
        return true;
    }

    // Binary operators for Cluster-s and Point pointers
    // - Friends
    const Cluster operator+(const Cluster &lhs, const Point &rhs)
    {
        if (lhs.__dimensionality != rhs.getDims())
        {
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.getDims());
        }

        Cluster c(lhs);
        c.add(rhs);
        return c;
    }

    // Binary operators for Cluster-s and Point pointers
    // - Friends
    const Cluster operator-(const Cluster &lhs, const Point &rhs)
    {
        if (lhs.__dimensionality != rhs.getDims())
        {
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.getDims());
        }

        Cluster c(lhs);
        if (c.contains(rhs))
        {
            c.remove(rhs);
        }
        return c;
    }

    // Binary operators for Cluster-s
    // - Friends
    const Cluster operator+(const Cluster &lhs, const Cluster &rhs) //Union
    {
        if (lhs.__dimensionality != rhs.__dimensionality)
        {
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);
        }

        Cluster c(lhs);
        c += rhs;
        return c;
    }

    // Binary operators for Cluster-s
    // - Friends
    const Cluster operator-(const Cluster &lhs, const Cluster &rhs) // (asymmetric) difference
    {
        if (lhs.__dimensionality != rhs.__dimensionality)
        {
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);
        }

        Cluster c(lhs);
        c -= rhs;
        return c;
    }

    // Move
    Cluster::Move::Move(const Point &p, Cluster &from, Cluster &to) : __p(p), __from(from), __to(to)
    {
    }

    void Cluster::Move::perform()
    {
        __to.add(__from.remove(__p));
    }
}