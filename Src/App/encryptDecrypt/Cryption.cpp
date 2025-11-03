#include "Cryption.hpp"
#include "../Processes/Task.hpp"
#include "../FileHandling/ReadEnv.hpp"
#include <string>

int excecuteCryption(const std::string &taskData){
  Task task = Task::fromString(taskData);
  ReadEnv env;
  const std::string env_Key = env.getTheEnv();
  const int key = std::stoi(env_Key);
  if(task.action == Action::ENCRYPT){
    char ch;
    while(task.f_stream.get(ch)){
      ch = (ch+key)%256;
      task.f_stream.seekp(-1,std::ios::cur);
      task.f_stream.put(ch);
    }
    task.f_stream.close();
  }else {
    char ch;
    while(task.f_stream.get(ch)){
      ch = (ch-(key%256) +256)%256;
      task.f_stream.seekp(-1,std::ios::cur);
      task.f_stream.put(ch);
    }
    task.f_stream.close();
  }
  return 0;
}
