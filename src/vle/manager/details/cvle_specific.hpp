

#ifndef VLE_MANAGER_DETAILS_CVLE_FILE_HANDLERS_HPP_
#define VLE_MANAGER_DETAILS_CVLE_FILE_HANDLERS_HPP_

#include <iomanip>
#include <iostream>
#include <fstream>

#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>

namespace vle {
namespace manager {


//write kth element of exp (a Set or a Tuple) as xml into out
bool
cvle_write_xml(const value::Value& exp, unsigned int k, std::ostream& out,
        manager::Error& err)
{
    switch(exp.getType()) {
    case value::Value::SET:
        exp.toSet().get(k)->writeXml(out);
        return true;
        break;
    case value::Value::TUPLE:
        out << std::setprecision(std::numeric_limits<double>::digits10)
        << "<double>" << exp.toTuple().at(k) <<"</double>";
        return true;
        break;
    default:
        if (k == 0){
            out << exp.writeToXml();
            return true;
        }
        break;
    }
    err.code = -1;
    err.message = "[Manager] error while producing cvle file";
    return false;
}


//write kth element of exp (a Set or a Tuple) as xml into out
void
cvle_write(const ManagerObjects& manObjs,
        const wrapper_init& init, const std::string& inPath,
        manager::Error& err)
{
    std::ofstream inFile;
    inFile.open (inPath, std::ios_base::trunc);
    //write header
    inFile << "_cvle_complex_values\n";
    //write content
    std::string curr_cond = "";
    bool has_simulation_engine = false;
    std::set<std::string> conds_seen;
    for (unsigned int i = 0; i < manObjs.inputsSize(); i++) {
        for (unsigned int j = 0; j < manObjs.replicasSize(); j++) {
            curr_cond = "";
            has_simulation_engine = false;
            inFile << "<?xml version='1.0' encoding='UTF-8'?>"
                    "<vle_project date=\"\" version=\"1.0\" author=\"cvle\">"
                    "<experiment name=\"cvle_exp\"><conditions>";
            for (unsigned int in=0; in < manObjs.nbInputs(); in++) {
                const ManInput& tmp_input = *manObjs.mInputs[in];
                has_simulation_engine = has_simulation_engine or
                        (tmp_input.cond == "simulation_engine");
                if (curr_cond != "" and tmp_input.cond != curr_cond){
                    inFile << "</condition>";
                }
                if (tmp_input.cond != curr_cond) {
                    inFile << "<condition name =\"" << tmp_input.cond << "\">";
                }
                curr_cond = tmp_input.cond;

                inFile << "<port name =\"" << tmp_input.port << "\">";
                cvle_write_xml(tmp_input.values(init), i, inFile, err);
                inFile << "</port>";
                //write replicate if the same cond name
                for (auto& tmp_repl : manObjs.mReplicates) {
                    if (tmp_repl->cond == curr_cond) {
                        inFile << "<port name =\"" << tmp_repl->port << "\">";
                        cvle_write_xml(tmp_repl->values(init), j, inFile, err);
                        inFile << "</port>";

                    }
                }
                if ((int) in == ((int) manObjs.nbInputs())-1) {
                    inFile << "</condition>";
                }
                conds_seen.emplace(curr_cond);
            }
            //write replicates if not already written
            for (auto& tmp_repl : manObjs.mReplicates) {
                if (conds_seen.find(tmp_repl->cond) == conds_seen.end()) {
                    has_simulation_engine = has_simulation_engine or
                            (tmp_repl->cond == "simulation_engine");
                    inFile << "<condition name =\"" << tmp_repl->cond << "\">";
                    inFile << "<port name =\"" << tmp_repl->port << "\">";
                    cvle_write_xml(tmp_repl->values(init), j, inFile, err);
                    inFile << "</port>";
                    inFile << "</condition>";
                }
            }
            //write _cvle_cond
            inFile << "<condition name =\"_cvle_cond\">";
            inFile << "<port name =\"id\">";
            inFile << "<string>id_" << i << "_" << j << "</string>";
            inFile << "</port>";
            if (has_simulation_engine) {
                inFile << "<port name =\"has_simulation_engine\">";
                inFile << "<boolean>true</boolean>";
                inFile << "</port>";
            }
            inFile << "</condition>";
            inFile << "</conditions>";
            inFile << "</experiment>";
            inFile << "</vle_project>";
            inFile <<"\n";
        }
    }
    inFile.close();
}

//write the host file for mpirun
void
cvle_write_hostfile(unsigned int slots, const std::string& inPath)
{
    std::ofstream inFile;
    inFile.open (inPath, std::ios_base::trunc);
    inFile << "localhost slots=" << slots << "\n";
    inFile.close();
}


/////////////// Output reader
bool
cvle_read_IdHeader(std::ifstream& outFile,
            std::string& line, std::vector <std::string>& tokens,
            int& inputId, int& inputRepl)
{
    //read empty lines at the beginning
    if (outFile.eof()) {
        return false;
    }
    std::streampos stream_place = outFile.tellg();
    std::getline(outFile, line);
    while(line.empty()) {
        if (outFile.eof()) {
            return false;
        }
        stream_place = outFile.tellg();
        std::getline(outFile, line);
    }

    //split id_10_2
    tokens.clear();
    utils::tokenize(line, tokens, "_", false);
    if (tokens.size() != 3 or tokens[0] != "id") {
        outFile.seekg(stream_place);
        return false;
    } else {
        inputId = std::stoi(tokens[1]);
        inputRepl = std::stoi(tokens[2]);
        return true;
    }
}

///////////////
bool
cvle_read_ViewHeader(std::ifstream& outFile, std::string& line,
        std::vector <std::string>& tokens, std::string& viewName)
{
    //read empty lines at the beginning
    if (outFile.eof()) {
        return false;
    }
    std::streampos stream_place = outFile.tellg();
    std::getline(outFile, line);
    while(line.empty()) {
        if (outFile.eof()) {
            return false;
        }
        stream_place = outFile.tellg();
        std::getline(outFile, line);
    }

    //split view:viewNoise
    tokens.clear();
    utils::tokenize(line, tokens, ":", false);
    if (tokens.size() != 2 or tokens[0] != "view") {
        outFile.seekg(stream_place);
        return false;
    } else {
        viewName.assign(tokens[1]);
        return true;
    }
}


/////////////////////////
std::unique_ptr<value::Matrix>
cvle_read_Matrix(std::ifstream& outFile, std::string& line,
        std::vector <std::string>& tokens, unsigned int nb_rows)
{
    std::unique_ptr<value::Matrix> viewMatrix(nullptr);

    //read empty lines at the beginning
    std::streampos stream_place = outFile.tellg();
    std::getline(outFile, line);
    while(line.empty()) {
        stream_place = outFile.tellg();
        std::getline(outFile, line);
    }

    //fill matrix header
    tokens.clear();
    utils::tokenize(line, tokens, " ", true);
    unsigned int nbCols = tokens.size();
    viewMatrix.reset(new value::Matrix(nbCols,1,nbCols, nb_rows));
    for (unsigned int c=0; c<nbCols; c++){
        viewMatrix->set(c,0,value::String::create(tokens[c]));
    }

    //read content
    while(true){
        stream_place = outFile.tellg();
        std::getline(outFile,line);
        tokens.clear();
        utils::tokenize(line, tokens, " ", true);
        if (tokens.size() != nbCols) {
            outFile.seekg(stream_place);
            break;
        }
        viewMatrix->addRow();
        for (unsigned int c=0; c<nbCols; c++){
            if (c == 0 and tokens[c] == "inf") {
                viewMatrix->set(c,viewMatrix->rows()-1,
                        value::Double::create(
                                std::numeric_limits<double>::max()));
            } else {
                viewMatrix->set(c,viewMatrix->rows()-1,
                        value::Double::create(
                                std::stod(tokens[c])));
            }
        }
    }
    return viewMatrix;
}


//read output file
std::unique_ptr<value::Map>
cvle_read(const utils::Path& tempOutCsv, unsigned int inputSize,
        unsigned int repSize, std::vector<std::unique_ptr<ManOutput>>& outputs,
        manager::Error& err)
{
    std::unique_ptr<value::Map> results = init_results(outputs, err);
    if (err.code) {
        return nullptr;
    }

    std::ifstream outFile;
    outFile.open (tempOutCsv.string(), std::ios_base::in);
    std::string line;

    int inputId = -1;
    int inputRepl = -1;
    std::string viewName;
    std::map<std::string, int> insightsViewRows;
    std::unique_ptr<value::Matrix> viewMatrix;

    std::vector <std::string> tokens;
    bool finishViews = false;
    bool finishIds = false;
    unsigned int nbSimus = 0;
    while(not finishIds){
        //read id_1_0
        if (not cvle_read_IdHeader(outFile, line, tokens, inputId, inputRepl)) {
            finishIds = true;
            break;
        }
        finishViews = false;
        while(not finishViews){
            //read view:viewName
            if (not cvle_read_ViewHeader(outFile, line, tokens, viewName)) {
                finishViews= true;
                break;
            }
            //get/set insight of the number of rows
            bool getInsight = (insightsViewRows.find(viewName) !=
                    insightsViewRows.end());
            if (not getInsight) {
                insightsViewRows.insert(std::make_pair(viewName,100));
            }
            int rows = insightsViewRows[viewName];
            //read matrix of values
            viewMatrix = std::move(cvle_read_Matrix(outFile, line, tokens, rows));

            if (not getInsight) {
                insightsViewRows[viewName] = viewMatrix->rows();
            }
            //insert replicate for
            std::unique_ptr<value::Value> aggrValue;
            for (unsigned int o=0; o< outputs.size(); o++) {
                ManOutput& outId = *outputs[o];
                if (outId.view == viewName) {
                    for (unsigned int p=0; p<viewMatrix->columns(); p++) {
                        if (viewMatrix->getString(p,0) ==
                                outId.absolutePort) {
                            aggrValue = std::move(outId.insertReplicate(
                                    *viewMatrix, inputId, inputSize,
                                    repSize));
                            if (aggrValue) {
                                results->set(outId.id, std::move(aggrValue));
                            }
                        }
                    }
                }
            }
        }
        nbSimus ++;
    }
    if (nbSimus != inputSize*repSize) {
        err.code = -1;
        err.message = "[Manager] ";
        err.message += vle::utils::format("Error in simu id=%d, repl=%d ",
                inputId, inputRepl);
        std::string prefix_id = "id_";
        bool stop = false;
        while (not stop) {
            std::getline(outFile, line);
            if (line.substr(0, prefix_id.size()) == prefix_id) {
                stop = true;
            } else {
                err.message +=  line+" ";
            }
            stop = stop or outFile.eof();
        }
        results.reset(nullptr);
    }
    outFile.close();
    return results;
};

}
} // namespace vle manager

#endif
