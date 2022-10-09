#ifndef __TEST_MODULE_H__
#define __TEST_MODULE_H__

#include <iostream>
#include <string>


#define STRINGIFY(Name) #Name
#define WSTRINGIFY(Name) L #Name

#define TEST_ASSERT(lhsExpr, rhsExpr, operator, flag)\
   if((lhsExpr) operator (rhsExpr))\
   {\
      std::cout << "Assertion failed in " << __FILE__ << " on line " << __LINE__ << "\n"; \
      flag = -1;\
      break;\
   }\


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