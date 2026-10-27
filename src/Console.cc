/**
 * @author      <a href="mailto:michele.lora@univr.it">Michele Lora</a>
 * @date        1/23/2020
 *              This project is released under the 3-Clause BSD License.
 *
 */

#include <fstream>
#include <sstream>
#include "Console.hh"
#include "simplify.hh"

using namespace chase;

Console::Console(System *system, std::string outDir, bool verbose) :
    _system(system), _outDir(outDir), _verbose(verbose)
{
}

int Console::run(std::string cmd)
{
    // Identify the type of contracts in the specification.

    LogicIdentificationVisitor logicIdVisit;
    for(auto & cit :  _system->getContractsSet())
    {
        auto t = logicIdVisit.identifyContractType(cit);
        auto it = _used_logics.find(cit);
        if (it != _used_logics.end()) { it->second = t; }
        else
            _used_logics.insert(std::pair<Contract *, logics_type>(cit, t));
    }

    if(cmd.empty()) {
        std::cout << std::endl << "$> ";
        std::getline(std::cin, cmd);
        if(cmd.empty()) return 1;
    }
    if(cmd == "exit") return 0;
    else {
        if(_verbose && cmd != "break") {
            std::cout << "$> " << cmd << std::endl;
        }
        int rv = _execCommand(cmd);
        if( rv == 0 )
            std::cout << "Wrong command:\n\t" << cmd
            << "\n\tHINT:\tType help to see instructions.\n" <<
            std::endl;
    }
    return 1;
}

int Console::_execCommand(std::string cmd)
{
    // Tokenize.
    std::stringstream ss(cmd);
    std::vector< std::string > tokens;
    std::string intermediate;
    while(std::getline(ss, intermediate, ' '))
        tokens.push_back(intermediate);

    // The names of the first version of the console are still accepted.
    for(size_t t = 0; t < tokens.size() && t < 2; ++t) {
        if(tokens[t] == "conjunction") tokens[t] = "conjoin";
        else if(tokens[t] == "quotient") tokens[t] = "divide";
        else if(tokens[t] == "refinement") tokens[t] = "refines";
        if(tokens[0] != "help") break;
    }

    if(tokens[0] == "help")
    {
        if(tokens.size() > 1)
        {
            cmd = tokens[1];
            _printHelp(cmd);
            return 1;
        }
        else
        {
            _printHelp();
            return 1;
        }
    }
    else if(tokens[0] == "saturate")
        return _execSaturation(tokens);
    else if(tokens[0] == "compose")
        return _execComposition(tokens);
    else if(tokens[0] == "conjoin")
        return _execConjunction(tokens);
    else if(tokens[0] == "synthesize")
        return _execSynthesis(tokens);
    else if(tokens[0] == "verify")
        return _execVerification(tokens);
    else if(tokens[0] == "refines")
        return _checkRefinement(tokens);
    else if(tokens[0] == "divide")
        return _computeQuotient(tokens);
    else if(tokens[0] == "separate")
        return _computeSeparation(tokens);
    else if(tokens[0] == "disjoin")
        return _computeDisjunction(tokens);
    else if(tokens[0] == "merge")
        return _computeMerging(tokens);
    else if(tokens[0] == "imply")
        return _computeImplication(tokens);
    else if(tokens[0] == "coimply")
        return _computeCoimplication(tokens);
    else if(tokens[0] == "invert")
        return _computeReciprocal(tokens);
    else if(tokens[0] == "show")
        return _execShow(tokens);
    else if(tokens[0] == "simplify")
        return _simplify(tokens);
    else if(tokens[0] == "clear"){
        std::cout << "\033[2J\033[1;1H";
        return 1;
    }
    else if(tokens[0] == "break") {
        std::cin.get();
        return 1;
    }
    else if(tokens[0] == "stl")
        return _stlContract(tokens);
    return 0;
}

void Console::_createProjectionMap(
        names_projection_map &m, std::string mode,
        Contract * c1, Contract * c2)
{
    if(mode == "manual")
        _createProjectionMapManual(m);
    else if(mode == "name")
        _createProjectionMapName(m, c1, c2);
    else
        _createProjectionMapFile(m, mode);
}

