#include <stdexcept>

#include "Include/Vector2D.h"


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
}

template <typename T>
Vector2D<T>&
Vector2D<T>::operator=(
    Vector2D<T>&& Rhs)
{
    data = std::move(Rhs.data);
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
    size_t Index)
{
    if(0 == Index || 5000 < Index) { return false;}
    return true;
}


/*template <typename T>
inline
bool
Vector2D<T>::CheckParameters()
{
    
}*/