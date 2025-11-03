#include "ProcessManagement.hpp"
#include "Task.hpp"
#include <queue>
#include <memory>
#include <iostream>
#include <cstring>
#if defined(_WIN32) || defined(_WIN64)
// Windows does not provide <sys/wait.h>; the POSIX wait APIs are not available.
// Include <sys/wait.h> only on POSIX systems.
#else
#include <sys/wait.h>
#endif
#include "../encryptDecrypt/Cryption.hpp"

ProcessManagement::ProcessManagement(){}
bool ProcessManagement::submitToQueue(std::unique_ptr<Task> &task){
  taskQueue.push(std::move(task));
  return true;
}
void ProcessManagement::executeTasks(){
  while(!taskQueue.empty()){
    std::unique_ptr<Task> taskToExcecute = std::move(taskQueue.front());
    taskQueue.pop();
    const std::string task_to_exe = taskToExcecute->toString();
    std::cout<<"Excuting task: "<<task_to_exe<<std::endl;
    excecuteCryption(task_to_exe);
  }
}




