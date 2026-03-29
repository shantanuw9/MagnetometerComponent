module Rm3100 {
    @ Manager component for the RM3100 Magnetometer using I2C
    active component MagnetometerManager {

        output port busWriteRead: Drv.I2cWriteRead
        output port busWrite: Drv.I2c

        @ Port receiving calls from the rate group (triggers data read)
        sync input port run: Svc.Sched


        # ----------------------------------------------------------------------
        # Telemetry
        # ----------------------------------------------------------------------
        
        @ X-Axis Magnetic Field
        telemetry MagX: I32

        @ Y-Axis Magnetic Field
        telemetry MagY: I32

        @ Z-Axis Magnetic Field
        telemetry MagZ: I32

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------
        
        event InitError(status: I32) severity warning high id 0 format "I2C Init Error: {}"
        event ReadError(status: I32) severity warning high id 1 format "I2C Read Error: {}"
        event SensorConfigured() severity activity high id 2 format "RM3100 Configured"
        
        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        @ Initialize the sensor (Set Cycle Counts + Continuous Mode)
        async command INIT_SENSOR opcode 0

        ###############################################################################
        # Standard AC Ports
        ###############################################################################
        time get port timeCaller
        import Fw.Command
        import Fw.Event
        import Fw.Channel
        param get port prmGetOut
        param set port prmSetOut

    }
}