// talschneider 206897514

#pragma once

#include "GISNamedTypes.h"
#include "CoordinatesMath.h"
#include <vector>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <deque>
#include <unordered_set>

#define NORTH_ABSOLUTE_BEARING 0.0
#define SOUTH_ABSOLUTE_BEARING 180.0
#define DEGREES_180 180.0
#define DEGREES_360 360.0

// Two concepts required by the below functions:

template<class Me, class Other> concept derived_or_same =
std::same_as<Me, Other> || std::derived_from<Me, Other>;

template<class Me, class Other> concept concrete_derived_or_same =
!std::is_abstract_v<Me> && derived_or_same<Me, Other>;

template<typename Entity, std::size_t num_rows> requires (num_rows > 0)
class Grid {
    class Cell {
        friend class Grid<Entity, num_rows>;
        const std::size_t rowIndex;
        const std::size_t colIndex;
        Coordinates coordinates; // coordinates inside the cell (the center)
        mutable std::unordered_map<std::type_index, std::vector<Entity *>> entitiesMap;
        mutable std::size_t entityAmount = 0;

        Cell(std::size_t rowIndex, std::size_t colIndex, Coordinates coordinates) : rowIndex(rowIndex),
                                                                                    colIndex(colIndex),
                                                                                    coordinates(coordinates) {}

    public:
        /**
         * (Cell::A1)
         * Getting Entities from a Cell.
         * returns all entities in that cell, which return true for the PredicateFunc.
         */
        template<typename PredicateFunc>
        std::vector<Entity *> getEntities(PredicateFunc &&predicateFunc) const {
            std::vector<Entity *> entities;
            for (auto &type: entitiesMap) {
                for (Entity *entity: type.second) {
                    if (predicateFunc(*entity)) {
                        entities.push_back(entity);
                    }
                }
            }
            return entities;
        }

        /** (Cell::B1)
         * Getting specific type of Entities from a Cell:
         * @return returns all entities of type ActualT which return true for the PredicateFunc.
         */
        template<typename ActualT, typename PredicateFunc>
        requires concrete_derived_or_same<ActualT, Entity>
        std::vector<ActualT *> getEntities(PredicateFunc &&predicateFunc) const {
            return getEntities<ActualT, PredicateFunc>(std::forward<PredicateFunc>(predicateFunc), SIZE_MAX);
        }

        /**
         * (Cell::B2)
         * same as (Cell::B1) above but with a limit on the number of returned entities (up to limit entities).
         */
        template<typename ActualT, typename PredicateFunc>
        requires concrete_derived_or_same<ActualT, Entity>
        std::vector<ActualT *> getEntities(PredicateFunc &&predicateFunc, std::size_t limit) const {
            std::vector<ActualT *> entities;
            if (numEntities<ActualT>() != 0) {
                for (Entity *entity: entitiesMap.at(typeid(ActualT))) {
                    if (entities.size() == limit) {
                        break;
                    }
                    if (predicateFunc(*dynamic_cast<ActualT *>(entity))) {
                        entities.push_back(dynamic_cast<ActualT *>(entity));
                    }
                }
            }
            return entities;
        }

