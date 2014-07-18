require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Start maintenance mode")

    TestCaseBegin("cmdStartMaintenanceMode")

        sendCmdWithoutParams(Address["PC"], Cmd["startMaintenanceMode"])

        VerifyThat(CheckFrameValid(400) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0004 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0005 (1)")

    TestCaseEnd()

    TestCaseBegin("Start preset mode manufacturing and start maintenance mode")

        sendReset(Address["STM32"], 3)
        CheckFrameValid(2000)

        WaitMs(4000)
        OpenConnexion(Address["STM32"])

        VerifyThat(CheckFrameValid(6000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["bootloaderVersion"]) == true, "Frame command valid")
        VerifyThat(checkSize(0) == true, "Frame size valid")

        emulateRM48STM32Synchro(true)

        CloseCnx()

        sendCmdWithoutParams(Address["STM32"], Cmd["startMaintenanceMode"])

        VerifyThat(CheckFrameValid(400) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startMaintenanceMode"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        WaitMs(350)
 
        --ValidateRequirement("CC7518_SRS_Bootloader0017")
        --ValidateRequirement("CC8050_SRS_Pump_Boot_Com0006")

    TestCaseEnd()

ScenarioEnd()