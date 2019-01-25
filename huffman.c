/* ************************************
* Tony Nguyen  CS-241 
* 12/8/17
* 
* Encode:
* Given a text file, reads in frequencys and 
  creates huffman codes by building a huffman tree, compresses
  given text file to file specified.
* "The program in huffencode.c takes two command line arguments. 
  The first is the name of the file to encode and the 
  second is the name of the file where the encoded data will be
  written. Exit with an error if either of the files are unable 
  to be opened."
  Format of Encoded file:
  1) Number of symbols in table (unsigned char), 256 is zero.
  2) Symbols and Frequency Pairs
    - Symbol (unsigned char)
    - Frequency for symbol (unsigned long) 
  3) Total number of encoded symbols (unsigned long)
  4) Bits of encoded data.
* Prints table of frequences and huffman codes to standard out.
  Decode:
  Given an encoded file that was encoded by this program, decodes
  by reading in bits and writing text to file specified.

  
************************************ */

#include <stdio.h>
#include "huffman.h"
#include <stdlib.h>


/* Used to build huffman tree. */
struct TreeNode
{
  unsigned char symbol;
  unsigned long freqcy;
  struct TreeNode* left;
  struct TreeNode* right;
};
/* Used as a priority queue to concatinate huffman trees.*/
struct Queue
{
  struct TreeNode* tree;
  struct Queue* next;
};
/* Used to represent huffman codes. */
struct huffCode
{
  char bit;
  struct huffCode* next;
};

/* HUFFCODE STRUCT METHODS ----------------------*/
/* ************************************
* Each parameter ’s type and name :
  char c
* Function ’s return value .
  struct huffCode* - returns new node.
* Description of what the function
* does .
  Allocates memory to create a new huffman node.
************************************ */
struct huffCode* createCode(char c)
{
  struct huffCode* node = malloc(sizeof(struct huffCode));
  node->next = NULL;
  node->bit = c;
  return node;
}

/* ************************************
* Each parameter ’s type and name :
* input and/or output ,
* its meaning ,
* its range of values .
* Function ’s return value .
* Description of what the function
* does .
  Add node to end of list. 
* Function ’s algorithm
  1) Recursively step into function until end of list.
  2) Uses createCode and adds to the end of list.
  3) Returns head of the list.
************************************ */
struct huffCode* addHuffC(struct huffCode* head, char c)
{
  if(head == NULL)
  {
    struct huffCode* node = createCode(c);
    head = node; 
  }
  else
  {
    head->next = addHuffC(head->next,c);
  }
  return head;
}

/* ************************************
* Each parameter ’s type and name :
  struct huffCode** headRef
* Function ’s return value .
  char, in other words the bit represented as a char.
* Description of what the function
* does .
  Removes the last node given head refrence to a list.
  Assumes there is atleast 1 node in list. 
* Function ’s algorithm
  1) Recursively steps through list until last node is found.
  2) Removes last node.
************************************ */
char removeLSB(struct huffCode** headRef)
{
  struct huffCode* node;
  char bit;
  if((*headRef)->next == NULL)
  {
    node = *headRef;
    bit = node->bit; 
    free(node);
    *headRef = NULL;
    return bit;
  }
  else
  {
    return removeLSB(&((*headRef)->next)); 
  }
}
/* ************************************
* Each parameter ’s type and name :
  struct huffCode** head
* its meaning ,
  Reference to a pointer that points to head of list.
* Function ’s return value .
  void
* Description of what the function
* does .
  Removes the first node given list.
************************************ */
void popHC(struct huffCode** head)
{
  struct huffCode* temp = *head;

  *head = temp->next;
  free(temp);
}
/* ************************************
* Each parameter ’s type and name :
  struct huffCode* head
* its meaning ,
  Pointer to head of list.
* Function ’s return value .
  void
* Description of what the function
* does .
  Prints each char contained in each node of list.
* Function ’s algorithm
  1) Recursively steps through list and prints each.
************************************ */
void printHuffC(struct huffCode* head)
{
  struct huffCode* curr = head;

  while(curr != NULL)
  {
    printf("%c", curr->bit);
    curr = curr->next;
  }
}
/* ************************************
* Each parameter ’s type and name :
  struct huffCode* head 
* its meaning ,
  Pointer to head of list.
* Function ’s return value .
  void
* Description of what the function
* does .
  Frees huffmanCode list given head.
* Function ’s algorithm
  1) Recursively steps through list and frees.
************************************ */
void freeHuffC(struct huffCode* head )
{
  struct huffCode* current = head ;
  if(current != NULL)
  {
    freeHuffC(current->next);
    current->next = NULL ;
    free(current);
  }
}
/* ************************************
* Each parameter ’s type and name :
  struct huffCode* huffC[]
* its meaning ,
  Array of Pointers to lists.
  Array size assumed to be 256.
* Function ’s return value .
  void
* Description of what the function
* does .
  If pointer exists, frees.
  
************************************ */
void freeHCarray(struct huffCode* huffC[])
{
  int i;
  for(i = 0; i < 256; i++)
  {
    if(huffC[i] != NULL)
    {
      freeHuffC(huffC[i]);
    }
  }
  
}