void Console::_createProjectionMapManual(names_projection_map &m)
{
    std::cout << "INSERT THE PROJECTION MAP. Insert END when finished" << std::endl;
    std::string line;
    while(std::getline(std::cin, line))
    {
        if(line == "END") return;
        std::stringstream ss(line);
        std::vector< std::string > tokens;
        std::string intermediate;
        while(std::getline(ss, intermediate, ' '))
        {
            tokens.push_back(intermediate);
        }
        if(tokens.size() == 2)
        {
            std::pair< std::string, std::string > p(tokens[0], tokens[1]);
            m.insert(p);
        } else {
            messageWarning("Wrong format in projection.");
        }
    }
}

void Console::_createProjectionMapName(
        names_projection_map & m, Contract * c1, Contract * c2)
{
    // Variables, and constants (shared by the contracts of a system), with
    // the same name are the same.
    for(auto & dec1 : c1->declarations)
    {
        if(dec1->IsA() != variable_node && dec1->IsA() != constant_node) continue;
        for(auto & dec2 : c2->declarations ) {
            if (dec2->IsA() != dec1->IsA()) continue;

            std::string n1 = dec1->getName()->getString();
            std::string n2 = dec2->getName()->getString();
            if(n1 == n2)
                m.insert(std::pair<std::string, std::string>(n1, n2));
        }
    }
}

void Console::_createProjectionMapFile(
        names_projection_map &m, std::string file)
{
    std::ifstream ifile(file);
    std::string line;
    while(std::getline(ifile, line))
    {
        if(line == "END") return;
        std::stringstream ss(line);
        std::vector< std::string > tokens;
        std::string intermediate;
        while(std::getline(ss, intermediate, ' '))
        {
            tokens.push_back(intermediate);
        }
        if(tokens.size() == 2)
        {
            std::pair< std::string, std::string > p(tokens[0], tokens[1]);
            m.insert(p);
        } else {
            messageWarning("Wrong format in projection.");
        }
    }
}


