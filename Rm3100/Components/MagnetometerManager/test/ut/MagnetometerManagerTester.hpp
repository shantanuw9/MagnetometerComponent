// ======================================================================
// \title  MagnetometerManagerTester.hpp
// \author shantanuwad
// \brief  hpp file for MagnetometerManager component test harness implementation class
// ======================================================================

#ifndef Rm3100_MagnetometerManagerTester_HPP
#define Rm3100_MagnetometerManagerTester_HPP

#include "Rm3100/Components/MagnetometerManager/MagnetometerManager.hpp"
#include "Rm3100/Components/MagnetometerManager/MagnetometerManagerGTestBase.hpp"

namespace Rm3100 {

class MagnetometerManagerTester final : public MagnetometerManagerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    enum { RM3100_ADDR = 0x20 };
    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object MagnetometerManagerTester
    MagnetometerManagerTester();

    //! Destroy object MagnetometerManagerTester
    ~MagnetometerManagerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testInitialization();

    void testReadData();
    //! To do

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    Drv::I2cStatus from_busWriteRead_handler(
        FwIndexType portNum,
        U32 addr,
        Fw::Buffer& writeBuffer,
        Fw::Buffer& readBuffer
    ) override;

    Drv::I2cStatus from_busWrite_handler(
        FwIndexType portNum,
        U32 addr,
        Fw::Buffer& serBuffer
    ) override;

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    U8 m_mockStatusRegister;  // Simulated 0x34 register
    U8 m_mockData[9];         // Simulated 0x24-0x2C registers
    
    // Track internal state
    U32 m_writeCount;
    //! The component under test
    MagnetometerManager component;
};

}  // namespace Rm3100

#endif
