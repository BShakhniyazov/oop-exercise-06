#pragma once

#include <memory>
#include <list>

template<class T, size_t N = 100> //type of values and size
class Allocator
{
private:
    using store_t = std::list<std::pair<char*,char*>>; //pointers to begin and end of chunk
    std::shared_ptr<store_t> freeLinks; //list of free chunks
    std::shared_ptr<char[]> pool; //array of pointers
    
public:
    using value_type = T;
    using pointer = T * ;
    using const_pointer = const T*;
    using size_type = std::size_t;

    template<typename U>
    struct rebind {
        using other = Allocator<U, N>;
    };

    //getters
    std::shared_ptr<char[]> GetPool() const { return pool; }
    std::shared_ptr<store_t> GetList() const { return freeLinks; }

    Allocator() noexcept //default constructor
    {
        pool = std::shared_ptr<char[]>(new char[N], [](char *p){ delete[] p; }); //allocate memory

        static_assert(N > 0);
        freeLinks = std::make_shared<store_t>(store_t());

        store_t& l = *freeLinks.get(); //fill list
        l.push_back(std::pair(pool.get(), pool.get() + N)); //one huge chunk
    }
    template<class U>
    Allocator(const Allocator<U,N>& a) noexcept
    {
        pool = a.GetPool();
        freeLinks = a.GetList();
    }

    ~Allocator() {}

    T* allocate(size_t n) //allocate chunk 
    {
        n *= sizeof(T); //convert number of blocks to number of bytes

        store_t& l = *freeLinks.get();
        for (auto i = l.begin(); i != l.end(); ++i) //find chunk
        {
            auto &chunk = *i;
            if (chunk.second - chunk.first < n) continue; //not big enough
            //chunk is big enough, crop

            size_t diff = chunk.second - chunk.first - n;
            if (diff > 0) //not whole chunk cropped
            {
                char* ptr = chunk.first + n;
                l.insert(i, std::pair(ptr, ptr + diff));
            }
            l.erase(i); //remove old chunk from list
            return (T*)chunk.first;
        }
        //if we reached here, we haven't found chunk
        throw std::bad_alloc();
    }
    void deallocate(T* ptr1, size_t n) //return chunk to the list
    {
        n *= sizeof(T); //convert number of blocks to number of bytes
        char *ptr = (char*)ptr1;

        store_t& l = *freeLinks.get();
        auto i = l.begin();

        for (; i != l.end(); ++i) //find place for chunk (list contains them sorted by pointer)
        {
            if ((*i).first > ptr) break; //found first block with ptr larger than my
        }
        //insert our chunk before him
        l.insert(i, std::pair(ptr, ptr + n));
        //merge front
        if (i != l.end())
        {
            auto &chunk = *i; //next chunk
            --i; //iterator to us
            if (ptr + n == chunk.first) //can merge
            {
                //extend us and remove chunk
                (*i).second += chunk.second - chunk.first;
                n += chunk.second - chunk.first;
                auto j = i; ++j; //iterator to next chunk
                l.erase(j); //remove it
            }
        }
        else --i;

        //merge back
        if (i != l.begin())
        {
            auto j = i; --j; //iterator to prev chunk
            auto &chunk = *j; //prev chunk and our chunk
            if (chunk.second == ptr) //can merge
            {
                //extend chunk and remove our
                chunk.second += n;
                l.erase(i);
            }
        }
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args &&...args) //call constructor
    {
        new (p) U(std::forward<Args>(args)...);
    }
    void destroy(pointer p) //call destructor
    {
        p->~T();
    }
};
template<class T, size_t N1, class U, size_t N2>
bool operator==(Allocator<T,N1>&, Allocator<U,N2>&) noexcept { return true; }

template<class T, size_t N1, class U, size_t N2>
bool operator!=(Allocator<T,N1>&, Allocator<U,N2>&) noexcept { return false; }
