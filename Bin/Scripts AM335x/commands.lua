function sendCmdWithoutParams(src, cmd)
    SetUByteInTxBuffer(2, 9)  
    SetUByteInTxBuffer(3, 0)
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 0)   -- Size
    SetUByteInTxBuffer(12, cmd)  -- Message protocol
    SendFrameWithSource(src, 13)
end

function sendReset(src, mode)
    SetUByteInTxBuffer(2, 9)  
    SetUByteInTxBuffer(3, 0)
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 1)   -- Size
    SetUByteInTxBuffer(12, Cmd["reset"])  -- Message protocol
    SetUByteInTxBuffer(13, mode)  -- Preset mode
    SendFrameWithSource(src, 14)
end

function sendResetPump(src, mode)
    SetUByteInTxBuffer(2, 9)  
    SetUByteInTxBuffer(3, 0)
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 1)   -- Size
    SetUByteInTxBuffer(12, Cmd["resetPump"])  -- Message protocol
    SetUByteInTxBuffer(13, mode)  -- Preset mode
    SendFrameWithSource(src, 14)
end

function sendWriteMedicalApp(src, data)      -- todo change data
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 4)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeMedicalApplication"])  -- Message protocol
        SetULongInTxBuffer(13, data)   -- Image size
        SendFrameWithSource(src, 17)
end

function sendReadParam(src, addr, size)     
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 4)   -- Size
        SetUByteInTxBuffer(12, Cmd["readOperationalParameter"])  -- Message protocol
        SetUShortInTxBuffer(13, addr)   -- Address 43
        SetUShortInTxBuffer(15, size)   -- Size 5
        SendFrameWithSource(src, 17)
end

function sendWriteManufacturingApp(src, data)        -- todo change 
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 4)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeManufacturingApplication"])  -- Message protocol
        SetULongInTxBuffer(13, data)   -- Image size
        SendFrameWithSource(src, 17)
end

function sendWriteBootloaderApp(src, frameNb)        -- todo change 
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, frameNb)  -- Frame number
        SetULongInTxBuffer(8, 8192)   -- Size
        SetUByteInTxBuffer(12, Cmd["writeBootloader"])  -- Message protocol
        SetUByteInTxBuffer(13, 1)
        SetUByteInTxBuffer(13, 1)
        SetUByteInTxBuffer(23, 1)   -- set target 1
        SendFrameWithSource(src, 8205)
end

function sendPrepareWriteMedicalApp(src, size)
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 4)   -- Size
        SetUByteInTxBuffer(12, Cmd["prepareWriteMedicalApplication"])  -- Message protocol
        SetULongInTxBuffer(13, size)   -- Image size
        SendFrameWithSource(src, 17)
end

function sendPrepareWriteManufacturingApp(src, size) 
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 4)   -- Size
        SetUByteInTxBuffer(12, Cmd["prepareWriteManufacturingApplication"])  -- Message protocol
        SetULongInTxBuffer(13, size)   -- Image size
        SendFrameWithSource(src, 17)
end

function sendPrepareWriteBootloader(src, size) 
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 4)   -- Size
        SetUByteInTxBuffer(12, Cmd["prepareWriteBootloader"])  -- Message protocol
        SetULongInTxBuffer(13, size)   -- Image size
        SendFrameWithSource(src, 17)
end

function sendTftpwriteBinary(src, dst, app, name, size)
    SetUByteInTxBuffer(2, 9)  
    SetUByteInTxBuffer(3, 0)
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 2 + 2 + 4 + 32)   -- Size
    SetUByteInTxBuffer(12, 0x11)  -- Message protocol
    SetUShortInTxBuffer(13, dst)   
    SetUShortInTxBuffer(15, app)   
    SetULongInTxBuffer(17, size)

    for i = 1, 32 do
      SetUByteInTxBuffer(20 + i, 0)
    end

    for i = 1, string.len(name) do
      char = string.sub(name, i, i);
      SetUByteInTxBuffer(20 + i, string.byte(char))
    end

    SendFrameWithSource(src, 53)
end

function emulateRM48STM32Synchro(valid)

        SetUByteInTxBuffer(0, 9)   -- from STM32
        SetUByteInTxBuffer(1, 2)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 2)   -- Size
        SetUByteInTxBuffer(12, 0x1)  -- Message protocol
        SetUShortInTxBuffer(13, 0x100)  
        SendFrame(15)

        WaitMs(3)

        SetUByteInTxBuffer(0, 9)   -- from RM48
        SetUByteInTxBuffer(1, 1)   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 2)   -- Size
        SetUByteInTxBuffer(12, 0x1)  -- Message protocol
        if (valid) then
            SetUShortInTxBuffer(13, 0x100)
        else
            SetUShortInTxBuffer(13, 0x108)
        end
        SendFrame(15)
end

function emulateRemoteRep(src, cmd, code, pending)

    if (pending == true) then
        WaitMs(150)

        SetUByteInTxBuffer(0, src[1])   -- from STM32 or RM48
        SetUByteInTxBuffer(1, src[2])   
        SetUByteInTxBuffer(2, 9)  
        SetUByteInTxBuffer(3, 0)
        SetUShortInTxBuffer(4, 1)  -- Protocol ID
        SetUShortInTxBuffer(6, 0)  -- Frame number
        SetULongInTxBuffer(8, 1)   -- Size
        SetUByteInTxBuffer(12, cmd)  -- Message protocol
        SetUByteInTxBuffer(13, Code["CODE_PENDING"])  
        SendFrame(14)
    end

    WaitMs(150)

    SetUByteInTxBuffer(0, src[1])   -- from STM32 or RM48
    SetUByteInTxBuffer(1, src[2])   
    SetUByteInTxBuffer(2, 9)  
    SetUByteInTxBuffer(3, 0)
    SetUShortInTxBuffer(4, 1)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 1)   -- Size
    SetUByteInTxBuffer(12, cmd)  -- Message protocol
    SetUByteInTxBuffer(13, code)  
    SendFrame(14)

    WaitMs(150)
end