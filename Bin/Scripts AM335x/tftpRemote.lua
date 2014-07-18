require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("tftpWriteBinary command on remote")

    TestCaseBegin("upload on target success")

        sendTftpwriteBinary(Address["STM32"], 2, Bin["Bootloader"], "SITARA_BL", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")
        
        -- Check frame prepare write send to AM335x to STM32
        VerifyThat(CheckFrameValid(8000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareWriteBootloader"]) == true, "Frame command valid")
        VerifyThat(checkSize(4) == true, "Frame size valid")
      
        emulateRemoteRep(Address["STM32"], Cmd["prepareWriteBootloader"], Code["CODE_NONE"], true)

        VerifyThat(CheckFrameValid(2000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "cmdWrite received")
        VerifyThat(checkSize(8192) == true, "Frame size valid")

        emulateRemoteRep(Address["STM32"], Cmd["writeBootloader"], Code["CODE_NONE"], false)

        VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(1) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "cmdWrite received")
        VerifyThat(checkSize(8192) == true, "Frame size valid")

        emulateRemoteRep(Address["STM32"], Cmd["writeBootloader"], Code["CODE_NONE"], false)
    
        VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(2) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "cmdWrite received")
        VerifyThat(checkSize(8192) == true, "Frame size valid")

        -- Verify retry
        OpenConnexion(Address["STM32"])
        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(2) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "cmdWrite received")
        VerifyThat(checkSize(8192) == true, "Frame size valid")

        emulateRemoteRep(Address["STM32"], Cmd["writeBootloader"], Code["CODE_NONE"], false)
    
        VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(3) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "cmdWrite received")
        VerifyThat(checkSize(8192) == true, "Frame size valid")

        -- Verify retry
        OpenConnexion(Address["STM32"])
        VerifyThat(CheckFrameValid(1200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(3) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "cmdWrite received")
        VerifyThat(checkSize(8192) == true, "Frame size valid")

        -- 2nd try -> Reset
        OpenConnexion(Address["STM32"])
        VerifyThat(CheckFrameValid(1200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["reset"]) == true, "cmdWrite received")
        VerifyThat(checkSize(1) == true, "Frame size valid")

        OpenConnexion(Address["STM32"])
        VerifyThat(CheckFrameValid(1200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_DOWNLOAD_FAILURE"]) == true, "download failed")

        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0053 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0098 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0104 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0097 (1)")

    TestCaseEnd()

ScenarioEnd()