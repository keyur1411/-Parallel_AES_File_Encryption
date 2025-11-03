#include "IO.hpp"
#include "ReadEnv.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

ReadEnv::ReadEnv(){}
std::string ReadEnv::getTheEnv(){
  std::string env_path = ".env";
  IO io(env_path);
  std::fstream f_stream = io.getFileStream();
  std::stringstream buffer;
  buffer << f_stream.rdbuf();
  std::string content = buffer.str();
  return content;
}