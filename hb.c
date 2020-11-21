#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

enum status {empty, shelved};
typedef enum status status;

enum color {black, red, green, yellow, blue, magenta, cyan, white};
typedef enum color color;

enum bool {false, true};
typedef enum bool bool;

struct bookcase {
   char **ary; /*max size is 9x9*/
   int *bookends; /*the final book index for each shelf*/
   int height;
   int width;
   bookcase *prev;
};
typedef struct bookcase bookcase;

struct layer{
   bookcases **siblings; /*not 2d, array of pointers*/
   int generation;
   struct tree *next;
};
typedef struct layer layer;

struct pyramid {
   layer *start;
   int layers;
};
typedef struct pyramid pyramid;

bookcase *processCaseFile();

/*returns a happy child bookcase*/
void makeHappy();

/*returns true if it's not possible to make a happy bookcase with the given configuration*/
bool isdoomed();

/*returns true if valid colors are used, the case is uniformly sorted*/
bool ishappy();

/*Check if uniform shelf? If */
bool uniformCase();

bool isBookOnShelf(char *shelf, char book);

/*is all of shelf the same as the first character? Can be done with a single loop*/
bool uniformShelf(bookcase b, int shelf);

/*returns true if the books color is one of the colors in enum bool*/
bool validColors(bookcase b);

pyramid createPyramid(bookcase *b)

layer *newLayer(layer *top);

bool createChildren(bookcase *b, layer y);

bookcase *moveBook(bookcase *b, int old, int new);

bool canMoveBook(bookcase b, int old, int new);

int getBookend(bookcase b, int shelf);

bool shelfEmpty(bookcase b, int shelf);

bool shelfFull(bookcase b, int shelf);

bookcase *createBookcase(char **ary, int height, int width);

color mapChars(char c);

test();

int main(void){
   test();
   return 0;
}

void test(){
}

void readFile(char *filename);

bool isValid(){
   /*file formatting is incorrect*/
   /*if total number of books is greater than width * height, return false*/
   /*check each shelf for a status empty*/
   /*if a status of shelved follows this on the same shelf, return false*/
   /*height and width are missing return false*/
   /*actual number of shelves doesn't match return false*/
   /*actual width for any shelf doesn't match return false*/
   /*if validColor is false, return false*/
}

bool isDoomed(){
   /*file formatting is corret, but a solution isn't possible*/
   /*this is just one of several checks, maybe go in is doomed*/
   /*create array/histogram length of num of colors*/
   /*loop through bookcase and set hist[color]++*/
   /*if hist[color] is greater than width, return false*/
   /*if number of colors in hist > 0 is greater than height, return false*/
   /*if you make it to the end return true*/
}

void makeHappy(happycases *h){
   /*bool test = false*/
   /*while test == false do createchildren*/
   /*newPyramid addLayer()*/
}

int[] bookcaseHist(bookcase b){
   /*create int[COLORQUANT]*/
   /*use color map*/
   /*loop through bookcase and set hist[color] = val++*/
   /*return int[]*/
}

bool isHappy(bookcase b){
   /*loop through shelves, if any aren't uniform, return false*/
   /*if uniformCase is false, return false*/
   /*return true*/
}

bool uniformCase(){
   /*loop through each book on a shelf*/
   /*grab b[shelf][i] and see what color it is*/
   /*loop through all other shelves*/
   /*for each shelf that is not the current shelf, isBookOnShelf(b[checkshelf], b[shelf][i])*/
   /*if isBookOnShelf == true, return false*/
   /*if you make it to the end, return true*/
}

bool isBookOnShelf(char *shelf, char book){
   /*loop through shelf and see if book is on the shelf*/
   /*if so return true*/
   /*return false*/
}

bool uniformShelf(bookcase b, int shelf){
   /*color = b[shelf][i] then loop*/
   /*if b[shelf][i++] != color return false*/
   /*return true*/
}

bool validColors(bookcase b){
   /*for j = 0, j++; for i = 0, i++*/
   /*if color < black >> return false*/
   /*else return true*/
}

pyramid createPyramid(bookcase *b){
   /*create a new pyramid*/
   /*create a null *layer*/
   /*call newLayer to populate*/
   /*layers = 0*/
}

layer *newLayer(layer *top){
   /*if top == NULL, this is the start*/
   /*newY = calloc new layer*/
   /*create new **bookcase, and allocate room of sizeof(bookcase *)*/
   /*get number of children and calloc an array to bookcase*/
   /*use layer->generations to get the right number*/
   /*loop through and use createChildren for each one*/
}

bool createChildren(bookcase *b, layer y){
   /*get number of children and calloc an array to bookcase*/
   /*assign the array to b->children*/
   /*loop through num of shelves twice*/
   /*in first loop, grab old shelf*/
   /*in inner loop, grab new shelves*/
   /*in inner loop child = moveBook(*b, old, new)*/
   /*b->children[counter] = child, counter++*/
   /*add the child to y->siblings*/
   /*check is happy?*/
   /*if so return true, else false*/
}

bookcase *moveBook(bookcase *b, int old, int new){
   /*if canMoveBook == true do the below*/
   /*create a new bookcase use a function for this*/
   /*getbookend(old), set to empty.*/
   /*getbookend(new), set arry[new][index + 1]*/
   /*else, do nothing*/
}

bool canMoveBook(bookcase b, int old, int new){
   /*quit if old shelf is empty*/
   /*quit if new shelf is full*/
}

int getBookend(bookcase b, int shelf){
   /*loop through a shelf number and get the index that contains the last book*/
}

bool shelfEmpty(bookcase b, int shelf){
   /*shelf ranges from 0 to height - 1*/
   /*for a given row, does it have 0 books?*/
   /*if so return true; else false*/
}

bool shelfFull(bookcase b, int shelf){
   /*shelf starts from 0 and goes to height - 1*/
   /*for a given row, does it have width - 1 books?*/
   /*if so return true; else false;*/
}

bookcase *createBookcase(char **ary, int height, int width){
   /*calloc a new space for a bookcase*/
   /*set the ary, height, width to parameters*/
   /*set prev = NULL*/
   /*return bookcase*/
}

color mapChars(char c){
   /*switch statement for mapping the chars to colors*/
   c = toupper(c);
   switch(c){
      case 'K':
         return black;
      case 'R':
         return red;
      case 'G':
         return green;
      case 'Y':
         return yellow;
      case 'B';
         return blue;
      case 'M':
         return magenta;
      case 'C':
         return cyan;
      case 'W':
         return white;
   }
}
