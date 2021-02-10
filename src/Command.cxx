#include "Command.hxx"

namespace SoDa {

  Command::Command() {
    // not much to do. 
  }

  Command & Command::addInfo(const std::string & info) {
    info_list.push_back(info); 
    return * this; 
  }

  std::ostream &  Command::printHelp(std::ostream & os) {
    // print the description
    for(auto istr : info_list) {
      os << istr << "\n";
    }

    // now print the arguments
    for(auto ele : long_map) {
      auto arg_p = ele.second;
      os << "--" << ele.first << "\t-" << long_2_ab_map[ele.first] << "\t";
      arg_p->printHelp(os);
    }

    return os; 
  }

  std::string Command::getPosArg(int idx) {
    if(idx > pos_arg_vec.size()) return std::string("");
    else return pos_arg_vec[idx]; 
  }

  std::list<std::string> Command::buildTokenList(int argc, char * argv[]) {
    std::list<std::string> ret; 
    if(argc < 2) return ret;

    for(int i = 1; i < argc; i++) {
      ret.push_back(std::string(argv[i]));
    }
    
    return ret; 
  }

  int Command::isSwitch(const std::string & tkn) {
    if(tkn.length() < 2) return 0;

    // does it start with anything other than - ? 
    if(tkn[0] != '-') {
      return 0; 
    }
    
    // it starts with a -.  Is that all there is? 
    if(tkn.length() < 2) return 0;
    
    // it starts with a -. It is at least two instrs long.
    // does it start with -- ? 
    if(tkn[1] == '-') {
      // it starts with -- .  If it isn't at least 3 long, then this is just --
      if(tkn.length() < 3) return 0; 

      // it starts with -- and is followed by something
      return 2; 
    }

    // it starts with - and is followed by something. 
    return 1; 
  }

  Command & Command::parse(int argc, char * argv[]) {
    std::list<std::string> tokens = buildTokenList(argc, argv);

    ArgBase * arg_p = nullptr;
    
    while(!tokens.empty()) {
      std::string tkn = tokens.front();
      tokens.pop_front();

      int sw_len = isSwitch(tkn);

      if(sw_len == 0) {
	// we know this is either a positional argument, or 
	// a value for the previous arg_p;
	if(arg_p != nullptr) {
	  arg_p->setVal(tkn);
	  arg_p = nullptr; 
	}
	else {
	  // it is a positional argument.
	  pos_arg_vec.push_back(tkn);
	}
      }
      else {
	if(arg_p != nullptr) {
	  arg_p->setPresent(); 
	  arg_p = nullptr; 	  
	}
	
	if(sw_len == 1) {
	  arg_p = findArg(tkn[1]);
	}
	else {
	  arg_p = findArg(tkn.substr(2));
	}

	if(arg_p == nullptr) {
	  throw BadArgException(tkn); 
	}

	if(arg_p->isPresentArg()) {
	  arg_p->setPresent();
	  arg_p = nullptr; 
	}
      }
    }
    
    return * this; 
  }

  Command::ArgBase * Command::findArg(const std::string & long_name) {
    if(long_map.find(long_name) == long_map.end()) {
      return nullptr;
    }
    return long_map[long_name];
  }

  Command::ArgBase * Command::findArg(char ab_name) {
    if(ab_map.find(ab_name) == ab_map.end()) {
      return nullptr;
    }
    return ab_map[ab_name];
  }


  bool Command::isPresent(const std::string & long_name) {
    if(long_map.find(long_name) == long_map.end()) {
      return false; 
    }
    return long_map[long_name]->isPresent();
  }

  bool Command::isPresent(char ab_name) {
    if(ab_map.find(ab_name) == ab_map.end()) {
      return false; 
    }
    return ab_map[ab_name]->isPresent();
  }

  void Command::registerArg(ArgBase * arg_p, 
		  const std::string & long_name, 
			    char ab_name) {
    arg_p->setNames(long_name, ab_name); 
    long_map[long_name] = arg_p;
    ab_map[ab_name] = arg_p;       
    long_2_ab_map[long_name] = ab_name; 
  }  

  
  std::ostream & Command::ArgBase::printHelp(std::ostream & os) {
    os << doc_str << "\n";
    return os; 
  }
}