void Console::_printHelp(std::string cmd) {
    if(cmd.empty()) {
        std::string contracts = "";
        for (auto & c : _system->getContractsSet())
        {
            contracts += "\t- " + c->getName()->getString() + "\n";
        }
        std::cout
                << "Contracts:\n" << contracts
                << std::endl
                << "Commands:" << std::endl
                << "\tshow\t\t: print to screen the system, or a contract."
                << std::endl
                << "\tcompose\t\t: compute the composition of two contracts."
                << std::endl
                << "\tconjoin\t\t: compute the conjunction of two contracts"
                << " (also: conjunction)." << std::endl
                << "\tdisjoin\t\t: compute the disjunction of two contracts."
                << std::endl
                << "\tmerge\t\t: compute the merging of two contracts."
                << std::endl
                << "\tdivide\t\t: compute the quotient of two contracts"
                << " (also: quotient)." << std::endl
                << "\tseparate\t: compute the separation of two contracts."
                << std::endl
                << "\timply\t\t: compute the implication of two contracts."
                << std::endl
                << "\tcoimply\t\t: compute the coimplication of two contracts."
                << std::endl
                << "\tinvert\t\t: compute the reciprocal of a contract."
                << std::endl
                << "\tsaturate\t: saturate a contract."
                << std::endl
                << "\tsimplify\t: simplify the formulas of the system, or of contracts."
                << std::endl
                << "\tverify\t\t: verify compatibility and consistency of a contract, and that A & G can hold."
                << std::endl
                << "\trefines\t\t: check whether a contract refines another"
                << " (also: refinement)." << std::endl
                << "\tsynthesize\t: synthesize a contract."
                << std::endl;
    }
    else if(cmd == "show")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" [contract]\n")
                  << "\t\tcontract:\t\tfirst contract to print to screen (optional).\n"
                  << std::endl;
    else if(cmd == "compose")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode\n")
                  << "\t\tcontract_1:\t\tfirst contract to compose.\n"
                  << "\t\tcontract_2:\t\tsecond contract to compose.\n"
                  << "\t\tresult:\t\t\tcontract generated by the composition.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << std::endl;
    else if(cmd == "conjoin")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode\n")
                  << "\t\tcontract_1:\t\tfirst contract to conjoin.\n"
                  << "\t\tcontract_2:\t\tsecond contract to conjoin.\n"
                  << "\t\tresult:\t\tcontract generated by the conjunction.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << std::endl;
    else if(cmd == "simplify")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" [contract ...]\n")
                  << "\t\tcontract:\t\tcontracts to simplify (default: the whole system).\n"
                  << std::endl;
    else if(cmd == "disjoin")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode\n")
                  << "\t\tcontract_1:\t\tfirst contract.\n"
                  << "\t\tcontract_2:\t\tsecond contract.\n"
                  << "\t\tresult:\t\tcontract generated by the disjunction.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << std::endl;
    else if(cmd == "merge")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode\n")
                  << "\t\tcontract_1:\t\tfirst viewpoint.\n"
                  << "\t\tcontract_2:\t\tsecond viewpoint.\n"
                  << "\t\tresult:\t\tcontract generated by the merging.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << std::endl;
    else if(cmd == "divide")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode synthesizable\n")
                  << "\t\tcontract_1:\t\tdividend contract.\n"
                  << "\t\tcontract_2:\t\tdivisor contract.\n"
                  << "\t\tresult:\t\tcontract generated by the quotient.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << "\t\tsynthesizable:\t\ttrue if you whish to produce a GR(1) specification."
                  << std::endl;
    else if(cmd == "separate")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode\n")
                  << "\t\tcontract_1:\t\tcontract to separate.\n"
                  << "\t\tcontract_2:\t\tknown viewpoint contract.\n"
                  << "\t\tresult:\t\tcontract generated by the separation,\n"
                  << "\t\t\t\t\tthe smallest C such that contract_1 refines\n"
                  << "\t\t\t\t\tthe merging of C and contract_2.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << std::endl;
    else if(cmd == "imply")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode\n")
                  << "\t\tcontract_1:\t\tthe premise.\n"
                  << "\t\tcontract_2:\t\tthe conclusion.\n"
                  << "\t\tresult:\t\tcontract generated by the implication,\n"
                  << "\t\t\t\t\tthe largest C such that the conjunction\n"
                  << "\t\t\t\t\tof C and contract_1 refines contract_2.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << std::endl;
    else if(cmd == "coimply")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract_1 contract_2 result mode\n")
                  << "\t\tcontract_1:\t\tfirst contract.\n"
                  << "\t\tcontract_2:\t\tsecond contract.\n"
                  << "\t\tresult:\t\tcontract generated by the coimplication,\n"
                  << "\t\t\t\t\tthe smallest C such that contract_2\n"
                  << "\t\t\t\t\trefines the disjunction of C and contract_1.\n"
                  << "\t\tmode:\t\t\tbased on name or map.\n"
                  << std::endl;
    else if(cmd == "invert")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract result\n")
                  << "\t\tcontract:\t\tthe contract.\n"
                  << "\t\tresult:\t\tits reciprocal (G, A), seen from the environment.\n"
                  << std::endl;
    else if(cmd == "saturate")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract\n")
                  << "\t\tcontract:\t\tthe contract to saturate.\n"
                  << std::endl;
    else if(cmd == "synthesize")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract file solver\n")
                  << "\t\tcontract:\t\tthe contract to synthesize.\n"
                  << "\t\tfile:\t\tfile generated for the solver.\n"
                  << "\t\tsolver:\t\tmight be either slugs, gr1c or all (default, slugs).\n"
                  << std::endl;
    else if(cmd == "verify")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract file\n")
                  << "\tcontract:\t\tthe contract to verify.\n"
                  << "\tfile:\t\tfile generated for the solver.\n"
                  << std::endl;
    else if(cmd == "refines")
        std::cout << "Usage:\n\t$> " <<
                  cmd + std::string(" contract1 contract2 file mode\n")
                  << "\tThe operation checks whether contract1 refines contract 2."
                  << "\tcontract 1:\t\tthe first contract.\n"
                  << "\tcontract 2:\t\tthe first contract.\n"
                  << "\tfile:\t\tfile generated for the solver.\n"
                  << "\tmode: \t\t\tthe merge of variables is based on name or map.\n"
                  << std::endl;
    else
        std::cout << "Wrong command." << std::endl;
}












