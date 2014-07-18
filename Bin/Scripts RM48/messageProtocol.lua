
OpenCnx("192.168.111.26",9001,"192.168.111.39",9001);

ScenarioBegin("Message protocol")

    TestCaseBegin("cmdBootloaderVersionReq")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 1)  -- Message protocol
        SendFrame(13)

        ret, size, ts = ReceiveFrame(1000)
        ret8, val8 = GetULongInRxBuffer(8)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret6, val6 = GetUShortInRxBuffer(6)

        VerifyThat(ret == 0 and val12 == 1 and  val8 == 2, "cmdBootloaderVersionReq format valid")
        VerifyThat(val6 == 0, "cmdBootloaderVersionReq frame number = 0")

        WaitMs(100)

ScenarioEnd()

CloseCnx();