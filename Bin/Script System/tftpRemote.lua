require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("tftpWriteBinary command on remote")

    TestCaseBegin("upload on target success")

        sendTftpwriteBinary(Address["STM32"], 2, Bin["Medical"], "out.bin", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")
        
        VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_NONE"]) == true, "Code none")
        
    TestCaseEnd()

    -- TODO add test on invalid image and invalid size

ScenarioEnd()