/* Got from stackoverflow from templatetypedef,
   changed to work with my code. */
/* ************************************
* Each parameter ’s type and name :
  struct huffCode* head
* its meaning ,
  Pointer to list.
* Function ’s return value .
  struct huffCode* - head of new cloned list.
* Description of what the function
* does .
  Clones huffCode list,
* Function ’s algorithm
  1) Recursively steps through list and builds a clone 
  of list.
************************************ */
struct huffCode* cloneList(struct huffCode* head)
{
  struct huffCode* result;
  if(head == NULL) return NULL;
  result = createCode(head->bit);
  result->next = cloneList(head->next);
  return result; 
}

/* ************************************
* Each parameter ’s type and name :
  struct huffCode** headtemp, struct TreeNode* root,
    struct huffCode* huffC[]
* It's Meaning.
  headtemp - list that is to be cleared if a huffman code is completed.
  root - given huffman tree.
  huffC[] - assumed to be of size 256, used to store codes.
* Function ’s return value .
  void
* Description of what the function
* does .
  By going further into the tree there are more bits added to 
  headtemp list, recursively visits the leaves of given huffman tree
  and adds them to an array of pointers. When moving up the tree,
  bits are removed to represent the current codes at a current node.
************************************ */
void utilBuildHC(struct huffCode** headtemp, struct TreeNode* root,
    struct huffCode* huffC[])
{
  if(root != NULL)
  {
    /* Found leaf, clones current state of headtemp to a new list
       in the integer representation of the symbol in array. */
    if((root->left) == NULL && (root->right) == NULL)
    {
      huffC[root->symbol] = cloneList(*headtemp); 
      return;
    }
    /* About to go into left node add 0 to headtemp. */
    *headtemp = addHuffC(*headtemp,'0');
    utilBuildHC(headtemp, root->left, huffC);
    /* Came back from left node, removes 0 */
    removeLSB(headtemp);

    /* About to into right node add 1 to headtemp. */
    *headtemp = addHuffC(*headtemp, '1'); 
    utilBuildHC(headtemp, root->right, huffC); 
    /* Came back from right node, updates the codes by removing 1 */
    removeLSB(headtemp);
  } 
}
/* ************************************
* Each parameter ’s type and name :
  struct huffCode* huffC[], struct TreeNode* root
* its meaning ,
  huffC[] - assumed to be size 256, used to store huffman codes.
  root - given huffman tree.
* Function ’s return value .
  void
* Description of what the function
* does .
  Uses utilbuildHC to build huffman codes, and stores into array.
************************************ */
void buildHuffC(struct huffCode* huffC[], struct TreeNode* root)
{
  /* Used to keep track of what codes have acumulated when going
     through root Tree, zero is added to the end of list
     when going into left node and one is added when going right. */
  struct huffCode* headtemp = NULL;
  /* huffC and head temp expected to change */
  utilBuildHC(&headtemp, root, huffC);
  freeHuffC(headtemp);
}

