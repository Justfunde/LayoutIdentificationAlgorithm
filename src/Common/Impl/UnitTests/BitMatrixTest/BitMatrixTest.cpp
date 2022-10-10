#include "Include/LayoutMatrix.h"
#include "Include/TestModule.h"

#include <utility>
#include <random>

static
size_t
InRandMatrixSz()
{
   static std::random_device randDevice;
   static std::mt19937 generator(randDevice());
   static std::uniform_int_distribution<size_t> distribution(5,1000);
   return static_cast<size_t>(distribution(generator));
}




int32_t
Test__BitMatrixFuzzing()
{
   size_t cycleCnt = 1000;
   BEGIN_TEST_JOB

   while (cycleCnt--)
   {
      LayoutMatrix matrix(InRandMatrixSz(),InRandMatrixSz());

      for(uint32_t i = 0; i < matrix.GetIsize(); i++)
      {
         for(uint32_t j = 0; j < matrix.GetJsize(); j++)
         {
            TEST_ASSERT(matrix.Get(i, j), 0, !=, status);
            matrix.Set(i, j, 1);
            TEST_ASSERT(matrix.Get(i, j), 1, !=, status);
         }
         ASSERT_IF_ERROR();
      }
      ASSERT_IF_ERROR();
      matrix.Zeros();
      double zeroRatio = matrix.zeroRatio();
      double onesRatio = matrix.OneRatio();
      if(zeroRatio > 1.001 || zeroRatio < 0.999)
      {
         status = STATUS_FAILURE; break;
      }

      if(onesRatio > 0.001 || onesRatio < -0.001)
      {
         status = STATUS_FAILURE; break;
      }

      for(uint32_t i = 0; i < matrix.GetIsize(); i++)
      {
         for(uint32_t j = 0; j < matrix.GetJsize(); j++)
         {
            TEST_ASSERT(matrix.Get(i, j), 0, !=, status);
         }
         ASSERT_IF_ERROR();
      }
      ASSERT_IF_ERROR();

      matrix.Ones();
      zeroRatio = matrix.zeroRatio();
      onesRatio = matrix.OneRatio();
      if(onesRatio > 1.001 || onesRatio < 0.999)
      {
         status = STATUS_FAILURE; break;
      }

      if(zeroRatio > 0.001 || zeroRatio < -0.001)
      {
         status = STATUS_FAILURE; break;
      }

      for(uint32_t i = 0; i < matrix.GetIsize(); i++)
      {
         for(uint32_t j = 0; j < matrix.GetJsize(); j++)
         {
            TEST_ASSERT(matrix.Get(i, j), 1, !=, status);
         }
         ASSERT_IF_ERROR();
      }
      ASSERT_IF_ERROR();
   }
   END_TEST_JOB
}

int32_t
Test__BitMatrixEncodeFuzzing()
{
   size_t cycleCnt = 1000;
   BEGIN_TEST_JOB

   while(cycleCnt--)
   {
      LayoutMatrix matrix(InRandMatrixSz(),InRandMatrixSz());

      matrix.Randm();

      TEST_ASSERT(static_cast<BitMatrix>(LayoutMatrix::DecodeHash(LayoutMatrix::EncodeHash(matrix))),static_cast<BitMatrix>(matrix), !=, status);
   }

   END_TEST_JOB
}


TEST_MAIN()