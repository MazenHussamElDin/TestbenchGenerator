
#include "stimulus.cpp"

class TB_Writer
{
private:
    
public:
    TB_Writer();
    void tb_writer(){


    parser p;
    string Content;
    Content =p.readfile();


    ofstream test_bench(p.find_modulename(Content) + "tb.sv");
    if (test_bench.is_open()) {

        
        string modulename = p.find_modulename(Content);
        test_bench << "// Test bench code for " << modulename << " Module" << endl;
        
        //---------------------------------------------------------
        //module name
        test_bench << "module " <<  modulename << "_tb;" << endl;



        //---------------------------------------------------------
        // Regs and Wires
        vector<string> v_in = p.find_in(Content, "input");
        for(int i=0; i< v_in.size(); i++){
            test_bench << "reg " << v_in[i] << ";" << endl;
        }

        vector<string> v_out = p.find_in(Content,"output");
        for(int i=0; i< v_out.size(); i++){
            test_bench << "wire " << v_out[i] << ";" << endl;
        }
        if(!p.find_clk(Content)) {
            test_bench << "reg clk;" << endl;
        }


        //---------------------------------------------------------
        //instantiation
        test_bench << "\n\n\n //Instantiation \n" << p.find_modulename(Content) << " " << p.find_modulename(Content) <<"_test (";
        
        vector<string> v = p.instantiation(Content);
        for(int i=0; i<v.size()-1 ; i++){
            test_bench << "." << v[i] << "(" << v[i] << "), \n";
        }

        test_bench <<"." << v[v.size()-1] << "(" << v[v.size()-1] << ") "<< ");\n\n\n" << endl;
        //---------------------------------------------------------
        //clk
        test_bench << "//Clk Generation" << endl;
        test_bench << "initial begin\n clk = 0; \n forever\n #5 clk = ~clk;\n end\n\n" << endl;
        
        
        



        //---------------------------------------------------------
        // initial block

        test_bench << "\n\n\n initial\nbegin\n " << endl;
        
        stimulus s;
        v_in = p.removebrackets(v_in);
        //---------------------------------------------------------
        // reset on
        if(p.find_rst(Content)){
        test_bench << "rst=1;\n";
        for(int i=0; i<v_in.size(); i++){
            if(v_in[i] != "rst" && v_in[i] !="clk"){
            test_bench << v[i];
            test_bench << " = 0;\n";
            }
        }

        test_bench << "\n#10\nrst=0;";
        }
        vector<string> randomized(v_in.size());
        randomized = s.generate_random(v_in);
        test_bench << "\n\n\nrepeat(100)\n";
        for(int i=0; i<v_in.size(); i++){
            cout << v_in[i] << endl;
        }
        if(!(v_in.size() == 1 && (v_in[0] == "clk" || v_in[0] == "rst")) || 
        ((v_in.size() == 2) && 
        ((v_in[1] == "rst " )&&( v_in[0] == "clk")))){
            test_bench << "begin\n";
        }
        test_bench<< "#20\n";
        
        for(int i=0; i<randomized.size(); i++){
            test_bench << randomized[i] << endl;
        }
        if((v_in.size() == 1 && (v_in[0] == "clk" || v_in[0] == "rst")) || 
        ((v_in.size() == 2) && 
        ((v_in[1] == "rst" || v_in[1] == "clk")||(v_in[0] == "rst" || v_in[0] == "clk")))){
            test_bench << "\nend\n";
        }
            
        
        test_bench << "  \n\n $stop(); \n\n end";


        
        //---------------------------------------------------------
        // assertions
        vector<string> v_assert(v_out);
        v_assert = p.removebrackets(v_out);
        
        

        if(p.find_rst(Content)){
            test_bench << "\n\n //psl rst_assertion: assert always((rst ==1 ) -> next ( ";
            if(v_assert.size() == 1){
                v_assert[0] = p.removeSpaces(v_assert[0]);
                test_bench << v_assert[0] << "== 0) ) @(posedge clk);";
            }else{   
            for(int i=0; i<v_assert.size()-1; i++){
                v_assert[i] = p.removeSpaces(v_assert[i]);
                test_bench << v_assert[i] << "== 0 && ";
            }
            v_assert[v_assert.size() -1 ] = p.removeSpaces(v_assert[v_assert.size() -1 ]);
            test_bench << v_assert[v_assert.size()-1] ;
            test_bench << "))@(posedge clk);";
            }
        }



        //---------------------------------------------------------
        // monitor
        test_bench << " \n\n\ninitial \n$monitor(\"%t: "; 
        for(int i=0; i<v.size()-1 ; i++){
            test_bench << v[i] << " = %b,";
        }
        test_bench << v[v.size()-1]<< " = %b\"";
        test_bench << ",$time";
        for(int i=0; i<v.size() ; i++){
            test_bench << ", " << v[i] ;
        }
        test_bench <<");" << endl;
        
        test_bench << "\n\n\nendmodule" << endl;
        
        test_bench.close();
        cout << "Test bench file created: " << modulename << "_tb.sv" << endl;
        runfilewriter(modulename);
    } else {
        cerr << "Failed to create the test bench file." << endl;
        return; // Exit with an error code
    }
}

    void runfilewriter(string name){
        parser p;
        name = p.removeSpaces(name);
        ofstream run_do( name + "run.do");
        if (run_do.is_open()) {
            run_do << "vlib work\nvlog ";
            run_do << name;
            run_do << ".v ";
            run_do << name;
            run_do << "tb.sv +cover\nvsim -voptargs=+acc work.";
            run_do << name;
            run_do << "_tb -cover\nadd wave /";
            run_do << name;
            run_do << "_tb/";
            run_do << name;
            run_do << "_test/*\ncoverage save ";
            run_do << name;
            run_do << "_tb.ucdb -onexit -du ";
            run_do << name;
            run_do << "\nrun -all";

        } else {
        cerr << "Failed to create the test bench file." << endl;
        return; // Exit with an error code
        }
        


        
    }
};

TB_Writer::TB_Writer()
{
}


