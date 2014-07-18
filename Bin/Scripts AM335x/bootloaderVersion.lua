require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"




ScenarioBegin("Message protocol")

    TestCaseBegin("cmdBootloaderVersionReq")

        sendCmdWithoutParams(Address["PC"], Cmd["bootloaderVersion"])

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["bootloaderVersion"]) == true, "Frame command valid")
        VerifyThat(checkSize(2) == true, "Frame size valid")

        ValidateRequirement("CC7518_SRS_Bootloader0007 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0019 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0020 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0064 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0021 (1)")

ScenarioEnd()

CloseCnx();