require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Start pump without remote CPUs")

    TestCaseBegin("prepare start pump medical application invalid local image")

        sendCmdWithoutParams(Address["PC"], Cmd["prepareStartPumpMedicalApplication"])

        VerifyThat(VerifyPending(Cmd["prepareStartPumpMedicalApplication"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareStartPumpMedicalApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_IMAGE"]) == true, "failure invalid local image")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0010 (1)")

    TestCaseEnd()

    TestCaseBegin("prepare start pump manufacturing application failure connected board")

        sendCmdWithoutParams(Address["PC"], Cmd["prepareStartPumpManufacturingApplication"])

        VerifyThat(VerifyPending(Cmd["prepareStartPumpManufacturingApplication"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(13000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareStartPumpManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_FAILURE_CONNECTED_BOARD_COM"]) == true, "failure connected board")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0009 (1)")

    TestCaseEnd()

    TestCaseBegin("Start pump manufacturing application no prepare start")

        sendCmdWithoutParams(Address["STM32"], Cmd["startPumpMedicalApplication"])

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startPumpMedicalApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_PREPARE_START_MESSAGE"]) == true, "no prepare start message")

    TestCaseEnd()

ScenarioEnd()

ScenarioBegin("Start pump with remote CPUs")

    TestCaseBegin("prepare start pump manufacturing application emulate connected board")

        sendCmdWithoutParams(Address["PC"], Cmd["prepareStartPumpManufacturingApplication"])

        VerifyThat(VerifyPending(Cmd["prepareStartPumpManufacturingApplication"]) == true, "Pending OK")
        WaitMs(250)

        emulateRemoteRep(Address["RM48"], Cmd["prepareStartManufacturingApplication"], Code["CODE_NONE"], true)
        emulateRemoteRep(Address["STM32"], Cmd["prepareStartManufacturingApplication"], Code["CODE_NONE"], true)
        
        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareStartPumpManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "code none")

    TestCaseEnd()

    TestCaseBegin("Start pump manufacturing application with invalid image on remote")

        sendCmdWithoutParams(Address["PC"], Cmd["startPumpManufacturingApplication"])

        emulateRemoteRep(Address["RM48"], Cmd["startManufacturingApplication"], Code["CODE_NONE"], false)
        emulateRemoteRep(Address["STM32"], Cmd["startManufacturingApplication"], Code["CODE_INVALID_IMAGE"], false)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startPumpManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_IMAGE"]) == true, "code invalid image on remote")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0010 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0022 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0024 (1)")

    TestCaseEnd()

    TestCaseBegin("Start pump manufacturing application success")

        sendCmdWithoutParams(Address["PC"], Cmd["startPumpManufacturingApplication"])

        emulateRemoteRep(Address["RM48"], Cmd["startManufacturingApplication"], Code["CODE_NONE"], false)
        emulateRemoteRep(Address["STM32"], Cmd["startManufacturingApplication"], Code["CODE_NONE"], false)

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startPumpManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "valid start local")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0010 (1)")
        ValidateRequirement("CC8050_ SRS_Pump_Boot_Com0014 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0021 (2)")


    TestCaseEnd()

    MessageBox("Check the application is started and press reset button", "Reboot the CPU", 0)
    WaitMs(8000)

ScenarioEnd()