#include "Include/LayoutMatrix.h"
#include "Include/LayoutReader.hpp"
#include "Include/TestModule.h"
#include "Include/Layout_comparator.h"

#include <utility>
#include <string>
#include <string_view>
#include <random>


int32_t
Test__CarryCellNonModified()
{
   BEGIN_TEST_JOB

   constexpr std::string_view fileName = TEST_FILES_DIR "carryCell.MSK";
   LayoutData layoutData;

   LayoutReader* reader = GetReader(std::wstring(fileName.begin(), fileName.end()));
   TEST_ASSERT(reader, nullptr, ==);
   TEST_ASSERT(reader->Read(&layoutData),false,==);
   FreeReader(reader);

   LayoutBitmapGenerator generator;

   generator.init(&layoutData,{48,94} ,{108,150}, { 23 });
   generator.process(16, 16);
   LayoutMatrix res =  generator.getMatrix();

   END_TEST_JOB
}


TEST_MAIN()