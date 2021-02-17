#pragma once

#include "Grid.h"

template<typename Entity, std::size_t num_rows> requires (num_rows > 0)
class GridMock : public Grid<Entity, num_rows> {
    using Grid<Entity, num_rows>::Grid;
public:
    virtual std::size_t getRowIndex(Latitude lat) const override {
        return Grid<Entity, num_rows>::getRowIndex(lat);
    }

    virtual std::size_t getColIndex(Longitude lng, std::size_t rowIndex) const override {
        return Grid<Entity, num_rows>::getColIndex(lng, rowIndex);
    }

    virtual Meters distanceFromCell(Coordinates cellCoord, Coordinates coordinates) const override {
        return Grid<Entity, num_rows>::distanceFromCell(cellCoord, coordinates);
    }

    //TODO delete after submission
// works only if Cell is defined as protected
//    virtual Meters distanceFromCell(const typename Grid<Entity, num_rows>::Cell *cell, Coordinates coordinates) const {
//        return Grid<Entity, num_rows>::distanceFromCell(cell, coordinates);
//    }

};
