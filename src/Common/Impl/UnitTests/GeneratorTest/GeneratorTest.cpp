#include "Include/TestModule.h"
#include "Include/LayoutBitmapGenerator.h"
#include "Include/LayoutReader.hpp"


int32_t
Test__OutMsk()
{
    BEGIN_TEST_JOB
    constexpr std::string_view layoutFileName = TEST_FILES_DIR "out.msk";
    constexpr std::string_view etalonMatrixFileName = ETALON_FILES_DIR "outEtalonMatrix.txt";

    LayoutData layoutData;

    LayoutReader* reader = GetReader(std::wstring(layoutFileName.begin(), layoutFileName.end()));
    TEST_ASSERT(reader, nullptr, ==);
    TEST_ASSERT(reader->Read(&layoutData),false,==);
    FreeReader(reader);

    LayoutBitmapGenerator generator;
    Coord LeftTop(48,94);
    Coord RightBot(108, 150);
    generator.Init(&layoutData, { 23 }, LeftTop, RightBot);
    generator.Process(64, 64);

    LayoutMatrixPtr pMatrix =  generator.GetMatrix();

    LayoutMatrix etalonMatrix;
    etalonMatrix.ReadFile(std::string(etalonMatrixFileName.begin(),etalonMatrixFileName.end()));

    TEST_ASSERT(BitMatrix::Compare(*pMatrix, etalonMatrix), 0.99, <);

    END_TEST_JOB
}

TEST_MAIN()