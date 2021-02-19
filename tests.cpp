#include <random>
#include "gtest/gtest.h"
#include "Grid.h"
#include "GridMock.h"
#include "TestClasses.h"


TEST(GISEx4, CellInsertions) {
    Grid<A, 2> grid;
    std::shared_ptr<A> a1 = std::make_shared<A>(1, "A");
    std::shared_ptr<B> b1 = std::make_shared<B>(1, "B");
    std::shared_ptr<C> c1 = std::make_shared<C>(1, "C");
    Coordinates coord0(Longitude(0), Latitude(0));
    Coordinates coord1(Longitude(-1), Latitude(0));
    Coordinates coord2(Coordinates(Longitude(0), Latitude(45)));
    grid.add(coord0, *a1);
    grid.add(coord1, *a1);
    grid.add(coord2, *a1);
    grid.add(coord0, *b1);
    grid.add(coord1, *b1);
    grid.add(coord2, *b1);
    grid.add(coord0, *c1);
    grid.add(coord1, *c1);
    grid.add(coord2, *c1);

    EXPECT_EQ(grid.getCellAt(coord0)->numEntities(), std::size_t(3));
    EXPECT_EQ(grid.getCellAt(coord1)->numEntities(), std::size_t(3));
    EXPECT_EQ(grid.getCellAt(coord2)->numEntities(), std::size_t(3));
    auto cell0 = grid.getCellAt(coord0);
    auto cell1 = grid.getCellAt(coord1);
    auto cell2 = grid.getCellAt(coord2);
    EXPECT_EQ(cell0->numEntities<A>(), (std::size_t) 1);
    EXPECT_EQ(cell0->numEntities<B>(), (std::size_t) 1);
    EXPECT_EQ(cell0->numEntities<C>(), (std::size_t) 1);
    EXPECT_EQ(cell1->numEntities<A>(), (std::size_t) 1);
    EXPECT_EQ(cell1->numEntities<B>(), (std::size_t) 1);
    EXPECT_EQ(cell1->numEntities<C>(), (std::size_t) 1);
    EXPECT_EQ(cell2->numEntities<A>(), (std::size_t) 1);
    EXPECT_EQ(cell2->numEntities<B>(), (std::size_t) 1);
    EXPECT_EQ(cell2->numEntities<C>(), (std::size_t) 1);
    EXPECT_EQ(cell0->getEntities<A>([](A &a) { return a.predTrue; }).size(), (std::size_t) 1);
    EXPECT_EQ(cell0->getEntities<B>([](B &b) { return b.predTrue; }).size(), (std::size_t) 1);
    EXPECT_EQ(cell0->getEntities<C>([](C &c) { return c.predTrue; }).size(), (std::size_t) 1);
    EXPECT_EQ(cell0->getEntities([](A &a) { return a.predTrue; }).size(), (std::size_t) 3);
}

TEST(GISEx4, getEntitiesWithAndWithoutLimit) {
    Grid<A, 10> grid;
    std::shared_ptr<A> a1 = std::make_shared<A>((std::size_t) 1, "A");
    std::shared_ptr<B> b1 = std::make_shared<B>((std::size_t) 1, "B");
    std::shared_ptr<C> c1 = std::make_shared<C>((std::size_t) 1, "C");
    Coordinates coord0(Longitude(0), Latitude(0));
    grid.add(coord0, *a1);
    grid.add(coord0, *b1);
    grid.add(coord0, *c1);
    grid.add(coord0, *a1);
    grid.add(coord0, *b1);
    grid.add(coord0, *c1);
    grid.add(coord0, *a1);
    grid.add(coord0, *b1);
    grid.add(coord0, *c1);
    EXPECT_EQ(grid.getCellAt(coord0)->numEntities(), (std::size_t) 9);
    auto cell0 = grid.getCellAt(coord0);
    EXPECT_EQ(cell0->numEntities<A>(), (std::size_t) 3);
    EXPECT_EQ(cell0->numEntities<B>(), (std::size_t) 3);
    EXPECT_EQ(cell0->numEntities<C>(), (std::size_t) 3);
    EXPECT_EQ(cell0->getEntities([](O &o) { return o.predTrue; }).size(), (std::size_t) 9);
    EXPECT_EQ(cell0->getEntities([](A &a) { return a.predTrue; }).size(), (std::size_t) 9);


    // ---------- without limit ----------
    EXPECT_EQ(cell0->getEntities<A>([](A &a) { return a.predTrue; }).size(), (std::size_t) 3);
    EXPECT_EQ(cell0->getEntities<B>([](A &a) { return a.predTrue; }).size(), (std::size_t) 3);
    EXPECT_EQ(cell0->getEntities<C>([](A &a) { return a.predTrue; }).size(), (std::size_t) 3);

    // ---------- with limit ----------
    EXPECT_EQ(cell0->getEntities<A>([](A &a) { return a.predTrue; }, 1).size(), (std::size_t) 1);
    EXPECT_EQ(cell0->getEntities<B>([](B &b) { return b.predTrue; }, 2).size(), (std::size_t) 2);
    EXPECT_EQ(cell0->getEntities<C>([](C &c) { return c.predTrue; }, 3).size(), (std::size_t) 3);
    EXPECT_EQ(cell0->getEntities<B>([](B &b) { return b.predTrue; }, 0).size(), (std::size_t) 0);
    EXPECT_EQ(cell0->getEntities<B>([](B &b) { return b.predTrue; }, 10).size(), (std::size_t) 3);

}

