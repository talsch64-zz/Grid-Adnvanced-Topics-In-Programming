#include "gtest/gtest.h"
#include "../Grid.h"
#include "MockClasses.h"

class GridTest : public ::testing::Test {

public:
    void SetUp() override {
    }

    void TearDown() override {

    }
};


TEST_F(GridTest, numColsPositiveTest) {
    Grid<int, 1000> grid;
    Coordinates coord(Longitude(31.434), Latitude(56.14));

    int numCols = grid.numCols(coord);

    // column height=0.18, 20015.09 meters
    // row=188
    // top lat=56.16, bottom lat=55.98
    // top perimeter=22291827.6, bottom perimeter=22396171.91, avg=22343999.75
    int expectedColsAmount = 1117;
    EXPECT_EQ(numCols, expectedColsAmount);
}

TEST_F(GridTest, typeSeparationTest) {
    auto func = [](const A &a) { return true; };
    Grid<A, 1000> grid;
    Coordinates coord(Longitude(87.1332), Latitude(-2.678));
    std::vector<std::unique_ptr<B>> bEntities;
    std::vector<std::unique_ptr<C>> cEntities;
    for (int i = 0; i < 2; i++) {
        bEntities.push_back(std::make_unique<B>());
    }
    for (int i = 0; i < 3; i++) {
        cEntities.push_back(std::make_unique<C>());
    }

    for (auto &b : bEntities) {
        grid.add<B>(coord, *b);
    }
    for (auto &c : cEntities) {
        grid.add<C>(coord, *c);
    }

    auto cell = grid.getCellAt(coord);
    std::vector<B *> foundBEntities = cell->getEntities<B>(func);
    std::vector<C *> foundCEntities = cell->getEntities<C>(func);
    EXPECT_EQ(foundBEntities.size(), bEntities.size());
    EXPECT_EQ(foundCEntities.size(), cEntities.size());
    for (std::size_t i = 0; i < bEntities.size(); i++) {
        EXPECT_EQ(foundBEntities[i], bEntities[i].get());
    }
    for (std::size_t i = 0; i < cEntities.size(); i++) {
        EXPECT_EQ(foundCEntities[i], cEntities[i].get());
    }
}

TEST_F(GridTest, differentCellsAddTest) {
    auto func = [](const A &a) { return true; };
    Grid<A, 1500> grid;
    Coordinates coord1(Longitude(-57.12), Latitude(67.9945));
    Coordinates coord2(Longitude(-57.12), Latitude(89.935));
    Coordinates coord3(Longitude(-109.15), Latitude(67.9945));
    std::vector<std::unique_ptr<B>> entities1;
    entities1.emplace_back(std::make_unique<B>());
    entities1.emplace_back(std::make_unique<B>());
    std::vector<std::unique_ptr<B>> entities2;
    entities2.emplace_back(std::make_unique<B>());
    std::vector<std::unique_ptr<C>> entities3;
    entities3.emplace_back(std::make_unique<C>());

    grid.add<B>(coord1, *entities1[0]);
    grid.add<B>(coord2, *entities2[0]);
    grid.add<C>(coord3, *entities3[0]);
    grid.add<B>(coord1, *entities1[1]);

    auto cell1 = grid.getCellAt(coord1);
    auto cell2 = grid.getCellAt(coord2);
    auto cell3 = grid.getCellAt(coord3);
    std::vector<B *> foundEntities1 = cell1->getEntities<B>(func);
    std::vector<A *> foundEntities2 = cell2->getEntities(func);
    std::vector<A *> foundEntities3 = cell3->getEntities(func);
    EXPECT_EQ(foundEntities1.size(), entities1.size());
    EXPECT_EQ(foundEntities2.size(), entities2.size());
    EXPECT_EQ(foundEntities3.size(), entities3.size());
    for (int i = 0; i < entities1.size(); i++) {
        EXPECT_EQ(entities1[i].get(), foundEntities1[i]);
    }
    for (int i = 0; i < entities2.size(); i++) {
        EXPECT_EQ(entities2[i].get(), foundEntities2[i]);
    }
    for (int i = 0; i < entities3.size(); i++) {
        EXPECT_EQ(entities3[i].get(), foundEntities3[i]);
    }
}

TEST_F(GridTest, cellEntitiesViewTest) {
    Grid<A, 1500> grid;
    Coordinates coord(Longitude(-57.12), Latitude(67.9945));
    std::vector<std::unique_ptr<A>> entitiesVec;
    std::unordered_map<A *, bool> entities;
    for (int i = 0; i < 2; i++) {
        std::unique_ptr<B> entity = std::make_unique<B>();
        grid.add<B>(coord, *entity);
        entitiesVec.push_back(std::move(entity));
    }
    for (int i = 0; i < 5; i++) {
        std::unique_ptr<C> entity = std::make_unique<C>();
        grid.add<C>(coord, *entity);
        entities[entity.get()] = false;
        entitiesVec.push_back(std::move(entity));
    }

    auto cell = grid.getCellAt(coord);
    auto entitiesView = cell->getEntitiesView<C>();

    EXPECT_EQ(entitiesView.size(), entities.size());
    for (auto entity : entitiesView) {
        auto foundEntity = entities.find(entity);
        EXPECT_NE(foundEntity, entities.end());
        EXPECT_EQ(foundEntity->second, false);
        foundEntity->second = true;
    }

    //update
    for (auto &entityPair: entities) {
        entityPair.second = false;
    }
    for (int i = 0; i < 3; i++) {
        std::unique_ptr<C> entity = std::make_unique<C>();
        grid.add<C>(coord, *entity);
        entities[entity.get()] = false;
        entitiesVec.push_back(std::move(entity));
    }

    EXPECT_EQ(entitiesView.size(), entities.size());
    for (auto entity : entitiesView) {
        auto foundEntity = entities.find(entity);
        EXPECT_NE(foundEntity, entities.end());
        EXPECT_EQ(foundEntity->second, false);
        foundEntity->second = true;
    }
}

