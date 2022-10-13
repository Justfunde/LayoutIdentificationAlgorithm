#include "Include/TestModule.h"
#include "Include/LayoutReader_MSK.hpp"

#include <string>
#include <array>

int32_t
Test__MskReader()
{
   BEGIN_TEST_JOB
   
   constexpr uint32_t libraryCntEtalon = 1;
   constexpr uint32_t elementCntEtalon = 1;
   constexpr std::string_view elementNameEtalon = "carryCell";
   constexpr uint32_t geometriesCntEtalon = 76;
   constexpr uint32_t layerCntEtalon = 8;
   constexpr std::array<int16_t, layerCntEtalon> etalonLayers =  { 1, 17, 16, 19, 13, 23, 27, 34  };


   constexpr std::string_view fileName = TEST_FILES_DIR "carryCell.MSK";

   LayoutData  layout;

   LayoutReader* p_reader = GetReader(std::wstring(fileName.begin(), fileName.end()));
   TEST_ASSERT(p_reader, nullptr, ==);
   TEST_ASSERT(p_reader->Read(&layout), false, ==)
   FreeReader(p_reader);

   TEST_ASSERT(layout.libraries.size(),libraryCntEtalon,!=)
   TEST_ASSERT(layout.libraries[0]->elements.size(), elementCntEtalon, !=);
   TEST_ASSERT(layout.libraries[0]->layers.size(), layerCntEtalon, !=)

   const Element* pElem = layout.libraries[0]->elements[0];
   TEST_ASSERT(pElem->name, elementNameEtalon, !=);
   TEST_ASSERT(pElem->geometries.size(), geometriesCntEtalon, !=)

   for(size_t i = 0; i < layerCntEtalon; i++)
   {
      TEST_ASSERT(layout.libraries[0]->layers[i].layer, etalonLayers[i], !=);
   }
   END_TEST_JOB
}

TEST_MAIN()