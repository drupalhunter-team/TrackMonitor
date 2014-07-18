require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Reset with STM32 only")

    TestCaseBegin("cmdResetPumpReq valid")     -- Need board answer

        sendResetPump(Address["PC"], 2)

        emulateRemoteRep(Address["RM48"], Cmd["reset"], Code["CODE_NONE"], false)

        VerifyThat(CheckFrameValid(11000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["resetPump"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        WaitMs(8000)

    TestCaseEnd()

ScenarioEnd()