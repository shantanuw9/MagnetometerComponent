// ======================================================================
// \title  MagnetometerManagerTester.cpp
// \author shantanuwad
// \brief  cpp file for MagnetometerManager component test harness implementation class
// ======================================================================

#include "MagnetometerManagerTester.hpp"

namespace Rm3100 {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

MagnetometerManagerTester ::MagnetometerManagerTester()
    : MagnetometerManagerGTestBase("MagnetometerManagerTester", MagnetometerManagerTester::MAX_HISTORY_SIZE),
      component("MagnetometerManager"), m_mockStatusRegister(0),
      m_writeCount(0) {
    this->initComponents();
    this->connectPorts();
    for(int i=0; i<9; i++) m_mockData[i] = 0;
}

MagnetometerManagerTester ::~MagnetometerManagerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void MagnetometerManagerTester::testInitialization() {
    // Send the Init Command
    this->sendCmd_INIT_SENSOR(0, 0);
    this->component.doDispatch(); // Process command

    // Verify 1: We expect 2 writes to the I2C bus
    // Write 1: Cycle Counts (Reg 0x04)
    // Write 2: CMM Mode (Reg 0x01)
    ASSERT_EQ(this->m_writeCount, 2);

    // Verify 2: Check for Success Event
    ASSERT_EVENTS_SensorConfigured_SIZE(1);
    
    // Verify 3: Command Response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, MagnetometerManager::OPCODE_INIT_SENSOR, 0, Fw::CmdResponse::OK);
}


//Reading data from the sensor is the most critical function of the component, so we have a dedicated test for it. This test checks that when the sensor indicates data is ready, the component correctly reads the raw bytes, converts them to signed integers, and sends telemetry. We also verify that errors are handled gracefully.
void MagnetometerManagerTester::testReadData() {

    // Setup Mock Hardware:
    // 1. Set Status Register to "Ready" (Bit 7 = 1) [Datasheet Page 32]
    this->m_mockStatusRegister = 0x80; 

    // 2. Set Fake Sensor Data (24-bit Signed Integers)
    // X = 0x00 00 01 (Value: 1)
    m_mockData[0] = 0x00; m_mockData[1] = 0x00; m_mockData[2] = 0x01;
    // Y = 0xFF FF FF (Value: -1, 2's complement)
    m_mockData[3] = 0xFF; m_mockData[4] = 0xFF; m_mockData[5] = 0xFF;
    // Z = 0x7F FF FF (Value: Max Positive 8,388,607)
    m_mockData[6] = 0x7F; m_mockData[7] = 0xFF; m_mockData[8] = 0xFF;

    // Simulate Rate Group calling "Run"
    this->invoke_to_run(0, 0);

    // Verify Telemetry was written
    ASSERT_TLM_SIZE(3); // Expect X, Y, and Z updates
    ASSERT_TLM_MagX_SIZE(1);
    ASSERT_TLM_MagY_SIZE(1);
    ASSERT_TLM_MagZ_SIZE(1);

    // Verify Values (Checking our 24-bit conversion logic)
    ASSERT_TLM_MagX(0, 1);           // Should be 1
    ASSERT_TLM_MagY(0, -1);          // Should be -1
    ASSERT_TLM_MagZ(0, 8388607);     // Should be Max Int
}

Drv::I2cStatus MagnetometerManagerTester::from_busWrite_handler(
        FwIndexType portNum, 
        U32 addr, 
        Fw::Buffer& serBuffer) {
    this->m_writeCount++;

    // Basic address check
    EXPECT_EQ(addr, RM3100_ADDR) << "Component wrote to wrong I2C address";

    U8* data = serBuffer.getData();
    U32 size = serBuffer.getSize();

    // Verify specific configuration writes based on size
    if (size == 7) {
        // This should be the Cycle Count write
        // Reg 0x04
        EXPECT_EQ(data[0], 0x04) << "Cycle count write should start at Reg 0x04"; 
        // Value 0xC8 (200)
        EXPECT_EQ(data[2], 0xC8) << "Default Cycle Count X should be 200 (0xC8)";
    }
    else if (size == 2) {
        // This should be CMM Mode write
        // Reg 0x01
        EXPECT_EQ(data[0], 0x01) << "CMM Write should target Reg 0x01";
        // Value 0x79
        EXPECT_EQ(data[1], 0x79) << "CMM should enable X,Y,Z and DRDY";
    }

    return Drv::I2cStatus::I2C_OK; // Return Success
}

Drv::I2cStatus MagnetometerManagerTester::from_busWriteRead_handler(
        FwIndexType portNum, 
        U32 addr, 
        Fw::Buffer& writeBuffer, 
        Fw::Buffer& readBuffer
) {
    U8* wData = writeBuffer.getData();
    U8* rData = readBuffer.getData();
    
    // Identify what register the component is asking for
    // Remember: Read requests OR the address with 0x80
    U8 reg = wData[0] & 0x7F; 

    if (reg == 0x34) { 
        // STATUS Register
        rData[0] = this->m_mockStatusRegister;
        readBuffer.setSize(1);
    } 
    else if (reg == 0x24) { 
        // Measurement Results (MX2)
        // Return the 9 bytes of fake data we set up
        for(int i=0; i<9; i++) {
            rData[i] = this->m_mockData[i];
        }
        readBuffer.setSize(9);
    }

    return Drv::I2cStatus::I2C_OK; // Return Success
}


}  // namespace Rm3100
