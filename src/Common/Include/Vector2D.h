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
        std::vector<Type>& vec;

        public:
        Proxy(std::vector<Type>& Vec) :vec(Vec){ }

        Type& operator[](size_t Index) { return vec[Index];}
    };

public:

    Vector2D();
    Vector2D(
    size_t RowCnt,
    size_t ColCnt);
    Vector2D(const Vector2D& Rhs);
    Vector2D(Vector2D&& Rhs) noexcept;
    
    Proxy operator[](size_t Index) { return Proxy(data[Index]); }

public:

    void Resize(size_t RowCnt, size_t ColCnt);
    Type& At(size_t Row, size_t Col);
private:
    inline
    bool
    IsIndexValid(
        size_t Index);

    inline
    bool
    CheckParameters();

};

template <typename Type>
using Vector2DPtr = std::shared_ptr<Vector2D<Type>>;



#endif //!__2D_VECTOR_H__