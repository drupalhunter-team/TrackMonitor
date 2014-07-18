require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

randVal = os.time() % 255;

ScenarioBegin("Parameters")

    TestCaseBegin("cmdWriteOperationalParameterReq")
 
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 9)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeOperationalParameter"])  -- Message protocol
        SetUShortInTxBuffer(13, 43)   -- Address 43
        SetUShortInTxBuffer(15, 5)   -- Size 5
        SetUByteInTxBuffer(17, 1)
        SetUByteInTxBuffer(18, 2)
        SetUByteInTxBuffer(19, 3)
        SetUByteInTxBuffer(20, randVal)
        SetUByteInTxBuffer(21, 5)

        SendFrameWithSource(Address["STM32"], 22)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0055 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0056 (1)")

    TestCaseEnd()

    TestCaseBegin("cmdReadOperationalParameterReq")

        sendReadParam(Address["PC"], 43, 5)

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
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0040 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0007 (1)")

    TestCaseEnd()

    TestCaseBegin("invalid address")
 
        sendReadParam(Address["PC"], 430, 5)

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

        SendFrameWithSource(Address["STM32"], 22)

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

        SendFrameWithSource(Address["STM32"], 22)

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

        SendFrameWithSource(Address["STM32"], 22)

        VerifyThat(CheckFrameValid(5000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_PARAMETER_DATA_SIZE"]) == true, "Invalid paramter data size")

        ValidateRequirement("CC7601_SRS_Boot_Com0058 (1)")

    TestCaseEnd()

    TestCaseBegin("invalid read parameter data size")

        sendReadParam(Address["PC"], 90, 50)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_PARAMETER_DATA_SIZE"]) == true, "Invalid paramter data size")

        ValidateRequirement("CC7601_SRS_Boot_Com0054 (1)")

    TestCaseEnd()

    TestCaseBegin("Mapping target")
 
        sendReadParam(Address["PC"], 0, 1)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(2) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")
        
        -- read data
        ret14, val14 = GetUByteInRxBuffer(14)

        VerifyThat(val14 == 1, "Target == 1")

        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0062 (1)")

    TestCaseEnd()

    TestCaseBegin("Mapping preset mode")
 
        sendReadParam(Address["PC"], 1, 1)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
        VerifyThat(checkSize(2) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")
        
        -- read data
        ret14, val14 = GetUByteInRxBuffer(14)

        VerifyThat(val14 == 0, "Preset mode default")

        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0062 (1)")

    TestCaseEnd()

ScenarioEnd()