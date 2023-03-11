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
            std::generate(begin(), end(), [](){return Type();});
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
            std::fill(begin(), end(), value);
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
        if(size_) {return false;}
        else{return true;}
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
        }else{
            return array_[index];
        }
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
        if (new_size > size_) {
            SimpleVector<Type> array_new (new_size);
            std::generate(array_new.begin(),array_new.end(),[] { return Type(); });
            std::move(begin(), end(), array_new.begin());
            array_.swap(array_new.array_);
            size_ = move(new_size);
            capacity_ = move(new_size);
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
    
    SimpleVector(const SimpleVector& other) {
        ArrayPtr<Type> array{ other.capacity_};
        std::copy(other.begin(), other.end(), Iterator{array.Get()});
        array_.swap(array);
        size_= other.size_;
        capacity_= other.capacity_;
    }

    SimpleVector(SimpleVector&& other) {
        swap(other);
    }

    void Reserve(size_t new_capacity){
        ArrayPtr<Type> array{ new_capacity};
        std::copy(begin(),end(), Iterator{array.Get()});
        array_.swap(array);
        if(capacity_<new_capacity)
            capacity_= new_capacity;
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
            SimpleVector <Type> array_new(rhs);
            swap(rhs);
        }
        return *this;
    }

    void PushBack(const Type& item) {
        if(!capacity_){
            SimpleVector<Type> new_arr {item};
            swap(new_arr);
        return ;
        }
        
        if(size_ < capacity_){
            array_[size_]=item;
            size_++;
        }else{
            SimpleVector<Type> array_new (2*size_);
            std::copy(begin(), end(), array_new.begin());
            array_.swap(array_new.array_);
            capacity_ = array_new.capacity_;
            array_[size_]=item;
            size_++;
        }
    }

    void PushBack(Type&& item) {
        if(size_ < capacity_){
            array_[size_] = std::move(item);
            size_++;
        }else{
            if(!capacity_)
                capacity_ = 1;
            else
                capacity_ = capacity_*2;
            SimpleVector<Type> array_new (capacity_);
            std::move(begin(), end(), array_new.begin());
            array_.swap(array_new.array_);
            array_[size_]=std::move(item);
            size_++;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        if(!capacity_){
            SimpleVector<Type> new_arr {value};
            swap(new_arr);
            return begin();
        }
        auto dist_ = std::distance(cbegin(), pos);
        if(size_ == capacity_){
            SimpleVector<Type> array_new (2*size_);
            std::copy_backward(pos,cend(),Iterator{array_new.begin()+size_+1});
            array_new[dist_]=value;
            std::copy(cbegin(),pos,array_new.begin());
            array_new.size_=size_;
            swap(array_new);
            size_++;
        }else{
            size_++;
            std::copy_backward(pos,ConstIterator{end()-1},end());
            array_[dist_]=value;
        }
        return Iterator{begin()+dist_}; // Напишите тело самостоятельно
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
        if(!capacity_){
            SimpleVector<Type> array_new(1);
            swap(array_new);
            array_[0]=std::move(value);
            return begin();
        }
        auto dist_ = std::distance(cbegin(), pos);
        if(size_ == capacity_){
            SimpleVector<Type> array_new (2*size_);
            std::move_backward((Iterator)pos, end(),Iterator{array_new.begin()+size_+1});
            array_new[dist_]=std::move(value);
            std::move(begin(),(Iterator)pos,array_new.begin());
            array_new.size_=std::move(size_);
            swap(array_new);
            size_++;
        }else{
            size_++;
            std::move_backward((Iterator)pos,Iterator{end()-1},end());
            array_[dist_]=std::move(value);
        }
        return Iterator{begin()+dist_}; // Напишите тело самостоятельно
    }

    void PopBack() noexcept {
        if(IsEmpty()){
            return;
        }
        size_--;
    }

    Iterator Erase(ConstIterator pos) {
        if(IsEmpty()){
            return{};
        }
        auto dist_ = std::distance(cbegin(), pos);
        std::move((Iterator)pos+1,end(),(Iterator)pos);
        size_--;
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
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
} 