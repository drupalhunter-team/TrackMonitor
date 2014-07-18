require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Reset")

    TestCaseBegin("cmdResetPumpReq invalid mode")     -- Need board answer

        sendResetPump(Address["PC"], 8)

        emulateRemoteRep(Address["RM48"], Cmd["reset"], Code["CODE_NONE"], false)
        emulateRemoteRep(Address["STM32"], Cmd["reset"], Code["CODE_NONE"], false)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["resetPump"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_MODE"]) == true, "Frame size valid")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0016 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0018 (1)")

    TestCaseEnd()

    TestCaseBegin("cmdResetPumpReq failure board connected")

        sendResetPump(Address["PC"], 2)

        VerifyThat(CheckFrameValid(13000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["resetPump"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_FAILURE_CONNECTED_BOARD_COM"]) == true, "Frame size valid")
        
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0017 (1)")
        
    TestCaseEnd()

    TestCaseBegin("cmdResetPumpReq valid")     -- Need board answer

        sendResetPump(Address["PC"], 2)

        emulateRemoteRep(Address["RM48"], Cmd["reset"], Code["CODE_NONE"], false)
        emulateRemoteRep(Address["STM32"], Cmd["reset"], Code["CODE_NONE"], false)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["resetPump"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0015 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0019 (1)")

        WaitMs(8000)

    TestCaseEnd()

ScenarioEnd()