/* ------------------------------------------*/

/* HUFFTREE METHODS -------------------------*/
/* ************************************
* Each parameter ’s type and name :
  unsigned char symb, unsigned long f
* its meaning ,
  symb - symbol.
  f - frequency.
* Function ’s return value .
  struct TreeNode* - pointer to new created node.
* Description of what the function
* does .
  Allocates memory for a new tree node given symb and f.
************************************ */
struct TreeNode* createNode(unsigned char symb, unsigned long f)
{
  struct TreeNode* node = malloc(sizeof(struct TreeNode));

  node->symbol = symb;
  node->freqcy = f;
  node->right = NULL;
  node->left = NULL;
  return node; 
}

/* ************************************
* Each parameter ’s type and name :
  struct TreeNode* root
* Function ’s return value .
  void
* Description of what the function
* does .
  Recursive function, used in printTree() 
************************************ */
void printTreeHelper(struct TreeNode* root)
{
  if ( root != NULL )
  {
    printTreeHelper(root->left);
    printf("[%c,%lu] ", root->symbol, root->freqcy);
    printTreeHelper(root->right);
  }
}

/* ************************************
* Each parameter ’s type and name :
  struct TreeNode* root
* Function ’s return value .
  void
* Description of what the function
* does .
  Uses printTreeHelper to print tree.
************************************ */
void printTree(struct TreeNode* root)
{
  printf("Tree : ");
  printTreeHelper(root);
  printf("\n");
}
/* ) */
/* ************************************
* Each parameter ’s type and name :
  struct TreeNode* root
* Function ’s return value .
  void
* Description of what the function
* does .
  Recursive function, used in printLeaves()
************************************ */
void printLeavesHelper(struct TreeNode* root)
{
  if( root != NULL )
  {
    printLeavesHelper(root->left);
    printLeavesHelper(root->right);
    if(root->left == NULL && root->right == NULL)
    {
      printf("[%c,%lu]  ", root->symbol, root->freqcy);
    }
  }
}
/* ************************************
* Each parameter ’s type and name :
   struct TreeNode * root 
* Function ’s return value .
  void
* Description of what the function
* does .
  Uses printLeavesHelper to print leaves.
************************************ */
void printLeaves ( struct TreeNode * root )
{
  printf("Leaves: ");
  printLeavesHelper ( root );
  printf ("\n");
}

/* ************************************
* Each parameter ’s type and name :
  struct TreeNode* root
* Function ’s return value .
  void
* Description of what the function
* does .
  Recursively steps through tree and frees.
************************************ */
void freeTree(struct TreeNode* root)
{
  if(root != NULL)
  {
    freeTree(root->left);
    freeTree(root->right);
    free(root);
  }
}


/* ------------------------------------------*/

/* QUEUE METHODS ----------------------------*/
/* ************************************
* Each parameter ’s type and name :
  struct TreeNode* head
* Function ’s return value .
  struct Queue* - pointer to new node.
* Description of what the function
* does .
  Allocates memory for new Priority Queue node.
************************************ */
struct Queue* createQ(struct TreeNode* head)
{
  struct Queue* node = malloc(sizeof(struct Queue));

  node->tree = head;
  node->next = NULL;
  return node; 
}

/* ************************************
* Each parameter ’s type and name :
  struct Queue* head, struct TreeNode* tree 
* its meaning ,
  head - pointer to head of Priority Queue.
  tree - tree that must be made with Queue node, and placed
         in correct position based on left most char.
* Function ’s return value .
  struct Queue* - head of the Priority Queue.
* Description of what the function
* does .
  Finds a correct position where to insert given tree based
  on frequency, if there is a tie left most node of tree is looked at.
* Function ’s algorithm
  1) Recursively finds position comparing frequencys and then 
  left most symbol in tree.
  2) Creates new node with tree in the correct position.
************************************ */
struct Queue* insertPriority(struct Queue* head, struct TreeNode* tree ) 
{
  struct Queue* newNode;
  unsigned long f = tree->freqcy;
  unsigned char symb = tree->symbol;

