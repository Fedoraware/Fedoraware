#include <iostream>
#include <string>

int main() {
	const char * stringname = "sky_descent";
	long n = std::strtol(stringname,0,64);
	std::cout << stringname << "in base 64" << n << std::endl;
}
