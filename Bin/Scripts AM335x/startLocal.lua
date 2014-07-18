require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Start local")

    TestCaseBegin("prepare start medical application invalid")

        sendCmdWithoutParams(Address["PC"], Cmd["prepareStartMedicalApplication"])

        VerifyThat(VerifyPending(Cmd["prepareStartMedicalApplication"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareStartMedicalApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_IMAGE"]) == true, "image invalid")

        ValidateRequirement("CC7601_SRS_Boot_Com0079 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0080 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0081 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0007 (1)")

    TestCaseEnd()


    TestCaseBegin("prepare start manufacturing application valid")

        sendCmdWithoutParams(Address["PC"], Cmd["prepareStartManufacturingApplication"])

        VerifyThat(VerifyPending(Cmd["prepareStartManufacturingApplication"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareStartManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "image valid")

    TestCaseEnd()

    TestCaseBegin("Start medical application without prepare start")

        sendCmdWithoutParams(Address["STM32"], Cmd["startMedicalApplication"])

        VerifyThat(CheckFrameValid(300) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startMedicalApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_PREPARE_START_MESSAGE"]) == true, "no prepare start")

        ValidateRequirement("CC7601_SRS_Boot_Com0041 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0042 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0082 (1)")

    TestCaseEnd()

    TestCaseBegin("Start manufacturing application success")

        sendCmdWithoutParams(Address["PC"], Cmd["startManufacturingApplication"])

        VerifyThat(CheckFrameValid(300) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "image valid")
    
        ValidateRequirement("CC7601_SRS_Boot_Com0038 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0005 (1)")
       
    TestCaseEnd()

    MessageBox("Check the application is started and press reset button", "Reboot the CPU", 0)
    WaitMs(8000)

ScenarioEnd()