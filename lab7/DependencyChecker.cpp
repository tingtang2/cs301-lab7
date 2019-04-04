#include "DependencyChecker.h"

DependencyChecker::DependencyChecker(int numRegisters)
/* Creates RegisterInfo entries for each of the 32 registers and creates lists for
 * dependencies and instructions.
 */
{
  RegisterInfo r;

  // Create entries for all registers
  for(int i = 0; i < numRegisters; i++){
    myCurrentState.insert(make_pair(i, r));
  }
}

void DependencyChecker::addInstruction(Instruction i)
/* Adds an instruction to the list of instructions and checks to see if that
 * instruction results in any new data dependencies.  If new data dependencies
 * are created with the addition of this instruction, appropriate entries
 * are added to the list of dependences.
 */
{
  InstType iType = i.getInstType();

  switch(iType){
  case RTYPE:
    // 2 source reg's rs,rt -> 2x checkForReadDependence
    // 1 dest reg rt -> 1x checkForReadDependence
    if (i.getRS() != 32)
        checkForReadDependence(i.getRS());
    if (i.getRT() != 32)
        checkForReadDependence(i.getRT());
    if (i.getRD() != 32)
        checkForWriteDependence(i.getRD());
    //cout << '\n';
    break;
  case ITYPE:
    // 1 source reg rs -> 1x checkForReadDependence
    // 1 dest reg rt -> 1x checkForReadDependence
    if (i.getRS() != 32)
        checkForReadDependence(i.getRS());
    if (i.getRT() != 32)
        checkForWriteDependence(i.getRT());
    //cout << '\n';
    break;
  case JTYPE:
    // Your code here
    break;
  default:
    // do nothing
    break;
  }

  myInstructions.push_back(i);

}

void DependencyChecker::checkForReadDependence(unsigned int reg)
  /* Determines if a read data dependence occurs when reg is read by the current
   * instruction.  If so, adds an entry to the list of dependences. Also updates
   * the appropriate RegisterInfo entry regardless of dependence detection.
   */
{
    if(myCurrentState.at(reg).lastInstructionToAccess != -1) { // reg read or written in prev instruction
    // check if prev instruction wrote to reg (RAW)
        if (myCurrentState.at(reg).accessType == WRITE) {
            // RAW dependency
            Dependence d;
            d.dependenceType = RAW;
            d.registerNumber = reg;
            d.previousInstructionNumber = myCurrentState.at(reg).lastInstructionToAccess;
            d.currentInstructionNumber = myInstructions.size();
            myDependences.push_back(d);
        }
    }

    // Update RegisterInfo for reg - lastInstructionToAccess is now the current instruction
    myCurrentState.at(reg).lastInstructionToAccess = myInstructions.size();

    // Update RegisterInfo for reg - accessType is now READ
    myCurrentState.at(reg).accessType = READ;

  // Your code here

}


void DependencyChecker::checkForWriteDependence(unsigned int reg)
  /* Determines if a write data dependence occurs when reg is written by the current
   * instruction.  If so, adds an entry to the list of dependences. Also updates
   * the appropriate RegisterInfo entry regardless of dependence detection.
   */
{

    if(myCurrentState.at(reg).lastInstructionToAccess != -1) { // reg read or written in prev instruction
    // check if prev instruction read from reg (WAR), or wrote to reg (WAW)
        Dependence d;
        if (myCurrentState.at(reg).accessType == READ) {
            // WAR dependency
            d.dependenceType = WAR;
            d.registerNumber = reg;
            d.previousInstructionNumber = myCurrentState.at(reg).lastInstructionToAccess;
            d.currentInstructionNumber = myInstructions.size();
            myDependences.push_back(d);
        }
        else if (myCurrentState.at(reg).accessType == WRITE) {
            // WAW dependency
            d.dependenceType = WAW;
            d.registerNumber = reg;
            d.previousInstructionNumber = myCurrentState.at(reg).lastInstructionToAccess;
            d.currentInstructionNumber = myInstructions.size();
            myDependences.push_back(d);        }
    }
    // Update RegisterInfo for reg - lastInstructionToAccess is now the current instruction
    myCurrentState.at(reg).lastInstructionToAccess = myInstructions.size();

    // Update RegisterInfo for reg - accessType is now READ
    myCurrentState.at(reg).accessType = WRITE;
  // Your code here

}


void DependencyChecker::printDependences()
  /* Prints out the sequence of instructions followed by the sequence of data
   * dependencies.
   */
{
  // First, print all instructions
  list<Instruction>::iterator liter;
  int i = 0;
  cout << "INSTRUCTIONS:" << endl;
  for(liter = myInstructions.begin(); liter != myInstructions.end(); liter++){
    cout << i << ": " << (*liter).getAssembly() << endl;
    i++;
  }

  // Second, print all dependences
  list<Dependence>::iterator diter;
  cout << "DEPENDENCES: \nType Register (FirstInstr#, SecondInstr#) " << endl;
  for(diter = myDependences.begin(); diter != myDependences.end(); diter++){
    switch( (*diter).dependenceType){
    case RAW:
      cout << "RAW \t";
      break;
    case WAR:
      cout << "WAR \t";
      break;
    case WAW:
      cout << "WAW \t";
      break;
    default:
      break;
    }

    cout << "$" << (*diter).registerNumber << " \t";
    cout << "(" << (*diter).previousInstructionNumber << ", ";
    cout << (*diter).currentInstructionNumber << ")" << endl;
  }


}
