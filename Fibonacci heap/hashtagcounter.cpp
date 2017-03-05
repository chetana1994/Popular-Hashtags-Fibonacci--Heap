//Declaring all the headerfiles required
#include <iostream>
using namespace std;
#include <cstddef>
#include <math.h>
#include <limits>
#include <string.h>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <vector>

const char* const DELIMITER = " ";      // Defining space as delimiter for parsing the input file
template<class T>
class FibonacciHeap
{
 public:

  class Node                           //Defining the node structure of the heap
  {
  public:
    Node(T k, void *pl,std::string h)  //Constructor for the class Node
      :key(k),child_cut(false),p(NULL),left(NULL),right(NULL),child(NULL),degree(-1),hash(h),payload(pl)
    {
    }

    ~Node()                            //Destructor for the class Node
      {
      }

    T key;
    bool child_cut;
    Node *p;
    Node *left;
    Node *right;
    Node *child;
    int degree;
	std::string hash;
    void *payload;
  }; // end Node

  FibonacciHeap()                   //Constructor for the class FibonacciHeap
    :n(0),max(NULL)
    {
    }

  ~FibonacciHeap()                 //Destructor for the class FibonacciHeap
    {
      // delete all nodes.
      del_nodes(max);
    }


  //Insert the elements into the root list of the heap
  void insert(Node *x)
  {
    x->degree = 0;
    x->p = NULL;
    x->child = NULL;
    x->child_cut = false;
    if ( max == NULL)
      {
	max = x->left = x->right = x;
      }
    else
      {
	max->left->right = x;
	x->left = max->left;
	max->left = x;
	x->right = max;
	if ( x->key > max->key )
	  {
	    max = x;
	  }
      }
    ++n;
  }

  /*
   * The maximum node of the heap.
   */
  Node* maximum()
  {
    return max;
  }

  //Concatenating two heaps
  static FibonacciHeap* union_FibonacciHeap(FibonacciHeap *H1, FibonacciHeap *H2)
  {
    FibonacciHeap* H = new FibonacciHeap();
    H->max = H1->max;
    if ( H->max != NULL && H2->max != NULL )
      {
	H->max->right->left = H2->max->left;
	H2->max->left->right = H->max->right;
	H->max->right = H2->max;
	H2->max->left = H->max;
      }
    if ( H1->max == NULL || ( H2->max != NULL && H2->max->key > H1->max->key ) )
      {
	H->max = H2->max;
      }
    H->n = H1->n + H2->n;
    return H;
  }

// Extract max element from the heap
  Node* extract_max()
  {
    Node *z, *x, *next;
    Node ** childList;
    z = max;
    if ( z != NULL )
      {
	x = z->child;
	if ( x != NULL )
	  {
	    childList = new Node*[z->degree];
	    next = x;
	    for ( int i = 0; i < (int)z->degree; i++ )
	      {
		childList[i] = next;
		next = next->right;
	      }
	    for ( int i = 0; i < (int)z->degree; i++ )
	      {
		x = childList[i];
		max->left->right = x;
		x->left = max->left;
		max->left = x;
		x->right = max;
		x->p = NULL;
	      }
	    delete [] childList;
	  }
	z->left->right = z->right;
	z->right->left = z->left;
	if ( z == z->right )
	  {
	    max = NULL;
	  }
	else
	  {
	    max = z->right;
	    consolidate();
	  }
	n--;
      }
    return z;
  }

// heap consolidation
  void consolidate()
  {
    Node* w, * next, * x, * y, * temp;
    Node** A, ** rootList;
    // Max degree <= log base golden ratio of n
    int d, rootSize;
    int max_degree = static_cast<int>(floor(log(static_cast<double>(n))/log(static_cast<double>(1 + sqrt(static_cast<double>(5)))/2)));

    A = new Node*[max_degree+2]; // plus two both for indexing to max degree and so A[max_degree+1] == NIL
    std::fill_n(A, max_degree+2, NULL);

    w = max;
    rootSize = 0;
    next = w;
    do
      {
	rootSize++;
	next = next->right;
      } while ( next != w );
    rootList = new Node*[rootSize];
    for ( int i = 0; i < rootSize; i++ )
      {
	rootList[i] = next;
	next = next->right;
      }
    for ( int i = 0; i < rootSize; i++ )
      {
	w = rootList[i];

	x = w;

	d = x->degree;

	while ( A[d] != NULL )
	  {

	    y = A[d];

	    if ( x->key < y->key )
	      {

		temp = x;
		x = y;
		y = temp;
	      }

	    fib_heap_link(y,x);

	    A[d] = NULL;

	    d++;
	  }

	A[d] = x;
      }
    delete [] rootList;

    max = NULL;

    for ( int i = 0; i < max_degree+2; i++ )
      {

	if ( A[i] != NULL )
	  {

	    if ( max == NULL )
	      {

		max = A[i]->left = A[i]->right = A[i];
	      }
	    else
	      {

		max->left->right = A[i];
		A[i]->left = max->left;
		max->left = A[i];
		A[i]->right = max;

		if ( A[i]->key > max->key )
		  {

		    max = A[i];
		  }
	      }
	  }
      }
    delete [] A;
  }

/*
 * fib_heap_link(y,x)
 * 1. remove y from the root list of heap
 * 2. make y a child of x, incrementing x.degree
 * 3. y.child_cut = FALSE
 */
  void fib_heap_link( Node* y, Node* x )
  {

    y->left->right = y->right;
    y->right->left = y->left;

    if ( x->child != NULL )
      {
	x->child->left->right = y;
	y->left = x->child->left;
	x->child->left = y;
	y->right = x->child;
      }
    else
      {
	x->child = y;
	y->right = y;
	y->left = y;
      }
    y->p = x;
    x->degree++;

    y->child_cut = false;
  }