        /**
         * (Cell::B3)
         * returns a range of all entities of type ActualT.
         * This function returns a view that is updated “behind the scene” automatically in case additional objects of type ActualT are added to this Cell via the Grid.
         * Order of entities in the view shall be the same as the order of their insertion to the grid.
         * Iterating over the returned view generates pointers with the correct type, i.e. ActualT*.
         */
        template<typename ActualT>
        requires concrete_derived_or_same<ActualT, Entity>
        std::ranges::sized_range auto getEntitiesView() const {
            class View {
                std::vector<Entity *> &vec;
            public:
                struct pointersViewIterator : public std::vector<Entity *>::iterator {
                    pointersViewIterator() {}

                    pointersViewIterator(typename std::vector<Entity *>::iterator itr)
                            : std::vector<Entity *>::iterator(
                            itr) {}

                    ActualT *
                    operator*() { return static_cast<ActualT *>(this->std::vector<Entity *>::iterator::operator*()); }

                    const ActualT *
                    operator*() const { return static_cast<const ActualT *>(this->std::vector<Entity *>::iterator::operator*()); }

                    pointersViewIterator &operator++() {
                        this->std::vector<Entity *>::iterator::operator++();
                        return *this;
                    }

                    pointersViewIterator operator++(int) {
                        auto old = *this;
                        ++(*this);
                        return old;
                    }
                };

                View(std::vector<Entity *> &vec) : vec(vec) {}

                auto begin() const { return pointersViewIterator{vec.begin()}; }

                auto end() const { return pointersViewIterator{vec.end()}; }

                std::size_t size() const { return vec.size(); }
            };

            return View{entitiesMap[typeid(ActualT)]};
        }

        /**
         * (Cell::C1)
         * @return number of Entities inside the cell
         */
        std::size_t numEntities() const noexcept {
            return entityAmount;
        }

        /**
         * (Cell::C2)
         * @return number of Entities of type ActualT inside the cell
         */
        template<typename ActualT>
        requires concrete_derived_or_same<ActualT, Entity>
        std::size_t numEntities() const noexcept {
            std::size_t entityAmount = 0;
            const auto &entityMapping = entitiesMap.find(typeid(ActualT));
            if (entityMapping != entitiesMap.end()) {
                entityAmount = entityMapping->second.size();
            }
            return entityAmount;
        }

        /**
         * Iterators begin and end:
         * The Cell would have begin and end iterators for iterating over all pointers to Entity.
         * Retrieved pointers are non-const, i.e. the user can call non-const methods on the retrieved entities.
         * There is no defined order for the iteration. Iteration itself shall not create any copies.
         */
        auto begin() const noexcept {
            if (numEntities() == 0) {
                // just for optimization in case there are no entities inside and getEntitiesView was called
                return end();
            }
            return CellIterator(entitiesMap.begin(), entitiesMap.end());
        }

        auto end() const noexcept {
            return CellIterator(entitiesMap.end(), entitiesMap.end());
        }

        const Coordinates &getCoordinates() const {
            return coordinates;
        }

    private:
        template<typename ActualT>
        requires derived_or_same<ActualT, Entity>
        void addEntity(ActualT &entity) const {
            entitiesMap[typeid(entity)].push_back(&entity);
            entityAmount++;
        }

        /**
         * CellIterator is an iterator that iterates between all entities inside a cell.
         * typeIter represents an iterator on the different entity type containers in the cell (i.e. iterator of Cell::entitiesMap)
         * typeIterEnd represents the end of the typeIter
         * entityIter iterates over the entities themselves. It iterates on a specific type pointed bu typeIter
         * until iterated on all the entities in that specific type. When the end of the current specific type is reached,
         * typeIter is incremented and entityIter is assigned to the begin() of the next type.
         */
        struct CellIterator {
        private:
            friend class Cell;
            typename std::unordered_map<std::type_index, std::vector<Entity *>>::iterator typeIter;
            typename std::unordered_map<std::type_index, std::vector<Entity *>>::iterator typeIterEnd;
            typename std::vector<Entity *>::iterator entityIter;

            /**
             * @param begin iterator that represents the beginning which the typeIter begins with.
             * @param end iterator that represents the end of the typeIter (i.e entitiesMap.end())
             */
            CellIterator(typename std::unordered_map<std::type_index, std::vector<Entity *>>::iterator begin,
                         typename std::unordered_map<std::type_index, std::vector<Entity *>>::iterator end)
                    : typeIter(begin), typeIterEnd(end) {
                while (typeIter != typeIterEnd && (*typeIter).second.begin() == (*typeIter).second.end()) {
                    // while the current type is not empty - may be empty after calling getEntitiesView
                    typeIter++;
                }
                if (typeIter != typeIterEnd) {
                    entityIter = (*typeIter).second.begin();
                }
            }