/**
 * O is an abstract class
 * A, B, C are concrete and derived from O
 */
TEST(GISEx4, getEntitiesWithAbstractPredicate) {
    Grid<O, 18> grid;
    Coordinates coord1(Longitude(0), Latitude(10));
    std::shared_ptr<A> a1 = std::make_shared<A>(1, "A");
    std::shared_ptr<A> a2 = std::make_shared<A>(1, "A");
    std::shared_ptr<A> a3 = std::make_shared<A>(1, "A");
    std::shared_ptr<A> a4 = std::make_shared<A>(2, "A");
    std::shared_ptr<A> a5 = std::make_shared<A>(2, "A");
    std::shared_ptr<B> b1 = std::make_shared<B>(1, "B");
    std::shared_ptr<B> b2 = std::make_shared<B>(2, "B");
    std::shared_ptr<C> c1 = std::make_shared<C>(11, "C");
    std::shared_ptr<C> c2 = std::make_shared<C>(21, "C");
    std::shared_ptr<O> o1 = std::make_shared<A>(1, "OA");
    std::shared_ptr<O> o2 = std::make_shared<B>(10, "OB");
    std::shared_ptr<O> o3 = std::make_shared<C>(10, "OC");
    const auto cell = grid.getCellAt(coord1);
    grid.add(coord1, *a1);
    grid.add(coord1, *a2);
    grid.add(coord1, *a3);
    grid.add(coord1, *a4);
    grid.add(coord1, *a5);
    grid.add(coord1, *b1);
    grid.add(coord1, *b2);
    grid.add(coord1, *c1);
    grid.add(coord1, *c2);
    grid.add(coord1, *o1);
    grid.add(coord1, *o2);
    grid.add(coord1, *o3);

    // ---------- without limit ----------
    EXPECT_EQ(cell->getEntities<A>([](O &o) { return o.id == 1; }).size(), (std::size_t) 4); // a1, a2, a3, o1
    EXPECT_EQ(cell->getEntities<B>([](O &o) { return o.id > 0; }).size(), (std::size_t) 3); // b1, b2, o2
    EXPECT_EQ(cell->getEntities<C>([](O &o) { return o.id > 10; }).size(), (std::size_t) 2); // c1, c2
    EXPECT_EQ(cell->getEntities([](O &o) { return o.predTrue; }).size(), (std::size_t) 12);

    // ---------- with limit ----------

    EXPECT_EQ(cell->getEntities<A>([](O &o) { return o.id == 1; }, 3).size(), (std::size_t) 3); // a1, a2, a3
    EXPECT_EQ(cell->getEntities<B>([](O &o) { return o.id > 0; }, 1).size(), (std::size_t) 1); // b1 or b2
    EXPECT_EQ(cell->getEntities<C>([](O &o) { return o.id > 10; }, 0).size(), (std::size_t) 0); // none
    EXPECT_EQ(cell->getEntities<C>([](O &o) { return o.id > 10; }, 10).size(), (std::size_t) 2); // c1, c2

}

