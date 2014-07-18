require "Scripts Common/tables"
require "Scripts Common/slaves"
require "Scripts STM32F103/functions"
require "Scripts STM32F103/commands"

OpenCnx("COM1",Uart["speed"]);
ScenarioBegin("Start maintenance message test set")

    TestCaseBegin("cmdStartMaintenanceMode")

        sendCmdWithoutParams(AddressPC, AddressSTM32, Cmd["startMaintenanceMode"])

        VerifyThat(CheckFrameValid(400) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0004 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0005 (1)")

    TestCaseEnd()

    TestCaseBegin("Start preset mode manufacturing and start maintenance mode")

        sendReset(AddressPC, AddressSTM32, 3)
        CheckFrameValid(100)

        VerifyThat(CheckFrameValid(6000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["bootloaderVersion"]) == true, "Frame command valid")
        VerifyThat(checkSize(0) == true, "Frame size valid")

        sendCmdWithoutParams(AddressPC, AddressSTM32, Cmd["startMaintenanceMode"])

        VerifyThat(CheckFrameValid(400) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["startMaintenanceMode"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")
 
        ValidateRequirement("CC7518_SRS_Bootloader0017")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0006")

    TestCaseEnd()
ScenarioEnd()
CloseCnx();