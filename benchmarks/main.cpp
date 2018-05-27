#include "SimicsHeader.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

int main(int argCount, char** argString)
{
    assert(argCount == 2);
    
    std::string workingPath = argString[1];
    int threadCounter = 1;
    std::vector<FILE*> channel;
    std::string line;
    
    while (getline(std::cin, line))
    {
        std::istringstream linestream(line);
        std::string nodeName;
        int proc;
        std::cout << "TASK_DESC_" << channel.size() << std::endl;
        linestream >> nodeName;
        linestream >> proc;
        std::string rest;
        getline(linestream, rest);
        std::stringstream ss;
        ss << "cd " << workingPath << "/; ";
        ss << workingPath << "/" << nodeName << " " << proc << " " << proc + 1;
        std::cout << workingPath << "/" << nodeName << " " << proc << " " << proc + 1 << std::endl;

        fflush(NULL);
        FILE* f = popen(ss.str().c_str(), "w");
        assert(f);
        rest += "\n";
        std::cout << rest;
        assert(fputs(rest.c_str(), f) != EOF);
        fflush(NULL);
        fflush(f);
        channel.push_back(f);
        threadCounter++;
    }

    for (size_t i = 0; i < channel.size(); i++)
    {
        pclose(channel[i]);
    }
}