TEST(GISEx4, ViewTest) {
    Grid<O, 18> grid;
    Coordinates coord1(Longitude(0), Latitude(10));
    std::shared_ptr<A> a1 = std::make_shared<A>(1, "A");
    std::shared_ptr<A> a2 = std::make_shared<A>(1, "A");
    std::shared_ptr<A> a3 = std::make_shared<A>(1, "A");
    std::shared_ptr<A> a4 = std::make_shared<A>(2, "A");
    std::shared_ptr<A> a5 = std::make_shared<A>(2, "A");
    std::shared_ptr<B> b1 = std::make_shared<B>(1, "B");
    std::shared_ptr<B> b2 = std::make_shared<B>(2, "B");
    std::shared_ptr<C> c1 = std::make_shared<C>(1, "C");
    std::shared_ptr<C> c2 = std::make_shared<C>(2, "C");
    std::shared_ptr<O> o1 = std::make_shared<A>(1, "OA");
    std::shared_ptr<O> o2 = std::make_shared<B>(2, "OB");
    std::shared_ptr<O> o3 = std::make_shared<C>(3, "OC");

    const auto cell = grid.getCellAt(coord1);
    const auto &viewA = cell->getEntitiesView<A>();
    const auto &viewB = cell->getEntitiesView<B>();
    const auto &viewC = cell->getEntitiesView<C>();
    EXPECT_EQ(viewA.size(), (std::size_t) 0);
    EXPECT_EQ(viewB.size(), (std::size_t) 0);
    EXPECT_EQ(viewC.size(), (std::size_t) 0);

    grid.add(coord1, *a1);
    grid.add(coord1, *a2);
    grid.add(coord1, *a3);
    grid.add(coord1, *a4);
    grid.add(coord1, *a5);
    grid.add(coord1, *b1);
    grid.add(coord1, *b2);
    grid.add(coord1, *c1);
    grid.add(coord1, *c2);
    grid.add(coord1, *o1);
    grid.add(coord1, *o2);
    grid.add(coord1, *o3);

    EXPECT_EQ(viewA.size(), (std::size_t) 6);
    EXPECT_EQ(viewB.size(), (std::size_t) 3);
    EXPECT_EQ(viewC.size(), (std::size_t) 3);
    EXPECT_EQ(cell->numEntities<A>(), (std::size_t) 6);
    EXPECT_EQ(cell->numEntities<B>(), (std::size_t) 3);
    EXPECT_EQ(cell->numEntities<C>(), (std::size_t) 3);
    EXPECT_EQ(cell->numEntities(), (std::size_t) 12);
}


/**
 * Tests the basic functionality of CellIterator: ++, *, == operators and begin() and end() functions
 */
TEST(GISEx4, CellIterator) {
    Grid<A, 18> grid;
    Coordinates coord0(Longitude(0), Latitude(0));
    auto cell = grid.getCellAt(coord0);
    std::shared_ptr<A> a1 = std::make_shared<A>(1, "A");
    grid.add(coord0, *a1);
    grid.add(coord0, *a1);
    grid.add(coord0, *a1);
    grid.add(coord0, *a1);
    grid.add(coord0, *a1);
    grid.add(coord0, *a1);
    EXPECT_NE(cell->begin(), cell->end());
    auto cellIter = cell->begin();
    A *a = *cellIter;
    a->whatA(); //non-const method is allowed to be called
    EXPECT_EQ(typeid(*cellIter), typeid(A *));

    // ---------- test the ++ operators ----------
    std::size_t counter = 0;
    while (cellIter != cell->end()) {
        counter++;
        cellIter++;
    }
    EXPECT_EQ(counter, cell->numEntities());
    auto cellIter2 = cell->begin();
    auto beginCopy = cellIter2++;
    auto &cellIter3 = ++cellIter2;
    EXPECT_EQ(beginCopy, cell->begin());
    EXPECT_EQ(cellIter3, cellIter2);

    // ---------- test empty cell ----------
    auto emptyCell = grid.getCellAt(Coordinates(Longitude(100), Latitude(20)));
    EXPECT_EQ(emptyCell->begin(), emptyCell->end());
}

/**
 * Tests that the CellIterator functions properly after getEntitiesView was called.
 * I created 3 different cells and 3 entities of different type and inserted each cell different entity
 * so that the order of the iterations would be different in each cell.
 */
