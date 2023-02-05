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
   static std::uniform_int_distribution<size_t> distribution(50,120);
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
      matrix.Resize(InRandMatrixSz(),InRandMatrixSz());

      for(uint32_t i = 0; i < matrix.GetRowCount(); i++)
      {
         for(uint32_t j = 0; j < matrix.GetColumnCount(); j++)
         {
            TEST_ASSERT(matrix.Get(i, j), 0, !=);
            matrix.Set(i, j, 1);
            TEST_ASSERT(matrix.Get(i, j), 1, !=);
         }
      }
      matrix.Zeros();
      double ZeroRatio = matrix.ZeroRatio();
      double onesRatio = matrix.OneRatio();
      if(ZeroRatio > 1.001 || ZeroRatio < 0.999)
      {
         ASSERT();
      }

      if(onesRatio > 0.001 || onesRatio < -0.001)
      {
         ASSERT();
      }

      for(uint32_t i = 0; i < matrix.GetRowCount(); i++)
      {
         for(uint32_t j = 0; j < matrix.GetColumnCount(); j++)
         {
            TEST_ASSERT(matrix.Get(i, j), 0, !=);
         }
      }

      matrix.Ones();
      ZeroRatio = matrix.ZeroRatio();
      onesRatio = matrix.OneRatio();
      if(onesRatio > 1.001 || onesRatio < 0.999)
      {
         ASSERT();
      }

      if(ZeroRatio > 0.001 || ZeroRatio < -0.001)
      {
         ASSERT();
      }

      for(uint32_t i = 0; i < matrix.GetRowCount(); i++)
      {
         for(uint32_t j = 0; j < matrix.GetColumnCount(); j++)
         {
            TEST_ASSERT(matrix.Get(i, j), 1, !=);
         }
      }
   }
   END_TEST_JOB
}

int32_t
Test__BitMatrixEncodeFuzzing()
{
   size_t cycleCnt = 100;
   BEGIN_TEST_JOB

   while(cycleCnt--)
   {
      uint32_t rows = InRandMatrixSz();
      uint32_t cols = InRandMatrixSz();
      LayoutMatrix matrix(rows,cols);

      matrix.Randm();
      std::string encodedStr = LayoutMatrix::EncodeHash(matrix);
      BitMatrix decodedMatrix = LayoutMatrix::DecodeHash(encodedStr);
      TEST_ASSERT(decodedMatrix, matrix, !=);
   }

   END_TEST_JOB
}

int32_t
Test__BitMatrixSerializationFuzzing()
{
   size_t cycleCnt = 100;
   BEGIN_TEST_JOB

   while(cycleCnt--)
   {
      uint32_t rows = 100;
      uint32_t cols = 50;
      BitMatrix matrix(rows,cols);

      matrix.Randm();
      matrix.Print();
      
      ByteVector vec =  matrix.Serialize();
      TEST_ASSERT(vec.empty(), true, == );
      std::cout << "\n\n\n"; 
      BitMatrix resMatrix = BitMatrix::DeserializeMatrix(vec);
      resMatrix.Print();
      TEST_ASSERT(resMatrix, matrix, !=);
   }
   END_TEST_JOB
}


TEST_MAIN()