#pragma once

#include <fstream>
#include <iostream>
#include <pwd.h>
#include <string>
#include <unistd.h>

#ifdef DEBUG

template <typename... T>
void Log(const char *file, int line, const char *functionName,
         const T &...args) {
  std::cout << "LOG " << file << ":" << line << " (" << functionName << "):";
  ((std::cout << " " << args), ...);
  std::cout << std::endl;
}

#define LOG(...) Log(__FILE__, __LINE__, __func__, __VA_ARGS__)

#else
#define LOG(...)
#endif

static std::string getHomeDirectory() {
  const char *home = std::getenv("HOME");
  if (home)
    return home;

  struct passwd *pw = getpwuid(getuid());
  return pw ? pw->pw_dir : "";
}