TEST(GISEx4, CellIteratorAfterView) {
    Grid<A, 18> grid;
    Coordinates coord0(Longitude(0), Latitude(0));
    Coordinates coord1(Longitude(50), Latitude(50));
    Coordinates coord2(Longitude(-50), Latitude(-50));

    std::shared_ptr<A> a1 = std::make_shared<A>(1, "A");
    std::shared_ptr<B> b1 = std::make_shared<B>(1, "B");
    std::shared_ptr<C> c1 = std::make_shared<C>(1, "C");

    auto cell0 = grid.getCellAt(coord0);
    auto cell1 = grid.getCellAt(coord1);
    auto cell2 = grid.getCellAt(coord2);

    // initialize empty vector of entities inside the cells:
    cell0->getEntitiesView<A>();
    cell0->getEntitiesView<B>();
    cell0->getEntitiesView<C>();
    cell1->getEntitiesView<A>();
    cell1->getEntitiesView<B>();
    cell1->getEntitiesView<C>();
    cell2->getEntitiesView<A>();
    cell2->getEntitiesView<B>();
    cell2->getEntitiesView<C>();

    // no entities were added yet:
    EXPECT_EQ(cell0->begin(), cell0->end());
    EXPECT_EQ(cell1->begin(), cell1->end());
    EXPECT_EQ(cell2->begin(), cell2->end());

    // add different entity to different cells to test different orders of iterations:
    grid.add(coord0, *a1);
    grid.add(coord1, *b1);
    grid.add(coord2, *c1);

    auto iter0 = cell0->begin();
    for (std::size_t i = 0; i < cell0->numEntities(); i++) {
        *iter0;
        iter0++;
    }
    EXPECT_EQ(iter0, cell0->end());

    auto iter1 = cell1->begin();
    for (std::size_t i = 0; i < cell1->numEntities(); i++) {
        *iter1;
        iter1++;
    }
    EXPECT_EQ(iter1, cell1->end());

    auto iter2 = cell2->begin();
    for (std::size_t i = 0; i < cell2->numEntities(); i++) {
        *iter2;
        iter2++;
    }
    EXPECT_EQ(iter2, cell2->end());
}

TEST(GISEx4, GridIterator) {
    Grid<A, 18> grid;
    auto gridIter = grid.begin();
    std::size_t counter = 0;
    while (gridIter != grid.end()) {
        counter++;
        gridIter++;
    }
    EXPECT_EQ(counter, grid.numCells());
    gridIter = grid.begin();
    auto &cell = *gridIter;
    cell.begin();
    EXPECT_EQ(cell.numEntities(), (std::size_t) 0);
    auto beginCopy = gridIter++; // beginCopy should point to grid.begin()
    auto &IterCopy = ++gridIter; // IterCopy should point to gridIter
    EXPECT_EQ(beginCopy, grid.begin());
    EXPECT_EQ(IterCopy, gridIter);
}

