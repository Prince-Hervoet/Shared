#pragma once

#define DEFAULT_FLAG 1024

namespace letMeSee
{

    template <typename T>
    class SeeVector
    {
    public:
        SeeVector();
        ~SeeVector();

        void PushBack(const T &value);
        void Copy(const SeeVector<T> &other);
        T &PopBack();
        T &operator[](int index);
        int Size();
        void Clear();
        bool isEmpty();

    private:
        int size;
        int capacity;
        T *data;

        void bigger();
        void resize(int capacity);
        bool isBaseType();
    };

    template <typename T>
    SeeVector<T>::SeeVector()
        : size(0), capacity(1)
    {
        data = new T[1];
    }

    template <typename T>
    SeeVector<T>::~SeeVector()
    {
        if (data)
        {
            delete[] data;
        }
    }

    template <typename T>
    void SeeVector<T>::PushBack(const T &value)
    {
        if (size == capacity)
            bigger();
        data[size] = value;
        size += 1;
    }

    template <typename T>
    void SeeVector<T>::Copy(const SeeVector<T> &other)
    {
    }

    template <typename T>
    T &SeeVector<T>::PopBack()
    {
        if (size == 0)
            throw "error: empty";
        size -= 1;
        return data[size + 1];
    }

    template <typename T>
    int SeeVector<T>::Size()
    {
        return size;
    }

    template <typename T>
    void SeeVector<T>::Clear()
    {
        size = 0;
    }

    template <typename T>
    T &SeeVector<T>::operator[](int index)
    {
        if (index < 0 || index >= size)
            throw "error: index";
        return data[index];
    }

    template <typename T>
    bool SeeVector<T>::isEmpty()
    {
        return size == 0;
    }

    template <typename T>
    void SeeVector<T>::resize(int capacity)
    {
        
    }

    template <typename T>
    void SeeVector<T>::bigger()
    {
        // 扩容
        int nSize = capacity;
        if (capacity < DEFAULT_FLAG)
        {
            nSize = (nSize << 1);
        }
        else
        {
            nSize = (int)(nSize * 1.5);
        }

        T *temp = new T[nSize];
        for (int i = 0; i < nSize; i++)
        {
            temp[i] = data[i];
        }
        delete[] data;
        data = temp;
    }

}