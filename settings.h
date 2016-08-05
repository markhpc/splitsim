#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>

class Settings {
  public:
    uint32_t objects = 320;
    uint32_t split_threshold = 320;
    std::string directory = "./";
    static Settings* instance();
    void initialize(int argc, char *argv[]);
    ~Settings();
  protected:
    Settings(); // constructor
  private:
    static Settings* pInstance;
};
#endif /* SETTINGS_H_ */
