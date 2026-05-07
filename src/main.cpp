#include <fstream>
#include <iostream>
#include <string>
#include "core/PersistentRBTree.hpp"
#include "io/CommandParser.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada.txt>\n";
        return 1;
    }

    std::ifstream inFile(argv[1]);
    std::ofstream outFile("saida.txt");

    if (!inFile.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir " << argv[1] << "\n";
        return 1;
    }
    if (!outFile.is_open()) {
        std::cerr << "Erro: nao foi possivel criar saida.txt\n";
        return 1;
    }

    PersistentRBTree treeImpl;
    ITreeCommands&   tree = treeImpl;
    CommandParser    parser;
    std::string      line;

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        Command cmd = parser.parse(line);
        switch (cmd.type) {
            case CommandType::INC:
                tree.insert(cmd.arg1);
                break;
            case CommandType::REM:
                tree.remove(cmd.arg1);
                break;
            case CommandType::IMP:
                tree.printVersion(
                    cmd.hasArg1 ? cmd.arg1 : tree.currentVersion(),
                    outFile);
                break;
            case CommandType::SUC:
                tree.successor(cmd.arg1, cmd.arg2, outFile);
                break;
            default:
                break;
        }
    }

    return 0;
}
