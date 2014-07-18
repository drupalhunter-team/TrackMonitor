require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands" 

ScenarioBegin("Flip flop")

    TestCaseBegin("Prepare write bootloader and reset")

        sendPrepareWriteBootloader(Address["PC"], 1789)
        VerifyThat(VerifyPending(Cmd["prepareWriteBootloader"]) == true, "Pending")

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareWriteBootloader"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "code none")

        sendReset(Address["PC"], 2)

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")

        WaitMs(8000)

        -- Check the board has reboot
        sendCmdWithoutParams(Address["PC"], Cmd["cancelWriteApplication"])

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["cancelWriteApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_WRITE_REQ_IN_PROGESS"]) == true, "No write req in progress")

        ValidateRequirement("CC7601_SRS_Boot_Com0026 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0027 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0028 (1)")

    TestCaseEnd()

    TestCaseBegin("Write bad bootloader without prepare")

        sendWriteBootloaderApp(Address["STM32"], 0)
        VerifyThat(CheckFrameValid(100) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_PREPARE_WRITE_MESSAGE"]) == true, "No prepare write req")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "Frame command valid")

        ValidateRequirement("CC7601_SRS_Boot_Com0031 (1)")

    TestCaseEnd()

    TestCaseBegin("Invalid image size")

        sendPrepareWriteBootloader(Address["STM32"], 7000)
        VerifyThat(VerifyPending(Cmd["prepareWriteBootloader"]) == true, "Pending")

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareWriteBootloader"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "code none")

        sendWriteBootloaderApp(Address["STM32"], 0)

        VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "Frame command valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_IMAGE_SIZE"]) == true, "Code none")

        ValidateRequirement("CC7601_SRS_Boot_Com0034 (1)")

    TestCaseEnd()

    TestCaseBegin("Write bad bootloader and reset")

        sendPrepareWriteBootloader(Address["STM32"], 16384)
        VerifyThat(VerifyPending(Cmd["prepareWriteBootloader"]) == true, "Pending")

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareWriteBootloader"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "code none")

        sendWriteBootloaderApp(Address["STM32"], 0)

        VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "Frame command valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Code none")

        sendWriteBootloaderApp(Address["STM32"], 1)

        VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
        VerifyThat(checkCodeId(Cmd["writeBootloader"]) == true, "Frame command valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCode(13, Code["CODE_INVALID_IMAGE"]) == true, "End of write invalid image")

        sendReset(Address["STM32"], 2)

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Code none")

        WaitMs(8000)

        -- Check the board has reboot
        sendCmdWithoutParams(Address["PC"], Cmd["cancelWriteApplication"])

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["cancelWriteApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_WRITE_REQ_IN_PROGESS"]) == true, "No write req in progress")

        ValidateRequirement("CC7601_SRS_Boot_Com0029 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0030 (1)")
        ValidateRequirement("CC7518_SRS_Bootloader0036 (2)")
        ValidateRequirement("CC7601_SRS_Boot_Com0032 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0033 (1)")

    TestCaseEnd()

ScenarioEnd()