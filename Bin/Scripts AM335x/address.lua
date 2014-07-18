require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

function route(destDeviceId, destBoardId, addressInterface)
    SetUByteInTxBuffer(2, destDeviceId)  
    SetUByteInTxBuffer(3, destBoardId)
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 1)   -- Size
    SetUByteInTxBuffer(12, Cmd["reset"])  -- Message protocol
    SetUByteInTxBuffer(13, 2)
    SendFrameWithSource(addressInterface, 14)

    VerifyThat(CheckFrameValid(2000, true) == true, "Frame valid")
    VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
    VerifyThat(checkCodeId(Cmd["reset"]) == true, "Frame command valid")
    VerifyThat(checkSize(1) == true, "Frame size valid")
    VerifyThat(checkCode(13, 2) == true, "Frame content valid")
end

function bootloaderVersionRouteLocal(destDeviceId, destBoardId, addressInterface)

    SetUByteInTxBuffer(2, destDeviceId)  
    SetUByteInTxBuffer(3, destBoardId)
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 0)   -- Size
    SetUByteInTxBuffer(12, Cmd["bootloaderVersion"])  -- Message protocol
    SendFrameWithSource(addressInterface, 13)

    VerifyThat(CheckFrameValid(200) == true, "Frame valid")
    VerifyThat(checkFrameNumber(0) == true, "Frame number valid")
    VerifyThat(checkCodeId(Cmd["bootloaderVersion"]) == true, "Frame command valid")
    VerifyThat(checkSize(2) == true, "Frame size valid")
end

ScenarioBegin("Address")


    TestCaseBegin("Test address local")

    bootloaderVersionRouteLocal(9, 0, Address["PC"])
    bootloaderVersionRouteLocal(10, 0, Address["PC"])
    bootloaderVersionRouteLocal(16, 0, Address["PC"])
    bootloaderVersionRouteLocal(32, 0, Address["PC"])

    ValidateRequirement("CC7601_SRS_Boot_Com0096 (1)")

    TestCaseEnd()

    --TestCaseBegin("Test address remote")

    --route(2, 1, Address["STM32"])
    --route(0, 0, Address["PC"])
    --route(0, 14, Address["PC"])
    --route(0, 30, Address["PC"])
    --route(3, 6, Address["STM32"])
    --route(8, 1, Address["STM32"])
    --route(9, 2, Address["STM32"])
    --route(10, 2, Address["STM32"])
    --route(11, 2, Address["STM32"])
    --route(18, 2, Address["STM32"])
    --route(27, 2, Address["STM32"])
    --route(32, 2, Address["STM32"])

    --ValidateRequirement("CC7601_SRS_Boot_Com0096")

    --TestCaseEnd()

ScenarioEnd()