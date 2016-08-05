#include <cstring>
#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "settings.h"

using namespace std;

int create_dir(string dirname) {
  mode_t mode = 0775;
  if (0 != mkdir(dirname.c_str(), mode)) {
    std::cout << "Failed to make directory: " << dirname << std::endl;
    return -errno;
  }
}

int count_dir(string dirname, string prefix) {
  uint32_t i = 0;
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(dirname.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (strncmp(ent->d_name, prefix.c_str(), 6) == 0) {
        i++;
      }
    }
  }
}

int touch(const string &filename) {
  int fd = open(filename.c_str(), O_CREAT|O_WRONLY, 0644);
  if (fd < 0)
    return -errno;
  else
    close (fd);
    return 0;
}

int make_objects(const string &directory, const string &prefix, uint32_t total_objects, uint32_t cur_objects, uint32_t split_threshold) {
  if (directory.length() + prefix.length() > PATH_MAX - 8 - 1)
     return -errno;
  if (total_objects > 99999999)
     return -errno;

  uint32_t file_count = count_dir(directory, prefix);

  for (uint32_t i = cur_objects; i < total_objects; i++) {
    if (file_count >= split_threshold) {
       std::cout << "should split" << std::endl;
//      split_objects(
    }
    char buf[PATH_MAX];
    snprintf(buf, sizeof buf, "%s/%s%08d", directory.c_str(), prefix.c_str(), i);
    int r = touch(buf);
    if (r < 0) {
      std::cout << "Failed to create file: " << buf << endl;
      return -errno;
    }
    cur_objects++;
    file_count++;
  }
  return cur_objects;
}

int move_object(const string &src, const string &dst) {
  std::cout << "Moving " << src << " to " << dst << std::endl;
  int r;
  r = link(src.c_str(), dst.c_str());
  if (r < 0) {
    std::cerr << "failed to link " << src << " to " << dst << std::endl;
    return -errno;
  }
  r = unlink(src.c_str());
  if (r < 0) {
    std::cerr << "failed to unlink " << src << std::endl;
    return -errno;
  }
  return 0;
}

int split_objects(const string &directory, const string &prefix, uint32_t split_threshold, uint32_t fanout) {
  DIR *dir;
  struct dirent *ent;
  std::vector<std::string> filenames;
  if ((dir = opendir(directory.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (strncmp(ent->d_name, prefix.c_str(), 6) == 0) {
        filenames.push_back(std::string(ent->d_name));
      } else {
        std::cout << "found but ignored: " << ent->d_name << std::endl;
      }
    }
    closedir(dir);
    std::cout << "files: " << filenames.size() << std::endl;

    if (split_threshold > filenames.size())
      return 0;

    // Now we actually do something with the directory list
    for (int i = 0; i < fanout; i++) {
      create_dir(directory + "/" + "DIR_" + std::to_string(i));
    }

    for(std::vector<std::string>::size_type i = 0; i < filenames.size(); i++) {
      uint32_t dir_num = i % fanout;
      std::string src = directory + "/" + filenames[i];
      std::string dst = directory + "/" + "DIR_" + std::to_string(dir_num) + "/" + filenames[i];
      move_object(src, dst);
    }
  } else {
    perror ("Failed to open directory");
    return -errno;
  }
}

int main(int argc, char *argv[]) {
  Settings* s = Settings::instance();
  s->initialize(argc, argv);
  int foo = make_objects(s->directory, "object", s->objects, 0, s->split_threshold);
  std::cout << "objects created: " << foo << endl;
  split_objects(s->directory, "object", s->split_threshold, 4);
}
