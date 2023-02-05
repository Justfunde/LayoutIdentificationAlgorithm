#include "Include/TestModule.h"
#include "Include/LayoutBitmapGenerator.h"
#include "Include/LayoutReader.hpp"


int
Test__CarryCell()
{
    BEGIN_TEST_JOB
    constexpr std::string_view fileName = TEST_FILES_DIR "elements_1.msk";
    LayoutData layoutData;

    LayoutReader* reader = GetReader(std::wstring(fileName.begin(), fileName.end()));
    TEST_ASSERT(reader, nullptr, ==);
    TEST_ASSERT(reader->Read(&layoutData),false,==);
    FreeReader(reader);

    LayoutBitmapGenerator generator;
    Coord LeftTop(-121,-1013);
    Coord RightBot(1086,221);
    generator.Init(&layoutData, { 23 }, LeftTop, RightBot);
    generator.Process(1024, 256);

    END_TEST_JOB
}

TEST_MAIN()