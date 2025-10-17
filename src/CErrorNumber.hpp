#pragma once

class CErrorNumber
{
public:

    /// defines a unique number for all errors
    enum class number {
        ok,                     ///< No Error
        openPipeFailed,         ///< open pipe failed
        fileNotFound,           ///< file not found
        fileNotWritten,         ///< can't write to file
        invalidCommand,         ///< invalid command
        noSuchParameter,        ///< there is no such parameter in this driver
        valueNotAllowed,        ///< the value is not valid for that parameter
        hexfileError,           ///< the hexfile cannot be read
        wrongParameter,         ///< Wrong parameter
    };

    /// converts the number into a text
    static const char* name(CErrorNumber::number number)
    {
        switch (number)
        {
        case CErrorNumber::number::ok:                    return "ok";
        case CErrorNumber::number::openPipeFailed:        return "open pipe failed";
        case CErrorNumber::number::fileNotFound:          return "file not found";
        case CErrorNumber::number::fileNotWritten:        return "can't write to file";
        case CErrorNumber::number::invalidCommand:        return "invalid command";
        case CErrorNumber::number::noSuchParameter:       return "there is no such parameter in this driver";
        case CErrorNumber::number::valueNotAllowed:       return "the value is not valid for that parameter";
        case CErrorNumber::number::hexfileError:          return "the HEX file cannot be read";
        case CErrorNumber::number::wrongParameter:        return "wrong parameter";
        }
        return "unknown error";
    }
};