TEST(GISEx4, RowsAndColsCalculations) {
    Grid<O, 18> grid;
    Coordinates coord0(Longitude(0), Latitude(0));
    Coordinates coord1(Longitude(0), Latitude(10));
    Coordinates coord2(Longitude(0), Latitude(20));
    Coordinates coord3(Longitude(0), Latitude(90));
    Coordinates coord4(Longitude(0), Latitude(-10));
    Coordinates coord5(Longitude(0), Latitude(-90));
    Coordinates coord6(Longitude(10), Latitude(0));
    Coordinates coord7(Longitude(5), Latitude(0));
    Coordinates coord8(Longitude(-5), Latitude(0));
    Coordinates coord9(Longitude(-10), Latitude(0));
    Coordinates coord10(Longitude(-50), Latitude(0));
    Coordinates coord11(Longitude(-180), Latitude(0));
    Coordinates coord12(Longitude(180), Latitude(0));
    EXPECT_EQ(grid.numRows(), (std::size_t) 18);
    EXPECT_EQ(grid.numCols(coord1), (std::size_t) 36);
    EXPECT_EQ(grid.numCols(coord3), (std::size_t) 4); // north poll
    EXPECT_EQ(grid.numCols(coord5), (std::size_t) 4); // south poll


    // Internal testing:
//    EXPECT_EQ(grid.getRowIndex(coord0.latitude()), 9); // 10'th row
//    EXPECT_EQ(grid.getRowIndex(coord1.latitude()), 8); // 9'th row
//    EXPECT_EQ(grid.getRowIndex(coord2.latitude()), 7); // 8'th row
//    EXPECT_EQ(grid.getRowIndex(coord3.latitude()), 0); // 7'th row
//    EXPECT_EQ(grid.getRowIndex(coord4.latitude()), 10); // 11'th row
//    EXPECT_EQ(grid.getRowIndex(coord5.latitude()), 17); // 11'th row
//    EXPECT_EQ(grid.getColIndex(coord0.longitude(), grid.getRowIndex(coord0.latitude())), 0);
//    EXPECT_EQ(grid.getColIndex(coord7.longitude(), grid.getRowIndex(coord7.latitude())), 0);
//    EXPECT_EQ(grid.getColIndex(coord6.longitude(), grid.getRowIndex(coord6.latitude())), 1);
//    EXPECT_EQ(grid.getColIndex(coord8.longitude(), grid.getRowIndex(coord8.latitude())), 35);
//    EXPECT_EQ(grid.getColIndex(coord9.longitude(), grid.getRowIndex(coord9.latitude())), 35);
//    EXPECT_EQ(grid.getColIndex(coord10.longitude(), grid.getRowIndex(coord10.latitude())), 31);
//    EXPECT_EQ(grid.getColIndex(coord11.longitude(), grid.getRowIndex(coord11.latitude())), 18);
//    EXPECT_EQ(grid.getColIndex(coord12.longitude(), grid.getRowIndex(coord12.latitude())), 18);

    Grid<O, 25> grid2;
    EXPECT_EQ(grid2.numRows(), (std::size_t) 25);
    EXPECT_EQ(grid2.numCols(coord0), (std::size_t) 50);
    EXPECT_EQ(grid2.numCols(coord3), (std::size_t) 4); // north poll
    EXPECT_EQ(grid2.numCols(coord5), (std::size_t) 4); // south poll

    Grid<O, 33> grid3;
    EXPECT_EQ(grid3.numRows(), (std::size_t) 33);
    EXPECT_EQ(grid3.numCols(coord0), (std::size_t) 66);
    EXPECT_EQ(grid3.numCols(coord3), (std::size_t) 4); // north poll
    EXPECT_EQ(grid3.numCols(coord5), (std::size_t) 4); // south poll

    Grid<O, 2> grid4;
    EXPECT_EQ(grid4.numRows(), (std::size_t) 2);
    EXPECT_EQ(grid4.numCols(coord0), (std::size_t) 2);
    EXPECT_EQ(grid4.numCols(coord3), (std::size_t) 2); // north poll
    EXPECT_EQ(grid4.numCols(coord5), (std::size_t) 2); // south poll

    Grid<O, 1> grid5;
    EXPECT_EQ(grid5.numRows(), (std::size_t) 1);
    EXPECT_EQ(grid5.numCols(coord0), (std::size_t) 1);
    EXPECT_EQ(grid5.numCols(coord3), (std::size_t) 1); // north poll
    EXPECT_EQ(grid5.numCols(coord5), (std::size_t) 1); // south poll
}