        public:
            Entity *operator*() {
                return *entityIter;
            }

            CellIterator &operator++() {
                entityIter++;
                while (typeIter != typeIterEnd && entityIter == (*typeIter).second.end()) {
                    // skip empty vector of entities - may be empty after calling getEntitiesView
                    typeIter++; // move to the next type
                    if (typeIter != typeIterEnd) {
                        entityIter = (*typeIter).second.begin();
                    }
                }
                return *this;
            }

            CellIterator operator++(int) {
                auto old = *this;
                ++(*this);
                return old;
            }

            bool operator==(const CellIterator &other) const {
                if (typeIter != other.typeIter) {
                    return false;
                }
                if (typeIter != typeIterEnd && entityIter != other.entityIter) {
                    return false;
                }
                return true; // true if both iterators pointing to the same object or both reached the end
            }

            bool operator!=(const CellIterator &other) const {
                return !operator==(other);
            }
        };
    };

    // private members of Grid:
    std::vector<Cell> rows[num_rows + 1]; // +1 for GridIterator end()
    const Latitude rowLatitudeDiff;
    const Meters rowHeight;
    std::size_t cellsAmount = 0;

public:
    /**
     * @brief constructor for Grid.
     * Allocates array of cell vectors. Each vector represents a row and contains the correct number of cells for that row.
     */
    Grid() : rowLatitudeDiff(DEGREES_180 / num_rows), rowHeight(CoordinatesMath::half_earth_hemisphere * 2 / num_rows) {
        generateCells();
    }

    /**
     *(Grid::A1)
     * Adding Entities to the Grid
     */
    template<typename ActualT>
    requires derived_or_same<ActualT, Entity>
    const Cell *add(Coordinates c, ActualT &e) {
        if (!isValid(c)) {
            throw std::runtime_error("Invalid Coordinates!");
        }
        const Cell *cell = getCellAt(c);
        cell->addEntity(e);
        return cell;
    }


    /**
     * (Grid::B1)
     * @return the Cell that the given coordinates resigns in;
     */
    const Cell *getCellAt(Coordinates c) const {
        if (!isValid(c)) {
            throw std::runtime_error("Invalid Coordinates!");
        }
        std::size_t rowIndex = getRowIndex(c.latitude());
        std::size_t cellIndex = getColIndex(c.longitude(), rowIndex);
        return &(rows[rowIndex][cellIndex]);
    }

    /**
     * (Grid::B2) - BONUS
     * @brief BFS-like function that finds all the cell that are inside the circle represented by the parameters center and radius
     * @param center
     * @param radius
     * @return vector with all the cells inside the circle
     */
    std::vector<const Cell *> getCellsAt(Coordinates center, Meters radius) const {
        if (!isValid(center)) {
            throw std::runtime_error("Invalid Coordinates!");
        }
        std::vector<const Cell *> outputCells;
        std::unordered_set<const Cell *> seenCells;
        std::deque<const Cell *> queue;
        const Cell *initialCell = getCellAt(center);
        queue.push_back(initialCell);
        seenCells.insert(initialCell);
        while (!queue.empty()) {
            const Cell *currCell = queue.front();
            queue.pop_front();
            Meters distance = distanceFromCell(currCell, center);
            if (distance <= radius) {
                outputCells.push_back(currCell);
                std::vector<const Cell *> neighbors = getCellNeighbors(currCell);
                for (auto neighbor: neighbors) {
                    if (!seenCells.contains(neighbor)) {
                        queue.push_back(neighbor);
                        seenCells.insert(neighbor);
                    }
                }
            }
        }
        return outputCells;
    }

    // additional auxiliary functions:

    /**
     * (Grid::C1)
     * @return
     */
    std::size_t numRows() const noexcept {
        return num_rows;
    }

    /**
     * (Grid::C2)
     */
    std::size_t numCols(Coordinates c) const noexcept {
        return rows[getRowIndex(c.latitude())].size();
    }