  if(head == NULL){
    newNode = createQ(tree);
    return newNode;
  }
  else if( (f < (head->tree)->freqcy) ||
      (f == ((head->tree)->freqcy) &&
       symb < ((head->tree)->symbol)) )
  {
    newNode = createQ(tree);
    newNode->next = head;
    return newNode;
  }
  else
  {
    head->next = insertPriority(head->next,tree); 
    return head;
  }
}

/* ************************************
* Each parameter ’s type and name :
  struct Queue* head
* Function ’s return value .
  void
* Description of what the function
* does .
  Prints Priority Queue by going through each node and 
  calling printTree on all of trees nodes.
************************************ */
void printPQ(struct Queue* head)
{
  if(head == NULL) return;
  printTree(head->tree); 
  printPQ(head->next); 
}
/* ************************************
* Each parameter ’s type and name :
  struct Queue* head
* Function ’s return value .
  void
* Description of what the function
* does .
  Goes to end of list, frees tree and then frees node
  until PQ is completely freed.
************************************ */
void freePQ(struct Queue* head)
{
  if(head != NULL)
  {
    freePQ(head->next);
    freeTree(head->tree);
    free(head);
  }
}
/* ************************************
* Each parameter ’s type and name :
  struct Queue* head 
* Function ’s return value .
  int - length of PQ.
* Description of what the function
* does .
  Steps through list keeping track of count until end of list.
************************************ */
int lengthPQ(struct Queue* head )
{
  struct Queue* current = head ;
  int length = 0;

  while (current != NULL)
  {
    length++;
    current = current->next ;
  }
  return length ;
}

/* ************************************
* Each parameter ’s type and name :
  struct Queue** head
* Function ’s return value .
  struct TreeNode* - tree that was contained in removed node.
* Description of what the function
* does .
  Removes head of PQ, returning tree that was contained in head.
************************************ */
struct TreeNode* popPQ(struct Queue** head)
{
  struct Queue* temp = *head;
  struct TreeNode* tree = temp->tree;

  *head = temp->next;
  free(temp);
  return tree; 
}

/* Expects priority queue to have atleast 1 node. */
/* returns new head of list */
/* ************************************
* Each parameter ’s type and name :
  struct Queue* head
* its meaning ,
  head - expexts PQ to ahve atleast 1 node.
* Function ’s return value .
  struct Queue* - pointer to new head of list.
* Description of what the function
* does .
  Combines nodes of PQ until there is only 1 node.
* Function ’s algorithm
  1) While the length of PQ is not zero Pop two nodes.
  2) Combines trees that was contained in two nodes into
  a single node.
  3) Creates new node from combined trees.
  4) Inserts new node in PQ using insertPriority.
************************************ */
struct Queue* concatTrees(struct Queue* head)
{
  struct Queue* currHead = head;
  struct TreeNode* leftTree = NULL;
  struct TreeNode* rightTree = NULL;
  struct TreeNode* newTree = NULL;
  int weight = 0;

  if(currHead == NULL) return NULL;
  /* Iterively combines Trees into a single Tree.*/
  while(lengthPQ(currHead) != 1) 
  {
    leftTree = popPQ(&currHead);
    rightTree = popPQ(&currHead); 
    weight = (leftTree->freqcy) + (rightTree->freqcy);
    /* Builds new Tree with popped Trees. */
    newTree = createNode((leftTree->symbol),weight);
    newTree->left = leftTree;
    newTree->right = rightTree;
    /* Creates new Que struct containing combined Trees. */ 
    /* Returns new or old head of list */
    currHead = insertPriority(currHead,newTree);
  }
  return currHead;
}  

/* ************************************
* Each parameter ’s type and name :
  unsigned long freq[]
* its meaning ,
  freq[] - assumed to be size 256, contains frequency of symbols.
  note: array of 256 represents int values of ascii symbols.
* Function ’s return value .
  struct Queue* - head of list created based off of frequencys.
* Description of what the function
* does .
  Loops through entire arrray, if frequency exists create a new node
  and adds it to PQ using insertPriority.
************************************ */
struct Queue* setupPQ(unsigned long freq[])
{
  int i;
  struct Queue* head = NULL;
  struct TreeNode* temp = NULL;