TEST(GISEx4, BasicFunctionality) {
    Grid<A, 2> grid;
    std::shared_ptr<A> a1 = std::make_shared<A>(1, "A");
    std::shared_ptr<A> a2 = std::make_shared<A>(2, "A");
    std::shared_ptr<A> a3 = std::make_shared<A>(3, "A");
    std::shared_ptr<A> a4 = std::make_shared<A>(4, "A");
    std::shared_ptr<A> a5 = std::make_shared<A>(5, "A");
    std::shared_ptr<B> b1 = std::make_shared<B>(1, "B");
    std::shared_ptr<B> b2 = std::make_shared<B>(2, "B");
    std::shared_ptr<C> c1 = std::make_shared<C>(1, "C");
    std::shared_ptr<C> c2 = std::make_shared<C>(2, "C");
    grid.add(Coordinates(Longitude(0), Latitude(0)), *a1);
    grid.add(Coordinates(Longitude(0), Latitude(45)), *a2);
    grid.add(Coordinates(Longitude(-1), Latitude(0)), *a3);

    const auto &cell1 = grid.getCellAt(Coordinates(Longitude(0), Latitude(0)));
    const auto &cell3 = grid.getCellAt(Coordinates(Longitude(-1), Latitude(45)));
    std::vector<A *> entities = cell1->getEntities<A, std::function<bool(A &)>>([](A &a) { return a.predTrue; });

    EXPECT_EQ(entities.size(), (std::size_t) 1);
    EXPECT_EQ(entities[0]->id, a1->id);

    const auto &view1 = cell1->getEntitiesView<A>();
    auto viewIter1 = view1.begin();
    EXPECT_EQ((*viewIter1)->id, 1);
    EXPECT_EQ(typeid(*viewIter1), typeid(A *));
    EXPECT_EQ(view1.size(), (std::size_t) 1);
    grid.add(Coordinates(Longitude(0), Latitude(0)), *a4);
    grid.add(Coordinates(Longitude(0), Latitude(0)), *a5);
    EXPECT_EQ(view1.size(), (std::size_t) 3);
    viewIter1 = view1.begin();
    EXPECT_EQ((*viewIter1)->id, 1);
    ++viewIter1;
    EXPECT_EQ((*viewIter1)->id, 4);
    viewIter1++;
    EXPECT_EQ((*viewIter1)->id, 5);
    EXPECT_EQ(++viewIter1, view1.end());

    const auto &view3 = cell3->getEntitiesView<A>();
    EXPECT_EQ(view3.size(), (std::size_t) 0);
    auto viewIter3 = view3.begin();
    viewIter3++;
    EXPECT_EQ(view3.begin(), view3.end());

    EXPECT_EQ(grid.numRows(), (std::size_t) 2);
    EXPECT_EQ(grid.numCols(Coordinates(Longitude(0), Latitude(0))), (std::size_t) 2);
    EXPECT_EQ(grid.numCells(), (std::size_t) 4);

    grid.add(Coordinates(Longitude(0), Latitude(0)), *b1);
    grid.add(Coordinates(Longitude(0), Latitude(45)), *b2);
    grid.add(Coordinates(Longitude(-1), Latitude(45)), *c1);
    grid.add(Coordinates(Longitude(-1), Latitude(0)), *c2);

    // ============== test limits ==============

    auto entitiesWithoutLimit = cell1->getEntities<A>([](A &a) { return a.predTrue; });
    EXPECT_EQ(entitiesWithoutLimit.size(), (std::size_t) 3); // 3 a's 1 b
    auto entitiesWithLimit = cell1->getEntities<A>([](A &a) { return a.predTrue; }, 2);
    EXPECT_EQ(entitiesWithLimit.size(), (std::size_t) 2);

    auto B_entitiesWithLimit = cell1->getEntities<B>([](A &a) { return a.predTrue; }, 1);
    EXPECT_EQ(B_entitiesWithLimit.size(), (std::size_t) 1);

    auto B_entitiesWithLimit2 = cell1->getEntities<B>([](A &b) { return b.predTrue; }, 1);
    EXPECT_EQ(B_entitiesWithLimit2.size(), (std::size_t) 1);

    auto emptyEntities = cell1->getEntities<A>([](A &a) { return a.predFalse; }, 1);
    EXPECT_EQ(emptyEntities.size(), (std::size_t) 0);
}

/**
 * Tests Invalid coordinates queries.
 */
TEST(GISEx4, InvalidCoordinates) {
    Grid<A, 101> grid;
    bool res = false;
    try { grid.getCellAt({Longitude(190), Latitude(0)}); }
    catch (std::runtime_error &e) { res = true; }
    EXPECT_TRUE(res);
    res = false;
    try { grid.getCellsAt({Longitude(-190), Latitude(0)}, Meters(2000)); }
    catch (std::runtime_error &e) { res = true; }
    EXPECT_TRUE(res);
    res = false;
    A a(1);
    try { grid.add({Longitude(0), Latitude(100)}, a); }
    catch (std::runtime_error &e) { res = true; }
    EXPECT_TRUE(res);
    res = false;
    try { grid.add({Longitude(0), Latitude(-100)}, a); }
    catch (std::runtime_error &e) { res = true; }
    EXPECT_TRUE(res);
}

TEST(GISEx4, DocTest) {
    struct A {
        virtual ~A() {}

        virtual void foo() const = 0;
    };
    struct B : A {
        void foo() const override {}
    };
    struct C : A {
        void foo() const override {}
    };
    struct D : C {
        void foodie() const {}
    };

    Coordinates coord{Longitude{20}, Latitude{30}};
    Grid<A, 1917> grid;
    const auto cell_ptr = grid.getCellAt(coord);
    auto viewD = cell_ptr->getEntitiesView<D>(); // O(1), always, even for size == N
    EXPECT_EQ(viewD.size(), 0ul); // assume passed successfully

    // adding a D object to the Grid, such that it should be added to the cell at coord
    D dees[4];
    const auto cell_ptr2 = grid.add(coord, dees[0]);
    EXPECT_EQ(viewD.size(), 1ul); // the new object shall be in the view
    EXPECT_EQ(cell_ptr2, cell_ptr);

    // adding 3 D objects to the Grid, such that it should be added to the cell at coord
    for (int i = 1; i < 4; ++i) {
        EXPECT_EQ(grid.add(coord, dees[i]), cell_ptr);
    }

    EXPECT_EQ(viewD.size(), 4ul); // the new objects shall be in the view

    int i = 0;
    for (D *pd: viewD) { // correct type
        pd->foodie();
        EXPECT_EQ(pd, &dees[i]);
        ++i;
    }
}

