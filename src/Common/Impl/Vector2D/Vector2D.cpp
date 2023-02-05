#include <stdexcept>

#include "Include/Vector2D.h"
#include "Include/LayoutFragment.h"


template <typename T>
Vector2D<T>::Vector2D() {}

template <typename T>
Vector2D<T>::Vector2D(
    size_t RowCnt,
    size_t ColCnt)
{
   if(!IsIndexValid(RowCnt) || !IsIndexValid(ColCnt)) { return; }

   Resize(RowCnt,ColCnt);
}


template <typename T>
Vector2D<T>::Vector2D(
    const Vector2D& Rhs)
{
    data = Rhs.data;
}


template <typename T>
Vector2D<T>::Vector2D(
    Vector2D&& Rhs) noexcept
{
    data = std::move(Rhs.data);
}

template <typename T>
Vector2D<T>&
Vector2D<T>::operator=(
    const Vector2D<T>& Rhs)
{
     data = Rhs.data;
     return *this;
}

template <typename T>
Vector2D<T>&
Vector2D<T>::operator=(
    Vector2D<T>&& Rhs) noexcept
{
    data = std::move(Rhs.data);
    return *this;
}


template <typename T>
void
Vector2D<T>::Resize(
    size_t RowCnt,
    size_t ColCnt)
{
    data.resize(RowCnt);
    for(size_t i = 0; i < RowCnt; ++i)
    {
        data[i].resize(ColCnt);
    }
}


template <typename T>
const T&
Vector2D<T>::At(
    size_t Row,
    size_t Col) const
{
    if(data.size() <= Row) { throw std::invalid_argument("Invalid Row number"); }
    if(data[Row].size() <= Col) {throw std::invalid_argument("Invalid Col number"); }

    return data[Row][Col];
}

template <typename T>
T&
Vector2D<T>::At(
    size_t Row,
    size_t Col)
{
    if(data.size() <= Row) { throw std::invalid_argument("Invalid Row number"); }
    if(data[Row].size() <= Col) {throw std::invalid_argument("Invalid Col number"); }

    return data[Row][Col];
}


template <typename T>
inline
bool
Vector2D<T>::IsIndexValid(
    size_t Index) const
{
    if(0 == Index || 5000 < Index) { return false;}
    return true;
}


template <typename T>
inline
size_t
Vector2D<T>::RowCount() const
{ 
    return data.size();
}


template <typename T>
inline
size_t
Vector2D<T>::ColCount() const 
{
    //if(!CheckParameters()) { return 0;}
    return data[0].size();
}


template <typename T>
bool
Vector2D<T>::operator!() const noexcept
{ 
    return !CheckParameters(); 
}


template <typename T>
void
Vector2D<T>::Reset() 
{
    if(data.empty()) { return;}
    data.clear();
}


template <typename T>
inline
bool
Vector2D<T>::CheckParameters() const
{ 
    if(data.size() == 0) { return false;}

    const size_t etalonSz = data[0].size();
    for(size_t i = 0; i < data.size(); ++i)
    {
        if(0 == data[i].size() || etalonSz != data[i].size()) { return false;}
    }
    return true;
}

template class Vector2D<uint8_t>;
template class Vector2D<Fragment>;