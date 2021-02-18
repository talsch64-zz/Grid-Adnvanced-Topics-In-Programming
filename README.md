#Advanced Topics In Programming ex4 - Grid Implementation

##Grid
* The Grid contains an array of std::vector<Cell> for storing the Cells. The size of the array is num_rows + 1 and the last element is an empty vector to mark the end of the grid (for iterating the grid).
* The first row is begins in the north pole (latitude 90) and the last row at the south pole (latitude -90)

##Cell
* Each cell has a row index and a column index which represents it's position inside the grid.
* Cell also contains a entitiesMap which maps std::type_index to std::vector<Entity *>>.
* Cell's Coordinates represents it's center.

##CellIterator
* CellIterator is a nested class of Grid::Cell which iterates over the entities inside the cell.
* The CellIterator iterates over all the entities of each type before moving to the next type.
* The CellIterator uses two C++ iterators for iterating over the entities: typeIter and EntityIter.
* typeIter represents an iterator on the different entity type containers in the cell (i.e. iterator of Cell::entitiesMap)
* entityIter iterates over the entities themselves. It iterates on a specific type pointed bu typeIter


##GridIterator 
* GridIterator is a nested class of grid which iterates over all the cells inside the grid, starting from the first row (the upper row) and ending 
at the last row (lower row).
* Holds a reference to the Grid's row array, an index of the current row and a const_iterator which iterates over the current row.
* If the iterator reached the end of the row, current row index is incremented and the const_iterator will point to the beginning of the next row. 

##Bonus Implementation
* The bonus function getCellsAt(Coordinates center, Meters radius) is implemented with BFS-like algorithms using a dequeue.
* The algorithm uses the functions getCellsNeighbors and distanceFromCell for performing the BFS.
* The upper and lower neighbors of a given cell are found by calculating the cell which reside above/below the given cell. The calculation is done by using the function CoordinatesMath::coordinatesByBearingAndDistance(cell->coordinates, ABSOLUTE_BEARING, 
rowHeight), where ABSOLUTE_BEARING is either up or down.
* For measuring distance from a given coordinates to a cell, I implemented the function distanceFromCell which returns the minimal distance between the distances from the coordinates to the cell's 4 edges.

##Tests
###tests.cpp
* Includes tests for the grid and the bonus.
* The Entity classes that are in use in the tests are located in TestClasses.h.
###TestClasses.h
#### Entity classes
* The TestClasses.h file includes simple classes with a polymorphic hierarchy for testing the Grid.

####RandomTest fixture class
* Used for testing the grid with random parameters, entities, predicate functions, etc.
* Bonus testing as well with random parameters.
####EntitiesContainer
* In use by the RandomTest for storing the pointers to the different entities that were created during the random tests. 

###GridMock.h
* Mock class for grid used for testing internal functions as well as the bonus.
* The submission version includes only tests for the bonus. 
