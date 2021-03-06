#include "model/instructions.h"
#include "controller/controller.h"
#include <stdio.h>

//===========================================
// main function
int main(int argc, char *argv[]) {

  //Getting the Instructions from the file in the command line argument
  if(argc != 2) {
    printf("Machine code file not valid.\n");
    return 1;
  }
  const std::string inputFileName(argv[1]);
  const Instructions instructions(inputFileName);

  //if we successfully loaded instructions then start the program
  if(instructions.getNumOfInstructions() != -1 && instructions.getInstructions() != NULL) {
    Controller controller(instructions);

    //start the controller component
    controller.start(instructions);
    return 0;
  }
}