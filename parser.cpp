#include<iostream>
#include<fstream>
#include<string>
#include <sstream>
#include <vector>
using namespace std;

class parser
{
private:
    
public:
    parser();
        

    string readfile(){

        // string filename;
        // cout << "Enter file path: ";
        // cin >> filename;
        // cout << endl;

        
        ifstream inputFile("Counter.v");

        // Check if the file opened successfully
        if(!inputFile.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return NULL; // Exit with an error code
        }

    
        stringstream buffer;
        buffer << inputFile.rdbuf(); // Read the entire file into the buffer

        // Close the file
        inputFile.close();

        std::string RTL_content = buffer.str();
        
        return RTL_content ;
    }


    string find_modulename(string filecontent){


        size_t module_name_start_index = filecontent.find("module") + 6;
        size_t module_name_stop_index = filecontent.find("(");
        if (filecontent.find("#") != std::string::npos && filecontent.find("#") < module_name_stop_index) {
            module_name_stop_index = filecontent.find("#");
        }
        string module_name = filecontent.substr(module_name_start_index, module_name_stop_index - module_name_start_index);

        return module_name; 
    }

    bool find_clk(string filecontent){
        if (filecontent.find("clk") != std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    }

    bool find_parameters(string filecontent){
        if (filecontent.find("parameter") != std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    }


    size_t countSubstring(const std::string& str, const std::string& sub) {
        size_t count = 0;
        size_t pos = 0;
        while ((pos = str.find(sub, pos)) != std::string::npos) {
            count++;
            pos += sub.length();
        }
        return count;
    }


    string removeSpaces(const std::string& str) {
        std::string result;
        for (char ch : str) {
            if (ch != ' ') {
                result += ch;
            }
        }
        return result;
    }


    vector<string> removeKeywords(std::vector<std::string> &signal_vector) {
        for (size_t i = 0; i < signal_vector.size(); ++i) {
            if (signal_vector[i].find("wire") != std::string::npos) {
                size_t pos = signal_vector[i].find("wire");
                signal_vector[i] = signal_vector[i].substr(pos + 5); // Removes "wire" and following spaces
            } else if (signal_vector[i].find("reg") != std::string::npos) {
                size_t pos = signal_vector[i].find("reg");
                signal_vector[i] = signal_vector[i].substr(pos + 4); // Removes "reg" and following spaces
            }
        }
        return signal_vector;
    }





    vector<string> find_in(string rtl_code, string type){
        
        size_t first_index = (rtl_code.find("input") < rtl_code.find("output")) ? rtl_code.find("input") : rtl_code.find("output");

        // Find the index of the last occurrence of "input" or "output"
        size_t last_index = (rtl_code.rfind("output") > rtl_code.rfind("input")) ? rtl_code.find(';', rtl_code.rfind("output")) + 1 : rtl_code.find(';', rtl_code.rfind("input")) + 1;

        // Extract the code segment between the first occurrence and the last occurrence
        string extracted_code = rtl_code.substr(first_index, last_index - first_index);
        
        
        


        vector<std::string> input_vector;
        vector<std::string> output_vector;

        string temp = "";
        string signal = "";
        string size = "";
        bool discard_char = true;
        size_t inputCount;
        size_t outputCount;
        
        for (size_t i = 0; i < extracted_code.size(); i++) {
            temp += extracted_code[i];

            if (temp.find("input") != string::npos && temp.find("output") != string::npos) {
            temp = (temp.find("input") > temp.find("output")) ? "input" : "output";
            signal = "";
            size = "";
            discard_char = true;
            }


            // Second Block without std::count
            size_t inputCount = 0, outputCount = 0;
            size_t pos = 0;
            while ((pos = temp.find("input", pos)) != std::string::npos) {
                inputCount++;
                pos += 5; // Move past the found "input"
            }

            pos = 0;
            while ((pos = temp.find("output", pos)) != std::string::npos) {
                outputCount++;
                pos += 6; // Move past the found "output"
            }

            if (inputCount > 1 || outputCount > 1) {
                temp = (temp.find("input") != std::string::npos) ? "input" : "output";
                signal = "";
                size = "";
                // Continue with the loop iteration here or any other required action
            }
            
            if (extracted_code[i] == ')') {
                if (temp.find("input") != std::string::npos) {
                    input_vector.push_back(size + " " + signal);
                } else if (temp.find("output") != std::string::npos) {
                    output_vector.push_back(size + " " + signal);
                }
                break;
            }

            if (temp.find("input") != std::string::npos) {
                if (temp.find(']') != std::string::npos) {
                    size = temp.substr(temp.find('['), temp.find(']') - temp.find('[') + 1);
                    signal = "";
                    temp = "input";

                }
                else if (extracted_code[i] == ',' || extracted_code[i] == ';') {
                    input_vector.push_back(size + " " + signal);
                    signal = "";
                }
                else if (discard_char) {
                    discard_char = false;
                    continue;
                }
                else {
                    signal += extracted_code[i];
                }
            }

            if (temp.find("output") != std::string::npos) {
                if (temp.find(']') != std::string::npos) {
                    size = temp.substr(temp.find('['), temp.find(']') - temp.find('[') + 1);
                    signal = "";
                    temp = "output";
                }
                else if (extracted_code[i] == ',' || extracted_code[i] == ';') {
                    output_vector.push_back(size + " " + signal);
                    signal = "";
                }
                else if (discard_char) {
                    discard_char = false;
                    continue;
                }
                else {
                    signal += extracted_code[i];
                }
            }
        }

        output_vector = removeKeywords(output_vector);
        input_vector = removeKeywords(input_vector);

        if(type=="input"){
            return input_vector;
        }else{
            return output_vector;
        }
        

    
    }


    vector<string> removebrackets(std::vector<std::string> &signal_vector) {
        for (size_t i = 0; i < signal_vector.size(); ++i) {
            if (signal_vector[i].find("]") != std::string::npos) {
                size_t pos = signal_vector[i].find('[');
            if (pos != std::string::npos) {
              signal_vector[i] = signal_vector[i].substr(0, pos) + signal_vector[i].substr(signal_vector[i].find(']') + 1);
        }
            }
        }
        
        return signal_vector;
    }


    vector<string> instantiation(string rtl_code){
        vector<string>v1 = find_in(rtl_code, "input");
        vector<string>v2 = find_in(rtl_code, "output");
        v1 = removebrackets(v1);
        v2 = removebrackets(v2);
        vector<string> v3(v1.size() + v2.size());
        int count =v1.size();

        for(int i=0; i<v1.size();i++){v3[i] = v1[i];}
        for(int i= 0; i< v2.size() ; i++){
            v3[count++] = v2[i];
        }
        return v3;
    }
    

    
    bool find_rst(string filecontent){
        if (filecontent.find("rst") != std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    }


    bool find_if(string filecontent){
        if (filecontent.find("if") != std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    }

    bool find_Case(string filecontent){
        if (filecontent.find("case") != std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    }
    bool find_always_seq(string filecontent){
        if (filecontent.find("always(posedge clk") != std::string::npos || filecontent.find("always(negedge clk") != std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    }
    bool find_always_comb(string filecontent){
        if (filecontent.find("always(*)") != std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    }
    
};

parser::parser()
{
}


