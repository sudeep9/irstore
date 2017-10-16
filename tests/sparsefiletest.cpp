#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <iostream>
#include <unistd.h>

#include <sparsefile.h>

using namespace std;

TEST_CASE("sparse-file-test") {

    char cwd[2048];

    getcwd(cwd, sizeof(cwd));
    cout<<"CWD: "<<cwd<<endl;

    SUBCASE("open-close") {
        SparseFile f(".", 4096, 
        "/usr/local/include/spdlog/spdlog.h",
        "spdlog.h.cow");

        auto err = f.open();
        CHECK_MESSAGE(err == nullptr, "Error: " << *err);

        f.close();
    }

    SUBCASE("rw") {
        string store_path(".");
        string src_file("/usr/local/include/spdlog/spdlog.h");
        string cow_file("spdlog.h.rw.cow");

        size_t blocksz = 4096;

        SUBCASE("rw: write at 1 offset"){
            {
                char read_buf[4096];

                SparseFile f(store_path, blocksz, src_file, cow_file);

                auto err = f.open();
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                string buf("hello world");

                err = f.write(10, reinterpret_cast<const void*>(buf.c_str()), 11);
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                ssize_t bytes_read;
                err = f.read(10, read_buf, 11, &bytes_read);
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                CHECK_MESSAGE(buf == string(read_buf, bytes_read), "Read buffer not matching");

                f.close();
            }

            {
                SparseFile f(store_path, blocksz, src_file, cow_file);

                auto err = f.open();
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                CHECK_MESSAGE(f.is_cow(10), "Block was modified but reported as not");
                cout<<"cow @ 4097 = "<<f.is_cow(4097)<<endl;
                CHECK_MESSAGE(f.is_cow(4097) == 0, "Block was not modified but reported as one");

                char read_buf[4096];
                string buf("hello world");

                err = f.write(4097, reinterpret_cast<const void*>(buf.c_str()), 11);
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                ssize_t bytes_read;
                err = f.read(4097, read_buf, 11, &bytes_read);
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                CHECK_MESSAGE(buf == string(read_buf, bytes_read), "Read buffer not matching");

                f.close();
            }

            {
                SparseFile f(store_path, blocksz, src_file, cow_file);

                auto err = f.open();
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                CHECK_MESSAGE(f.is_cow(10), "Block was modified but reported as not");
                CHECK_MESSAGE(f.is_cow(4097), "Block was not modified but reported as one");

                f.close();
            }

            {
                SparseFile f(store_path, blocksz, src_file, cow_file);

                auto err = f.open();
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                char read_buf[4096];
                string buf("hello world");

                err = f.write(100, reinterpret_cast<const void*>(buf.c_str()), 11);
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);

                ssize_t bytes_read;
                err = f.read(10, read_buf, 11, &bytes_read);
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);
                CHECK_MESSAGE(buf == string(read_buf, bytes_read), "Read buffer not matching");

                err = f.read(100, read_buf, 11, &bytes_read);
                CHECK_MESSAGE(err == nullptr, "Error: " << *err);
                CHECK_MESSAGE(buf == string(read_buf, bytes_read), "Read buffer not matching");
                f.close();

            }
        }
    }
}