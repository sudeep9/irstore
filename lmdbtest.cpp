
#include <iostream>
#include <cstdlib>
#include <string>

#include <lmdbfile.h>

using namespace std;

void test();

void test() {
    LmdbFile f;
    string path = string(getenv("HOME")) + "/work/irdata/f1.dat";
    PrintErr(f.open(path));
}

int main() {
    test();
    return 0;
}