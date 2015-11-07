/*
 * Copyright Institute for Theoretical Physics, ETH Zurich 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef BIND_CONTAINER_PROXY_ITERATOR
#define BIND_CONTAINER_PROXY_ITERATOR

namespace bind {

    template<class Container>
    class proxy_iterator {
    public:
        typedef Container container_type;
        typedef typename std::iterator_traits<bind::proxy_iterator<Container> >::value_type value_type;

        proxy_iterator() : container(NULL), position(0) {}
        proxy_iterator(container_type& owner, size_t p) : container(&owner), position(p) {}
        proxy_iterator& operator += (size_t offset){
            position += offset;
            return *this;
        }
        proxy_iterator& operator++ (){
            position++;
            return *this;
        }
        proxy_iterator operator++ (int){
            proxy_iterator tmp(*this);
            operator++();
            return tmp;
        }
        proxy_iterator& operator -= (size_t offset){
            position -= offset;
            return *this;
        }
        proxy_iterator& operator-- (){
            position--;
            return *this;
        }
        proxy_iterator operator-- (int){
            proxy_iterator tmp(*this);
            operator--();
            return tmp;
        }
        value_type& operator* () const {
            return (*container)[position];
        }
        container_type& get_container(){
            return *container;
        }
        const container_type& get_container() const {
            return *container;
        }
        size_t position;
    public:
        template<typename T>
        friend bool operator == (const proxy_iterator<T>& lhs, const proxy_iterator<T>& rhs);
        template<typename T>
        friend bool operator != (const proxy_iterator<T>& lhs, const proxy_iterator<T>& rhs);
        container_type* container;
    };

    template <class Container> 
    bool operator == (const proxy_iterator<Container>& lhs, const proxy_iterator<Container>& rhs){
        return (lhs.position == rhs.position && lhs.container->allocator_.desc == rhs.container->allocator_.desc);
    }

    template <class Container> 
    bool operator != (const proxy_iterator<Container>& lhs, const proxy_iterator<Container>& rhs){
        return (lhs.position != rhs.position || lhs.container->allocator_.desc != rhs.container->allocator_.desc);
    }

    template <class Container, class OtherContainer> 
    size_t operator - (const proxy_iterator<Container>& lhs, const proxy_iterator<OtherContainer>& rhs){ 
        return lhs.position - rhs.position;
    }

    template <class Container> 
    proxy_iterator<Container> operator + (proxy_iterator<Container> lhs, size_t offset){ 
        return (lhs += offset);
    }

    template <class Container> 
    proxy_iterator<Container> operator - (proxy_iterator<Container> lhs, size_t offset){ 
        return (lhs -= offset);
    }

    template <class Container> 
    bool operator < (const proxy_iterator<Container>& lhs, const proxy_iterator<Container>& rhs){
        return (lhs.position < rhs.position);
    }

    template <class Container> 
    bool operator > (const proxy_iterator<Container>& lhs, const proxy_iterator<Container>& rhs){
        return (lhs.position > rhs.position);
    }

}

namespace std {

    template<class Container>
    class iterator_traits<bind::proxy_iterator<Container> > {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef typename Container::value_type value_type;
        typedef size_t difference_type;
    };

    template<class Container>
    class iterator_traits<bind::proxy_iterator<const Container> > {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef const typename Container::value_type value_type;
        typedef size_t difference_type;
    };
}

#endif
