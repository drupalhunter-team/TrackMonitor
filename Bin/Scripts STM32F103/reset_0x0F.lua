require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["speed"]);
ScenarioBegin("Reset test set")

    TestCaseBegin("cmdResetReq invalid mode")

        sendReset(Address["PC"], Address["STM32"], 8)

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_MODE"]) == true, "Frame return code valid")

        ValidateRequirement("CC7598_SRS_STM32_Bootloader0028 (2)")
        ValidateRequirement("CC7598_SRS_STM32_Bootloader0045 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0063 (1)")
        ValidateRequirement("CC7598_SRS_STM32_Bootloader0008 (1)")

    TestCaseEnd()

    TestCaseBegin("Reject and reply invalid frame number")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 1)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xF)  -- Message protocol
        SendFrame(13)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

    TestCaseEnd()

    TestCaseBegin("Reject and reply invalid protocol message size")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 1)  -- Frame number
        SetULongInTxBuffer(8, 10)   -- Size
        SetUByteInTxBuffer(12, 0xF)  -- Message protocol
        SendFrame(23)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0017 (2)")
        ValidateRequirement("CC7601_SRS_Boot_Com0095 (2)")

    TestCaseEnd()

    TestCaseBegin("cmdResetReq valid")

        sendReset(Address["PC"], Address["STM32"], 2)

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame return code valid")
        
        ValidateRequirement("CC7598_SRS_STM32_Bootloader0016 (1)")
        ValidateRequirement("CC7598_SRS_STM32_Bootloader0017 (2)")
        ValidateRequirement("CC7601_SRS_Boot_Com0108 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0005 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0007 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0107 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0060 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0061 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0062 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0095 (2)")
        ValidateRequirement("CC7598_SRS_STM32_Bootloader0001 (1)")

    TestCaseEnd()

ScenarioEnd()
CloseCnx();