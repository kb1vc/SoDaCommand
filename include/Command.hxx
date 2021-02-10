#pragma once
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
#include <stdexcept>

namespace SoDa {

  class BadArgException : public std::runtime_error {
  public:
    BadArgException(const std::string & tkn) : std::runtime_error("Bad argument specifier: " + tkn) {
    }
  }; 
  
  class Command {
  public:
    Command();

    Command & addInfo(const std::string & info);

    template <typename T>
    static bool allGood(T v) { return true; }
    
    template <typename T>
    Command & add(T * val, 
		  const std::string & long_name, 
		  char ab_name, 
		  T def_val,
		  const std::string & doc_str = std::string(""),
		  const std::function<bool(T)> & test_func = [](T v){ return true; },
		  const std::string & err_msg = std::string("")) {

      // create an arg object and push it. 
      auto arg_p = new Arg<T>(val, def_val,
			      doc_str, test_func, err_msg);

      registerArg(arg_p, long_name, ab_name);
      return *this;      
    }

		  
    Command & addP(bool * val,
		   const std::string & long_name, 
		   char ab_name, 
		   const std::string & doc_str = std::string("")) {
      auto arg_p = new ArgPresent(val, doc_str);
      registerArg(arg_p, long_name, ab_name);
      return *this;      
    }
		   
    template <typename T>
    Command & addV(std::vector<T> * val,
		   const std::string & long_name, 
		   char ab_name, 
		   const std::string & doc_str = std::string(""),
		   const std::function<bool(T)> & test_func = [](T val){ return true; },
		   const std::string & err_msg = std::string("")) {

      auto arg_p = new ArgVec<T>(val, doc_str, test_func, err_msg);
      registerArg(arg_p, long_name, ab_name);
      
      return *this;
    }
		   
		   
    Command & parse(int argc, char * argv[]);
    
    std::ostream & printHelp(std::ostream & ostr); 

    bool isPresent(const std::string & long_name);

    bool isPresent(char ab_name);

    std::string getPosArg(int idx);

    int numPosArgs() { return pos_arg_vec.size(); }

  private:

    int isSwitch(const std::string & tkn);
    
    class ArgBase {
    public:
      ArgBase(const std::string & doc_str, 
	      const std::string & err_msg) : 
	doc_str(doc_str), err_msg(err_msg) {
	present = false; 
      }

      std::ostream & printHelp(std::ostream & os);

      bool isPresent() {
	return present; 
      }

      virtual bool isPresentArg() { 
	return false; }

      virtual bool setVal(const std::string & vstr) = 0;

      virtual bool setPresent() {
	bool old = present; 
	present = true; 
	return old; 
      }
    
      void setNames(const std::string & ln, char abn) {
	long_name = ln;
	ab_name =abn; 
      }
      
    protected:
      
      template<typename T> 
      void setValBase(T * v, const std::string & vstr) {
	std::stringstream ss(vstr, std::ios::in); 
	ss >> *v;
      }

      template<>
      void setValBase(std::string * v, const std::string & vstr) {
	*v = vstr; 
      }
      
      std::string doc_str;
      std::string err_msg;
      
      std::string long_name;
      char ab_name; 
      bool present; 
    }; 

    template <typename T> 
    class Arg : public ArgBase {
    public:
      Arg(T * val, 
	  T def_val = {0},
	  const std::string & doc_str = std::string(""),
	  const std::function<bool(T)> & test_func = allGood,
	  const std::string & err_msg = std::string("")) : 
	ArgBase(doc_str, err_msg), val_p(val)
      {
	*val = def_val; 
      }

      bool setVal(const std::string & vstr) {
	setValBase(val_p, vstr); 
	return setPresent(); 
      }

    protected: 
      T * val_p;
    };


    template <typename T>
    class ArgVec : public ArgBase {
    public:
      ArgVec(std::vector<T> * v_vec, const std::string & doc_str, 
	     const std::function<bool(T)> & test_func = allGood, 
	     const std::string & err_msg = std::string("")) :
	ArgBase(doc_str, err_msg), 
	argvec_p(v_vec)
      {
      }

      bool setVal(const std::string & vstr) {
	T v;
	setValBase(&v, vstr); 
	argvec_p->push_back(v);

	present = true; 	
	
	return true; 
      }

      std::vector<T> * argvec_p; 
    };

    class ArgPresent : public ArgBase {
    public:
      ArgPresent(bool * val, 
		 const std::string & doc_str = std::string(""))
	: ArgBase(doc_str, "") {
	*val = false; 
	val_p = val; 
      }

      bool isPresentArg() { 
	return true; 
      }

      bool setVal(const std::string & vstr) { 
	*val_p = true; 
	present = true; 
	return true; 
      }

      bool setPresent() {
	*val_p = true;
	return ArgBase::setPresent();
      }
      
      bool * val_p; 
    };

    void registerArg(ArgBase * arg_p, 
		  const std::string & long_name, 
		  char ab_name); 

    ArgBase * findArg(char c);

    ArgBase * findArg(const std::string & key);


    std::list<std::string> buildTokenList(int argc, char * argv[]);
    
    std::map<std::string, ArgBase * > long_map;
    std::map<char, ArgBase * > ab_map; 
    std::map<std::string, char> long_2_ab_map; 

    std::list<std::string> info_list; 

    std::vector<std::string> pos_arg_vec; 
  };
  
}
