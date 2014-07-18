
OpenCnx("192.168.111.26",9001,"192.168.111.39",9001);

ScenarioBegin("Message validation")

    TestCaseBegin("Invalid CRC16")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  -- To AM335x
        SetUByteInTxBuffer(3, 1)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xB)  -- Message protocol
        -- Send invalid CRC
        SetULongInTxBuffer(13, 0)
        SendRawFrame(15)

        ret, size, ts = ReceiveFrame(1000)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)

        VerifyThat(ret == 0 and val12 == 0xFF and  val13 == 6, "Reject and reply CRC invalid")

        WaitMs(100)

    TestCaseEnd()

    TestCaseBegin("Invalid destination")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 5)    -- Invalid destination
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xB)  -- Message protocol
        SendFrame(13)

        ret, size, ts = ReceiveFrame(1000)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)

        VerifyThat(ret == 0 and val12 == 0xFF and  val13 == 2, "Reject and reply invalid destination")

        WaitMs(100)

    TestCaseEnd()

    TestCaseBegin("Invalid protocol")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 1)
        SetUShortInTxBuffer(4, 68)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xB)  -- Message protocol
        SendFrame(13)

        ret, size, ts = ReceiveFrame(1000)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)

        VerifyThat(ret == 0 and val12 == 0xFF and  val13 == 20, "Reject and reply invalid protocol")

        WaitMs(100)

    TestCaseEnd()

    TestCaseBegin("Invalid message type")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xE8)  -- Message protocol
        SendFrame(13)

        ret, size, ts = ReceiveFrame(1000)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)

        VerifyThat(ret == 0 and val12 == 0xFF and  val13 == 7, "Reject and reply invalid message type")

        WaitMs(100)

    TestCaseEnd()

    TestCaseBegin("Invalid frame number")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 1)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xF)  -- Message protocol
        SendFrame(13)

        ret, size, ts = ReceiveFrame(1000)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)

        VerifyThat(ret == 0 and val12 == 0xFF and  val13 == 5, "Reject and reply invalid frame number")

        WaitMs(100)

    TestCaseEnd()

    TestCaseBegin("Invalid size according to protocol")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 1)  -- Frame number
        SetULongInTxBuffer(8, 10)   -- Size
        SetUByteInTxBuffer(12, 0xF)  -- Message protocol
        SendFrame(23)

        ret, size, ts = ReceiveFrame(1000)
        ret12, val12 = GetUByteInRxBuffer(12)
        ret13, val13 = GetUByteInRxBuffer(13)

        VerifyThat(ret == 0 and val12 == 0xFF and  val13 == 22, "Reject and reply invalid protocol message size")

        WaitMs(100)

    TestCaseEnd()

ScenarioEnd()

CloseCnx();