//Christina Tsui

#include <iostream>
#include <fstream>
#include <limits>
#include "Cluster.h"
#include "Exceptions.h"
#include "KMeans.h"

using namespace std;

namespace Clustering
{
    // KMeans Constructor
    KMeans::KMeans(unsigned int dim, unsigned int k, std::string filename, unsigned int maxIter) :
            __dimensionality(dim),
            __k(k),
            __iFileName(filename),
            __maxIter(maxIter),
            __numIter(0),
            __numNonempty(0)
    {
        if (k == 0)
        {
            throw ZeroClustersEx();
        }

        __initCentroids = new Point*[k];

        std::ifstream dataIn(__iFileName);
        if (__iFileName != "")
        {
            __clusters = new Cluster*[__k];
            for (int index = 0; index < __k; ++index)
            {
                __clusters[index] = new Cluster(__dimensionality);
            }
            if (!dataIn.is_open())
            {
                for (int index = 0; index < k; ++index)
                {
                    delete __clusters[index];
                }
                delete [] __clusters;
                throw DataFileOpenEx(__iFileName);
            }
            else
            {
                dataIn >> *__clusters[0];
                __numNonempty = 1;
                dataIn.close();
            }
        }
        else
        {
            throw DataFileOpenEx(__iFileName);
        }

        for (int index1 = 0; index1 < __k; ++index1)
        {
            __initCentroids[index1] = new Point(__dimensionality);
        }

        __clusters[0]->pickCentroids(k, __initCentroids);

        for (int index2 = 0; index2 < __k; ++index2)
        {
            __clusters[index2]->centroid.set(*__initCentroids[index2]);
        }
    }

    // KMeans Destructor
    KMeans::~KMeans()
    {
        for (int index = 0; index < __k; ++index)
        {
            delete __initCentroids[index];
        }
        delete [] __initCentroids;

        for (int index1 = 0; index1 < __k; ++index1)
        {
            delete __clusters[index1];
        }
        delete [] __clusters;
    }

    // Accessors
    unsigned int KMeans::getMaxIter()
    {
        return __maxIter;
    }
    unsigned int KMeans::getNumIters()
    {
        return __numIter;
    }
    unsigned int KMeans::getNumNonemptyClusters()
    {
        return __numNonempty;
    }
    unsigned int KMeans::getNumMovesLastIter()
    {
        return __numMovesLastIter;
    }

    Cluster& KMeans::operator[](unsigned int u)
    {
        if (__k == 0)
        {
            throw ZeroClustersEx();
        }
        if (u >= __k)
        {
            throw DimensionalityMismatchEx(__k, u);
        }
        return *__clusters[u];
    }
    const Cluster& KMeans::operator[](unsigned int u) const
    {
        if (__k == 0)
        {
            throw ZeroClustersEx();
        }
        if (u >= __k)
        {
            throw DimensionalityMismatchEx(__k, u);
        }
        return *__clusters[u];
    }

    std::ostream& operator<<(std::ostream &os, const KMeans &kmeans)
    {
        Point inf(kmeans.__dimensionality);
        for (int index = 0; index < kmeans.__dimensionality; ++index)
        {
            inf[index] = std::numeric_limits<double>::max();
        }
        for (int index1 = 0; index1 < kmeans.__k; ++index1)
        {
            if (!kmeans.__clusters[index1]->centroid.equal(inf))
            {
                os << *kmeans.__clusters[index1] << std::endl;
            }
        }
        return os;
    }

    void KMeans::run()
    {

        int moves = 100; //max
        int iter = 0;
        int nonempty = 0;

        while (moves > 0 && iter < __maxIter)
        {
            moves = 0;

            for (int index = 0; index < __k; ++index)
            {

                for (int x = 0; x < __clusters[index]->getSize(); ++x)
                {

                    Cluster &c = *(__clusters[index]);

                    Point current_point(__dimensionality);
                    current_point = c[x];
                    int smallest_dist_index = 0;
                    double smallest_dist = current_point.distanceTo(*__initCentroids[0]);

                    for (int y = 0; y < __k; y++)
                    {

                        if (current_point.distanceTo(*__initCentroids[y]) < smallest_dist)
                        {
                            smallest_dist = current_point.distanceTo(*__initCentroids[y]);
                            smallest_dist_index = y;
                        }

                    }
                    Cluster::Move change_clusters(current_point, *__clusters[index], *__clusters[smallest_dist_index]);
                    change_clusters.perform();

                    for (int z = 0; z < __k; ++z)
                    {

                        __clusters[z]->centroid.compute();
                    }
                    if (*__clusters[index] != *__clusters[smallest_dist_index])
                    {
                        moves++;
                    }

                }
            }
            iter++;
        }
        Point inf(__dimensionality);
        for (int index1 = 0; index1 < __dimensionality; ++index1)
        {
            inf[index1] = std::numeric_limits<double>::max();
        }

        for (int index2 = 0; index2 < __k; ++index2)
        {
            if (__clusters[index2]->centroid.get() != inf)
            {
                ++nonempty;
            }
        }
        __numIter = iter;
        __numMovesLastIter = moves;
        __numNonempty = nonempty;
    }
}