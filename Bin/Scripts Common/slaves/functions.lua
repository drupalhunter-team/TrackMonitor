require "Scripts Common/tables"

function ValidateRequirement(req)
    print(req)
    io.output(io.open("requirement.txt","a"))
    io.write(req)
    io.write("\n")
    io.close()
end

function ClearRequirement()
    io.output(io.open("requirement.txt","w"))
    io.close()
end

function CheckFrameValid(timemout, close)
    ret, size, ts = ReceiveFrame(timemout)
    if (close) then
        CloseCnx()
    end

    if (ret ~= 0) then 
        return false
    end
    
    ret8, val8 = GetULongInRxBuffer(8)
    frameSize = val8 + 13

    if ((frameSize == size) == true) then
        return true
    else
        return false
    end
end

function checkFrameNumber(frameNb)
    ret6, val6 = GetUShortInRxBuffer(6)
    if (val6 == frameNb) then
        return true
    else
        return false
    end
end

function checkCodeId(codeId)
    ret12, val12 = GetUByteInRxBuffer(12)
    if (val12 == codeId) then
        return true
    else
        return false
    end
end

function checkSize(size)
    ret8, val8 = GetUByteInRxBuffer(8)
    if (val8 == size) then
        return true
    else
        return false
    end
end

function checkCode(position, code)
    ret, val = GetUByteInRxBuffer(position)
    if (val == code) then
        return true
    else
        return false
    end
end

function VerifyPending(cmd)
    if (CheckFrameValid(2000, false) == true and
        checkFrameNumber(0) == true and
        checkCodeId(cmd) == true and
        checkSize(1) == true and
        checkCode(13, Code["CODE_PENDING"]) == true) then
            return true
    else
        return false
    end
end