//
//  lru.cpp
//  LRU-Cache
//
//  Created by Данил Шкарупин on 28.09.16.
//  Copyright © 2016 Данил Шкарупин. All rights reserved.
//

#include "lru.hpp"


// Создает пустой lru_cache с указанной capacity.
lru_cache::lru_cache(size_t cap) : capacity(cap) {
    assert(cap > 0);
    root = new node();
    queBegin = root;
    size = 0;
}

// Деструктор. Вызывается при удалении объектов lru_cache.
// Инвалидирует все итераторы ссылающиеся на элементы этого lru_cache
// (включая итераторы ссылающиеся на элементы следующие за последними).
lru_cache::~lru_cache() {
    node * a = root, *b = a;
    while (a -> prev != nullptr) {
        a = a -> prev;
        delete b;
        b = a;
    }
    //invalidate iterators
}

// Поиск элемента.
// Возвращает итератор на найденный элемент, либо end().
// Если элемент найден, он помечается как наиболее поздно использованный.
lru_cache::iterator lru_cache::find(lru_cache::key_type tofind) {
    node *a = dofind(tofind);
    return iterator(((a -> key == tofind && a != root) ? a : root));
}

lru_cache::node* lru_cache::dofind(lru_cache::key_type tofind) {
    if (root -> left == nullptr) return root;
    node* a = root -> left;
    
    while (true) {
        
        if (a -> key == tofind) break;
        
        if (a -> key > tofind) {
            if (a -> left == nullptr) {
                return a;
            } else {
                a = a -> left;
            }
        }
        if (a -> key < tofind) {
            if (a -> right == nullptr) {
                return a;
            } else {
                a = a -> right;
            }
        }
        
    }
    
    a -> next -> prev = a -> prev;
    if (a -> prev != nullptr) a -> prev -> next = a -> next;
    a -> prev = nullptr;
    a -> next = queBegin;
    a -> next -> prev = a;
    
    return a;
}

// Вставка элемента.
// 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
//    на уже присутствующий элемент и false.
// 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
//    элемент и true.
// Если после вставки число элементов кеша превышает capacity, самый давно не
// использованный элемент удаляется. Все итераторы на него инвалидируется.
// Вставленный либо найденный с помощью этой функции элемент помечается как наиболее поздно
// использованный.
std::pair<lru_cache::iterator, bool> lru_cache::insert(lru_cache::value_type tofind) {
    node *a = dofind(tofind.first);
    
    if (a -> key == tofind.first && a != root) return std::pair<lru_cache::iterator, bool> (iterator(a), false);
    
    //if filled -- use old
    node *ins;
    if (size < capacity) {
        ins = new node();
        ++size;
    } else {
        ins = root -> prev;
        if (ins -> prev != nullptr) ins -> prev -> next = ins -> next;
        if (ins -> next != nullptr) ins -> next -> prev = ins -> prev;
        ins -> prev = nullptr;
    }
    ins -> key = tofind.first;
    ins -> mapped = &tofind.second;
    
    ins -> parent = a;
    
    ins -> next = queBegin;
    queBegin -> prev = ins;
    queBegin = ins;
    if (a -> key > tofind.first || a == root) {
        a -> left = ins;
    }
    else if (a -> key < tofind.first) {
        a -> right = ins;
    }
    
    return std::pair<lru_cache::iterator, bool> (iterator(ins), true);
}

// Удаление элемента.
// Все итераторы на указанный элемент инвалидируются.
void lru_cache::erase(lru_cache::iterator it) {
    if (it.current -> parent != nullptr) {
        bool parent;
        if (it.current -> parent -> left == it.current) parent = 0;
        else parent = 1;
        //Если нет детей -- всё хорошо
        if (it.current -> left == nullptr && it.current -> right == nullptr) {
            (parent == false ? it.current -> parent -> left: it.current -> parent -> right) = nullptr;
        }
        //Если два ребенка -- подвесить левого вместо current правого к самому большому ребенку левого
        else if (it.current -> left != nullptr && it.current -> right != nullptr) {
            (parent == false ? it.current -> parent -> left : it.current -> parent -> right) = it.current -> left;
            it.current -> left -> parent = it.current -> parent;
            
            node* trying = it.current -> left;
            while (trying -> right != nullptr) trying = trying -> right;
            
            trying -> right = it.current -> right;
            it.current -> right -> parent = trying;
        }
        //Если один ребенок -- подвесить вместо current
        else {
            (parent == false ? it.current -> parent -> left : it.current -> parent -> right) = (it.current -> left != nullptr ? it.current -> left : it.current -> right);
            (it.current -> left != nullptr ? it.current -> left : it.current -> right) -> parent = it.current -> parent;
        }
        
        if (it.current -> prev != nullptr) it.current -> prev -> next = it.current -> next;
        if (it.current -> next != nullptr) it.current -> next -> prev = it.current -> prev;
        
        delete it.current;
    }
}

// Возващает итератор на элемент с минимальный ключом.
lru_cache::iterator lru_cache::begin() const {
    node * a = root;
    while (a -> left != nullptr) a = a -> left;
    return iterator(a);
}
// Возващает итератор на элемент следующий за элементом с максимальным ключом.
lru_cache::iterator lru_cache::end() const {
    return iterator(root);
}

lru_cache::value_type const& lru_cache::iterator::operator*() const {
    return value_type(current -> key, *(current -> mapped));
}

lru_cache::iterator& lru_cache::iterator::operator++() {
    if (current -> right != nullptr) {
        current = current -> right;
        while (current -> left != nullptr) current = current -> left;
    } else {
        while ((current -> parent != nullptr) && (current -> parent -> right == current)) current = current -> parent;
        if (current -> parent != nullptr) current = current -> parent;
    }
    return *this;
}
lru_cache::iterator lru_cache::iterator::operator++(int) {
    iterator res(*this);
    ++(*this);
    return res;
}

lru_cache::iterator& lru_cache::iterator::operator--() {
    if (current -> left != nullptr) {
        current = current -> left;
        while (current -> right != nullptr) current = current -> right;
    } else {
        while ((current -> parent != nullptr) && (current -> parent -> left == current)) current = current -> parent;
        if (current -> parent != nullptr) current = current -> parent;
    }
    return *this;
}
lru_cache::iterator lru_cache::iterator::operator--(int) {
    iterator res(*this);
    --(*this);
    return res;
}
bool lru_cache::iterator::operator==(const iterator right) const {
    return current == right.current;
}
bool lru_cache::iterator::operator!=(const iterator right) const {
    return current != right.current;
}
