#pragma once

namespace volz
{
/// @brief a class that defines the error numbers and texts.
class CErrorNumber
{
public:

    /// defines a unique number for all errors
    enum class number {
        ok,                     ///< No Error
        deviceAlreadyOpen,      ///< device is already opened
        failedToOpenDevice,     ///< failed to open device
        openPipeFailed,         ///< open pipe failed
        fileNotFound,           ///< file not found
        noSuchDevice,           ///< no such device
        noSuchPowerControl,     ///< no such power control
        deviceIsBusy,           ///< device is busy
        noSuchProtocol,         ///< no such protocol
        noSuchLoader,           ///< no such loader
        noSuchDriver,           ///< no such driver
        invalidCommand,         ///< invalid command
        jobCannotBeKilled,      ///< job cannot be killed
        noSuchJob,              ///< no such job
        notSupported,           ///< not supported by this driver
        positionNotSupported,   ///< position not supported by this driver
        noSuchParameter,        ///< there is no such parameter in this driver
        statusNotInitialized,   ///< the status is not yet initialized
        statusTestFailed,       ///< the status is not as expected
        statusTestWrongType,    ///< the status is called with a value of wrong type
        valueNotAllowed,        ///< the value is not valid for that parameter
        deviceConnected,        ///< the device is connected to a protocol
        invalidStack,           ///< the stack does not exist
        timeToLong,             ///< the selected time value is to long
        timeMustBePos,          ///< the selected time must be positive
        invalidPosType,         ///< the position type is not supported by this driver
        hexfileError,           ///< the hexfile cannot be read
        notAllowedInState,      ///< the Command is not allowed in this state
        speedNotSupported,      ///< the selected speed is not supported
        noCanHardwareFound,     ///< No CAN Hardware found
        noCanDriverFound,       ///< Driver not loaded
        bootloaderNoSync,       ///< Bootloader can't sync
        bootloaderWriteFails,   ///< Bootloader can't write the firmware
        bootloaderVerifyFails,  ///< Bootloader can't verify the firmware
        bootloaderSettFails,    ///< Bootloader can't write settings
        mapfileError,           ///< the map file cannot be read
    };

    /// converts the number into a text
    static const char* name(CErrorNumber::number number)
    {
        switch (number)
        {
        case CErrorNumber::number::ok:                    return "ok";
        case CErrorNumber::number::deviceAlreadyOpen:     return "device is already opened";
        case CErrorNumber::number::failedToOpenDevice:    return "failed to open device";
        case CErrorNumber::number::openPipeFailed:        return "open pipe failed";
        case CErrorNumber::number::fileNotFound:          return "file not found";
        case CErrorNumber::number::noSuchDevice:          return "no such device";
        case CErrorNumber::number::deviceIsBusy:          return "device is busy";
        case CErrorNumber::number::noSuchProtocol:        return "no such protocol";
        case CErrorNumber::number::noSuchLoader:          return "no such loader";
        case CErrorNumber::number::noSuchDriver:          return "no such driver";
        case CErrorNumber::number::noSuchPowerControl:    return "no such power control";
        case CErrorNumber::number::invalidCommand:        return "invalid command";
        case CErrorNumber::number::jobCannotBeKilled:     return "job cannot be killed";
        case CErrorNumber::number::noSuchJob:             return "no such job";
        case CErrorNumber::number::notSupported:          return "not supported by this driver";
        case CErrorNumber::number::positionNotSupported:  return "position not supported by this driver";
        case CErrorNumber::number::noSuchParameter:       return "there is no such parameter in this driver";
        case CErrorNumber::number::deviceConnected:       return "the device is connected to a protocol";
        case CErrorNumber::number::invalidStack:          return "the stack does not exist";
        case CErrorNumber::number::timeToLong:            return "the selected time value is to long";
        case CErrorNumber::number::timeMustBePos:         return "the selected time must be positive";
        case CErrorNumber::number::invalidPosType:        return "the position type is not supported by this driver";
        case CErrorNumber::number::hexfileError:          return "the HEX file cannot be read";
        case CErrorNumber::number::notAllowedInState:     return "the Command is not allowed in this state";
        case CErrorNumber::number::speedNotSupported:     return "the selected speed is not supported";
        case CErrorNumber::number::noCanHardwareFound:    return "No CAN Hardware found";
        case CErrorNumber::number::noCanDriverFound:      return "Driver not loaded";
        case CErrorNumber::number::valueNotAllowed:       return "the value is not valid for that parameter";
        case CErrorNumber::number::statusNotInitialized:  return "the status is not yet initialized";
        case CErrorNumber::number::statusTestFailed:      return "the status is not as expected";
        case CErrorNumber::number::statusTestWrongType:   return "the status is called with a value of wrong type";
        case CErrorNumber::number::bootloaderNoSync:      return "bootloader can't synchronize";
        case CErrorNumber::number::bootloaderWriteFails:  return "bootloader can't write the firmware";
        case CErrorNumber::number::bootloaderVerifyFails: return "bootloader can't verify the firmware";
        case CErrorNumber::number::bootloaderSettFails:   return "bootloader can't write settings";
        case CErrorNumber::number::mapfileError:          return "the map file cannot be read";
        }
        return "unknown error";
    }
};
}
