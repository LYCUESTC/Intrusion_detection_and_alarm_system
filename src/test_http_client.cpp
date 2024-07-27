#include "httplib.h"
#include <iostream>
#include <fstream>
using namespace std;
int main()
{
    httplib::Client cli("localhost", 6008);
    // [1]得到二进制数据;
    ifstream i_f_stream("test.jpg", ifstream::binary);
    i_f_stream.seekg(0, i_f_stream.end);
    int length = i_f_stream.tellg();
    i_f_stream.seekg(0, i_f_stream.beg);

    char* buffer = new char[length];
    i_f_stream.read(buffer, length);
    i_f_stream.close();

    std::string body(length, 0);
    memcpy((char*)(body.c_str()), buffer, length);
    cli.Post("/alarm/test/test.jpg?name=tes2&alarm_time=2022-1-23 22\:08\:00", body, "image/jpeg");
    /*auto res = cli.Get("/hi");
    if (res) {
        cout << res->status << endl;
        cout << res->get_header_value("Content-Type") << endl;
        cout << res->body << endl;
    }
    else {
        cout << "error code: " << res->status << std::endl;
    }*/

}