require "Scripts Common/tables"

function sendBootversionReq(src, dest)
    SetUByteInTxBuffer(0, src[1])  -- Source device ID
    SetUByteInTxBuffer(1, src[2])  -- Source board ID
    SetUByteInTxBuffer(2, dest[1]) -- Destination device ID
    SetUByteInTxBuffer(3, dest[2]) -- Destination board ID
    SetUShortInTxBuffer(4, ProtocolID) -- Protocol ID
    SetUShortInTxBuffer(6, 0) -- Frame number
    SetULongInTxBuffer(8, 0)  -- Data size
    SetUByteInTxBuffer(12, Cmd["bootloaderVersion"]) -- Message Id
    SendFrame(13)
end

function sendCmdWithoutParams(src, dest, cmd)
    SetUByteInTxBuffer(0, src[1])
    SetUByteInTxBuffer(1, src[2])
    SetUByteInTxBuffer(2, dest[1])
    SetUByteInTxBuffer(3, dest[2])
    SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 0)   -- Size
    SetUByteInTxBuffer(12, cmd)  -- Message protocol
    SendFrame(13)
end

function sendReset(src, dest, mode)
    SetUByteInTxBuffer(0, src[1])
    SetUByteInTxBuffer(1, src[2])
    SetUByteInTxBuffer(2, dest[1])
    SetUByteInTxBuffer(3, dest[2])
    SetUShortInTxBuffer(4, ProtocolID)            -- Protocol ID
    SetUShortInTxBuffer(6, 0)            -- Frame number
    SetULongInTxBuffer(8, 1)             -- Size
    SetUByteInTxBuffer(12, Cmd["reset"]) -- Message protocol
    SetUByteInTxBuffer(13, mode)         -- Preset mode
    SendFrame(14)
end

function sendWriteMedicalApp(src, dest, data)
    SetUByteInTxBuffer(0, src[1])
    SetUByteInTxBuffer(1, src[2])
    SetUByteInTxBuffer(2, dest[1])
    SetUByteInTxBuffer(3, dest[2])
    SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 4)   -- Size
    SetUByteInTxBuffer(12, Cmd["writeMedicalApplication"])  -- Message protocol
    SetULongInTxBuffer(13, data)   -- Image size
    SendFrameWithSource(src, 17)
end

function sendReadParam(src, dest, addr, size)
    SetUByteInTxBuffer(0, src[1])
    SetUByteInTxBuffer(1, src[2])
    SetUByteInTxBuffer(2, dest[1])
    SetUByteInTxBuffer(3, dest[2])
    SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 4)   -- Size
    SetUByteInTxBuffer(12, Cmd["readOperationalParameter"])  -- Message protocol
    SetUShortInTxBuffer(13, addr)   -- Address 43
    SetUShortInTxBuffer(15, size)   -- Size 5
    SendFrameWithSource(src, 17)
end

function sendWriteManufacturingApp(src, dest, data)
    SetUByteInTxBuffer(0, src[1])
    SetUByteInTxBuffer(1, src[2])
    SetUByteInTxBuffer(2, dest[1])
    SetUByteInTxBuffer(3, dest[2])
    SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 4)   -- Size
    SetUByteInTxBuffer(12, Cmd["writeManufacturingApplication"])  -- Message protocol
    SetULongInTxBuffer(13, data)   -- Image size
    SendFrameWithSource(src, 17)
end

function sendPrepareWriteMedicalApp(src, dest, size)
    SetUByteInTxBuffer(0, src[1])
    SetUByteInTxBuffer(1, src[2])
    SetUByteInTxBuffer(2, dest[1])
    SetUByteInTxBuffer(3, dest[2])
    SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 4)   -- Size
    SetUByteInTxBuffer(12, Cmd["prepareWriteMedicalApplication"])  -- Message protocol
    SetULongInTxBuffer(13, size)   -- Image size
    SendFrameWithSource(src, 17)
end

function sendPrepareWriteManufacturingApp(src, dest, size)
    SetUByteInTxBuffer(0, src[1])
    SetUByteInTxBuffer(1, src[2])
    SetUByteInTxBuffer(2, dest[1])
    SetUByteInTxBuffer(3, dest[2])
    SetUShortInTxBuffer(4, ProtocolID)  -- Protocol ID
    SetUShortInTxBuffer(6, 0)  -- Frame number
    SetULongInTxBuffer(8, 4)   -- Size
    SetUByteInTxBuffer(12, Cmd["prepareWriteManufacturingApplication"])  -- Message protocol
    SetULongInTxBuffer(13, size)   -- Image size
    SendFrameWithSource(src, 17)
end
