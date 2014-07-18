require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["speed"]);
ScenarioBegin("Write operational parameter test set")
    TestCaseBegin("writeOperationalParameter nominal case")

        SetUByteInTxBuffer(0, AddressPC[1])
        SetUByteInTxBuffer(1, AddressPC[2])
        SetUByteInTxBuffer(2, AddressSTM32[1])
        SetUByteInTxBuffer(3, AddressSTM32[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0) -- Frame number
        SetULongInTxBuffer(8, 0) -- Size
        SetUByteInTxBuffer(12, 1) -- Message protocol
        SendFrame(13)

        ret, size, ts = ReceiveFrame(1000)
        ret8, val8 = GetULongInRxBuffer(8)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret6, val6 = GetUShortInRxBuffer(6)

        VerifyThat(ret == 0 and val12 == 1 and  val8 == 2, "writeOperationalParameter format valid")
        VerifyThat(val6 == 0, "writeOperationalParameter frame number = 0")

        WaitMs(100)
    TestCaseEnd()
ScenarioEnd()
CloseCnx();