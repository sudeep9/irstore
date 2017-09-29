
#include <stdio.h>
#include <stdint.h>

#include <irerror.h>
#include <irstore.h>
#include <file.h>

void cat_file(File* f) {
    ssize_t br;
    char buf[10];
    int64_t off = 0;

    do {
        IRError* err = read_file(f, off, buf, 10, &br);
        if(err != NULL) {
            printf("Error: %s (errno = %lld)\n", get_errmsg(err), get_errno(err));
            free_err(err);
            return;
        }
        off += 10;
        printf("%.*s", (int)br, buf);
    }while(br > 0);
}

int main() {
    Store* s = init_store("/Users/sudeepjathar/work/store");
    printf("%s\n", get_store_path(s));

    File* f = create_file(get_store_path(s), 10, "/Users/sudeepjathar/work/testoc.py", "xyz.cow");

    IRError* err = open_file(f);
    if(err == NULL) {
        printf("OK\n");
    }else{
        printf("%s:%lld\n", get_errmsg(err), get_errno(err));
        free_err(err);
        return 1;
    }

    char buf[10];
    ssize_t br;
    err = read_file(f, 30, buf, 10, &br);
    if(err == NULL) {
        printf("OK\n");
    }else{
        printf("%s:%lld\n", get_errmsg(err), get_errno(err));
        free_err(err);
        return 1;
    }

    cat_file(f);

    free_file(f);
    free_store(s);

    return 0;
}