
#include <stdio.h>
//#include <stdint.h>

#include <irerror.h>
#include <irstore.h>
#include <file.h>
#include <inttypes.h>

void cat_file(File* f) {
    ssize_t br;
    char buf[10];
    int64_t off = 0;

    do {
        IRError* err = read_file(f, off, buf, 10, &br);
        if(err != NULL) {
            printf("Error: %s (errno = %" PRId64 ")\n", get_errmsg(err), get_errno(err));
            free_err(err);
            return;
        }
        off += 10;
        printf("%.*s", (int)br, buf);
    }while(br > 0);
}

void update_file(File* f) {
    IRError* err = write_file(f, 0, "hello", 5);
    if(err != NULL) {
        printf("Error: %s (errno = %" PRId64 ")\n", get_errmsg(err), get_errno(err));
        return;
    }

    cat_file(f);
}

int main() {
    Store* s = init_store("/home/sudeep/work/store");
    printf("%s\n", get_store_path(s));

    File* f = create_file(get_store_path(s), 10, "/home/sudeep/work/copy.py", "xyz.cow");

    IRError* err = open_file(f);
    if(err == NULL) {
        printf("OK\n");
    }else{
        printf("%s:%" PRId64 "\n", get_errmsg(err), get_errno(err));
        free_err(err);
        return 1;
    }

    char buf[10];
    ssize_t br;
    err = read_file(f, 30, buf, 10, &br);
    if(err == NULL) {
        printf("OK\n");
    }else{
        printf("%s:%" PRId64 "\n", get_errmsg(err), get_errno(err));
        free_err(err);
        return 1;
    }

    update_file(f);

    free_file(f);
    free_store(s);

    return 0;
}