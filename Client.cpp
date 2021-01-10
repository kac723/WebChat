#include <iostream>
#include <boost/asio.hpp>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Object.h>

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/SocketAddress.h>
using std::cin;
using std::cout;
using std::endl;
using namespace boost::asio;
using ip::tcp;
const unsigned int maxBuffer = 2048;
class Client
{
private:
    tcp::socket _socket;
    std::mutex _mutex;
    std::vector<int> m_users;
    int _clientID = 0;

public:
    Client(boost::asio::io_service &io, const std::string &address, unsigned int port) : _socket(io)
    {
        cout << "Connecting to server " << address << ":" << port << endl;
        _socket.connect(tcp::endpoint(ip::address::from_string(address), port));
        cout << "Connection successfull!" << endl;
        std::string strData(maxBuffer, 0);
        //ss >> strData;
        //m_socket.receiveBytes(strData.data(), maxBuffer);
        _socket.read_some(buffer(strData));
        cout << "Received ID is " << strData << endl;
        _clientID = std::stoi(strData);
    }
    void startSend()
    {
        while (_socket.is_open())
        {
            std::string input;
            std::getline(cin, input);
            Poco::JSON::Object a;
            a.set("SendingClientID", _clientID);
            a.set("ReceivingClientID", _clientID == 0 ? 1 : _clientID == 2 ? 1 : 0);
            a.set("Message", input);
            std::ostringstream stream;
            a.stringify(stream);
            write(_socket, buffer(stream.str())); //buffer is a method
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void startReceive()
    {
        while (_socket.is_open())
        {
            std::string received(maxBuffer, 0);
            _socket.read_some(buffer(received));
            cout << "Received: " << received << endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

int main()
{
    io_service io;
    //tcp::socket socket(io);
    Client c(io, "127.0.0.1", 1234);
    std::thread tSend(&Client::startSend, &c);
    std::thread tRecv(&Client::startReceive, &c);
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
