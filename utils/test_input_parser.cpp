

#include "InputParser.hpp"
#include <iostream>

int main()
{
    std::vector<std::string> testInputs = {
        "NICK pinky\r\n",
        "USER pinky 0 * :Powerpuff Girl\r\n",
        "JOIN #cpp\r\n",
        "PRIVMSG #cpp :hello everyone!\r\n",
        "PING :123456\r\n",
        "QUIT :bye!\r\n",
        "    \r\n",                 // whitespace only
        "invalidcommand\r\n"
    };

    for (size_t i = 0; i < testInputs.size(); ++i)
    {
        std::cout << "=== Test " << i + 1 << " ===" << std::endl;
        ParsedCommand cmd = InputParser::parse(testInputs[i]);
        cmd.debugPrint();
    }

    return 0;
}
