require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["speed"]);
ScenarioBegin("Prepare bootloader write test set")

    TestCaseBegin("prepareBootloaderWrite wrong size")
        
        SetUByteInTxBuffer(0, AddressPC[1])
        SetUByteInTxBuffer(1, AddressPC[2])
        SetUByteInTxBuffer(2, AddressSTM32[1])
        SetUByteInTxBuffer(3, AddressSTM32[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0) -- Frame number
        SetULongInTxBuffer(8, 4) -- Data size
        SetUByteInTxBuffer(12, Cmd["prepareWriteBootloader"]) -- Message ID
        SetULongInTxBuffer(13, 65535 ) -- SIZE IS TOO BIG
        SendFrame(17)
        
        ret, size, ts = ReceiveFrame(1000)
        ret8, val8    = GetULongInRxBuffer(8)
        ret12, val12  = GetUByteInRxBuffer(12)
        ret6, val6    = GetUShortInRxBuffer(6)
        
        VerifyThat(ret != 0 and val12 == Cmd["prepareWriteBootloader"] and  val8 == 2, "Prepare bootloader write valid")
        VerifyThat(val6 == 0, "Prepare bootloader write frame number = 0")
        
        WaitMs(100)
    TestCaseEnd()

    TestCaseBegin("prepareBootloaderWrite nominal case")
        
        SetUByteInTxBuffer(0, AddressPC[1])
        SetUByteInTxBuffer(1, AddressPC[2])
        SetUByteInTxBuffer(2, AddressSTM32[1])
        SetUByteInTxBuffer(3, AddressSTM32[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0) -- Frame number
        SetULongInTxBuffer(8, 4) -- Data size
        SetUByteInTxBuffer(12, Cmd["prepareWriteBootloader"]) -- Message ID
        SetULongInTxBuffer(13, 0x80 ) -- Bootloader binary size (128 bytes)
        SendFrame(17)
        
        ret, size, ts = ReceiveFrame(1000)
        ret8, val8    = GetULongInRxBuffer(8)
        ret12, val12  = GetUByteInRxBuffer(12)
        ret6, val6    = GetUShortInRxBuffer(6)
        
        VerifyThat(ret == 0 and val12 == Cmd["prepareWriteBootloader"] and  val8 == 2, "Prepare bootloader write valid")
        VerifyThat(val6 == 0, "Prepare bootloader write frame number = 0")
        
        WaitMs(100)
    TestCaseEnd()
ScenarioEnd()
CloseCnx();