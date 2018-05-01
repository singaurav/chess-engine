#ifndef ADAPTER_INCLUDED
#define ADAPTER_INCLUDED

#include <string>
#include <vector>

namespace Adapter {
void hello_from_stockfish();
std::vector<std::string> run_uci_commands(std::vector<std::string> commands);
} // namespace Adapter

#endif // #ifndef ADAPTER_INCLUDED
