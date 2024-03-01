#include "parser.cpp"

class stimulus
{
private:
    
public:
    stimulus();
    vector<string> generate_random(vector<string> v_in){
        vector<string> random_values(v_in.size());
        
        for(int i=0; i<v_in.size(); i++){
            if(v_in[i] != "rst" && v_in[i]!= "clk"){
            random_values[i] = v_in[i] + " = $random();";
            }
        }
       return random_values;
    }
};

stimulus::stimulus(/* args */)
{
}


