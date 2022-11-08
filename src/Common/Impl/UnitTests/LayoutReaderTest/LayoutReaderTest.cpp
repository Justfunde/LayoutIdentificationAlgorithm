#include "Include/TestModule.h"
#include "Include/LayoutReader.hpp"

#include <string>
#include <vector>
#include <iostream>

int32_t
Test__MskReader()
{
   BEGIN_TEST_JOB
   
   constexpr uint32_t libraryCntEtalon = 1;
   constexpr uint32_t elementCntEtalon = 1;
   constexpr std::string_view elementNameEtalon = "carryCell";
   constexpr uint32_t geometriesCntEtalon = 75;
   const std::vector<int16_t> etalonLayers =  { -5, 1, 17, 16, 19, 13, 23, -4, 27, 34 };


   constexpr std::string_view fileName = TEST_FILES_DIR "carryCell.MSK";

   LayoutData  layout;

   LayoutReader* p_reader = GetReader(std::wstring(fileName.begin(), fileName.end()));
   TEST_ASSERT(p_reader, nullptr, ==);
   TEST_ASSERT(p_reader->Read(&layout), false, ==)
   FreeReader(p_reader);

   TEST_ASSERT(layout.libraries.size(),libraryCntEtalon,!=)
   TEST_ASSERT(layout.libraries[0]->elements.size(), elementCntEtalon, !=);
   for(auto it:layout.libraries[0]->layers)
   {
      std::cout << it.layer << std::endl;
   }
   TEST_ASSERT(layout.libraries[0]->layers.size(), etalonLayers.size(), !=)

   const Element* pElem = layout.libraries[0]->elements[0];
   TEST_ASSERT(pElem->name, elementNameEtalon, !=);
   TEST_ASSERT(pElem->geometries.size(), geometriesCntEtalon, !=)

   for(size_t i = 0; i < etalonLayers.size(); i++)
   {
      TEST_ASSERT(layout.libraries[0]->layers[i].layer, etalonLayers[i], !=);
   }
   END_TEST_JOB
}

TEST_MAIN()