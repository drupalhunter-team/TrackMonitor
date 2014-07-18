require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

-- Need :
-- * Bootloader version = 0x100

-- * Invalid image on manufacturing
sendPrepareWriteManufacturingApp(Address["STM32"], 1789)
VerifyPending(Cmd["prepareWriteManufacturingApplication"])
CheckFrameValid(10000)

-- * Valid image on medical
sendTftpwriteBinary(Address["PC"], 1, Bin["Medical"], "SITARA_APP", 0x1234)
VerifyPending(Cmd["tftpWriteBinary"])
CheckFrameValid(10000)

WaitMs(1000)

CloseCnx()  -- Au cas où