  for(i = 0; i < 256; i++)
  {
    if(freq[i] != 0)
    {
      temp = createNode(i , freq[i]); 
      head = insertPriority(head,temp);
    }
  }
  return head;
}
/* ----------------------------------------- */

/* ************************************
* Each parameter ’s type and name :
  unsigned long freq[], FILE* in, int* numSymb,
                     unsigned long * totalSymb
* its meaning ,
  freq[] - array of frequencys of symbols (represented by
  int values arary index up to 256.
  in - file pointer.
  numSymb - number of unique symbols.
  totalSymb - number of total symbols.
* Function ’s return value .
  void
* Description of what the function
* does .
  Reads symbols into given freq, then rewinds in. 
  Increments to keep track of number of symbols and total. 
************************************ */
void readSymbols(unsigned long freq[], FILE* in, int* numSymb,
                     unsigned long * totalSymb)
{
  int c;
  int i;
  while((c = getc(in)) != EOF)
  {
    (*totalSymb)++;
    (freq[c])++;
  }
   
  for(i = 0; i < 256; i++)
  {
    if(freq[i] != 0)
    {
      (*numSymb)++; 
    } 
  } 
  
  rewind(in);  


}



/* note: fix format to match. */
/* */
/* ************************************
* Each parameter ’s type and name :
  unsigned long freq[], struct huffCode* huffC[], 
               unsigned long totalSymb
* input and/or output ,
* its meaning ,
* its range of values .
* Function ’s return value .
* Description of what the function
* does .
  Prints symbols, frequencys, and code. 
* Function ’s algorithm
  1) Loops through freq and huffC array and prints.
  2) Keeps track of unique / total symbols in loops. 
************************************ */
void printTable(unsigned long freq[], struct huffCode* huffC[], 
               unsigned long totalSymb)
{
  int i;

  printf("Symbol Freq Code\n");

  for(i = 0; i < 33; i++)
  {
    if((freq[i]) != 0)
    {
      printf("=%d    %lu    ", i , freq[i]);
      printHuffC(huffC[i]);
      printf("\n");
    }
  }

  for(i = 33; i <= 126; i++)
  {
    if((freq[i]) != 0)
    {
      printf("%c      %lu    ", i , freq[i]);
      printHuffC(huffC[i]);
      printf("\n");
    }
  }

  for(i = 127; i < 256; i++)
  {
    if((freq[i]) != 0)
    {
      printf("=%d    %lu    ", i , freq[i]);
      printHuffC(huffC[i]);
      printf("\n");
    }
  }
  printf("Total chars = %lu\n", totalSymb);
}

void setHCnull(struct huffCode* huffC[])
{
  int i;
  for(i = 0; i < 256; i++)
  {
    huffC[i] = NULL;
  }
}

/****************************************************************************/
/* This function copies 2 bytes from an int to an unsigned char array where */
/*   the least significant byte of the int is placed in the first element   */
/*   of the array.                                                          */
/****************************************************************************/
void copyIntToAddress(int n, unsigned char bytes[])
{
  bytes[0] = n & 0xFF;
}

/* ************************************
* Each parameter ’s type and name :
  int i, FILE* out
* Function ’s return value .
  void
* Description of what the function
* does .
  Given int, writes out int as a unsigne char.
  note: int assumed to be 0 - 255;
************************************ */
void writeSymbol(int i, FILE* out)
{
  unsigned char bytes[1] = {0};
  
  copyIntToAddress(i , bytes);
  fwrite(bytes, 1, sizeof(bytes), out);
}
/* ************************************
* Each parameter ’s type and name :
  unsigned long ul, FILE* out
* its meaning ,
  ul - unsigned long.
  out - file out pointer.
* Function ’s return value .
  void 
* Description of what the function
* does .
  Writes unsigned long to out file.
************************************ */
void writeUnsignedLong(unsigned long ul, FILE* out)
{
  fwrite(&ul, 1, sizeof(unsigned long), out); 
}

