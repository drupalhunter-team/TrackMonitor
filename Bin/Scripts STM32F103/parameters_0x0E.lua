require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["Speed"]);

randVal = os.time() % 255;

ScenarioBegin("Parameters")

    TestCaseBegin("cmdWriteOperationalParameterReq")
 
        SetUByteInTxBuffer(0, AddressPC[1])
        SetUByteInTxBuffer(1, AddressPC[2])
        SetUByteInTxBuffer(2, AddressTarget[1])
        SetUByteInTxBuffer(3, AddressTarget[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 9)   -- Size
        SetUByteInTxBuffer(12, Cmd["cancelWriteApplication"])  -- Message protocol
        SetUShortInTxBuffer(13, 43)   -- Address 43
        SetUShortInTxBuffer(15, 5)   -- Size 5
        SetUByteInTxBuffer(17, 1)
        SetUByteInTxBuffer(18, 2)
        SetUByteInTxBuffer(19, 3)
        SetUByteInTxBuffer(20, randVal)
        SetUByteInTxBuffer(21, 5)

        SendFrameWithSource(AddressPC, 22)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0055 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0056 (1)")

    TestCaseEnd()

    TestCaseBegin("cmdReadOperationalParameterReq")

        sendReadParam(AddressPC, AddressSTM32, 43, 5)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(6) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")
        
        -- read data
        ret14, val14 = GetUByteInRxBuffer(14)
        ret15, val15 = GetUByteInRxBuffer(15)
        ret16, val16 = GetUByteInRxBuffer(16)
        ret17, val17 = GetUByteInRxBuffer(17)
        ret18, val18 = GetUByteInRxBuffer(18)

        VerifyThat(val14 == 1 and val15 == 2 and val16 == 3 and val17 == randVal and val18 == 5, "Parameters write read OK")

        ValidateRequirement("CC7518_SRS_Bootloader0022 (1)")
        ValidateRequirement("CC7518_SRS_Bootloader0023 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0051 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0052 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0053 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0059 (1)")
        ValidateRequirement("CC7598_SRS_STM32_Bootloader0040 (1)")
        ValidateRequirement("CC7598_SRS_STM32_Bootloader0007 (1)")

    TestCaseEnd()

    TestCaseBegin("invalid address")
 
        sendReadParam(AddressPC, AddressSTM32, 430, 5)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_PARAMTER_ADDRESS"]) == true, "Invalid paramter address")

        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 9)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeOperationalParameter"])  -- Message protocol
        SetUShortInTxBuffer(13, 102)   -- Address 102
        SetUShortInTxBuffer(15, 5)   -- Size 5
        SetUByteInTxBuffer(17, 1)
        SetUByteInTxBuffer(18, 2)
        SetUByteInTxBuffer(19, 3)
        SetUByteInTxBuffer(20, randVal)
        SetUByteInTxBuffer(21, 5)

        SendFrameWithSource(AddressPC, 22)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_PARAMTER_ADDRESS"]) == true, "Invalid paramter address")

        ValidateRequirement("CC7601_SRS_Boot_Com0066 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0059 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0057 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0054 (1)")

    TestCaseEnd()

    TestCaseBegin("Write in read only area")
 
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 9)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeOperationalParameter"])  -- Message protocol
        SetUShortInTxBuffer(13, 2)   -- Address 102
        SetUShortInTxBuffer(15, 5)   -- Size 5
        SetUByteInTxBuffer(17, 1)
        SetUByteInTxBuffer(18, 2)
        SetUByteInTxBuffer(19, 3)
        SetUByteInTxBuffer(20, randVal)
        SetUByteInTxBuffer(21, 5)

        SendFrameWithSource(AddressPC, 22)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_PARAMTER_ADDRESS"]) == true, "Invalid paramter address")

    TestCaseEnd()


    TestCaseBegin("invalid write parameter data size")
 
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 9)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeOperationalParameter"])  -- Message protocol
        SetUShortInTxBuffer(13, 43)   -- Address 43
        SetUShortInTxBuffer(15, 3)   -- Size 5
        SetUByteInTxBuffer(17, 1)
        SetUByteInTxBuffer(18, 2)
        SetUByteInTxBuffer(19, 3)
        SetUByteInTxBuffer(20, randVal)
        SetUByteInTxBuffer(21, 5)

        SendFrameWithSource(AddressPC, 22)

        VerifyThat(CheckFrameValid(5000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_PARAMETER_DATA_SIZE"]) == true, "Invalid paramter data size")

        ValidateRequirement("CC7601_SRS_Boot_Com0058 (1)")

    TestCaseEnd()

    TestCaseBegin("invalid read parameter data size")

        sendReadParam(AddressPC, AddressSTM32, 90, 50)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_PARAMETER_DATA_SIZE"]) == true, "Invalid paramter data size")

        ValidateRequirement("CC7601_SRS_Boot_Com0054 (1)")

    TestCaseEnd()

    TestCaseBegin("Mapping target")
 
        sendReadParam(AddressPC, AddressSTM32,, 0, 1)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(2) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")
        
        -- read data
        ret14, val14 = GetUByteInRxBuffer(14)

        VerifyThat(val14 == 1, "Target == 1")

        ValidateRequirement("CC7598_SRS_STM32_Bootloader0062 (1)")

    TestCaseEnd()

    TestCaseBegin("Mapping preset mode")
 
        sendReadParam(AddressPC, AddressSTM32, 1, 1)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(2) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")
        
        -- read data
        ret14, val14 = GetUByteInRxBuffer(14)

        VerifyThat(val14 == 0, "Preset mode default")

        ValidateRequirement("CC7598_SRS_STM32_Bootloader0062 (1)")

    TestCaseEnd()

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