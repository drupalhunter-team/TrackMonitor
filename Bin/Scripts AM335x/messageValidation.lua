require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Message validation")

    TestCaseBegin("Reject and reply CRC invalid")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  -- To AM335x
        SetUByteInTxBuffer(3, 1)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeBootloader"])  -- Message protocol
        -- Send invalid CRC
        SetULongInTxBuffer(13, 0)
        OpenConnexion(Address["PC"])
        SendRawFrame(15)

        VerifyThat(CheckFrameValid(2000, true) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0007 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0014 (2)")

    TestCaseEnd()

    TestCaseBegin("Reject and reply invalid destination")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 5)    -- Invalid destination
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xB)  -- Message protocol
        OpenConnexion(Address["PC"])
        SendFrame(13)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        SetUByteInTxBuffer(0, 9)   -- from STM32
        SetUByteInTxBuffer(1, 2)   
        OpenConnexion(Address["STM32"])
        SendFrame(13)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0012 (2)")
        ValidateRequirement("CC7601_SRS_Boot_Com0013 (2)")
        ValidateRequirement("CC7601_SRS_Boot_Com0015 (2)")

    TestCaseEnd()

    TestCaseBegin("Reject and reply invalid protocol")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 1)
        SetUShortInTxBuffer(4, 68)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xB)  -- Message protocol
        OpenConnexion(Address["PC"])
        SendFrame(13)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0016 (2)")

    TestCaseEnd()

    TestCaseBegin("Reject and reply invalid message type")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xE8)  -- Message protocol
        OpenConnexion(Address["PC"])
        SendFrame(13)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0005 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0017 (2)")

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
        OpenConnexion(Address["PC"])
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
        OpenConnexion(Address["PC"])
        SendFrame(23)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0017 (2)")
        ValidateRequirement("CC7601_SRS_Boot_Com0095 (2)")

    TestCaseEnd()

ScenarioEnd()

CloseCnx();