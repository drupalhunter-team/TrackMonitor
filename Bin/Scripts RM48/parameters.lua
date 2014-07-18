randVal = os.time() % 255;

OpenCnx("192.168.111.26",9001,"192.168.111.39",9001);

ScenarioBegin("Parameters")

    

    TestCaseBegin("cmdWriteOperationalParameterReq")


        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 9)   -- Size
        SetUByteInTxBuffer(12, 0xE)  -- Message protocol
        SetUShortInTxBuffer(13, 43)   -- Address 43
        SetUShortInTxBuffer(15, 5)   -- Size 5
        SetUByteInTxBuffer(17, 1)
        SetUByteInTxBuffer(18, 2)
        SetUByteInTxBuffer(19, 3)
        SetUByteInTxBuffer(20, randVal)
        SetUByteInTxBuffer(21, 5)
        SendFrame(22)

        ret, size, ts = ReceiveFrame(2000)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)

        VerifyThat(ret == 0 and val12 == 0xE and val13 == 0, "cmdWriteOperationalParameterReq processed without error")

        WaitMs(100)

    TestCaseEnd()


    TestCaseBegin("cmdReadOperationalParameterReq")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 4)   -- Size
        SetUByteInTxBuffer(12, 0xD)  -- Message protocol
        SetUShortInTxBuffer(13, 43)   -- Address 43
        SetUShortInTxBuffer(15, 5)   -- Size 5
        SendFrame(17)

        ret, size, ts = ReceiveFrame(2000)
        
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)
        
        -- read data
        ret14, val14 = GetUByteInRxBuffer(14)
        ret15, val15 = GetUByteInRxBuffer(15)
        ret16, val16 = GetUByteInRxBuffer(16)
        ret17, val17 = GetUByteInRxBuffer(17)
        ret18, val18 = GetUByteInRxBuffer(18)
        
        VerifyThat(ret == 0 and val12 == 0xD and val13 == 0, "cmdWriteOperationalParameterReq processed without error")

        VerifyThat(val14 == 1 and val15 == 2 and val16 == 3 and val17 == randVal and val18 == 5, "Parameters write read OK")

        WaitMs(100)

    TestCaseEnd()

ScenarioEnd()

CloseCnx();