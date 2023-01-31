/**
 * @file Vector2D.h
 * @author Mikhail Kotlyarov (m.kotlyarov@elvis.ru)
 * @brief Implementation of a class for working with 2d matrices
 * @version 1
 * @date 2022-12-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __2D_VECTOR_H__
#define __2D_VECTOR_H__

#include <vector>
#include <memory>


/**
 * @class Vector2D
 * @brief Declaration of 2d matrix class
 */
template <typename Type>
class Vector2D final
{
private:
    std::vector<std::vector<Type>> data; ///< Matrix data

    /**
     * @class Proxy
     * @brief Proxy class for operator[] overload
     * 
     */
    class Proxy
    {
        private:
        std::vector<Type> constVec;///< Data for const operator[]
        std::vector<Type>& vec;///< Data ref for operator[] 
        
        public:
        Proxy(std::vector<Type>& Vec) :vec(Vec){ }
        //todo: Remove crutch
        Proxy(const std::vector<Type>& Vec) : constVec(Vec),vec(constVec) {}


        Type& operator[](size_t Index)  { return vec[Index];}
        const Type& operator[](size_t Index) const { return constVec[Index];}
    };

public:

    /**
     * @brief Construct a new Vector 2D object
     * 
     */
    Vector2D();

    /**
     * @brief Construct a new Vector 2 D object
     * 
     * @param RowCnt Row count
     * @param ColCnt Column count
     */
    Vector2D(
    size_t RowCnt,
    size_t ColCnt);

    /**
     * @brief Copy construct a new Vector 2 D object
     * 
     * @param Rhs Obj to copy
     */
    Vector2D(const Vector2D& Rhs);


    /**
     * @brief Move construct a new Vector 2 D object
     * 
     * @param Rhs Obj to move
     */
    Vector2D(Vector2D&& Rhs) noexcept;


    /**
     * @brief Overloaded copy operator=
     * 
     * @param Rhs Obj to copy
     * @return Vector2D<Type>& 
     */
    Vector2D<Type>&
    operator=(
        const Vector2D& Rhs);


    /**
     * @brief Overloaded move operator=
     * 
     * @param Rhs Obj to move
     * @return Vector2D<Type>& 
     */
    Vector2D<Type>&
    operator=(
        Vector2D&& Rhs) noexcept;


    /**
     * @brief Overload of operator[][]([] with Proxy element)
     * 
     * @param Index Row number
     * @return Proxy Proxy object for [][]
     */
    Proxy
    operator[](
        size_t Index) 
    { 
        return Proxy(data[Index]);
    }

     /**
     * @brief Overload of const operator[][]([] with Proxy element)
     * 
     * @param Index Row number
     * @return Proxy Proxy object for [][]
     */
    const
    Proxy
    operator[](
        size_t Index) const 
    {
        return Proxy(data[Index]);
    }

public:

    /**
     * @brief Resize of matrix. Can throw std::invalid_argument if params are invalid.
     * 
     * @param RowCnt New row count
     * @param ColCnt New column count
     */
    void
    Resize(
        size_t RowCnt,
        size_t ColCnt);

    /**
     * @brief As operator[][] but throws std::invalid_argument exception
     * 
     * @param Row Row num
     * @param Col Column num
     * @return Type& Ref to obj at data[Row][Col]
     */
    const Type&
    At(
        size_t Row,
        size_t Col) const;

      /**
     * @brief As operator[][] but throws std::invalid_argument exception
     * 
     * @param Row Row num
     * @param Col Column numа
     * @return Type& Ref to obj at data[Row][Col]
     */
    Type&
    At(
        size_t Row,
        size_t Col);

    /**
     * @brief Get row count
     * 
     * @return size_t  Row count
     */
    inline
    size_t
    RowCount() const;

    /**
     * @brief Get column count
     * 
     * @return size_t Column count
     */
    inline
    size_t
    ColCount() const;

    /**
     * @brief Check matrix validity
     * 
     * @return true 
     * @return false 
     */
    bool
    operator!() const noexcept;

    /**
     * @brief Reset matrix data
     * 
     */
    void
    Reset();
private:


    /**
     * @brief Check index validity
     * 
     * @param Index Index
     * @return true 
     * @return false 
     */
    inline
    bool
    IsIndexValid(
        size_t Index) const;

    /**
     * @brief Check 2dVector validity members
     * 
     * @return true 
     * @return false 
     */
    inline
    bool
    CheckParameters() const;
};

/*! Aliases */
template <typename Type>
using Vector2DPtr = std::shared_ptr<Vector2D<Type>>;

using Vector2DUint8 = Vector2D<uint8_t>;



#endif //!__2D_VECTOR_H__