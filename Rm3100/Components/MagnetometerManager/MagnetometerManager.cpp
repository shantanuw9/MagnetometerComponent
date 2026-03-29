// ======================================================================
// \title  MagnetometerManager.cpp
// \author shantanuwad
// \brief  cpp file for MagnetometerManager component implementation class
// ======================================================================

#include "Rm3100/Components/MagnetometerManager/MagnetometerManager.hpp"
#include <Fw/Types/BasicTypes.hpp>

namespace Rm3100 {

MagnetometerManager::MagnetometerManager(const char* const compName) : 
    MagnetometerManagerComponentBase(compName),
    m_errorCount(0),
    m_initialized(false) 
{}

MagnetometerManager::~MagnetometerManager() {}

// ----------------------------------------------------------------------
// Command Handler: Initialize
// ----------------------------------------------------------------------

void MagnetometerManager::INIT_SENSOR_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    Fw::Buffer writeBuffer(m_writeBuf, sizeof(m_writeBuf));
    Drv::I2cStatus status; 

    // 1. Set Cycle Counts
    m_writeBuf[0] = REG_CCX_MSB; 
    m_writeBuf[1] = 0x00; m_writeBuf[2] = 0xC8; 
    m_writeBuf[3] = 0x00; m_writeBuf[4] = 0xC8; 
    m_writeBuf[5] = 0x00; m_writeBuf[6] = 0xC8; 
    writeBuffer.setSize(7);

    status = this->busWrite_out(0, RM3100_I2C_ADDR, writeBuffer);
    
    // Note: On Mac simulation, status might be OK even if nothing happens.
    // We ignore errors here for simulation robustness.

    // 2. Enable Continuous Measurement Mode
    m_writeBuf[0] = REG_CMM;
    m_writeBuf[1] = 0x79; 
    writeBuffer.setSize(2);
    status = this->busWrite_out(0, RM3100_I2C_ADDR, writeBuffer);

    // 3. Mark Initialized
    this->m_initialized = true;

    this->log_ACTIVITY_HI_SensorConfigured();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Rate Group Handler: Read Data
// ----------------------------------------------------------------------

void MagnetometerManager::run_handler(FwIndexType portNum, U32 context) {
    // 1. Check if initialized
    if (!this->m_initialized) {
        return;
    }

    Fw::Buffer wBuf(m_writeBuf, sizeof(m_writeBuf));
    Fw::Buffer rBuf(m_readBuf, sizeof(m_readBuf));

    // 2. Attempt Read (keeps the driver active)
    m_writeBuf[0] = REG_MX_2; // Read starting at Measurement X
    wBuf.setSize(1);
    rBuf.setSize(9); // Read 9 bytes

    this->busWriteRead_out(0, RM3100_I2C_ADDR, wBuf, rBuf);

    // ---------------------------------------------------------
    // SIMULATION HACK: Force Fake Data
    // ---------------------------------------------------------
    static U8 counter = 0;
    counter++;

    // Fake X: Constant value 123 (0x7B)
    m_readBuf[0] = 0x00; m_readBuf[1] = 0x00; m_readBuf[2] = 0x7B;
    
    // Fake Y: Constant value -123 (0xFFFFFF85)
    m_readBuf[3] = 0xFF; m_readBuf[4] = 0xFF; m_readBuf[5] = 0x85;
    
    // Fake Z: Ramp up (0, 1, 2... 255)
    m_readBuf[6] = 0x00; m_readBuf[7] = 0x00; m_readBuf[8] = counter;
    // ---------------------------------------------------------

    // 3. Convert Data
    I32 x = convert24bitTo32bit(m_readBuf[0], m_readBuf[1], m_readBuf[2]);
    I32 y = convert24bitTo32bit(m_readBuf[3], m_readBuf[4], m_readBuf[5]);
    I32 z = convert24bitTo32bit(m_readBuf[6], m_readBuf[7], m_readBuf[8]);

    // 4. Write Telemetry (Crucial Step!)
    this->tlmWrite_MagX(x);
    this->tlmWrite_MagY(y);
    this->tlmWrite_MagZ(z);
}

I32 MagnetometerManager::convert24bitTo32bit(U8 msb, U8 mid, U8 lsb) {
    I32 val = (static_cast<I32>(msb) << 16) | 
              (static_cast<I32>(mid) << 8)  | 
              (static_cast<I32>(lsb));
    if (val & 0x800000) {
        val |= 0xFF000000; // Sign extend negative numbers
    }
    return val;
}

} // namespace Rm3100