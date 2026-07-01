#include "cmd_version.h"
#include "../../src/common/version.h"
#include <iostream>

int cmd_version(){
    std::cout<<"Bery "<<BERY_VERSION<<"\n";
    return 0;
}