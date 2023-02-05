#include "Include/TestModule.h"
#include "Include/LayoutBitmapGenerator.h"
#include "Include/LayoutReader.hpp"


static
std::vector<int16_t>
InGetLayerNumArr(
    LayoutData* Data)
{
    std::vector<int16_t> layerNumArr;

    if(nullptr == Data) { return layerNumArr;}

    for(auto& it : Data->libraries[0]->layers)
    {
        layerNumArr.push_back(it.layer);
    }
    return layerNumArr;
}

constexpr size_t g_matrSz = 64;





int32_t
Test__OutMsk()
{
    BEGIN_TEST_JOB
    constexpr std::string_view layoutFileName = TEST_FILES_DIR "out.msk";

    LayoutData layoutData;
    LayoutReader* reader = GetReader(std::wstring(layoutFileName.begin(), layoutFileName.end()));
    LayoutBitmapGenerator generator;
     

    TEST_ASSERT(reader, nullptr, ==);
    TEST_ASSERT(reader->Read(&layoutData),false,==);
    FreeReader(reader);

    
    Coord LeftTop(48, 94);
    Coord RightBot(108, 150);
    generator.Init(&layoutData, { 0 }, LeftTop, RightBot);


    const auto layerNumArr = InGetLayerNumArr(&layoutData);
    for(size_t i = 0; i < layerNumArr.size(); ++i)
    {
        generator.SetLayerArray({layerNumArr[i]});
        generator.Process(g_matrSz, g_matrSz);

        LayoutMatrixPtr pMatrix =  generator.GetMatrix();
        std::string resFileName = ETALON_FILES_DIR "outEtalonMatrix_" + std::to_string(layerNumArr[i]) + ".txt";

        //pMatrix->FlushFile(resFileName);
        LayoutMatrix etalonMatrix;
        etalonMatrix.ReadFile(std::string(resFileName.begin(),resFileName.end()));
        TEST_ASSERT(BitMatrix::Compare(etalonMatrix, *pMatrix), 0.99, <);
    }
    END_TEST_JOB
}


int32_t
Test__Elements_1()
{
    BEGIN_TEST_JOB
    constexpr std::string_view layoutFileName = TEST_FILES_DIR "elements_1.msk";

    LayoutData layoutData;
    LayoutReader* reader = GetReader(std::wstring(layoutFileName.begin(), layoutFileName.end()));
    LayoutBitmapGenerator generator;
     

    TEST_ASSERT(reader, nullptr, ==);
    TEST_ASSERT(reader->Read(&layoutData),false,==);
    FreeReader(reader);

    
    Coord LeftTop(-121, -1013);
    Coord RightBot(1086, 221);
    generator.Init(&layoutData, { 0 }, LeftTop, RightBot);


    const auto layerNumArr = InGetLayerNumArr(&layoutData);
    for(size_t i = 0; i < layerNumArr.size(); ++i)
    {
        generator.SetLayerArray({layerNumArr[i]});
        generator.Process(g_matrSz, g_matrSz);

        LayoutMatrixPtr pMatrix =  generator.GetMatrix();
        std::string resFileName = ETALON_FILES_DIR "elements_1_" + std::to_string(layerNumArr[i]) + ".txt";

        //pMatrix->FlushFile(resFileName);
        LayoutMatrix etalonMatrix;
        etalonMatrix.ReadFile(std::string(resFileName.begin(),resFileName.end()));
        TEST_ASSERT(BitMatrix::Compare(etalonMatrix, *pMatrix), 0.99, <);
    }
    END_TEST_JOB
}

TEST_MAIN()