TEST_F(RandomTest, randomTest) {
    constexpr int gridSize = 600;
    int entityAmount = 1000;
    int coordinatesAmount = 50;
    Grid<A, gridSize> grid;
    EntitiesContainer<A> container; // container to store references to the entities
    std::vector<Coordinates> coordVec;
    for (int i = 0; i < coordinatesAmount; i++) {
        coordVec.push_back(randCoordinates());
    }
    for (int i = 0; i < entityAmount; i++) {
        int type = std::rand() % 3;
        int coordIndex = std::rand() % coordinatesAmount;
        int id = std::rand() % 100;
        std::shared_ptr<A> entity;
        switch (type) {
            case 0:
                entity = std::make_shared<A>(id);
                grid.add(coordVec[coordIndex], *dynamic_cast<A *>(entity.get()));
                container.addEntity(entity, 0);
                break;
            case 1:
                entity = std::make_shared<B>(id);
                grid.add(coordVec[coordIndex], *dynamic_cast<B *>(entity.get()));
                container.addEntity(entity, 1);
                break;
            case 2:
                entity = std::make_shared<C>(id);
                grid.add(coordVec[coordIndex], *dynamic_cast<C *>(entity.get()));
                container.addEntity(entity, 2);
                break;
        }
    }
    int queries = 100;
    for (int i = 0; i < queries; i++) {
        int randCoordIndex = std::rand() % coordinatesAmount;
        std::size_t randLimit = std::rand() % 10;
        int randId = std::rand() % 100;
        auto cell = grid.getCellAt(coordVec[randCoordIndex]);
        auto entitiesA = cell->getEntities<A>([randId](A &a) { return a.id < randId; }, randLimit);
        EXPECT_LE(entitiesA.size(), randLimit);
        for (auto entity: entitiesA) {
            EXPECT_LT(entity->id, randId);
            entity->whatA();
        }
        auto entitiesB = cell->getEntities<B>([randId](B &b) { return b.id < randId; }, randLimit);
        EXPECT_LE(entitiesB.size(), randLimit);
        for (auto entity: entitiesB) {
            EXPECT_LT(entity->id, randId);
            entity->whatB();
        }
        auto entitiesC = cell->getEntities<C>([randId](C &c) { return c.id < randId; }, randLimit);
        EXPECT_LE(entitiesC.size(), randLimit);
        for (auto entity: entitiesC) {
            EXPECT_LT(entity->id, randId);
            entity->whatC();
        }
        auto viewA = cell->getEntitiesView<A>();
        EXPECT_EQ(viewA.size(), cell->numEntities<A>());
        auto viewB = cell->getEntitiesView<B>();
        EXPECT_EQ(viewB.size(), cell->numEntities<B>());
        auto viewC = cell->getEntitiesView<C>();
        EXPECT_EQ(viewC.size(), cell->numEntities<C>());
    }
}

TEST_F(RandomTest, bonusRandTest) {
    constexpr int gridSize = 200;
    int searches = 10;
    for (int i = 0; i < searches; i++) {
        GridMock<A, gridSize> grid;
        Coordinates center = randCoordinates();
        Meters radius(fRand(100, 10000000));
        auto cells = grid.getCellsAt(center, radius);
        for (auto &cell: cells) {
            auto distance = grid.distanceFromCell(cell->getCoordinates(), center);
            EXPECT_LE(distance, radius);
        }
    }
}

