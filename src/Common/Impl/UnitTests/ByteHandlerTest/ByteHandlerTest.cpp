#include "Include/ByteHandler.h"
#include "Include/TestModule.h"

int32_t
Test__ByteHandler()
{
   int32_t status = 0;
   do
   {
      uint8_t testVar = 0;
      try
      {
         //test getters
         for(uint8_t i = 0; i < BITS_IN_BYTE; i++)
         {
            TEST_ASSERT(ByteHandler::GetBit(testVar, i), 0, !=, status);
         }
         TEST_ASSERT(status, 0, !=, status);

         testVar = 0xFF;
         for(uint8_t i = 0; i < BITS_IN_BYTE; i++)
         {
            TEST_ASSERT(ByteHandler::GetBit(testVar, i), 1, !=, status);
         }
         TEST_ASSERT(status, 0, !=, status);

         //test set-get combination

         testVar = 0;
         for(int8_t i = BITS_IN_BYTE - 1; i >= 0; i--)
         {
            ByteHandler::SetBit(testVar, i, 1);
            TEST_ASSERT(ByteHandler::GetBit(testVar, i), 1, !=, status);
         }
         TEST_ASSERT(status, 0, !=, status);

         testVar = 0b01001000;
         TEST_ASSERT(ByteHandler::GetHigherBitPos(testVar), 6, !=, status);
         TEST_ASSERT(ByteHandler::GetLowerBitPos(testVar), 3, !=, status);
         ////array test;
         //constexpr size_t arrSz = 200;
         //int32_t arr[arrSz] = {0};
         //for(size_t i = 0; i<)
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
         status = -1;
      }
   } while (false);
   return status;   
}


TEST_MAIN()