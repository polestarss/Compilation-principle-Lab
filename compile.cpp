#ifndef COMPILE
#define COMPILE

#include "helpMIPS.cpp"
#include "generateIR.cpp"
int main()
{
    generateIR();
    Compile();
}
#endif // !COMPILE