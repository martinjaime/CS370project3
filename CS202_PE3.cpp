// Martin Jaime CS-202 Exersice #3
// Input: a file through file redirection that contains two lists of numbers seperated by the special
// number 99999.
// Processing: reads first array of numbers into a doubly linked list, and orders them into ascending order.
// reads the second list after 99999 into a second doubly linked list, and orders them into ascending order.
// Then, the lists are merged, and checks if there are any distinct numbers in the merged list. Checks how 
// many times an integer is repeated. Then, it splits the merged into two.
// Output: lists the two inputted lists with appropriate labels, prints the merged list, tells if the items
// in the merged list are distinct and how many times each integer is repeated, and finally, it prints the 
// two lists that are formed by splitting the merged list.

#include<iostream>

using namespace std;

struct nodeType
{
  int info;
  nodeType *next = NULL;
  nodeType *back = NULL;
};

class orderedDLL    // Ordered Doubly Linked List
{
public: 
  bool isEmpty()
  // returns true if the list is empty, otherwise, it returns false.
  {
    if (first == NULL) return true; 
    else 
        return false;
  }
  
  void insertNode(int x)
  // given an integer x, insertNode will insert x into the appropriate place 
  // in the list to keep it sorted in ascending order. 
  {
    nodeType *current, *newNode, *trail;
    bool found = false;
    newNode = new nodeType;
    newNode->info = x;
    newNode->next = NULL; newNode->back = NULL;
    current = first;
    if (first == NULL)
      {
        first = new nodeType;
        first->info = x; 
      }
    else
      {
        while(current != NULL && !found)
          if (current->info >= x)
            found = true;
          else
            {
              trail = current;
              current = current->next;
            }
        if (current == first)   // if the newNode goes at the beginning.
          {
            newNode->next = current;
            current->back = newNode;
            first = newNode;
          }
        else 
          {
            trail->next = newNode;
            newNode->back = trail;
            newNode->next = current;
          }
      }
  }
  
  void deleteNode(int x)
  // given an integer x, deleteNode will delete the node containing x in the doubly linked list.
  {
    nodeType *current;
    bool found = false;
    current = first;
    if (first == NULL)
      cout << "List is empty.\n";
    else 
      {
	while(current != NULL && !found)
	  {
	    if (current->info == x) found = true;
	    else current = current->next;
	  }
	if (found)
	  {
	    current->back->next = current->next;
	    current->next->back = current->back;
	    delete current;
	  }
	else cout << "Target not found.\n";
      }
  }
  
  orderedDLL()
  // constructor 
  {
    first = NULL;
  }
  
  void print()
  // print will print the data elements contents of the doubly linked list.
  {
    nodeType *current;
    current = first;
    if (current == NULL)
      cout << "The list is empty";
    while (current != NULL)
      {
	cout << current->info << " ";
	current = current->next;
      }
  }
  
  void mergeLists(orderedDLL &list1, orderedDLL &list2)
  // given two doubly linked lists, mergeLists will merge them in the appropriate order and store them into
  // the member data.
  {
    nodeType *current;
    int temp;
    current = list1.first;
    while (current != NULL)
      { 
	insertNode(current->info);
	current = current->next;
      }
    current = list2.first;
    while (current != NULL)
      { 
	this->insertNode(current->info);
	current = current->next;
      }
    cout << endl;
  }

  
  bool distinctItems()
  {
    bool distinct = true;
    nodeType *current;
    current = first;
    if (first == NULL)
      return distinct;
    else
      while (current != NULL)
	{
	  if (current->next == NULL) ;
	  else if (current->info == current->next->info)
	    distinct = false;
	  current = current->next;
	}
    return distinct;
  }
  
  void printRepeats()
  // printRepeats will print repeated elements in the doubly linkedlist.
  {
    nodeType *current;
    int count = 1, repeat;
    
    current = first;
    
    if (current == NULL)
      cout << "The list is empty.\n";
    else if (!distinctItems())
      {
	while(current != NULL)
	  {
	    if (current->next == NULL);
	    else if (current->info == current->next->info)
	      {
		while (current->next != NULL && current->info == current->next->info)
		  {
		    count++;
		    if (count == 2)
		      repeat = current->info;
		    current = current->next;
		  }
		if (count > 1)
		  cout << "Item " << repeat << " is repeated " << count << " times.\n";
	      }
	    current = current->next;
	    count = 1;
	  }
      }
    else
      cout << "There are no repeated items.\n";

  }

  void splitList(orderedDLL &list1, orderedDLL &list2)
  // given two orderedDLL lists, splitList will divide the member data list in half, and store each half into
  // the two lists.
  {
    nodeType *current;
    int size = 0, half;
    current = first;
    while(current!= NULL)
      {
	size++;
	current = current->next;
      }
    half = size / 2;
    list1.first = first;
    current = first;
    for (int i = 1; i < half + 1; i++)
      {
	if (i == half)
	  {
	    list2.first = current->next;
	    current->next = NULL;
	    list2.first->back = NULL;
	  }
	else
	  current = current->next;
      }	
  }

private:
  nodeType *first; // pointer to first node in the doubly linked list.
};

void readData(orderedDLL&);

int main()
{
  orderedDLL L1, L2, L3;   // three doubly linked lists
  
  readData(L1);
  readData(L2);

  L3.mergeLists(L1,L2);
  
  cout << "First ordered list:  "; L1.print(); cout << endl;
  cout << "Second ordered list: "; L2.print(); cout << "\n\n";
  cout << "Merged ordered list: "; L3.print(); cout << "\n\n";
  cout << "are items in the merged list distinct? \n";
  
  if (L3.distinctItems()) cout << "Yes\n\n";
  else cout << "No\n\n";
  
  cout << "List repeated items in the list:\n";
  L3.printRepeats(); cout << endl;

  cout << "Two lists formed by splitting merged list in the middle are:\n";
  L3.splitList(L1,L2);
  L1.print(); 
  cout << "\n and\n";
  L2.print();
  cout << endl << endl;
  
  return 0;
}

void readData(orderedDLL& list)
// given an orderedDLL list, readData will input integers and store them into the list by using the member 
// function insertNode(int).
{
  int temp;

  cin >> temp;
  while (cin && temp != 99999)
    {
      list.insertNode(temp);
      cin >> temp; 
    }
}
