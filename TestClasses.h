#pragma once


#include <gtest/gtest.h>

/**
 * class EntitiesContainer - class that is dedicated to hold a reference to the entities inserted to the grid.
 */
template<class Entity>
class EntitiesContainer {
    mutable std::unordered_map<int, std::vector<std::shared_ptr<Entity>>> entitiesMap;
public:
    template<typename ActualT>
    requires concrete_derived_or_same<ActualT, Entity>
    void addEntity(std::shared_ptr<ActualT> entity, int type) {
        entitiesMap[type].push_back(entity);
    }
};

class RandomTest : public ::testing::Test {
    void SetUp() override {}

    void TearDown() override {}


public:
    double fRand(double fMin, double fMax) {
        double f = (double) rand() / RAND_MAX;
        return fMin + f * (fMax - fMin);
    }

    Coordinates randCoordinates() {
        Longitude longitude(fRand(-180, 180));
        Latitude latitude(fRand(-90, 90));
        return Coordinates(longitude, latitude);
    }
};


struct O {
    int id;
    bool predTrue = true;
    bool predFalse = false;
    std::string type;

    O(int id, std::string type) : id(id), type(type) {}

    virtual void what() = 0;
};

struct A : public O {
    A(int id, std::string type) : O(id, type) {}

    A(int id) : O(id, "A") {}

    void what() override {}

    void whatA() {}
};

struct B : public A {
    B(int id, std::string type) : A(id, type) {}

    B(int id) : A(id, "B") {}

    void what() override {}

    void whatB() {}
};

struct C : public A {
    C(int id, std::string type) : A(id, type) {}

    C(int id) : A(id, "C") {}

    void what() override {}

    void whatC() {}
};