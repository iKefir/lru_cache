//
//  lru.hpp
//  LRU-Cache
//
//  Created by Данил Шкарупин on 28.09.16.
//  Copyright © 2016 Данил Шкарупин. All rights reserved.
//

#ifndef lru_hpp
#define lru_hpp

#include <cstdio>
#include <cassert>
#include <iostream>

struct lru_cache
{
    // Вы можете определить эти тайпдефы по вашему усмотрению.
    typedef int key_type;
    typedef int mapped_type;
    typedef std::pair<key_type, mapped_type> value_type;
    
    // Bidirectional iterator
    struct iterator;
    struct node;
    
    // Создает пустой lru_cache с указанной capacity.
    lru_cache(size_t capacity);
    
    // Деструктор. Вызывается при удалении объектов lru_cache.
    // Инвалидирует все итераторы ссылающиеся на элементы этого lru_cache
    // (включая итераторы ссылающиеся на элементы следующие за последними).
    ~lru_cache();
    
    lru_cache(lru_cache &) = delete;
    lru_cache& operator=(lru_cache const&) = delete;
    // Поиск элемента.
    // Возвращает итератор на элемент найденный элемент, либо end().
    // Если элемент найден, он помечается как наиболее поздно использованный.
    iterator find(key_type);
    
    // Вставка элемента.
    // 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
    //    на уже присутствующий элемент и false.
    // 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
    //    элемент и true.
    // Если после вставки число элементов кеша превышает capacity, самый давно не
    // использованный элемент удаляется. Все итераторы на него инвалидируется.
    // Вставленный либо найденный с помощью этой функции элемент помечается как наиболее поздно
    // использованный.
    std::pair<iterator, bool> insert(value_type);
    
    // Удаление элемента.
    // Все итераторы на указанный элемент инвалидируются.
    void erase(iterator);
    
    // Возващает итератор на элемент с минимальный ключом.
    iterator begin() const;
    // Возващает итератор на элемент следующий за элементом с максимальным ключом.
    iterator end() const;
    
private:
    size_t const capacity;
    size_t size;
    
    node* root;
    
    node* queBegin;
    
    node* dofind(key_type);
    void removerefers(node*);
};

struct lru_cache::node
{
    key_type     key;
    mapped_type mapped;
    
    node*        left;
    node*        right;
    node*        parent;
    
    node*        next;
    node*        prev;
};

struct lru_cache::iterator
{
    friend struct lru_cache;
    
    iterator();
    iterator(node*);
    
    mapped_type const& operator*() const;
    
    iterator& operator++();
    iterator operator++(int);
    
    iterator& operator--();
    iterator operator--(int);
    
    bool operator==(const iterator) const;
    bool operator!=(const iterator) const;
    
private:
    node* current;
};

#endif /* lru_hpp */
