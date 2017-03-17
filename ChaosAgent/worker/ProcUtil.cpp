/*
 *	ProcUtil.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/03/2017 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "ProcUtil.h"
#include <chaos/common/global.h>

#define READ   0
#define WRITE  1

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace chaos::agent::worker;

FILE * ProcUtil::popen2(const std::string& command,
                        const std::string& type,
                        int & pid) {
    pid_t child_pid;
    int fd[2];
    pipe(fd);
    
    if((child_pid = fork()) == -1)     {
        perror("fork");
        exit(0);
    }
    
    /* child process */
    if (child_pid == 0) {
        if (type == "r") {
            close(fd[READ]);    //Close the READ end of the pipe since the child's fd is write-only
            dup2(fd[WRITE], 1); //Redirect stdout to pipe
        } else {
            close(fd[WRITE]);    //Close the WRITE end of the pipe since the child's fd is read-only
            dup2(fd[READ], 0);   //Redirect stdin to pipe
        }
        setpgid(0, 0); //Needed so negative PIDs can kill children of /bin/sh
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        exit(0);
    } else {
        if (type == "r") {
            close(fd[WRITE]); //Close the WRITE end of the pipe since parent's fd is read-only
        } else {
            close(fd[READ]); //Close the READ end of the pipe since parent's fd is write-only
        }
    }
    
    pid = child_pid;
    if (type == "r") {
        return fdopen(fd[READ], "r");
    }
    return fdopen(fd[WRITE], "w");
}

bool ProcUtil::popen2NoPipe(const std::string& command,
                            int & pid) {
    if((pid = fork()) == -1)     {
        return false;
    }
    
    /* child process */
    if (pid == 0) {
        setpgid(0, 0); //Needed so negative PIDs can kill children of /bin/sh
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        exit(0);
    }
    return true;
}

bool ProcUtil::popen2ToNamedPipe(const std::string& command,
                                 const std::string& named_pipe) {
    int pid, sid = 0;
    if((pid = fork()) == -1)     {
        return false;
    }
    
    if (pid == 0) {
        /* child process */
        if((pid = fork()) < 0){
            //error forking
            exit(EXIT_FAILURE);
        } else if(pid > 0) {
            //firs child exis because became parent
            exit(0);
        }
        
        //second child launch the process
        int fd = 0;
        
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
            exit(EXIT_FAILURE);
        }
        
        //redirect standard output and error to the named pipe
        if((fd = open(named_pipe.c_str(), O_RDWR | O_CREAT,S_IRUSR | S_IWUSR))==-1){ /*open the file */
            perror("open");
            return 1;
        }
        
        dup2(fd,STDOUT_FILENO); /*copy the file descriptor fd into standard output*/
        dup2(fd,STDERR_FILENO); /* same, for the standard error */
        close(fd); /* close the file descriptor as we don't need it more  */
        
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
    }
    return true;
}

int ProcUtil::pclose2(FILE * fp,
                      pid_t pid,
                      bool wait_pid) {
    int stat;
    fclose(fp);
    if(wait_pid) {
        while (waitpid(pid, &stat, 0) == -1) {
            if (errno != EINTR) {
                stat = -1;
                break;
            }
        }
    }
    return stat;
}

int ProcUtil::createNamedPipe(const std::string& named_pipe_path) {
    mkfifo(named_pipe_path.c_str(), 0666);
    return errno;
}

int ProcUtil::removeNamedPipe(const std::string& named_pipe_path) {
    unlink(named_pipe_path.c_str());
    return errno;
}
