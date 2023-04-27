#include "netutils.h"

int main(int argc, char** argv)
{  
    InitServer();
    while(1)
    {
        int status = ProcServer();
        if (status < 0) break;
    }
    KillServer();
    return 0;
}    
