#include <iostream>

namespace Fay
{
#define FAY_LOG_ERROR(...) std::cerr << "\033[31m" << __VA_ARGS__ << "\033[0m"<<std::endl;
#define FAY_LOG_WARN(...) std::cout << "\033[33m" << __VA_ARGS__ << "\033[0m"<<std::endl;
#define FAY_LOG_INFO(...) std::cout << "\033[32m" << __VA_ARGS__ << "\033[0m"<<std::endl;
#define FAY_LOG_DEBUG(...) std::cout << "\033[34m" << __VA_ARGS__ << "\033[0m"<<std::endl;
}