require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Synchronisation")

    TestCaseBegin("Ask version to STM32")

        sendReset(Address["PC"], 3)
        CheckFrameValid(2000)

        WaitMs(4000)
        OpenConnexion(Address["STM32"])

        VerifyThat(CheckFrameValid(6000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["bootloaderVersion"]) == true, "Frame command valid")
        VerifyThat(checkSize(0) == true, "Frame size valid")

        ValidateRequirement("CC7518_SRS_Bootloader0015 (1)")

        WaitMs(200)

    TestCaseEnd()

    TestCaseBegin("Start preset mode manufacturing")

        sendReset(Address["STM32"], 3)
        CheckFrameValid(2000)

        WaitMs(4000)
        OpenConnexion(Address["STM32"])

        VerifyThat(CheckFrameValid(6000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["bootloaderVersion"]) == true, "Frame command valid")
        VerifyThat(checkSize(0) == true, "Frame size valid")

        emulateRM48STM32Synchro(true)

        WaitMs(350)
        emulateRemoteRep(Address["RM48"], Cmd["prepareStartManufacturingApplication"], Code["CODE_NONE"], true)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareStartManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(0) == true, "Frame size valid")

        ValidateRequirement("CC7518_SRS_Bootloader0017 (1)")
  
        WaitMs(2000)

    TestCaseEnd()

    TestCaseBegin("Start maintanance because synchro version don't match")

        sendReset(Address["STM32"], 2)
        CheckFrameValid(2000)

        WaitMs(4000)
        OpenConnexion(Address["STM32"])

        VerifyThat(CheckFrameValid(6000) == true, "Frame valid")

        emulateRM48STM32Synchro(false)

        WaitMs(250)
        emulateRemoteRep(Address["RM48"], Cmd["prepareStartMedicalApplication"], Code["CODE_NONE"], true)

        VerifyThat(CheckFrameValid(3000) == false, "No frame send")

    TestCaseEnd()

ScenarioEnd()
