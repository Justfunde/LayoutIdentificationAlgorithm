#ifndef __TEST_MODULE_H__
#define __TEST_MODULE_H__

#include <iostream>
#include <string>
#include <stdexcept>


#define STRINGIFY(Name) #Name
#define WSTRINGIFY(Name) L #Name



#define BEGIN_TEST_JOB\
   try{\

#define END_TEST_JOB\
   }\
   catch(const std::exception& ex)\
   {\
      std::cerr << "Exception happened :( Error:" << ex.what() << std::endl;\
      return -1;\
   }\
   return 0;\


#define TEST_ASSERT(lhsExpr, rhsExpr, operator)\
   if((lhsExpr) operator (rhsExpr))  { throw std::runtime_error(std::string(std::string("Assertion failed in ") + std::string(__FILE__) + std::string(" on line "))  + std::to_string(__LINE__ )); }\

#define ASSERT() (throw std::runtime_error(std::string(std::string("Assertion failed in ") + std::string(__FILE__) + std::string(" on line "))  + std::to_string(__LINE__ )))

#define EXEC_TEST_FUNCTION(STEP, FUNC)\
   const std::string infoStr(std::string(STRINGIFY(STEP)) + " step: '" STRINGIFY(FUNC) "' ");\
   std::cout << infoStr << "started\n";\
   status = FUNC();\
   if(0 != status) { std::cout << infoStr << "failed\n"; break;}\
   else { std::cout << infoStr << "completed\n"; break;}\

#define JOB_STEP EXEC_TEST_FUNCTION(Job, FUNCTION_NAME)

#define TEST_MAIN() \
int main()\
{\
   int32_t status = 0;\
   do\
   {\
      JOB_STEP\
   } while (false);\
   return status;\
}


#endif //!__TEST_MODULE_H__