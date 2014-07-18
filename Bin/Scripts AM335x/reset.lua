require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Reset")

    TestCaseBegin("cmdResetReq invalid mode")

        sendReset(Address["PC"], 8)

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_MODE"]) == true, "Frame size valid")

        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0028 (2)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0045 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0063 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0008 (1)")

    TestCaseEnd()

    TestCaseBegin("cmdResetReq valid")

        sendReset(Address["STM32"], 2)

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        WaitMs(8000)
        
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0016 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0017 (2)")
        ValidateRequirement("CC7601_SRS_Boot_Com0108 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0005 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0007 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0107 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0060 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0061 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0062 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0095 (2)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0001 (1)")

    TestCaseEnd()

ScenarioEnd()