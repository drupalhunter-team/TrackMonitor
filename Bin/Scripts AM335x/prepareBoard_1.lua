require "Scripts Common/tables"
require "Scripts AM335x/functions"
require "Scripts AM335x/commands"

-- Need :
-- * Bootloader version = 0x100

-- * Update bootloader
sendTftpwriteBinary(Address["PC"], 1, Bin["Bootloader"], "SITARA_BL", 0x1234)
VerifyPending(Cmd["tftpWriteBinary"])
CheckFrameValid(10000)

-- * Invalid image on medical
sendPrepareWriteMedicalApp(Address["STM32"], 1789)
VerifyPending(Cmd["prepareWriteMedicalApplication"])
CheckFrameValid(10000)

-- * Valid image on manufacturing
sendTftpwriteBinary(Address["PC"], 1, Bin["Manufacturing"], "SITARA_APP", 0x1234)
VerifyPending(Cmd["tftpWriteBinary"])
CheckFrameValid(10000)

WaitMs(1000)

CloseCnx()  -- Au cas où