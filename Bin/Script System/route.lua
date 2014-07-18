require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands" 

OpenConnexion(Address["PC"])

ScenarioBegin("Route messages req and rep to STM32")

    TestCaseBegin("cmdBootloaderVersionReq")

        WaitMs(500)

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 2)  
        SetUByteInTxBuffer(3, 1)
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

        WaitMs(10)

    TestCaseEnd()
    
    TestCaseBegin("cmdResetReq to STM32")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 2)  
        SetUByteInTxBuffer(3, 1)
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
        print(GetErrorText(ret),size)
        VerifyThat(ret == 0 and val12 == 0xF and  val8 == 1, "cmdResetRep format valid")
        VerifyThat(val6 == 0, "cmdResetRep frame number = 0")

        WaitMs(20)
        
    TestCaseEnd()

ScenarioEnd()

CloseCnx();