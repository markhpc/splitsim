#include <iostream>
#include <string>
#include <string.h>
#include <limits.h>
#include "settings.h"

extern int opterr;

Settings* Settings::pInstance = NULL;

Settings* Settings::instance() {
  if (pInstance == NULL) {
    pInstance = new Settings();
  }
  return pInstance;
}

void Settings::initialize(int argc, char *argv[]) {
  // Parse command line options
  int c;
  opterr = 0;
  while((c =  getopt(argc, argv, "o:s:d:")) != EOF) {
    if (optarg == NULL) {
      std::cout << "Got NULL optarg for " << c << std::endl;
      continue;
    }
    switch (c) {
      case 'o':
        objects = strtol(optarg, NULL, 0);
        break;
      case 's':
        split_threshold = strtol(optarg, NULL, 0);
        break;
      case 'd':
        directory = strndup(optarg, PATH_MAX);
        break;
    }
  }
}

Settings::Settings() {}