/* ************************************
* Each parameter ’s type and name :
  char bitsOutArray[]
* its meaning ,
  bitsOutArray[] - array of bits, array assumed to be size 8.
* Function ’s return value .
  unsigned char - bit represention of given array.
* Description of what the function
* does .
  MSB should be in lowest index of array, shifts unsigned char
  such that MSB is in proper position. Assumed to be working with
  8 bits. 
************************************ */
unsigned char utilbitsOut(char bitsOutArray[])
{
  unsigned char bits = 0;
  int i;
  for(i = 0; i < 8; i++)
  {
    if(bitsOutArray[i] == '1')
    {
      bits |= ( 1 << ( 7 - i) );
    } 
  }
  return bits;
}

/* ************************************
* Each parameter ’s type and name :
  char bitsOutArray[]
* its meaning ,
  bitsOutArray[] - array of bits.
* Function ’s return value .
  void
* Description of what the function
* does .
  Sets entire array to 0, prepares for the next 8 bits
  to be inputted into array.
************************************ */
void reset(char bitsOutArray[])
{
  int i;
  for(i = 0; i < 8; i++)
  {
    bitsOutArray[i] = 0;
  }
}


/* ************************************
* Each parameter ’s type and name :
  FILE* in, FILE* out
* its meaning ,
  in - input file (txt)
  out - file to be written to.
* Function ’s return value .
  void
* Description of what the function
* does .
  Encodes given input file into out according to specified format
  , see description at top of file. 
* Function ’s algorithm
  1) Reads in symbols frequencys from input file and store into array.
  2) Builds Priority Queue based on frequency array.
  3) Concat PQ into a single node which contains a single tree.
  4) Builds huffman codes from huffman tree, stores in array,
  huffman codes represented by a list, MSB is head of huffCode*.
  5) Writes specified format to out file.
  6) Writes out frequency and pairs.
  7) Writes out encoded file using huffman codes. 
************************************ */
void encodeFile(FILE* in, FILE* out)
{
  /* decided to change to unsigned long */
  unsigned long freq[256] = {0}; 
  struct huffCode* huffC[256];
  struct Queue* head = NULL;
  /* Used when encoding. */
  struct huffCode* curr = NULL;
  char bitsOutArray[8] = {0}; 
  /* Used when writing the header. */
  int numSymb = 0;
  unsigned long totalSymb = 0;

  unsigned char bitsOut = 0; 
  int index = 0;
  /*int shift = 7;*/
  int c;
  int i;

  setHCnull(huffC);
  readSymbols(freq, in, &numSymb, &totalSymb);
  
  head = setupPQ(freq);
  head = concatTrees(head);

  buildHuffC(huffC, head->tree);
  printTable(freq, huffC, totalSymb); 
  if(numSymb != 256)
  {
    writeSymbol(numSymb, out);
  } 
  else 
  {
    writeSymbol(0, out);
  }
  /* Writes symbols and frequencies to out. */ 
  for(i = 0; i < 256; i++)
  {
    if(freq[i] != 0)
    {
      /* i represents symbol in int form, IntToAddress puts bit
         form of int in unsigned char. */
      writeSymbol(i , out);
      /* Writes frequency.  */
      writeUnsignedLong(freq[i], out); 
    
    }
  } 
  fwrite(&totalSymb, 1, sizeof(unsigned long), out);
  /* Writes encoded data, pads with zero if encoding didn't end on byte. */
  while((c = getc(in)) != EOF)
  {
    curr = huffC[c];
    while(curr != NULL)
    {
      bitsOutArray[index] = curr->bit; 
      index++;
      /* bitsOutArray is full write to out. */
      if(index == 8)
      {
        bitsOut = utilbitsOut(bitsOutArray);  
        /* write to out */
        fwrite(&bitsOut, 1, sizeof(unsigned char), out);  
        /* reset bitsOutArray */
        reset(bitsOutArray);
        index = 0;
      }
      
      curr = curr->next;      
    }
  }
  /* Pads end if it did'nt end on a byte. */
  if(index != 0)
  {
    while(index != 8)
    {
      bitsOutArray[index] = 0;
      index++;
    }
    bitsOut = utilbitsOut(bitsOutArray);
    fwrite(&bitsOut, 1, sizeof(unsigned char), out);  
  } 
  /* Clean up. */ 
  freePQ(head);
  freeHCarray(huffC);
}
/* */
/* ************************************
* Each parameter ’s type and name :
  unsigned char temp, struct huffCode** head
* its meaning ,
  temp - read in char to analyze.
  head - head of list containing bits.
* Function ’s return value .
  void
* Description of what the function
* does .
  Adds to list based on what bits are in given unsigned char,
  starting with MSB.
************************************ */
void bitsDecode(unsigned char temp, struct huffCode** head)
{
  int i;
  for(i = 0; i < 8; i++)
  {
    *head = (temp & ( 1 << (7 - i))) ?
                 addHuffC(*head, '1') : addHuffC(*head, '0');
  }
}

