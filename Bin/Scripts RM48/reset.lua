
OpenCnx("192.168.111.26",9001,"192.168.111.39",9001);

ScenarioBegin("Reset")

    TestCaseBegin("cmdResetReq")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 1)   -- Size
        SetUByteInTxBuffer(12, 0xF)  -- Message protocol
        SetUByteInTxBuffer(13, 2)  -- Preset mode
        SendFrame(14)

        ret, size, ts = ReceiveFrame(2000)

        ret8, val8 = GetULongInRxBuffer(8)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret6, val6 = GetUShortInRxBuffer(6)

        VerifyThat(ret == 0 and val12 == 0xF and  val8 == 0, "cmdResetRep format valid")
        VerifyThat(val6 == 0, "cmdResetRep frame number = 0")

        WaitMs(100)

ScenarioEnd()

CloseCnx();