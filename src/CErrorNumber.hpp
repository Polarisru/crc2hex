#pragma once

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

class CErrorNumber : public std::exception {
public:
    CErrorNumber(number err)
        : error_code(err) {
        switch (err)
        {
          case number::ok:                    message = "ok"; break;
          case number::openPipeFailed:        message = "open pipe failed"; break;
          case number::fileNotFound:          message = "file not found"; break;
          case number::fileNotWritten:        message = "can't write to file"; break;
          case number::invalidCommand:        message = "invalid command"; break;
          case number::noSuchParameter:       message = "there is no such parameter in this driver"; break;
          case number::valueNotAllowed:       message = "the value is not valid for that parameter"; break;
          case number::hexfileError:          message = "the HEX file cannot be read"; break;
          case number::wrongParameter:        message = "wrong parameter"; break;
          default:                            message = "unknown error"; break;
        }
    }

    CErrorNumber(number err, const std::string& msg)
        : error_code(err), message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

    int getErrorCode() const { return static_cast<int>(error_code); }

private:
    number error_code;
    std::string message;
};
