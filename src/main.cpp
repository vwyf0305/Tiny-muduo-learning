#include"Timestamp.h"
#include<fmt/core.h>

int main(int argc, char* argv[])
{
     fmt::print(Timestamp::nowa().toString()+"\n");
     return 0;
}
