// вставьте сюда ваш код для класса SimpleVector
// внесите необходимые изменения для поддержки move-семантики
#pragma once
#include <algorithm>
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <iostream>

 class ReserveProxyObj{
    public:
    explicit ReserveProxyObj(size_t capacity):capacity_(capacity){}
        size_t capacity_;
    };

    ReserveProxyObj Reserve(size_t capacity_to_reserve) {
        return ReserveProxyObj(capacity_to_reserve);
    }


template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
    SimpleVector() noexcept = default;
    
   
    SimpleVector(ReserveProxyObj cap):
        array_(cap.capacity_),
        size_(0),
        capacity_(cap.capacity_)
        {}



    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) :
        array_(size),
        size_(size),
        capacity_(size) {
            std::generate(begin(), end(), []{return Type();});
        }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) :
        array_(size),
        size_ (size),
        capacity_ (size){
            std::fill(begin(), end(), value);
        }

    SimpleVector(size_t size, const Type&& value) :
        array_(size),
        size_ (size),
        capacity_ (size){
            std::move(begin(), end(), value);
        }

    SimpleVector(std::initializer_list<Type> init):
        array_(init.size()),
        size_(init.size()),
        capacity_(init.size())
        {
            std::copy(init.begin(), init.end(), begin()); // Напишите тело конструктора самостоятельно
        }


    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_==0;
    }

    Type& operator[](size_t index) noexcept {
        return array_[index];
    }
    
    const Type& operator[](size_t index) const noexcept {
        return array_[index];
    }

    Type& At(size_t index) {
        if(index >= size_){
            throw std::out_of_range("invalid index, out of range");
        }
        return array_[index];
    }

    const Type& At(size_t index) const {
        if(index >= size_){
            throw std::out_of_range("invalid index, out of range");
        }else{
            return array_[index];
        }
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            Reserve(new_size);
            std::generate(end(), array_.Get() + new_size, [] { return Type(); });
            size_ = move(new_size);
        } else  if (new_size > size_){
            std::generate(end(), array_.Get() + new_size, [] { return Type(); });
        } else {
            size_ = move(new_size);
        }
        
     }

    Iterator begin() noexcept {
        return Iterator{array_.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return Iterator{array_.Get()+size_};
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return ConstIterator{array_.Get()};
    }

    ConstIterator end() const noexcept {
        return ConstIterator{array_.Get()+size_};
    }

    ConstIterator cbegin() const noexcept {
       return ConstIterator{array_.Get()};
    }

    ConstIterator cend() const noexcept {
        return ConstIterator{array_.Get()+size_};
    }
    
    SimpleVector(const SimpleVector& other): 
        array_(other.capacity_),
        size_(other.size_),
        capacity_(other.capacity_)
        { 
        std::copy(other.begin(), other.end(), Iterator{array_.Get()}); }

    SimpleVector(SimpleVector&& other) {
        swap(other);
    }

    void Reserve(size_t new_capacity){
        //cerr<<new_capacity<<' '<<size_<<endl;
        if(capacity_<new_capacity){
        ArrayPtr<Type> array(new_capacity);
        std::move(begin(),end(), Iterator{array.Get()});
        array_.swap(array);
        capacity_= move(new_capacity);
        }
        //cerr<<capacity_<<' '<<size_<<endl;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if(this != &rhs){
            SimpleVector <Type> array_new(rhs);
            swap(array_new);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if(this != &rhs){
            swap(rhs);
        }
        return *this;
    }

    void PushBack(const Type& item) {
        if(size_ >= capacity_) {
            capacity_ = std::max(size_t(1), capacity_*2);
            Reserve(2*capacity_);}
        array_[size_]=item;
        size_++;
    }

    void PushBack(Type&& item) {
        if(size_ >= capacity_){
            capacity_ = std::max(size_t(1), capacity_*2);
            Reserve(2*capacity_);}
        array_[size_]=std::move(item);
        size_++;
    }

   Iterator create_insert_pos(ConstIterator pos){
        auto dist_ = std::distance(cbegin(), pos);
        assert(pos>=cbegin()&&pos<=cend());
        if(size_ == capacity_){
            capacity_ = std::max(size_t(1), capacity_*2);
            ArrayPtr<Type> array(capacity_);
            std::move_backward(Iterator(begin()+dist_), end(),Iterator{array.Get()+size_+1});
            std::move(begin(), Iterator(begin()+dist_), array.Get());
            array_.swap(array);
            size_++;
        }else{
            size_++;
            std::move_backward(Iterator(pos),Iterator{end()},end());
        }
        return Iterator(begin()+dist_);
    }


    Iterator Insert(ConstIterator pos, const Type& value) {
            auto newpos = create_insert_pos(pos);
            *newpos = value;
            return newpos;
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
            auto newpos = create_insert_pos(pos);
            *newpos = std::move(value);
            return newpos;
    }

    void PopBack() noexcept {
        assert(!IsEmpty());
        size_--;
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos>=cbegin()&&pos<cend());
        auto dist_ = std::distance(cbegin(), pos);
        std::move(Iterator(pos+1),end(),Iterator(pos));
        size_--;
        //std::cerr<<<begin()<endl;
        return Iterator{begin()+dist_};
    }

    void swap(SimpleVector& other) noexcept {
        array_.swap(other.array_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    
 private:
    ArrayPtr<Type> array_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize()==rhs.GetSize()&&std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs==rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs>rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs<rhs);
} 