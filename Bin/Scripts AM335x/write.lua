require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

ScenarioBegin("Write")

    TestCaseBegin("Cancel write application")

        sendCmdWithoutParams(Address["STM32"], Cmd["cancelWriteApplication"])
        ret, size, ts = ReceiveFrame(300)

        sendCmdWithoutParams(Address["STM32"], Cmd["cancelWriteApplication"])

        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["cancelWriteApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_WRITE_REQ_IN_PROGESS"]) == true, "No write req in progress")

        ValidateRequirement("CC7601_SRS_Boot_Com0075 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0076 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0077 (1)")

    TestCaseEnd()

    TestCaseBegin("write medical application without prepare")

        sendWriteMedicalApp(Address["STM32"], 1789)

        VerifyThat(CheckFrameValid(100) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeMedicalApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_PREPARE_WRITE_MESSAGE"]) == true, "code none")

        ValidateRequirement("CC7601_SRS_Boot_Com0044 (1)")

    TestCaseEnd()

    TestCaseBegin("prepare write medical application")

        sendPrepareWriteMedicalApp(Address["STM32"], 1789)

        VerifyThat(VerifyPending(Cmd["prepareWriteMedicalApplication"]) == true, "Pending OK")

        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["prepareWriteMedicalApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "code none")

        ValidateRequirement("CC7601_SRS_Boot_Com0039 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0040 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0043 (1)")

    TestCaseEnd()

    TestCaseBegin("write manufactory application")

        sendWriteManufacturingApp(Address["STM32"], 1789)

        VerifyThat(CheckFrameValid(100) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeManufacturingApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_PREPARE_WRITE_MESSAGE"]) == true, "code none")

    TestCaseEnd()

    TestCaseBegin("write medical application")

        sendWriteMedicalApp(Address["STM32"], 1789)

        VerifyThat(CheckFrameValid(100) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["err"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "code none")

        ValidateRequirement("CC7601_SRS_Boot_Com0045 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0046 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0047 (1)")
        ValidateRequirement("CC7601_SRS_Boot_Com0048 (1)")

    TestCaseEnd()

    TestCaseBegin("cancel write medical application")

        -- Prepare write
        sendPrepareWriteMedicalApp(Address["STM32"], 1789)
        VerifyThat(VerifyPending(Cmd["prepareWriteMedicalApplication"]) == true, "Pending OK")
        VerifyThat(CheckFrameValid(3000) == true, "Frame valid")

        -- Cancel write
        sendCmdWithoutParams(Address["STM32"], Cmd["cancelWriteApplication"])
        VerifyThat(CheckFrameValid(200) == true, "Frame valid")
        VerifyThat(checkCodeId(Cmd["cancelWriteApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "No write req in progress")

        -- Write
        sendWriteMedicalApp(Address["STM32"], 1789)
        VerifyThat(CheckFrameValid(100) == true, "Frame valid")
        VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
        VerifyThat(checkCodeId(Cmd["writeMedicalApplication"]) == true, "Frame command valid")
        VerifyThat(checkSize(1) == true, "Frame size valid")
        VerifyThat(checkCode(13, Code["CODE_NO_PREPARE_WRITE_MESSAGE"]) == true, "code none")

        ValidateRequirement("CC7601_SRS_Boot_Com0078 (1)")

    TestCaseEnd()

ScenarioEnd()