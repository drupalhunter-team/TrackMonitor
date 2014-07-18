require "Scripts STM32F103/functions" -- clear requirement declaration/defns
require "Scripts Common/slaves" -- clear requirement declaration/defns

-- Need :
--   * Valid image on manufacturing image
--   * Invalid image on medical
--   * Bootloader version = 0x100
ClearRequirement()

ProtocolID = 1
AddressTarget = Address["STM32"]
AddressPC = AddressPC

-- Try just one to start with
dofile "Scripts STM32F103/bootloader_version_0x01.lua" -- OK
-- dofile "Scripts STM32F103/reset_0x0F.lua"              -- OK
-- dofile "Scripts STM32F103/parameters_0xE.lua"          -- OK
-- dofile "Scripts STM32F103/reset.lua"                   -- OK
-- dofile "Scripts STM32F103/startMaintenanceMode.lua"    -- OK
-- dofile "Scripts STM32F103/synchronisation.lua"         -- OK
-- dofile "Scripts STM32F103/write.lua"                   -- OK
-- dofile "Scripts STM32F103/presetMode.lua"              -- OK

-- Start comment block
--[[
OpenCnx("COM1",Uart["speed"]);

ScenarioBegin("Message validation")

    TestCaseBegin("Reject and reply CRC invalid")

        SetUByteInTxBuffer(0, 0)   -- from PC
        SetUByteInTxBuffer(1, 0)   
        SetUByteInTxBuffer(2, AddressTarget[1])  -- To STM32
        SetUByteInTxBuffer(3, AddressTarget[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeBootloader"])  -- Message protocol
        
        -- Send invalid CRC
        SetULongInTxBuffer(13, 0)
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
        SetUByteInTxBuffer(2, AddressTarget[1])  -- To STM32
        SetUByteInTxBuffer(3, AddressTarget[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, Cmd["prepareWriteManufacturingApplication"])  -- Message protocol
        SendFrame(13)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        SetUByteInTxBuffer(0, AddressTarget[1])   -- from STM32
        SetUByteInTxBuffer(1, AddressTarget[2])   
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
        SetUByteInTxBuffer(2, AddressTarget[1])  -- To STM32
        SetUByteInTxBuffer(3, AddressTarget[2])
        SetUShortInTxBuffer(4, 68)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, Cmd["prepareWriteManufacturingApplication"])  -- Message protocol
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
        SetUByteInTxBuffer(2, AddressTarget[1])  -- To STM32
        SetUByteInTxBuffer(3, AddressTarget[2])
        SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 0)   -- Size
        SetUByteInTxBuffer(12, 0xE8)  -- Message protocol
        SendFrame(13)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0005 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0017 (2)")

    TestCaseEnd()

ScenarioEnd()
CloseCnx();
--]]