    /**
     * (Grid::C3)
     */
    std::size_t numCells() const noexcept {
        return cellsAmount;
    }

    static constexpr bool is_sparse = false;

    // Following would iterate over:
    // 1. Only non empty Cells, if is_sparse==true
    // 2. All Cells, if is_sparse==false
    auto begin() const noexcept {
        return GridIterator(rows, 0);
    }

    auto end() const noexcept {
        return GridIterator(rows, num_rows);
    }

protected:    // protected for mocking
    // For mocking! Needed for testing the distance from a cell because cell is private.
    virtual Meters distanceFromCell(Coordinates cellCoord, Coordinates coordinates) const {
        auto cell = getCellAt(cellCoord);
        return distanceFromCell(cell, coordinates);
    }

private:
    /**
     * @brief Generates all the rows and cells inside the Grid
     */
    void generateCells() {
        Meters upperPerimeter(0);
        Latitude upperLatitude = CoordinatesMath::n_pole_lat;
        // the rows are added from north to south
        for (std::size_t row = 0; row < num_rows; row++) {
            Latitude lowerLatitude = upperLatitude - rowLatitudeDiff;
            Meters lowerPerimeter = CoordinatesMath::perimeterOnLatitude(lowerLatitude);
            std::size_t cellsInRow = std::ceil(
                    static_cast<double>((lowerPerimeter + upperPerimeter) / (2 * rowHeight)));

            // mid latitude of the upper and lower boundaries
            Latitude cellMidLatitude = (upperLatitude + lowerLatitude) / 2;
            Longitude colLongitudeDiff(DEGREES_360 / cellsInRow);
            // mid longitude of left and right boundaries of the first cell
            Longitude cellMidLongitude = Longitude(CoordinatesMath::wrap180(static_cast<double>(colLongitudeDiff) / 2));
            for (std::size_t col = 0; col < cellsInRow; col++) {
                // add the mid point of the cell as its coordinates
                rows[row].push_back(Cell(row, col, Coordinates(cellMidLongitude, cellMidLatitude)));
                cellMidLongitude = Longitude(CoordinatesMath::wrap180(
                        static_cast<double>(cellMidLongitude + colLongitudeDiff))); // mid longitude of the next cell
            }
            cellsAmount += cellsInRow;
            upperLatitude = lowerLatitude;
            upperPerimeter = lowerPerimeter;
        }

    }

    std::size_t getRowIndex(Latitude lat) const {
        std::size_t rowIndex = std::floor(
                static_cast<double>(CoordinatesMath::n_pole_lat - lat) / static_cast<double>(rowLatitudeDiff));
        return std::min(rowIndex, num_rows - 1); // if lat == -90 then rowIndex should be the last index
    }

    std::size_t getColIndex(Longitude lng, std::size_t rowIndex) const {
        std::size_t rowSize = rows[rowIndex].size();
        double lngDiff = DEGREES_360 / rowSize;
        double coordsLng = static_cast<double>(lng);
        if (coordsLng < 0) {
            coordsLng += DEGREES_360;
        }
        std::size_t cellIndex = std::floor(coordsLng / lngDiff);
        return cellIndex;
    }

    /**
     * @param cell
     * @return The neighboring cell to the left, right, up and down.
     */
    std::vector<const Cell *> getCellNeighbors(const Cell *cell) const {
        std::size_t rowSize = rows[cell->rowIndex].size();
        auto rightNeighbor = &rows[cell->rowIndex][(cell->colIndex + 1) % rowSize];
        auto leftNeighbor = &rows[cell->rowIndex][(cell->colIndex - 1 + rowSize) % rowSize];
        auto upperNeighbor = getCellAt(
                CoordinatesMath::coordinatesByBearingAndDistance(cell->coordinates, NORTH_ABSOLUTE_BEARING,
                                                                 rowHeight)); // go up!
        auto lowerNeighbor = getCellAt(
                CoordinatesMath::coordinatesByBearingAndDistance(cell->coordinates, SOUTH_ABSOLUTE_BEARING,
                                                                 rowHeight)); // go down!
        std::vector<const Cell *> neighbors{rightNeighbor, leftNeighbor, upperNeighbor, lowerNeighbor};
        return neighbors;
    }

