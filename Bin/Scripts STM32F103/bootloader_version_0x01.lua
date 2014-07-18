require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["speed"]);
ScenarioBegin("Bootloader version")

    TestCaseBegin("cmdBootloaderVersionReq invalid mode")
    
        -- Send command
        sendBootversionReq(AddressPC,AddressTarget)

        -- Verification
        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["bootloaderVersion"]) == true, "Frame command valid")
        VerifyThat(checkSize(2) == true, "Frame size valid")

        -- Requirement validation
        ValidateRequirement("CC7518_SRS_Bootloader0007 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0019 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0020 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0064 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0021 (1)")

        WaitMs(100)
        
    TestCaseEnd()

ScenarioEnd()
CloseCnx();