
#include <iostream>
#include <sstream>
#include <utility>
#include <algorithm>

//Exception --> .hpp
class BadChunkRequestException: public std::exception { public: virtual char const *what() const throw(); };

//Exception --> .cpp
char const *BadChunkRequestException::what() const throw() { return "Chunk request is not ended by a \"\\r\\n\""; }


//la fonction en question
std::pair<unsigned int, std::string>	unchunk(std::string str) {
		std::stringstream	s;
		unsigned int		len_hex;
		size_t 				pos = str.find("\r\n");

		if (str.substr(str.length() - 2) != "\r\n")
			throw BadChunkRequestException();
		str.erase(str.length() - 2, 2);

		s.clear();
		s << std::hex << str.substr(0, pos);
		s >> len_hex;
		return std::make_pair(std::min(len_hex, static_cast<unsigned int>(str.length() - (pos + 2))), str.substr(pos + 2));
}


//TESTER
int	main() {
	try {
	std::string s = "A\r\nBonjour je suis un exemple.\r\n\r\no";
	unchunk(s);
	std::cout << "LEN = " << unchunk(s).first << std::endl;
	std::cout << "STR = " << unchunk(s).second << std::endl;
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}

