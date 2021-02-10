#include <iostream>
#include <SoDa/Command.hxx>
#include <string>
#include <vector>

int main(int argc, char * argv[])
{
  SoDa::Command cmd;
  
  int int_arg;
  bool bool_arg, pres_arg;
  std::string str_arg;
  std::vector<std::string> strvec_arg; 
  
  // ptr-to-value, long name, short name, [def val [, str doc [, test func [, errmsg]]]]
  cmd.add<int>(&int_arg, "intarg", 'i', -1,
	       "An integer argument between -5 and 5 inclusive", 
	       [](int v) { return (v >= -5) && (v <= 5); },
	       "Please pick something from -5 to 5.")
    .addP(&pres_arg, "presarg", 'p')    
    .add<bool>(&bool_arg, "boolarg", 'b', false, "true/false/zero/non-zero")
    .add<std::string>(&str_arg, "strarg", 's', "Not Specified")
    .addV<std::string>(&strvec_arg, "strvecarg", 'l');
  
  cmd.parse(argc, argv);

  std::cout << "intarg = " << int_arg << "\n";
  std::cout << "boolarg = " << bool_arg << "\n";
  std::cout << "pres_arg = " << pres_arg << "\n";
  std::cout << "str_arg = [" << str_arg << "]\n";
  std::cout << "strvecarg s = \n";
  for(auto sa : strvec_arg) {
    std::cout << "\t[" << sa << "]\n";
  }

  std::cout << "posargs = \n";
  for(int i = 0; i < cmd.numPosArgs(); i++) {
    std::cout << "\t" << i << "\t" << cmd.getPosArg(i) << "\n";
  }
  
}
