/*
 * MskReader.cpp
 *
 * uWind (MicroWind) MSK file format reader by Mikhail S. Kotlyarov
 * 02.10.2021
 */
#include "Include/MskReader.hpp"

#include <algorithm>
#include <codecvt>
#include <locale>
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <exception>
#include <cstring>
#include <regex>

static 
std::unordered_map <std::string, LayerNum>
g_layerMap =
{
    {"TITLE",-6},
    {"BB",-5},
    {"VI",-4},
    {"NW",1},
    {"DN",16},
    {"DP",17},
    {"PO",13},
    {"CO",19},
    {"ME",23},
    {"M2",27},
    {"M3",34},
    {"M4",36},
    {"M5",53},
    {"M6",55},
};

constexpr
LayerNum
g_undefinedLayerNum = std::numeric_limits<LayerNum>::min();


bool
MskReader::IsMyFormat(
   const std::wstring& Fname)
{
   bool retVal = true;
   do 
   {
      std::regex fileNameRegex("\\w+\\.msk$",std::regex_constants::icase);
      if(!std::regex_search(std::string(Fname.begin(), Fname.end()),fileNameRegex)) { return false;}

      file.open(std::string(Fname.begin(),Fname.end()), std::ios::in);
      if (!file) { retVal = false; break; }
      fileName = Fname;

      std::string line;
      std::getline(file, line);
      if (line.length() < 7) { retVal = false; break; }

      if (line.substr(0, 7) != "VERSION") { retVal = false; break; }
   } while (false);
   
   if (file) { file.close(); }

   return retVal;
}


LayerNum
MskReader::LayerNameToLayerNum(
   const std::string& LayerName)
{
    auto it = g_layerMap.find(LayerName);
    if (it == g_layerMap.end()) { return g_undefinedLayerNum; }

    return it->second;
}


// URL: https://microeducate.tech/convert-wstring-to-string-encoded-in-utf-8/
std::string WcsStrToUtf8(const std::wstring& str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
  return myconv.to_bytes(str);
}


std::string 
MskReader::GetElemName()//elementName == fileName
{
    const std::wstring& tempWcs = fileName;
    const size_t lastCommaPos = tempWcs.find_last_of(L".");
    const size_t firstCommaPos = tempWcs.find_last_of(L"/");
    if (firstCommaPos == std::string::npos) { return WcsStrToUtf8(tempWcs.substr(0, lastCommaPos)); }
    else { return WcsStrToUtf8(tempWcs.substr(firstCommaPos + 1, lastCommaPos - firstCommaPos - 1)); }
}


bool 
MskReader::Read(
   LayoutData* layout)
{
   try {
      if (!layout) throw std::invalid_argument("Layout");

      file.open(std::string(fileName.begin(), fileName.end()));
      if (!file.is_open()) throw std::runtime_error("File was not opened");
      p_data = layout;

      p_activeLibrary = new Library;
      p_activeElement = new Element;

      p_data->fileName = this->fileName;
      p_activeElement->name = GetElemName();
      p_activeLibrary->name = WcsStrToUtf8(fileName);

      //Переменная для хранения одной строки из файла
      std::string fileLine;
      while (std::getline(file, fileLine))
      {
         fileLine.erase(std::remove(fileLine.begin(), fileLine.end(), '\r'), fileLine.end());
         fileLine.erase(std::remove(fileLine.begin(), fileLine.end(), '\n'), fileLine.end());
         
         if (fileLine.find("BB") != std::string::npos)  { ReadSectionBoundingBox(fileLine); }
         if (fileLine.find("REC") != std::string::npos) { ReadSectionRectangle(fileLine); }
         if (fileLine.find("TITLE") != std::string::npos) { ReadSectionTitle(fileLine); }
      }
      p_activeLibrary->elements.push_back(p_activeElement);
      p_data->libraries.push_back(p_activeLibrary);

      layout->fileName = fileName;
      layout->fileFormat = LayoutFileFormat::MSK;

      layout->libraries[0]->elements[0]->min = layout->libraries[0]->elements[0]->geometries[0]->min;
      layout->libraries[0]->elements[0]->max = layout->libraries[0]->elements[0]->geometries[0]->max;

      for (size_t i = 1; i < layout->libraries[0]->elements[0]->geometries.size(); ++i) {
         if (layout->libraries[0]->elements[0]->min.x > layout->libraries[0]->elements[0]->geometries[i]->min.x)
            layout->libraries[0]->elements[0]->min.x = layout->libraries[0]->elements[0]->geometries[i]->min.x;
         if (layout->libraries[0]->elements[0]->min.y > layout->libraries[0]->elements[0]->geometries[i]->min.y)
            layout->libraries[0]->elements[0]->min.y = layout->libraries[0]->elements[0]->geometries[i]->min.y;
         if (layout->libraries[0]->elements[0]->max.x < layout->libraries[0]->elements[0]->geometries[i]->max.x)
            layout->libraries[0]->elements[0]->max.x = layout->libraries[0]->elements[0]->geometries[i]->max.x;
         if (layout->libraries[0]->elements[0]->max.y < layout->libraries[0]->elements[0]->geometries[i]->max.y)
            layout->libraries[0]->elements[0]->max.y = layout->libraries[0]->elements[0]->geometries[i]->max.y;
      }
   }
   catch (const std::exception& ex)
   {

      if (file.is_open()) { file.close(); }
      if (layout)
      {
         layout->fileFormat = LayoutFileFormat::undefined;
         if (p_activeElement)
         {
            delete p_activeElement;
            p_activeElement = nullptr;
         }
         if (p_activeLibrary)
         {
            delete p_activeLibrary;
            p_activeLibrary = nullptr;
         }
         
      }
      return false;
   }
   if (file.is_open()) { file.close(); }
    return true; 
}