/* ************************************
* Each parameter ’s type and name :
  FILE* in, FILE* out
* its meaning ,
  in - encoded file to be decoded.
  out - where decoded text will be written to.
* Function ’s return value .
  void
* Description of what the function
* does .
  Reads in format and decodes encoded file, see description
  at top of file.
* Function ’s algorithm
  1) Read in format, symbol frequencies stored into array.
  2) Builds PQ based on frequency array.
  3) Concat PQ into one node containing one tree.
  4) Build huffman codes from huffman tree.
  5) Reads in 1 byte of encoded upto number of total symbols. 
  6) Treverses based off of bits until a leaf node, writes out
  symbol at leaf and starts agian at top of tree.
************************************ */
void decodeFile(FILE* in, FILE* out)
{
  struct Queue* head = NULL;
  unsigned long freq[256] = {0};
  struct TreeNode* curr = NULL;
  struct huffCode* bitsOuthead = NULL;
  unsigned char numSymb = 0;
  unsigned long totalSymb = 0;
  int i;
  int count = 0;

  /* Reads number of unique symbols. */
  fread(&numSymb, sizeof(unsigned char), 1, in); 
  /* Loops up to amount of unique symbols. */
  for(i = 0; i < numSymb; i++)
  {
    unsigned char tempSymb = 0;
    unsigned long tempFreq = 0;
    /* Reads in symbol and freq, then assigns to array. */
    fread(&tempSymb, sizeof(unsigned char), 1, in);
    fread(&tempFreq, sizeof(unsigned long), 1, in);
    freq[tempSymb] = tempFreq; 
  }

  /* Builds huffman tree using read in frequencys. */
  head = setupPQ(freq);
  head = concatTrees(head);
  /*  printTree(head->tree); */
  /* Reads number of total symbols. */
  fread(&totalSymb, sizeof(unsigned long), 1, in);
    
  while(count != totalSymb ) 
  {
    curr = head->tree; 
     /* Stops once curr is at a leaf. */
    while((curr->left) != NULL && (curr->right) != NULL)
    {
      if(bitsOuthead == NULL)
      {
        unsigned char temp = 0;
        fread(&temp, sizeof(unsigned char), 1, in);
         /* bitsOuthead is assigned a list with the next 8 bits. */
        bitsDecode(temp, &bitsOuthead); 
      }
       /* If head contains 1 goes into right, if 0 goes into left. */
      curr = ((bitsOuthead->bit) == '1' ? curr->right : curr->left); 
       /* Removes bit from list. */
      popHC(&bitsOuthead);
    }
     /* Found symbol, writes to out.*/
    fwrite(&(curr->symbol), sizeof(unsigned char), 1, out); 
     /* Keeps track of total symbols. */
    count++;
  } 
  
  putc('\n', out); 
  freeHuffC(bitsOuthead);

}




