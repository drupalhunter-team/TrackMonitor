require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["Speed"]);

ScenarioBegin("Prepare start manufacturing test set")

    TestCaseBegin("cmdPrepareStartMAnufacturing nominal case")

        SetUByteInTxBuffer(0, AddressPC[1])
        SetUByteInTxBuffer(1, AddressPC[2])
        SetUByteInTxBuffer(2, AddressSTM32[1])
        SetUByteInTxBuffer(3, AddressSTM32[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0 ) -- Frame number
        SetULongInTxBuffer(8, 0 )  -- Data size
        SetUByteInTxBuffer(12, Cmd["cmdPrepareStartMAnufacturing"])  -- Message ID
        SendFrame(13)

        ret, size, ts = ReceiveFrame(1000)
        ret8, val8 = GetULongInRxBuffer(8)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret6, val6 = GetUShortInRxBuffer(6)

        print(ret,val12,val8,size)
        VerifyThat(ret == 0 and val12 == Cmd["cmdPrepareStartMAnufacturing"] and  val8 == 1, "cmdPrepareStartMAnufacturing format valid")
        VerifyThat(val6 == 0, "cmdPrepareStartMAnufacturing frame number = 0")

        WaitMs(100)

ScenarioEnd()
CloseCnx();