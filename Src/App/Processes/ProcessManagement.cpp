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
#include <sys/mman.h>
#include <sys/fcntl.h>
#endif
#include "../encryptDecrypt/Cryption.hpp"
#include <atomic>
#include <semaphore.h>
#include <thread>

ProcessManagement::ProcessManagement()
{
   itemsSemaphore = sem_open("/item_semaphore",O_CREAT,0666,0);
   emptySlotsSemaphore = sem_open("/empty_slots_semaphore",O_CREAT,0666,1000);

  shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  ftruncate(shmFd, sizeof(SharedMemory));
  sharedMem = static_cast<SharedMemory *>(mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
  sharedMem->front = 0;
  sharedMem->rear = 0;
  sharedMem->size.store(0);
}
bool ProcessManagement::submitToQueue(std::unique_ptr<Task> &task)
{
  sem_wait(emptySlotsSemaphore);
  std::unique_lock<std::mutex> lock(queueLock);
  if (sharedMem->size.load() >= 1000)
  {
    return false;
  }
  strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());
  sharedMem->rear = (sharedMem->rear + 1) % 1000;
  sharedMem->size.fetch_add(1);
  lock.unlock();
  sem_post(itemsSemaphore);

  std::thread thread1(&ProcessManagement::executeTasks,this);
  // int pid = fork();
  // if (pid < 0)
  // {
  //   return false;
  // }
  // else if (pid > 0)
  // {
  //   std::cout << "Entering Parent Process" << std::endl;
  // }
  // else
  // {
  //   std::cout << "Entering Child Process" << std::endl;
  //   executeTasks();
  //   std::cout << "Exiting Child Process" << std::endl;
  //   exit(0);
  // }
  thread1.detach();
  // thread1.join();
  return true;
}
void ProcessManagement::executeTasks()
{
  auto start_time = std::chrono::high_resolution_clock::now();
  sem_wait(itemsSemaphore);
  std::unique_lock<std::mutex>lock(queueLock);
  char taskStr[256];
  strcpy(taskStr,sharedMem->tasks[sharedMem->front]);
  sharedMem->front = (sharedMem->front+1)%1000;
  sharedMem->size.fetch_sub(1);
  lock.unlock();
  sem_post(emptySlotsSemaphore);
  std::cout << "Excuting task: " << taskStr << std::endl;
  excecuteCryption(taskStr);

  auto end_time = std::chrono::high_resolution_clock::now();

std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
std::cout << "--- Task Finished: " << taskStr << " ---" << std::endl;
std::cout << "    Time taken: " << duration_ms.count() << " ms" << std::endl;
std::cout << "-----------------------------------" << std::endl;

}

ProcessManagement::~ProcessManagement(){
  munmap(sharedMem,sizeof(SharedMemory));
  shm_unlink(SHM_NAME);
    sem_close(itemsSemaphore);
    sem_close(emptySlotsSemaphore);
    sem_unlink("/items_semaphore");
    sem_unlink("/empty_slots_semaphore");
}
