#include "Include/TestModule.h"
#include "Include/Base64.h"

#include <vector>
#include <random>
#include <numeric>
#include <stdexcept>

static
std::string
InCreateRandomString()
{
   static std::random_device randomDevice;
   static std::mt19937 generator(randomDevice());
   static std::uniform_int_distribution<int32_t> symbDistr(std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
   static std::uniform_int_distribution<size_t> strlenDistr(200,250);
   
   std::string retVal;
   const size_t strLen = strlenDistr(generator);
   retVal.reserve(strLen);

   for(size_t i = 0; i < strLen; i++)
   {
      retVal.push_back(symbDistr(generator));
   }
   return retVal;
}


int32_t
Test__Base64Standard_Fuzzing()
{
   using namespace Base64;
   int32_t status = 0;
   constexpr size_t iterationCnt = 1000;
   do
   {
      std::string nonEncodedStr;
      std::string encodedStr;
      try
      { 
         for(size_t i = 0; i < iterationCnt; i++)
         {
            nonEncodedStr = InCreateRandomString();
            encodedStr = Base64Encode(nonEncodedStr,EncodeType::standard);
            TEST_ASSERT(nonEncodedStr,Base64Decode(encodedStr,EncodeType::standard),!=,status);
         }
         if(0 != status)
         {
            throw std::runtime_error("Bad str" + nonEncodedStr);
         }
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
      }
   } while (false);
   return status;
}

int32_t
Test__Base64Mime_Fuzzing()
{
   using namespace Base64;
   int32_t status = 0;
   constexpr size_t iterationCnt = 1000;
   do
   {
      std::string nonEncodedStr;
      std::string encodedStr;
      try
      { 
         for(size_t i = 0; i < iterationCnt; i++)
         {
            nonEncodedStr = InCreateRandomString();
            encodedStr = Base64Encode(nonEncodedStr,EncodeType::mime) ;
            std::cout << Base64Encode(nonEncodedStr, EncodeType::standard) << std::endl << std::endl;
            std::cout <<encodedStr;
            TEST_ASSERT(nonEncodedStr,Base64Decode(encodedStr,EncodeType::mime),!=,status);
         }
         if(0 != status)
         {
            throw std::runtime_error("Bad str" + nonEncodedStr);
            status = -1;
         }
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
      }
   } while (false);
   return status;
}

int32_t
Test__Base64Pem_Fuzzing()
{
   using namespace Base64;
   int32_t status = 0;
   constexpr size_t iterationCnt = 1000;
   do
   {
      std::string nonEncodedStr;
      std::string encodedStr;
      try
      { 
         for(size_t i = 0; i < iterationCnt; i++)
         {
            nonEncodedStr = InCreateRandomString();
            encodedStr = Base64Encode(nonEncodedStr,EncodeType::pem);
            TEST_ASSERT(nonEncodedStr,Base64Decode(encodedStr,EncodeType::pem),!=,status);
         }
         if(0 != status)
         {
            throw std::runtime_error("Bad str" + nonEncodedStr);
         }
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
      }
   } while (false);
   return status;
}


int32_t
Test__Radix64_Fuzzing()
{
   using namespace Base64;
   int32_t status = 0;
   constexpr size_t iterationCnt = 1000;
   do
   {
      std::string nonEncodedStr;
      std::string encodedStr;
      try
      { 
         for(size_t i = 0; i < iterationCnt; i++)
         {
            nonEncodedStr = InCreateRandomString();
            encodedStr = Base64Encode(nonEncodedStr,EncodeType::radix64);
            TEST_ASSERT(nonEncodedStr,Base64Decode(encodedStr,EncodeType::radix64),!=,status);
         }
         if(0 != status)
         {
            throw std::runtime_error("Bad str" + nonEncodedStr);
         }
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
      }
   } while (false);
   return status;
}

TEST_MAIN()