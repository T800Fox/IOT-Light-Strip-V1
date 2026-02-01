// RGB Hallway Light Strip Controller: Version 1.0
// File: MotionDetection.cpp
// Author: MrT (https://stackoverflow.com/users/10888308/mrt)
// Adapted: Jeremy Fox
//
// Neat little implementation of a linked list, felt cleaner than porting over std::cpp
// https://stackoverflow.com/questions/9986591/vectors-in-arduino

#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

template <class T>
class ListNode {
  public:
    T element;
    ListNode* next;
    ListNode* prev;

    ListNode(T element, ListNode* prev, ListNode* next) : element(element)
    {
      this->next = next;
      this->prev = prev;
    };
};

template <class T>
class LinkedList  {
  private:
    int length;
    ListNode<T>* head;
    ListNode<T>* tail;
    ListNode<T>* curr;
  public:
    LinkedList();
    LinkedList(const LinkedList<T>&);
    ~LinkedList();

    T& getCurrent();
    T& First() const;
    T& Last() const;

    int getLength();

    void Append(T);
    void DeleteLast();
    void DeleteFirst();
    void DeleteCurrent();

    bool next();
    bool moveToStart();
    bool prev();
    void Delete(T&);
    bool Search(T);
    void Clear();
    void PutFirstToLast();
    void Update(T elem);

    LinkedList& operator = (const LinkedList<T>&);
};

#endif