#include <iostream>
#include <fstream>
#include <string>	
using namespace std;

int printOpcode(string op, ofstream *newfile) {
	if(op == "ADD") {
		*newfile << 0;
	}
	else if(op == "ADDI") {
		*newfile << 1;
	}
	else if(op == "SUB") {
		*newfile << 2;
	}
	else if(op == "B") {
		*newfile << 3;
	}
	else if(op == "LD") {
		*newfile << 4;
	}
	else if(op == "STR") {
		*newfile << 5;
	}
	*newfile << " ";
}

void printOperands(string operands, ofstream *newfile) {
	int pos = operands.find(" ");
	string operand;
	while(pos != string::npos) {
		operand = operands.substr(0,pos);
		if(operand[0] == 'R') {
			operand = operand.substr(1, operand.size());
		}
		*newfile << operand << ' ';
		operands = operands.substr(pos + 1, operands.size());
		pos = operands.find(" ");
	}
	*newfile << operands;
}

string createOutputFileName(string inputFileName) {
    string name = inputFileName.substr(0, inputFileName.find('.'));
    string extension = ".mac";
    string outputFileName = name + extension;
    return outputFileName;
}

int main(int argc, char *argv[]) {

	//make sure an argument is given
	if(argc != 2) {
		cout << "incorrect arguments\n";
		return 1;
	}

    //get the input file name
    string inputFileName(argv[1]);
    cout << "Input assembly file: " + inputFileName + '\n';

    //create the new file name
	string outputFileName = createOutputFileName(inputFileName);
    cout << "Output machine file: " + outputFileName + "\n";

	//the input and output files
	ifstream inputFile(inputFileName.c_str());
	ofstream outputFile(outputFileName.c_str());

	//begin writing machine to the output file
	string line;
	if(inputFile.is_open() && outputFile.is_open()) {
		while (getline(inputFile,line)) {
            
            //get the opcode
			string delimiter = " ";
			int delimiterPos = line.find(delimiter);
			string opstr = line.substr(0, delimiterPos);
			printOpcode(opstr, &outputFile);

			//get the operands
			string operands = line.substr(delimiterPos + 1, line.size());
			printOperands(operands, &outputFile);

            //finish line
			outputFile << '\n';

		}

		//close the files
		inputFile.close();
		outputFile.close();

		//print success message
		cout << "assembler was successful.\n";
	}
	else {
		//print failure message
		cout << "Unable to open file\n";
	}
	return 0;
}