TEST(GISEx4, BonusGetCellsAt) {
    Coordinates c0(Longitude(0), Latitude(0));
    Coordinates c1(Longitude(0), Latitude(90));
    Coordinates c2(Longitude(12), Latitude(15));
    Coordinates c3(Longitude(0), Latitude(-90));
    Coordinates c4(Longitude(12), Latitude(-15));

    GridMock<A, 18> grid;
    auto cells = grid.getCellsAt(c0, Meters(10)); // expected 4 cells
    EXPECT_EQ(cells.size(), (std::size_t) 4);
    for (auto cell: cells) {
        EXPECT_LE(grid.distanceFromCell(cell->getCoordinates(), c0), Meters(0));
    }
    cells = grid.getCellsAt(c2, Meters(10)); // expected 4 cells
    EXPECT_EQ(cells.size(), (std::size_t) 1);
    cells = grid.getCellsAt(c0, Meters(5000000));
    for (auto cell: cells) {
        EXPECT_LE(grid.distanceFromCell(cell->getCoordinates(), c0), Meters(5000000));
    }

    GridMock<A, 36> grid2;
    auto cells2 = grid2.getCellsAt(c1, Meters(0));
    EXPECT_EQ(cells2.size(), (std::size_t) 4);
    cells2 = grid2.getCellsAt(c1, Meters(5000000));
    for (auto cell: cells2) {
        EXPECT_LE(grid2.distanceFromCell(cell->getCoordinates(), c1), Meters(5000000));
    }

    GridMock<A, 2> grid3;
    auto cells3 = grid3.getCellsAt(c2, Meters(0));
    EXPECT_EQ(cells3.size(), (std::size_t) 1);
    cells3 = grid3.getCellsAt(c0, Meters(0));
    EXPECT_EQ(cells3.size(), (std::size_t) 4);
    cells3 = grid3.getCellsAt(c1, Meters(0));
    EXPECT_EQ(cells3.size(), (std::size_t) 2);
    cells3 = grid3.getCellsAt(c2, Meters(2000000));
    for (auto cell: cells3) {
        EXPECT_LE(grid3.distanceFromCell(cell->getCoordinates(), c2), Meters(2000000));
    }
//
    EXPECT_EQ(grid3.getCellsAt(c1, Meters(10000)).size(), grid3.getCellsAt(c3, Meters(10000)).size()); // symmetric
    EXPECT_EQ(grid3.getCellsAt(c2, Meters(10000)).size(), grid3.getCellsAt(c4, Meters(10000)).size()); // symmetric

}

TEST(GISEx4, BonusZeroDistance) {
    Coordinates c0(Longitude(0), Latitude(0));
    Coordinates c1(Longitude(0), Latitude(90));
    Coordinates c2(Longitude(12), Latitude(15));
    Coordinates c3(Longitude(1), Latitude(0));
    Coordinates c4(Longitude(0), Latitude(1));

    GridMock<A, 36> grid;
    auto cells = grid.getCellsAt(c0, Meters(0));
    EXPECT_EQ(cells.size(), (std::size_t) 4); // c0 is placed in the border of 4 cells
    for (auto cell: cells) {
        EXPECT_EQ(grid.distanceFromCell(cell->getCoordinates(), c0), Meters(0));
    }

    cells = grid.getCellsAt(c1, Meters(0));
    EXPECT_EQ(cells.size(), (std::size_t) 4); // c1 is placed in the border of 4 cells
    for (auto cell: cells) {
        EXPECT_EQ(grid.distanceFromCell(cell->getCoordinates(), c1), Meters(0));
    }

    cells = grid.getCellsAt(c2, Meters(0));
    EXPECT_EQ(cells.size(), (std::size_t) 1); // c2 is placed inside a cell
    for (auto cell: cells) {
        EXPECT_EQ(grid.distanceFromCell(cell->getCoordinates(), c2), Meters(0));
    }

    cells = grid.getCellsAt(c3, Meters(0));
    EXPECT_EQ(cells.size(), (std::size_t) 2); // c3 is placed in the border of 2 cells (up and down neighbors)
    for (auto cell: cells) {
        EXPECT_EQ(grid.distanceFromCell(cell->getCoordinates(), c3), Meters(0));
    }

    cells = grid.getCellsAt(c4, Meters(0));
    EXPECT_EQ(cells.size(), (std::size_t) 2); // c4 is placed in the border of 2 cells (left and right neighbors)
    for (auto cell: cells) {
        EXPECT_EQ(grid.distanceFromCell(cell->getCoordinates(), c4), Meters(0));
    }

    GridMock<A, 2> grid2;
    auto cells2 = grid2.getCellsAt(c2, Meters(0));
    EXPECT_EQ(cells2.size(), (std::size_t) 1); // c2 is inside a cell
    cells2 = grid2.getCellsAt(c0, Meters(0));
    EXPECT_EQ(cells2.size(), (std::size_t) 4); // c0 is placed in the border of 4 cells
    cells2 = grid2.getCellsAt(c1, Meters(0));
    EXPECT_EQ(cells2.size(), (std::size_t) 2); // c1 is placed in the border of 4 cells

}


