// ======================================================================
// \title  MagnetometerManager.hpp
// \author shantanuwad
// \brief  hpp file for MagnetometerManager component implementation class
// ======================================================================

#ifndef Rm3100_MagnetometerManager_HPP
#define Rm3100_MagnetometerManager_HPP

#include <Fw/Buffer/Buffer.hpp>
#include "Rm3100/Components/MagnetometerManager/MagnetometerManagerComponentAc.hpp"
#include <Fw/Types/MemAllocator.hpp>

namespace Rm3100 {

class MagnetometerManager final : public MagnetometerManagerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    static const U32 RM3100_I2C_ADDR = 0x20; 

    enum RegisterMap {
        REG_POLL    = 0x00,
        REG_CMM     = 0x01,
        REG_CCX_MSB = 0x04, 
        REG_TMRC    = 0x0B,
        REG_MX_2    = 0x24, 
        REG_STATUS  = 0x34,
        REG_REVID   = 0x36
    };

    MagnetometerManager(const char* const compName);
    ~MagnetometerManager();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations
    // ----------------------------------------------------------------------

    void INIT_SENSOR_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) override;
    void run_handler(FwIndexType portNum, U32 context) override;

    I32 convert24bitTo32bit(U8 msb, U8 mid, U8 lsb);

    // Buffers for I2C transactions
    U8 m_writeBuf[16];
    U8 m_readBuf[16];
    
    // State Variables
    U32 m_errorCount;
    bool m_initialized; // <--- ADDED THIS
};

}  // namespace Rm3100

#endif