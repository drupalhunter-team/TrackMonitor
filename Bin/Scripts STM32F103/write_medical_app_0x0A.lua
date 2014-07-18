require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["speed"]);
ScenarioBegin("Write medical application test set")
    TestCaseBegin("WriteMedicalApplicationReq nominal case")

        SetUByteInTxBuffer(0, AddressPC[1])
        SetUByteInTxBuffer(1, AddressPC[2])
        SetUByteInTxBuffer(2, AddressSTM32[1])
        SetUByteInTxBuffer(3, AddressSTM32[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0) -- Frame number
        SetULongInTxBuffer(8, 40) -- Data size
        SetUByteInTxBuffer(12, Cmd["writeMedicalApplication"] )  -- Message ID
        SetULongInTxBuffer(13, 0x1C ) -- Header - Binary size (32 bytes)
        SetULongInTxBuffer(17, 0xdeadbeef ) -- Header - CRC32
        SetULongInTxBuffer(21, 0xde02beef ) -- Header - version, target, reserved
        SetULongInTxBuffer(25, 0xdeadbeaf ) -- Data
        SetULongInTxBuffer(29, 0xdeadbeaf ) -- Data
        SetULongInTxBuffer(33, 0xdeadbeaf ) -- Data
        SetULongInTxBuffer(37, 0xdeadbeaf ) -- Data
        SetULongInTxBuffer(41, 0xdeadbeaf ) -- Data
        SetULongInTxBuffer(45, 0xdeadbeaf ) -- Data
        SetULongInTxBuffer(49, 0xdeadbeaf ) -- Data
        SendFrame(53)   

        ret, size, ts = ReceiveFrame(1000)
        ret8, val8 = GetULongInRxBuffer(8)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret6, val6 = GetUShortInRxBuffer(6)

        VerifyThat(ret == 0 and val12 == 0xA and  val8 == 1, "writeMedicalApplication format valid")
        VerifyThat(val6 == 1, "writeMedicalApplication frame number = 0")

        WaitMs(100)
    TestCaseEnd()
ScenarioEnd()
CloseCnx();