    /**
     * @param cell
     * @param coordinates
     * @return the distance from coordinates to the cell. The distance is calculated by taking the minimal distance
     * between the distances from coordinates to the 4 edges/sides of the cell.
     */
    Meters distanceFromCell(const Cell *cell, Coordinates coordinates) const {
        if (getCellAt(coordinates) == cell) {
            return Meters(0); // coordinates is inside the cell
        }
        Latitude topLat = CoordinatesMath::n_pole_lat - rowLatitudeDiff * cell->rowIndex;
        Latitude bottomLat = CoordinatesMath::n_pole_lat - rowLatitudeDiff * (cell->rowIndex + 1);
        Longitude leftLng(CoordinatesMath::wrap180(cell->colIndex * (DEGREES_360 / rows[cell->rowIndex].size())));
        Longitude rightLng(
                CoordinatesMath::wrap180((cell->colIndex + 1) * (DEGREES_360 / rows[cell->rowIndex].size())));
        Coordinates topLeft(leftLng, topLat);
        Coordinates topRight(rightLng, topLat);
        Coordinates bottomLeft(leftLng, bottomLat);
        Coordinates bottomRight(rightLng, bottomLat);

        Meters distanceFromTop = CoordinatesMath::distanceFromSegment(coordinates, topLeft, topRight);
        Meters distanceFromBottom = CoordinatesMath::distanceFromSegment(coordinates, bottomLeft, bottomRight);
        Meters distanceFromLeft = CoordinatesMath::distanceFromSegment(coordinates, bottomLeft, topLeft);
        Meters distanceFromRight = CoordinatesMath::distanceFromSegment(coordinates, bottomRight, topRight);

        return std::min(std::min<Meters>(distanceFromTop, distanceFromBottom),
                        std::min<Meters>(distanceFromLeft, distanceFromRight));
    }

    /**
     * validtaes that the given coordinates is valid: -180 < longitude <= 180, -90 <= latitude < 90

     */
    bool isValid(const Coordinates &c) const {
        if (Longitude(-180) > c.longitude() || c.longitude() > Longitude(180) || c.latitude() < Latitude(-90) ||
            c.latitude() > Latitude(90)) {
            return false;
        }
        return true;
    }


    /**
     * @brief GridIterator iterates over all the cells in the grid, starting from the first row (the upper row) and ending
     * at the last row (lower row).
     * rows - reference to the rows array Grid.
     * currRowIndex - the current row's index which the iterator iterates over
     * cellIter - iterates over the cells of the row number currRowIndex
     */
    struct GridIterator {
    private:
        friend class Grid<Entity, num_rows>;

        const std::vector<Cell> *rows;
        std::size_t currRowIndex;
        typename std::vector<Grid::Cell>::const_iterator cellIter;

        GridIterator(const std::vector<Cell> *rows, std::size_t rowIndex) : rows(rows), currRowIndex(rowIndex), cellIter(rows[rowIndex].begin()) {}

    public:
        const Cell &operator*() {
            return *cellIter;
        }

        GridIterator &operator++() {
            cellIter++;
            if (cellIter == rows[currRowIndex].end()) {
                cellIter = currRowIndex != num_rows ? rows[++currRowIndex].begin()
                                                    : std::vector<Grid::Cell>().begin();
                // If hasn't reached the last row increment currRowIndex and iterate over the next row,
                // else return an empty iterator to mark the end.
            }
            return *this;
        }

        GridIterator operator++(int) {
            auto old = *this;
            ++(*this);
            return old;
        }

        bool operator==(const GridIterator &other) const {
            return cellIter == other.cellIter;
        }

        bool operator!=(const GridIterator &other) const {
            return !operator==(other);
        }
    };


};



