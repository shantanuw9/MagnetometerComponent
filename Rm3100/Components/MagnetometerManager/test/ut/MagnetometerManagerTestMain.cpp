// ======================================================================
// \title  MagnetometerManagerTestMain.cpp
// \author shantanuwad
// \brief  cpp file for MagnetometerManager component test main function
// ======================================================================

#include "MagnetometerManagerTester.hpp"

TEST(Nominal, Initialization) {
    Rm3100::MagnetometerManagerTester tester;
    tester.testInitialization();
}

TEST(Nominal, ReadData) {
    Rm3100::MagnetometerManagerTester tester;
    tester.testReadData();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
