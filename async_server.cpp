#include <iostream>
#include <string>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

using namespace std;

vector<char> vBuffer(2 * 1024);

void grab_some_data(asio::ip::tcp::socket& socket){
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()), 
        [&](error_code ec, size_t length){
            if(!ec){
                cout << "\n\nRead " << length << " bytes.\n\n";
                for(int i = 0; i < length; i++){
                    cout << vBuffer[i];
                }
                cout << endl;

                grab_some_data(socket);
            }
            return;
        }
    );
}

int main(){

    asio::error_code ec;
    asio::io_context context;
    asio::io_context::work idleWork(context);
    std::thread thrContext([&](){ context.run(); });
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("87.247.245.130", ec), 80);
    asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if(!ec){
        cout << "connected" << endl;
    }else{
        cout << "Failed to connect to address:\n" << ec.message() << endl;
    }

    if(socket.is_open()){
        grab_some_data(socket);
        string sRequest = "GET / HTTP/1.1\r\n"
                            "Host: david-barr.co.uk\r\n"
                            "Connection: close\r\n\r\n";
        socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);
        sleep(10);        
    }

    return 0;
}
