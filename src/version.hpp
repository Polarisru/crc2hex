#pragma once

#ifndef JOB_BASE_NAME
  #define JOB_BASE_NAME "unknown"
#endif

#ifndef BUILD_NUMBER
  #define BUILD_NUMBER "unknown"
#endif

#ifndef GIT_COMMIT
  #define GIT_COMMIT "unknown"
#endif

#ifndef SW_VERSION
  #define SW_VERSION "unknown"
#endif

class Version {
public:
  static void print()
  {
    std::cout << "  Version:\t" << SW_VERSION << std::endl;
    std::cout << "  Build number:\t" << BUILD_NUMBER << std::endl;
    std::cout << "  Git commit:\t" << GIT_COMMIT << std::endl;
  }
};