  /*
   * increase_key(x,k)
   * 3. x.key = k
   * 4. y = x.p
   * 5. if y != NIL and x.key > y.key
   * 6. 	CUT(H,x,y)
   * 7. 	CASCADING-CUT(H,y)
   * 8. if x.key > H.max.key
   * 9. 	H.max = x
   */
  void increase_key( Node* x, int k )
  {
    Node* y;



    x->key = x->key + k;

    y = x->p;

    if ( y != NULL && x->key > y->key )
      {

	cut(x,y);

	cascading_cut(y);
      }

    if ( x->key > max->key )
      {

	max = x;
      }
  }

  /*
   * cut(x,y)
   * 1. remove x from the child list of y, decrementing y.degree
   * 2. add x to the root list of H
   * 3. x.p = NIL
   * 4. x.child_cut = FALSE
   */
  void cut( Node* x, Node* y )
  {

    if ( x->right == x )
      {
	y->child = NULL;
      }
    else
      {
	x->right->left = x->left;
	x->left->right = x->right;
	if ( y->child == x )
	  {
	    y->child = x->right;
	  }
      }
    y->degree--;

    max->right->left = x;
    x->right = max->right;
    max->right = x;
    x->left = max;

    x->p = NULL;

    x->child_cut = false;
  }

/*
  * cascading_cut(y)
  * 1. z = y.p
  * 2. if z != NIL
  * 3. 	if y.child_cut == FALSE
  * 4. 		y.child_cut = TRUE
  * 5. 	else CUT(H,y,z)
  * 6. 		CASCADING-CUT(H,z)
  */
  void cascading_cut( Node* y )
  {
    Node* z;


    z = y->p;

    if ( z != NULL )
      {

	if ( y->child_cut == false )
	  {

	    y->child_cut = true;
	  }
	else
	  {

	    cut(y,z);

	    cascading_cut(z);
	  }
      }
  }

  /*
   * set to infinity so that it becomes easy to remove the node from top of the heap
   */
  void remove_Node( Node* x )
  {
    increase_key(x,std::numeric_limits<T>::max());
    Node *fn = extract_max();
    delete fn;
  }
  void del_nodes(Node *x)      //Delete Nodes in heap
  {
    if (!x)
      return;

    Node *current = x;
    while(true)
      {

	if (current->left && current->left != x)
	  {
	    Node *tmp = current;
	    current = current->left;
	    if (tmp->child)
	      del_nodes(tmp->child);
	    delete tmp;
	  }
	else
	  {
	    if (current->child)
	      del_nodes(current->child);
	    delete current;
	    break;
	  }
      }
  }
  /*
   * mapping operations to STL-compatible signatures.
   */
  bool empty() const
  {
    return n == 0;
  }

