require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

function checkPresetValueInEeprom(val)
    sendReset(Address["PC"], val)
    CheckFrameValid(2000)

    WaitMs(8000)
    sendReadParam(Address["PC"], 1, 1)

    VerifyThat(CheckFrameValid(1000) == true, "Frame valid")
    VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
    VerifyThat(checkCodeId(Cmd["readOperationalParameter"]) == true, "Frame command valid")
    VerifyThat(checkSize(2) == true, "Frame size valid")
    VerifyThat(checkCode(13, Code["CODE_NONE"]) == true, "Frame size valid")
    ret14, val14 = GetUByteInRxBuffer(14)
    VerifyThat(val14 == 0, "Preset mode reset to default mode")
end


ScenarioBegin("Preset mode check")

    TestCaseBegin("Preset mode check value in eeprom")

    checkPresetValueInEeprom(0)
    checkPresetValueInEeprom(1)
    checkPresetValueInEeprom(2)
    checkPresetValueInEeprom(3)

    ValidateRequirement("CC7598_SRS_AM335x_Bootloader0022 (1)")
    ValidateRequirement("CC7601_SRS_Boot_Com0062 (1)")

    TestCaseEnd()
 
ScenarioEnd()
