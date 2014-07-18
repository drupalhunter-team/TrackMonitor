require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("tftpWriteBinary command")

    TestCaseBegin("tftpWriteBinaryReq invalid image name in case download failure")

        sendTftpwriteBinary(Address["PC"], 1, Bin["Bootloader"], "SITARA_xx", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(4000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_INVALID_IMAGE_NAME"]) == true, "image invalid name")

        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0050 (1)")

    TestCaseEnd()

    TestCaseBegin("tftpWriteBinaryReq invalid image name")

        sendTftpwriteBinary(Address["PC"], 1, Bin["Bootloader"], "", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(4000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_INVALID_IMAGE_NAME"]) == true, "image invalid name")

        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0050 (1)")

    TestCaseEnd()

    TestCaseBegin("tftpWriteBinaryReq invalid destination")

        sendTftpwriteBinary(Address["PC"], 5, Bin["Bootloader"], "SITARA_BL", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(4000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_INVALID_DESTINATION"]) == true, "invalid destination")

        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0052 (1)")

    TestCaseEnd()

    TestCaseBegin("tftpWriteBinaryReq invalid signature")

        sendTftpwriteBinary(Address["PC"], 1, Bin["Bootloader"], "invalidSignature", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(25000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_INVALID_SIGNATURE"]) == true, "invalid signature")

        ValidateRequirement("CC7518_SRS_Bootloader0005 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0057 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0020 (1)")

    TestCaseEnd()

    TestCaseBegin("upload on target fail")

        sendTftpwriteBinary(Address["PC"], 2, Bin["Bootloader"], "SITARA_BL", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(13000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_DOWNLOAD_FAILURE"]) == true, "download failed")

    TestCaseEnd()

    TestCaseBegin("tftpWriteBinaryReq local valid")

        sendTftpwriteBinary(Address["PC"], 1, Bin["Bootloader"], "SITARA_BL", 0x1234)

        VerifyThat(VerifyPending(Cmd["tftpWriteBinary"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(10000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["tftpWriteBinary"]) == true, "Frame command valid")
        VerifyThat(checkSize(5) == true, "Frame size valid")
        VerifyThat(checkCode(17, Code["CODE_NONE"]) == true, "download valid")

        -- Reboot
        sendReset(Address["STM32"], 2)
        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        WaitMs(8000)

        ValidateRequirement("CC7518_SRS_Bootloader0003 (1)")
        ValidateRequirement("CC7518_SRS_Bootloader0004 (1)")
        ValidateRequirement("CC7518_SRS_Bootloader0006 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0046 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0048 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0049 (2)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0053 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0107 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0020 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0006 (1)")
        ValidateRequirement("CC7598_SRS_AM335x_Bootloader0039 (1)")
        ValidateRequirement("CC8050_SRS_Pump_Boot_Com0022 (1)")

    TestCaseEnd()

ScenarioEnd()