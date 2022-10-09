#include "Include/ByteHandler.h"
#include "Include/TestModule.h"

int32_t
Test__ByteHandler()
{
   uint8_t testVar = 0;

   BEGIN_TEST_JOB
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
   END_TEST_JOB
}


TEST_MAIN()