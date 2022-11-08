#include "Include/LayoutReader.hpp"

#include "Include/MskReader.hpp"

LayoutReader::LayoutReader() : p_data(nullptr) {}

LayoutReader *GetReader(const std::wstring &fName) {
  LayoutReader *p_reader = nullptr;


 
  p_reader = new MskReader;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  return p_reader;
}

void FreeReader(LayoutReader *reader) {
  if (!reader)
    return;
  delete reader;
  reader = nullptr;
}