inline 
bool 
MskReader::ParseRecLine(
   const std::string& Line,
   Coord& LeftBot,
   Coord& RightTop,
   std::string& LayerName)
{
   char layerNameCstr[8] = { '\0' };
   int32_t width = 0, height = 0;
   if (!sscanf(Line.c_str(), "REC(%d,%d,%d,%d,%s)", &LeftBot.x, &LeftBot.y, &width, &height, layerNameCstr)) { return false; }

   LayerName = layerNameCstr;
   if(')' == LayerName[LayerName.length() - 1]) { LayerName.pop_back(); }
   //std::cout << layer_name << std::endl;
   RightTop.x = LeftBot.x + width;
   RightTop.y = LeftBot.y + height;
   return true;
}


void 
MskReader::FillBox(
   Geometry* Box2Fill,
   const Coord& LeftBot,
   const Coord& RightTop,
   LayerNum LNum)
{
    Coord currCoord;
    int32_t dx = CalcDelta(LeftBot.x, RightTop.x);
    int32_t dy = CalcDelta(LeftBot.y, RightTop.y);

    //Left top
    currCoord.x = RightTop.x - dx;
    currCoord.y = RightTop.y;
    Box2Fill->coords.push_back(currCoord);
    
    //Right top
    Box2Fill->coords.push_back(RightTop);

    //Right bot
    currCoord.x = RightTop.x;
    currCoord.y = RightTop.y - dy;
    Box2Fill->coords.push_back(currCoord);

    //Left bot
    currCoord.x = RightTop.x - dx;
    currCoord.y = RightTop.y - dy;
    Box2Fill->coords.push_back(currCoord);

    //Left top
    currCoord.x = RightTop.x - dx;
    currCoord.y = RightTop.y;
    Box2Fill->coords.push_back(currCoord);

    Box2Fill->layer = LNum;

    Box2Fill->min = Box2Fill->max = Box2Fill->coords[0];
    for (size_t i = 1; i < Box2Fill->coords.size(); ++i) {
      if (Box2Fill->min.x > Box2Fill->coords[i].x)
        Box2Fill->min.x = Box2Fill->coords[i].x;
      if (Box2Fill->min.y > Box2Fill->coords[i].y)
        Box2Fill->min.y = Box2Fill->coords[i].y;
      if (Box2Fill->max.x < Box2Fill->coords[i].x)
        Box2Fill->max.x = Box2Fill->coords[i].x;
      if (Box2Fill->max.y < Box2Fill->coords[i].y)
        Box2Fill->max.y = Box2Fill->coords[i].y;
    }
}


void
MskReader::ReadSectionRectangle(
   const std::string& FileLine)
{
   Geometry* currBox = new Rectangle;
   try {
      Coord leftBot;
      Coord rightTop;
      std::string layerName;
      if (!ParseRecLine(FileLine, leftBot, rightTop, layerName)) {throw std::runtime_error("Coordinates was not read");}

      const int16_t layerNum = LayerNameToLayerNum(layerName);
      if (layerNum == g_undefinedLayerNum) {throw std::runtime_error("File contains invalid layer!");}

      FillBox(currBox, leftBot, rightTop, layerNum);

      if (auto it = std::find_if(p_activeLibrary->layers.begin(), p_activeLibrary->layers.end(),[&](const Layer& val){ return layerNum == val.layer;}); p_activeLibrary->layers.end() == it )
      {
         Layer tmpLayer;
         tmpLayer.layer = currBox->layer;
         tmpLayer.name = layerName;
         tmpLayer.geometries.push_back(currBox);
         p_activeLibrary->layers.push_back(tmpLayer);
      }
      else
      {
         it->geometries.push_back(currBox);
      }

      p_activeElement->geometries.push_back(currBox);
   }
   catch (const std::exception& ex)
   {
      if (currBox)
      {
         delete currBox;
      }
   }
}


void 
MskReader::ReadSectionBoundingBox(
   const std::string& FileLine)
{
   Geometry* boundingBox = new Rectangle;
   try {
      Coord leftBot;
      Coord rightTop;
      if (!sscanf(FileLine.c_str(), "BB(%d,%d,%d,%d)", &leftBot.x, &leftBot.y, &rightTop.x, &rightTop.y)) {throw std::runtime_error("Coordinates was not read");}
      Layer boundingBoxLayer;
      int16_t layerNum = g_layerMap.find("BB")->second;
      boundingBoxLayer.layer = layerNum;
      boundingBoxLayer.name = "BB";
     
      FillBox(boundingBox, leftBot, rightTop, layerNum);

      boundingBoxLayer.geometries.push_back(boundingBox);
      p_activeLibrary->layers.push_back(boundingBoxLayer);
      p_activeElement->geometries.push_back(boundingBox);
   }
   catch (std::exception& ex)
   {
      if (boundingBox)
      {
         delete boundingBox;
      }
   }
}


void 
MskReader::ReadSectionTitle(
   const std::string& FileLine)
{
   Geometry* text = new Text;
   Text* p_text = static_cast<Text*>(text);
   try
   {
      char buf[64] = { '\0' };
      Coord leftBot;

      if (!sscanf(FileLine.c_str(), "TITLE %d %d  #%s", &leftBot.x, &leftBot.y, buf)) {throw std::runtime_error("Title was not read");}

      p_text->coords.push_back(leftBot);
      p_text->layer = g_layerMap.find("TITLE")->second;
      p_text->min = p_text->max =  leftBot;
      p_text->width = strlen(buf);
      p_text->stringValue = buf;
   }
   catch (std::exception& ex)
   {
      if (text)
      {
         delete text;
      }
   }
}


inline
int32_t
MskReader::CalcDelta(
   int32_t first,
   int32_t second)
{
   return second - first;
}