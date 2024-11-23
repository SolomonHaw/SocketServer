#include <array>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using std::string;
using std::cout;
using std::endl;

string read_(tcp::socket &sock) {
	boost::asio::streambuf buf;
	boost::asio::read_until(sock, buf, "\n");
	string data = boost::asio::buffer_cast<const char*>(buf.data());
	return data;
}

void write_(tcp::socket& sock, const string &message) {
	const string msg = message + "\n";
	boost::asio::write(sock, boost::asio::buffer(message));
}

int main() {
	boost::asio::io_context ctn;
	//server should be able to set to any address, it defaults to localhost
	tcp::endpoint ep(boost::asio::ip::address_v4::any(), 1234);

	try {
		tcp::acceptor acceptor_(ctn, ep.protocol());
		tcp::socket sock(ctn);
		cout << "Socket successfully created." << endl;
		//Bind the acceptor to IP endpoint
		acceptor_.bind(ep);
		//Tell acceptor function to start listening for incoming connection requests
		acceptor_.listen();
		//Accept any available connections
		acceptor_.accept(sock);
		cout << "Connection accepted!" << endl;

		while (1) {
			string data = read_(sock);
			write_(sock, data);
		}
		cout << "Connection established." << endl;
	}
	catch (boost::system::system_error& e) {
		cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();

		return e.code().value();
	}
	return 0;
}