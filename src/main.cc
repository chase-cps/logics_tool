/**
 * @author      <a href="mailto:michele.lora@univr.it">Michele Lora</a>
 * @date        2019
 *              This project is released under the 3-Clause BSD License.
 * */

#include "chase-core.hh"
#include "LogicsSpecsBuilder.hh"

#include <unistd.h>
#include <fstream>
#include <sys/stat.h>


#include "Console.hh"
#include "Factory.hh"

using namespace chase;

typedef struct _params
{
    /// @brief Input file path.
    std::string fileIn;
    /// @brief Commands file path.
    std::string cmdFile;

    /// @brief Output directory path.
    std::string outDir;

    /// @brief Verbose flag. Execution is verbose when flag is set to true.
    bool verbose;

    _params() :
        fileIn("chase_spec.txt"),
        cmdFile(""),
        outDir(""),
        verbose(false)
    {
    }
} Params;


/// @brief Method parsing the input command line.
/// @param argc The number of parameters to parse.
/// @param argv The array of parameters.
/// @return A structure storing all the parameters sorted out.
Params * parseCmdLine( int argc, char * argv[] );

/// @brief Method printing usage banner of the tool.
void printHelp();


int main( int argc, char * argv[] )
{
    auto params = parseCmdLine(argc, argv);

    LogicsSpecsBuilder builder;
    builder.parseSpecificationFile(params->fileIn);

    System * system = builder.getSystem();

    Console console(system, params->outDir, params->verbose);

    if(params->cmdFile.empty()) {
        while (console.run());
    } else {
        std::ifstream cmds;
        std::string line;
        cmds.open(params->cmdFile.c_str());
        int ret = 1;
        while(std::getline(cmds, line) && ret != 0)
        {
            if(line == "\r" || line == "\n") continue;
            size_t semicolon = line.find(';');
            if( semicolon == std::string::npos ) {
                messageWarning("Missing semicolon. Skipped command: " + line);
                continue;
            }
            line = line.substr(0, semicolon);
            ret = console.run(line);
        }
    }
    delete system;
}

Params * parseCmdLine(int argc, char **argv) {
    auto *parameters = new Params();

    opterr = 0;
    int c;

    while ((c = getopt(argc, argv, "i:c:b:o:V")) != -1) {
        switch (c) {
            case 'i':
                parameters->fileIn = std::string(optarg);
                break;
            case 'c':
                parameters->cmdFile = std::string(optarg);
                break;
            case '?':
                printHelp();
                exit(-1);
            case 'o':
                parameters->outDir = std::string(optarg);
                if(parameters->outDir.back() != '/')
                    parameters->outDir += "/";
                break;
            case 'V':
                parameters->verbose = true;
                break;
            default:
                printHelp();
                exit(-1);
        }
    }

    std::ifstream f(parameters->fileIn.c_str());
    if( ! f.good() )
    {
        printHelp();
        f.close();
        std::cout << "fileIn: " << parameters->fileIn.c_str() << std::endl;
        exit(-1);
    }
    f.close();

    struct stat info;
    if(stat (parameters->outDir.c_str(), &info) != 0 && !parameters->outDir
            .empty()) {
        if (mkdir(parameters->outDir.c_str(), 0755) == -1)
        {
            std::cout << strerror(errno) << std::endl;
            exit(-1);

        } else messageInfo(parameters->outDir + " created.");
    }
    return parameters;
}

void printHelp()
{
    std::cerr << "[USAGE]\n" <<
        "logics_frontend -i input_file -c commands_file [-V]"
        << std::endl <<
        std::endl <<
        "\t-i : specifies the txt input file containing the specifications."
        << std::endl <<
        "\t-c : command file to be executed."
        << std::endl <<
        "\t-o : output directory path."
        << std::endl <<
        "\t-V : activate the verbose mode." << std::endl;
}
