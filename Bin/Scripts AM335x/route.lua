require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

function route(address)
    SetUByteInTxBuffer(2, address[1])  
    SetUByteInTxBuffer(3, address[2])
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 1)   -- Size
    SetUByteInTxBuffer(12, Cmd["reset"])  -- Message protocol
    SetUByteInTxBuffer(13, 2)
    SendFrameWithSource(address, 14)

    VerifyThat(CheckFrameValid(2000, true) == true, "Frame valid")
    VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
    VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
    VerifyThat(checkSize(1) == true, "Frame size valid")
    VerifyThat(checkCode(13, 2) == true, "Frame content valid")
end

ScenarioBegin("route")

    TestCaseBegin("Route message")

    route(Address["STM32"])
    route(Address["PC"])

    ValidateRequirement("CC7598_SRS_AM335x_Bootloader0029 (1)")
    ValidateRequirement("CC7598_SRS_AM335x_Bootloader0030 (1)")
    ValidateRequirement("CC7601_SRS_Boot_Com0010 (2)")
    ValidateRequirement("CC7601_SRS_Boot_Com0011 (2)")

    TestCaseEnd()

ScenarioEnd()