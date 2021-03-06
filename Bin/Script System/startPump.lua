require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Start pump with remote CPUs")


    TestCaseBegin("prepare start pump manufacturing application emulate connected board")

        sendCmdWithoutParams(Address["PC"], Cmd["prepareStartPumpManufacturingApplication"])

        VerifyThat(VerifyPending(Cmd["prepareStartPumpManufacturingApplication"]) == true, "Pending OK")
        WaitMs(250)

        emulateRemoteRep(Address["RM48"], Cmd["prepareStartManufacturingApplication"], Code["CODE_NONE"], true)
        
        VerifyThat(CheckFrameValid(15000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareStartPumpManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "failure connected board")

    TestCaseEnd()

    TestCaseBegin("Start pump manufacturing application success")

        sendCmdWithoutParams(Address["PC"], Cmd["startPumpManufacturingApplication"])

        emulateRemoteRep(Address["RM48"], Cmd["startManufacturingApplication"], Code["CODE_NONE"], false)
 
        VerifyThat(CheckFrameValid(8000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startPumpManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "valid start local")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0011")
        ValidateRequirement("CC8050_ SRS_Pump_Boot_Com0014")

    TestCaseEnd()

    MessageBox("Check the application is started and press reset button", "Reboot the CPU", 0)
    WaitMs(8000)

ScenarioEnd()