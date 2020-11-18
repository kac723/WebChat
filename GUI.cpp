#include <iostream>
// #include <wx/wx.h>
// class MyApp : public wxApp
// {
// public:
//     virtual bool OnInit();
// };

// class Simple : public wxFrame
// {
// public:
//     Simple(const wxString &title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
//     {
//         Centre();
//     }
// };

// IMPLEMENT_APP(MyApp)

// bool MyApp::OnInit()
// {
//     Simple *simple = new Simple(wxT("Simple"));
//     simple->Show(true);

//     return true;
// <
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/StreamCopier.h>
#include <sstream>
const unsigned int maxBuffer = 2048;
int main()
{
    /* Poco::Net::ServerSocket srv(Poco::Net::SocketAddress("127.0.0.1:1234"));
    auto streamSocket = srv.acceptConnection();
    Poco::Net::SocketStream str(streamSocket);
    str << "0";
    */
    Poco::Net::StreamSocket ss(Poco::Net::SocketAddress("127.0.0.1:1234"));
    //ss.setReceiveTimeout(Poco::Timespan(1, 0));
    std::cout << "Connected to server " << std::endl;
    std::string recv(maxBuffer, 0);
    ss.receiveBytes(recv.data(), maxBuffer);
    std::cout << "Recv " << recv;
    return 0;
    while (1)
    {
    }
    return 0;
}
