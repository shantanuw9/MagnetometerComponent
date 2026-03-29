// ======================================================================
// \title  Rm3100DeploymentTopology.cpp
// \brief cpp file containing the topology instantiation code
// ======================================================================
#include <Rm3100Deployment/Top/Rm3100DeploymentTopologyAc.hpp>

// Includes
#include "Rm3100/Components/MagnetometerManager/MagnetometerManager.hpp"
#include <Drv/LinuxI2cDriver/LinuxI2cDriver.hpp>
#include <Fw/Types/MallocAllocator.hpp>

namespace Rm3100Deployment {

// Instantiate a malloc allocator for cmdSeq buffer allocation
Fw::MallocAllocator mallocator;

// Rate group divisors
Svc::RateGroupDriver::DividerSet rateGroupDivisorsSet{{{1, 0}, {2, 0}, {4, 0}}};

// Context arrays
U32 rateGroup1Context[Svc::ActiveRateGroup::CONNECTION_COUNT_MAX] = {};
U32 rateGroup2Context[Svc::ActiveRateGroup::CONNECTION_COUNT_MAX] = {};
U32 rateGroup3Context[Svc::ActiveRateGroup::CONNECTION_COUNT_MAX] = {};

// --- DELETED MANUAL INSTANTIATION HERE ---
// The Autocoder (instances.fpp) now handles creating 'magManager' and 'linuxI2cDriver'
// Do NOT declare them here.

enum TopologyConstants {
    COMM_PRIORITY = 34,
};

void configureTopology() {
    // Rate group driver needs a divisor list
    rateGroupDriver.configure(rateGroupDivisorsSet);

    // Rate groups require context arrays.
    rateGroup1.configure(rateGroup1Context, FW_NUM_ARRAY_ELEMENTS(rateGroup1Context));
    rateGroup2.configure(rateGroup2Context, FW_NUM_ARRAY_ELEMENTS(rateGroup2Context));
    rateGroup3.configure(rateGroup3Context, FW_NUM_ARRAY_ELEMENTS(rateGroup3Context));

    // Command sequencer needs to allocate memory
    cmdSeq.allocateBuffer(0, mallocator, 5 * 1024);
    
    // --- DELETED MANUAL INIT CALLS HERE ---
    // The Autocoder automatically calls .init() for instances in instances.fpp.
    // Calling them again here would cause an assertion failure.
    
    // --- KEEP THIS: Custom Driver Configuration ---
    // We still need to tell the driver which file to open.
    // Since 'linuxI2cDriver' is declared in the AutoCoder header, we can just use it.
    linuxI2cDriver.open("/dev/i2c-1");
}

void setupTopology(const TopologyState& state) {
    // Autocoded initialization. Function provided by autocoder.
    initComponents(state);
    // Autocoded id setup. Function provided by autocoder.
    setBaseIds();
    // Autocoded connection wiring. Function provided by autocoder.
    connectComponents();
    // Autocoded command registration. Function provided by autocoder.
    regCommands();
    // Autocoded configuration. Function provided by autocoder.
    configComponents(state);
    if (state.hostname != nullptr && state.port != 0) {
        comDriver.configure(state.hostname, state.port);
    }
    // Project-specific component configuration.
    configureTopology();
    // Autocoded parameter loading. Function provided by autocoder.
    loadParameters();
    // Autocoded task kick-off (active components). Function provided by autocoder.
    startTasks(state);
    if (state.hostname != nullptr && state.port != 0) {
        Os::TaskString name("ReceiveTask");
        comDriver.start(name, COMM_PRIORITY, Default::STACK_SIZE);
    }
}

void startRateGroups(const Fw::TimeInterval& interval) {
    timer.startTimer(interval);
}

void stopRateGroups() {
    timer.quit();
}

void teardownTopology(const TopologyState& state) {
    stopTasks(state);
    freeThreads(state);
    comDriver.stop();
    (void)comDriver.join();
    cmdSeq.deallocateBuffer(mallocator);
    tearDownComponents(state);
}
};  // namespace Rm3100Deployment