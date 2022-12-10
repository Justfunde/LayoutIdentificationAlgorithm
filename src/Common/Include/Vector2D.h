#ifndef __2D_VECTOR_H__
#define __2D_VECTOR_H__

#include <vector>
#include <memory>

template <typename Type>
class Vector2D final
{
private:
    std::vector<std::vector<Type>> data;

    class Proxy
    {
        private:
        std::vector<Type> constVec;
        std::vector<Type>& vec;
        

        public:
        Proxy(std::vector<Type>& Vec) :vec(Vec){ }
        //todo: Убрать костыль
        Proxy(const std::vector<Type>& Vec) : constVec(Vec),vec(constVec) {}


        Type& operator[](size_t Index)  { return vec[Index];}
        const Type& operator[](size_t Index) const { return constVec[Index];}
    };

public:

    Vector2D();


    Vector2D(
    size_t RowCnt,
    size_t ColCnt);

    Vector2D(const Vector2D& Rhs);


    Vector2D(Vector2D&& Rhs) noexcept;


    Vector2D<Type>& operator=(const Vector2D& Rhs);

    Vector2D<Type>& operator=(Vector2D&& Rhs);

    Proxy operator[](size_t Index) { return Proxy(data[Index]); }
    const Proxy operator[](size_t Index) const { return Proxy(data[Index]); }

public:

    void Resize(size_t RowCnt, size_t ColCnt);
    Type& At(size_t Row, size_t Col);

    inline size_t RowSize() const { return data.size();}

    inline size_t ColCnt() const 
    {
        //if(!CheckParameters()) { return 0;}
        return data[0].size();
    }

    bool operator!() noexcept { return !CheckParameters(); }

    void Reset() { data.clear();}
private:
    inline
    bool
    IsIndexValid(
        size_t Index);

    inline
    bool
    CheckParameters() 
    { 
        if(data.size() == 0) { return false;}

    const size_t etalonSz = data[0].size();
    for(size_t i = 0; i < data.size(); ++i)
    {
        if(0 == data[i].size() || etalonSz != data[i].size()) { return false;}
    }
    return true;}


};

template <typename Type>
using Vector2DPtr = std::shared_ptr<Vector2D<Type>>;

using Vector2DUint8 = Vector2D<uint8_t>;



#endif //!__2D_VECTOR_H__