
#include <stdio.h>

#include <irstore.h>
#include <irerror.h>

int main() {
    Store* s = init_store("/abc");
    printf("%s\n", get_store_path(s));
    return 0;
}