  Node* topNode()
  {
    return maximum();
  }

  T top()
  {
    return maximum()->key;
  }

  void pop()
  {
    if (empty())
      return;
    Node *x = extract_max();
    if (x)
      delete x;
  }

  Node* push(T k, void *pl,std::string h)
  {
    Node *x = new Node(k,pl,h);
    insert(x);
    return x;
  }

  Node* push(T k,std::string h)
  {
    return push(k,NULL,h);
  }

  unsigned int size()
  {
    return (unsigned int) n;
  }

  int n;
  Node *max;

};

bool isnum(char *buffer)
{

	if( *buffer>='0' && *buffer<='9')
	{
		return true;
 	}
	return false;
}

int main(int argc,char* argv[])
     {
        if(argc<2)
		{
          cerr<<"Usage "<<argv[0]<<" Input_File_Name";
		  return 0;
		}
		
		cout<<"Open the file 'output.txt' to view the output"<<endl;
		
		freopen("output.txt","w",stdout);  //Writing the output to a file
	    FibonacciHeap<int> fh;
	    FibonacciHeap<int>::Node *node;
	    map<string,FibonacciHeap<int>::Node *> ht; //Declaring a hash map to store the key(hashtag) and it's value(frequency)
	    map <string,FibonacciHeap<int>::Node *> :: iterator htiter; //Defining an iterator for the map
	    char ch='y';
	    int i=0;

	    int n,m,l;
	    FibonacciHeap<int>::Node * p;
	    FibonacciHeap<int>::Node * s;

	    string line="";
	    ifstream fis;
       
	    fis.open (argv[1]);
	    while(!fis.eof())
	    {
		    char buf[100];
            fis.getline(buf, 100);//specifying the number of characters per line
		    int n = 0; // a for-loop index
               
            char* token[20] = {}; 
		    token[0] = strtok(buf, DELIMITER); // first token
               
		    if (token[0]) // zero if line is blank
		        {        
                       
                        if(token[0][0]=='S' && token[0][1]=='T' &&      // Condition for exiting the file reading while loop
                            token[0][2] == 'O' && token[0][3]=='P')
                              {  
                                 return 0;
                              }
			
			if(isnum(token[0]))                                 // Check if the 1st token in the line of input file is a numeric value 
			   {
				
				   vector<FibonacciHeap<int>::Node*> top_k;
				   int num=atoi(token[0]);

				for(int i=0;i<num;i++)
				{
					top_k.push_back(fh.extract_max());          // Extract the max frequency nodes from the heap and insert them into the vector,'top_k'
					
				}
				for(int i=0;i<num;i++)
				{
					cout<<(top_k[i]->hash).substr(1,(top_k[i]->hash).size()) << " ,";      // Display the most popular n hashtags from the vector 'top_k'
					p = fh.push(top_k[i]->key,top_k[i]->hash);                             // Reinsert the extracted values of frequencies into the heap
					ht[top_k[i]->hash]= p;
				}
			}
			else{
				for (n = 1; n < 20; n++)
				{
					token[n] = strtok(0, DELIMITER); // subsequent tokens
					if (!token[n]) break; // no more tokens
				}
				if(ht.find(token[0])==ht.end())                                            //Check if the hashtag is already present in the hashtable
				{
	          	    p=fh.push(atoi((token[1])),token[0]);                                     // Inserting hashtag and it's value into the heap
					ht[token[0]]= p;                                                     // Insert into the hashtable as well
				
				}
				else                                                                    //if hashtag already existing in the hashtable 
				{
					p = ht[token[0]];                                                  //Retrieve the pointer to the heap node where this hashtag and it's value are stored
					fh.increase_key(p,atoi(token[1]));                                 //Perform increase_key operation on the value of the node to update the frequencies
				}
			}
		}
    }
	return 0;
}