TEST_F(GridTest, cellEntitiesViewEmptyTest) {
    Grid<A, 1500> grid;
    Coordinates coord(Longitude(-157.12), Latitude(-32.9945));

    auto cell = grid.getCellAt(coord);
    auto entitiesView = cell->getEntitiesView<C>();

    EXPECT_EQ(entitiesView.size(), (std::size_t) 0);
    for (auto entity : entitiesView) {
        EXPECT_TRUE(false);
    }

    //update
    std::vector<std::unique_ptr<A>> entitiesVec;
    std::unordered_map<A *, bool> entities;
    for (int i = 0; i < 15; i++) {
        std::unique_ptr<C> entity = std::make_unique<C>();
        grid.add<C>(coord, *entity);
        entities[entity.get()] = false;
        entitiesVec.push_back(std::move(entity));
    }

    EXPECT_EQ(entitiesView.size(), entities.size());
    for (auto entity : entitiesView) {
        auto foundEntity = entities.find(entity);
        EXPECT_NE(foundEntity, entities.end());
        EXPECT_EQ(foundEntity->second, false);
        foundEntity->second = true;
    }
}

TEST_F(GridTest, numCellsTest) {
    Grid<A, 10> grid1;
    Grid<A, 1500> grid2;
    Grid<A, 3000> grid3;
    int expectedNum1 = 130;
    int expectedNum2 = 2865566;
    int expectedNum3 = 11460688;

    int num1 = grid1.numCells();
    int num2 = grid2.numCells();
    int num3 = grid3.numCells();

    EXPECT_EQ(num1, expectedNum1);
    EXPECT_EQ(num2, expectedNum2);
    EXPECT_EQ(num3, expectedNum3);
}

TEST_F(GridTest, getEntitiesWithLimitTest) {
    auto funcTrue = [](const A &a) { return a.prop; };
    auto funcFalse = [](const A &a) { return !a.prop; };
    Grid<A, 1500> grid;
    Coordinates coord(Longitude(175), Latitude(13));
    std::vector<std::unique_ptr<A>> entitiesVec;
    std::unordered_map<B *, bool> entitiesTrue;
    std::unordered_map<B *, bool> entitiesFalse;
    std::size_t limit = 30;
    for (int i = 0; i < 45; i++) {
        std::unique_ptr<B> entity = std::make_unique<B>(false);
        grid.add<B>(coord, *entity);
        if (i < limit) {
            entitiesFalse[entity.get()] = false;
        }
        entitiesVec.push_back(std::move(entity));
    }
    for (int i = 0; i < 120; i++) {
        std::unique_ptr<B> entity = std::make_unique<B>(true);
        grid.add<B>(coord, *entity);
        entitiesTrue[entity.get()] = false;
        entitiesVec.push_back(std::move(entity));
    }

    auto cell = grid.getCellAt(coord);
    std::vector<B *> foundEntitiesFalse = cell->getEntities<B>(funcFalse, limit);
    std::vector<B *> foundEntitiesTrue = cell->getEntities<B>(funcTrue, 150);

    EXPECT_EQ(foundEntitiesFalse.size(), limit);
    EXPECT_EQ(foundEntitiesTrue.size(), entitiesTrue.size());
    for (B *foundEntity : foundEntitiesFalse) {
        auto entity = entitiesFalse.find(foundEntity);
        EXPECT_NE(entity, entitiesFalse.end());
        EXPECT_EQ(entity->second, false);
        entity->second = true;
    }
    for (B *foundEntity : foundEntitiesTrue) {
        auto entity = entitiesTrue.find(foundEntity);
        EXPECT_NE(entity, entitiesTrue.end());
        EXPECT_EQ(entity->second, false);
        entity->second = true;
    }
}

TEST_F(GridTest, bonusGetCellsAtTest) {
    Grid<int, 2500> grid;
    std::vector<std::unique_ptr<int>> entities;
    for (int i = 0; i < 8; i++) {
        entities.emplace_back(std::make_unique<int>(1));
    }
    grid.add<int>(Coordinates(Longitude(34.897823), Latitude(31.069517)), *entities[0]);
    grid.add<int>(Coordinates(Longitude(34.798810), Latitude(31.173084)), *entities[1]);
    grid.add<int>(Coordinates(Longitude(34.894434), Latitude(30.849406)), *entities[2]);
    grid.add<int>(Coordinates(Longitude(35.154892), Latitude(31.141191)), *entities[3]);
    grid.add<int>(Coordinates(Longitude(34.051371), Latitude(31.105179)), *entities[4]);
    grid.add<int>(Coordinates(Longitude(35.226156), Latitude(32.040169)), *entities[5]);
    grid.add<int>(Coordinates(Longitude(35.628108), Latitude(31.049635)), *entities[6]);
    grid.add<int>(Coordinates(Longitude(35.382163), Latitude(30.419575)), *entities[7]);

    auto cells = grid.getCellsAt(Coordinates(Longitude(34.983843), Latitude(31.011878)), Meters(32129));

    std::set<int *> foundEntities;
    auto func = [](const A &a) { return true; };
    for (auto cell : cells) {
        std::vector<int *> entitiesInCell = cell->getEntities(func);
        for (int *e : entitiesInCell) {
            foundEntities.insert(e);
        }
    }
    EXPECT_EQ(foundEntities.size(), (std::size_t) 4);
    for (int i = 0; i < 4; i++) {
        auto foundEntity = foundEntities.find(entities[i].get());
        EXPECT_NE(foundEntity, foundEntities.end());
    }
}
