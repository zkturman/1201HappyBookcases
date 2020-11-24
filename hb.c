#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "neillsimplescreen.h"

#define EMPTY '.'
#define NUMTYPES 9
#define SEARCHMAX 7500000
#define BUFFER 50
#define MAXSHELVES 9
#define REALLOC_PLUS 5

/*delete this*/
enum index {invalid = -1, empty, blk, rd, grn, ylw, bl, mgnt, cyn, wht};
typedef enum index index;

enum bool {false, true};
typedef enum bool bool;

typedef char book;

struct bookcase {
   book **ary; /*max size is 9x9*/ /*rename shelves*/
   int height;
   int width;
   struct bookcase *prev;
   struct bookcase *next;
};
typedef struct bookcase bookcase;

struct layer{
   bookcase **siblings; /*not 2d, array of pointers*/
   bookcase *solution;
   int size;
   int generation;
   struct layer *next;
};
typedef struct layer layer;

struct problem {
   layer *start;
   bookcase *solution;
   bool noSolution;
   int moves;
};
typedef struct problem problem;


void printSolution(problem p, bool verbose);

void printBookcase(bookcase *bc);

void printBook(book b);

bookcase *readFile(char *filename);

bool isValid(book **books, int height, int width, int lineCount);

bool isValidHeight(int height, int lineCount);

bool isValidWidth(book **books, unsigned int height, unsigned int width);

/*returns true if the books color is one of the colors in enum bool*/
bool validColors(book **books, int height, int width);

/*returns true if it's not possible to make a happy bookcase with the given configuration*/
bool isDoomed(bookcase *bc, int numTypes);

int *bookcaseHist(bookcase *bc, int numTypes);

void buildSolutions(bookcase **bc, bookcase *solution);

/*returns a happy child bookcase*/
void makeHappy(problem *p);

problem createProblem(bookcase *bc);

layer *createLayer(layer **prev, bookcase *bc);

bool createChildren(layer **y);

/*returns true if valid colors are used, the case is uniformly sorted*/
bool isHappy(bookcase *bc);

/*Check if uniform shelf? If */
bool isolatedColors(bookcase *bc);

/*Returns true if the specified book exists in a string (shelf)*/
bool isBookOnShelf(char *shelf, book b);

/*Returns true if all books for this index (shelf) are the same.*/
/*Also returns true if there are no books the shelf.*/
bool uniformShelf(bookcase *bc, int shelf);

/*Returns a new bookcase where the last book on oldShelf is moved to the first*/
/*available position on newShelf.*/
bookcase *moveBook(bookcase *bc, int oldShelf, int newShelf);

/*Returns true for the following conditions:*/
/*oldShelf is non-empty, containing something other than '.'*/
/*newShelf is non-full, containing at least one '.'*/
bool canMoveBook(bookcase *bc, int oldShelf, int newShelf);

/*Returns the number of books for an index (shelf) of the bookcase's array*/
int numBooks(bookcase *bc, int shelf);

/*Determines if an index (shelf) for a bookcase is empty. Empty means there
are only empty spaces('.') on the shelf.*/
bool shelfEmpty(bookcase *bc, int shelf);

/*Determines if a given shelf for a bookcase is full. Full means there are
no empty spaces('.') on the shelf.*/
bool shelfFull(bookcase *bc, int shelf);

/*Initializes a bookcase pointer using an input ary and height/width*/
bookcase *createBookcase(char **ary, int height, int width);

/*Frees allocated memory for a struct pyramid*/
void freeProblem(problem *p);

/*Frees allocated memory for a struct layer*/
void freeLayer(layer **y);

/*Frees allocated memory for a struct bookcase*/
void freeBookcase(bookcase **bc);

/*Allocates memory and exits the program with an error if the requested*/
/*memory couldn't be allocated.*/
void *smartCalloc(int quant, int size);

/*takes a string, prints it to stderr, and exits the program*/
/*used to handle errors if the program cannot continue*/
void errorQuit(char *message);

/*Uses an enum to index the different types of books*/
/*Weird names used to avoid conflicts with neillcol*/
index bookToIndex(char c);

/*Used to get foreground color based on a type of book.*/
neillcol bookToColor(book b);

/*returns val raised to the exp*/
int exponent(int val, int exp);

void test();

int main(int argc, char **argv){
   bool verbose;
   bookcase *bc;
   problem p;
   test();
   if (argc == 2){
      verbose = false;
   }
   else if (argc == 3 && strcmp(argv[2], "verbose") == 0){
      verbose = true;
   }
   else{
      errorQuit("Incorrect arguments...exiting\n");
   }
   bc = readFile(argv[1]);
   p = createProblem(bc);
   makeHappy(&p);
   printSolution(p, verbose);
   return 0;
}

void test(){
   book **aryTest1, **emptyBooks;
   bookcase *b1, *b2, *b3, *easyBc, *emptyBc; /**happyBc, *bigBc, *scaryBc;*/
   layer *y1, *y2;
   problem p;
   int j, *hist;
   assert(bookToIndex('c') == cyn);
   assert(bookToIndex('b') == bl);
   assert(bookToIndex('B') == bl);
   assert(bookToIndex('k') == blk);
   assert(bookToIndex('.') == empty);
   assert(bookToIndex(' ') == -1);

   aryTest1 = (book **) smartCalloc(3, sizeof(book *));
   /*should quit instead if smartCalloc fails*/
   assert(aryTest1 != NULL);

   for (j = 0; j < 3; j++){
      aryTest1[j] = (book *) smartCalloc(4, sizeof(book));
   }
   strcpy(aryTest1[0], "RB.");
   strcpy(aryTest1[1], "Y..");
   strcpy(aryTest1[2], "BR.");

   b1 = createBookcase(aryTest1, 3, 3);
   assert(strcmp(b1->ary[0], aryTest1[0]) == 0);
   assert(b1->height == 3);
   assert(b1->width == 3);

   b1->ary[0][2] = 'B';
   assert(shelfFull(b1, 0) == true);
   assert(shelfFull(b1, 1) == false);

   b1->ary[1][0] = '.';
   assert(shelfEmpty(b1, 0) == false);
   assert(shelfEmpty(b1, 1) == true);

   /*for easier tracking: b1 = "RBB"
                               "..."
                               "BR."
   */

   assert(numBooks(b1, 0) == 3);
   assert(numBooks(b1, 1) == 0);
   assert(numBooks(b1, 2) == 2);

   /*test empty shelf*/
   assert(canMoveBook(b1, 1, 2) == false);
   assert(canMoveBook(b1, 2, 1) == true);

   /*test full shelf*/
   assert(canMoveBook(b1, 2, 0) == false);
   assert(canMoveBook(b1, 0, 2) == true);

   /*test moving a book*/
   b2 = moveBook(b1, 2, 1);
   assert(strcmp(b2->ary[2], "B..") == 0);
   assert(strcmp(b2->ary[1], "R..") == 0);
   b3 = moveBook(b2, 1, 2);
   assert(strcmp(b1->ary[1], b3->ary[1]) == 0);
   assert(strcmp(b1->ary[2], b3->ary[2]) == 0);

   /*make sure uniform shelves return true, including empty shelves*/
   assert(uniformShelf(b1, 1) == true);
   assert(uniformShelf(b2, 2) == true);
   assert(uniformShelf(b2, 1) == true);
   assert(uniformShelf(b1, 0) == false);
   assert(uniformShelf(b2, 0) == false);

   assert(isBookOnShelf(b1->ary[0], 'R') == true);
   assert(isBookOnShelf(b1->ary[0], 'B') == true);
   assert(isBookOnShelf(b1->ary[0], 'Y') == false);
   assert(isBookOnShelf(b2->ary[2], 'B') == true);

   /*test isolatedColors*/
   strcpy(b2->ary[0], "...");
   /*b2 = "..."
          "R.."
          "B.."
   */
   emptyBooks = (book **) smartCalloc(3, sizeof(book *));
   for (j = 0; j < 3; j++){
      emptyBooks[j] = (book *) smartCalloc(4, sizeof(book));
   }
   strcpy(emptyBooks[0], "...");
   strcpy(emptyBooks[1], "...");
   strcpy(emptyBooks[2], "...");
   emptyBc = createBookcase(emptyBooks, 3, 3);
   assert(isolatedColors(b2) == true);
   assert(isolatedColors(b1) == false);
   assert(isolatedColors(emptyBc) == true);

   assert(isHappy(emptyBc) == true);
   assert(isHappy(b1) == false);
   assert(isHappy(b2) == true);

   assert(exponent(2, 2) == 4);
   assert(exponent(3, 4) == 81);
   assert(exponent(4, 3) == 64);
   assert(exponent(23, 1) == 23);
   assert(exponent(21, 0) == 1);

   /*test creating a layer*/
   y1 = NULL;
   y1 = createLayer(&y1, b3);
   assert(y1->generation == 0);
   y2 = createLayer(&y1, y1->siblings[0]);
   assert(y2->generation == 1);
   assert(y2->size == 6);
   y2->siblings[5] = b2;
   freeLayer(&y1);
   freeBookcase(&b2);
   free(y2->siblings);
   free(y2);

   b2 = moveBook(b1, 0, 1);
   p = createProblem(b2);
   createChildren(&(p.start));
   createChildren(&(p.start->next));
   freeProblem(&p);

   /*for easier tracking: b1 = "RBB"
                               "..."
                               "BR."
   */

   /*try finding solutions various steps away*/
   b2 = moveBook(b1, 0, 1);
   b3 = moveBook(b2, 0, 1);
   easyBc = moveBook(b3, 2, 0);
   p = createProblem(easyBc);
   makeHappy(&p);
   assert(p.moves == 2);
   freeProblem(&p);
   p = createProblem(b3);
   makeHappy(&p);
   assert(p.moves == 3);
   freeProblem(&p);
   p = createProblem(b1);
   makeHappy(&p);
   assert(p.moves == 5);
   assert(isHappy(p.solution) == true);
   assert(isHappy(p.solution->prev) == false);

   /*test colors and solvability*/
   hist = bookcaseHist(p.solution, NUMTYPES);
   assert(hist[bl] == 3);
   assert(hist[rd] == 2);
   free(hist);
   strcpy(b1->ary[1], "YW.");
   hist = bookcaseHist(b1, NUMTYPES);
   assert(hist[ylw] == 1);
   assert(hist[wht] == 1);
   assert(hist[blk] == 0);
   assert(isDoomed(b1, NUMTYPES) == true);
   assert(validColors(b1->ary, 3, 3) == true);
   free(hist);
   strcpy(b1->ary[1], "RRR");
   hist = bookcaseHist(b1, NUMTYPES);
   assert(isDoomed(b1, NUMTYPES) == true);
   assert(validColors(b1->ary, 3, 3) == true);
   free(hist);
   hist = bookcaseHist(b1, NUMTYPES);
   strcpy(b1->ary[1], "...");
   assert(isDoomed(b1, NUMTYPES) == false);
   assert(validColors(b1->ary, 3, 3) == true);

   strcpy(b1->ary[1], "XXX");
   assert(validColors(b1->ary, 3, 3) == false);

   /*test valid widths*/
   assert(isValidWidth(b1->ary, 3, 3) == true);
   assert(isValidWidth(b1->ary, 3, 2) == false);

   /*height will be one fewer than lineCount*/
   assert(isValidHeight(b1->height, 4) == true);
   assert(isValidHeight(b1->height, 3) == false);

   assert(isValid(b1->ary, 3, 3, 4) == false);
   strcpy(b1->ary[1], "RRR");
   assert(isValid(b1->ary, 3, 3, 4) == true);
   assert(isValid(b1->ary, 2, 3, 4) == false);
   assert(isValid(b1->ary, 3, 2, 4) == false);
   assert(isValid(b1->ary, 3, 3, 3) == false);
   assert(isValid(b1->ary, -1, 3, 3) == false);

   assert(bookToColor('k') == black);
   assert(bookToColor('K') == black);
   assert(bookToColor('b') == blue);

   /*freeBookcase(&b1);*/
   free(hist);
   freeBookcase(&b2);
   freeBookcase(&emptyBc);
   freeProblem(&p);
   /*freeLayer(&y2);*/
}

void printSolution(problem p, bool verbose){
   bookcase *current;
   if (p.noSolution == true){
      fprintf(stdout, "No solution?\n");

   }
   else{
      fprintf(stdout, "%d\n\n", p.moves);
      if (verbose == true){
         current = p.solution;
         while(current->prev != NULL){
            printBookcase(current);
            current = current->prev;
         }
         printBookcase(current);
      }
   }
}

void printBookcase(bookcase *bc){
   int j, i;
   for (j = 0; j < bc->height; j++){
      for (i = 0; i < bc->width; i++){
         printBook(bc->ary[j][i]);
      }
      fprintf(stdout, "\n");
   }
   fprintf(stdout, "\n");
}

void printBook(book b){
   neillcol nc;
   nc = bookToColor(b);
   if (nc == black){
      neillbgcol(white);
   }
   neillfgcol(nc);
   fprintf(stdout, "%c", b);
   neillfgcol(white);
   neillbgcol(black);
}

bookcase *readFile(char *filename){
   char **ary, str[BUFFER];
   FILE *fp;
   int height, width, guess, lineCount = 0;
   int size = 0, shelfFactor = MAXSHELVES;
   bookcase *bc;
   ary = (char **)smartCalloc(shelfFactor, sizeof(char *));
   if ((fp = fopen(filename, "r")) == NULL){
      errorQuit("Could not open file...exiting\n");
   }
   while ((fgets(str, BUFFER, fp)) != NULL){
      lineCount++;
      if (lineCount == 1){
         if (sscanf(str, "%d %d %d\n", &height, &width, &guess) != 3){
            if (sscanf(str, "%d %d\n", &height, &width) != 2){
               errorQuit("Could not read parameter from file... exiting\n");
            }
         }
      }
      else{
         if (sscanf(str, "%s\n", str) != 1){
            errorQuit("Could not read shelf of books... exiting\n");
         }
         if (size >= shelfFactor){
            shelfFactor += REALLOC_PLUS;
            ary = realloc(ary, shelfFactor * sizeof(char *));
         }
         ary[size] = (char *)smartCalloc(strlen(str) + 1, sizeof(char));
         strcpy(ary[size++], str);
      }
   }
   if (isValid(ary, height, width, lineCount) == false){
      errorQuit("Invalid file formatting... exiting\n");
   }
   bc = createBookcase(ary, height, width);
   return bc;
}

bool isValid(book **books, int height, int width, int lineCount){
   if (lineCount == 0){
      return false;
   }
   if (height > 9 || width > 9){
      return false;
   }
   if (height < 0 || width < 0){
      return false;
   }
   if (isValidHeight(height, lineCount) == false){
      return false;
   }
   if (isValidWidth(books, height, width) == false){
      return false;
   }
   if (validColors(books, height, width) == false){
      return false;
   }
   return true;
}

bool isValidHeight(int height, int lineCount){
   return height == (lineCount - 1);
}

bool isValidWidth(book **books, unsigned int height, unsigned int width){
   int j;
   for (j = 0; j < height; j++){
      if (strlen(books[j]) != width){
         return false;
      }
   }
   return true;
}

bool validColors(book **books, int height, int width){
   int j, i;
   for (j = 0; j < height; j++){
      for (i = 0; i < width; i++){
         if (bookToIndex(books[j][i]) == -1){
            return false;
         }
      }
   }
   return true;
}

bool isDoomed(bookcase *bc, int numTypes){
   int i, *hist, sum = 0;
   hist = bookcaseHist(bc, numTypes);
   /*ignore empty, so start at black*/
   for (i = blk; i < numTypes; i++){
      if (hist[i] > bc->width){
         free(hist);
         return true;
      }
      if (hist[i] > 0){
         sum += 1;
      }
   }
   if (sum > bc->height){
      free(hist);
      return true;
   }
   free(hist);
   return false;
}

int *bookcaseHist(bookcase *bc, int numTypes){
   int *hist, j, i;
   index n;
   hist = (int *) smartCalloc(numTypes, sizeof(int));
   for (j = 0; j < bc->height; j++){
      for (i = 0; i < bc->width; i++){
         n = bookToIndex(bc->ary[j][i]);
         hist[n] += 1;
      }
   }
   return hist;
}

/*if time to print solution in correct order*/
void flipStructure(bookcase **bc, bookcase *solution);

void makeHappy(problem *p){
   layer *y;
   y = p->start;
   while (y->solution == NULL && y->size < SEARCHMAX){
      createChildren(&y);
      y = y->next;
   }
   if (y->size >= SEARCHMAX){
      p->noSolution = true;
   }
   else{
      p->moves = y->generation + 1;
      p->solution = y->solution;
   }
}

problem createProblem(bookcase *bc){
   layer *y;
   problem p;
   y = NULL;
   y = createLayer(&y, bc);
   p.start = y;
   p.moves = 1;
   return p;
}

bool createChildren(layer **y){
   layer *nextY;
   int j1, j2, i, counter = 0;
   bookcase *bc, **newSibs;
   bc = (*y)->siblings[0];
   nextY = createLayer(y, bc);
   for (i = 0; i < (*y)->size; i++){
      for (j1 = 0; j1 < bc->height; j1++){
         for (j2 = 0; j2 < bc->height; j2++){
            if (j1 != j2 && canMoveBook((*y)->siblings[i], j1, j2) == true){
               nextY->siblings[counter] = moveBook((*y)->siblings[i], j1, j2);
               if (isHappy(nextY->siblings[counter]) == true){
                  nextY->solution = nextY->siblings[counter];
                  nextY->siblings = realloc(nextY->siblings, sizeof(*bc) * ++counter);
                  nextY->size = counter;
                  return true;
               }
               counter++;
            }
         }
      }
   }
   nextY->siblings = realloc(nextY->siblings, sizeof(*bc) * counter);
   nextY->size = counter;
   return false;
}

layer *createLayer(layer **prev, bookcase *prevBc){
   layer *y;
   int genFactor;
   y = smartCalloc(1, sizeof(layer));

   if (*prev == NULL){
      y->siblings = smartCalloc(1, sizeof(bookcase*));
      y->siblings[0] = prevBc;
      y->size = 1;
      y->generation = 0;
      y->next = NULL;
      return y;
   }

   (*prev)->next = y;
   y->generation = (*prev)->generation + 1;
   genFactor = (prevBc->height) * (prevBc->height - 1);
   y->size = exponent(genFactor, y->generation);
   y->siblings = smartCalloc(y->size, sizeof(bookcase*));
   return y;
}

bool isHappy(bookcase *bc){
   int j;
   for (j = 0; j < bc->height; j++){
      if (uniformShelf(bc, j) == false){
         return false;
      }
   }
   if (isolatedColors(bc) == false){
      return false;
   }
   return true;
}

bool isolatedColors(bookcase *bc){
   int shelfLoop1, shelfLoop2, i;
   book b;
   for (shelfLoop1 = 0; shelfLoop1 < bc->height; shelfLoop1++){
      for (i = 0; i < bc->width ; i++){
         b = bc->ary[shelfLoop1][i];
         for (shelfLoop2 = 0; shelfLoop2 < bc->height; shelfLoop2++){
            if (b != '.' && (shelfLoop2 != shelfLoop1)){
               if (isBookOnShelf(bc->ary[shelfLoop2], b) == true){
                  return false;
               }
            }
         }
      }
   }
   return true;
}

bool isBookOnShelf(char *shelf, book b){
   int i = 0;
   while (shelf[i] != '\0'){
      if(shelf[i] == b){
         return true;
      }
      i++;
   }
   return false;
}

bool uniformShelf(bookcase *bc, int shelf){
   int i = 0;
   char c, curBook;
   c = curBook = bc->ary[shelf][i];
   while (curBook != EMPTY && curBook != '\0'){
      if (c != curBook){
         return false;
      }
      i++;
      curBook = bc->ary[shelf][i];
   }
   return true;
}

bookcase *moveBook(bookcase *bc, int oldShelf, int newShelf){
   bookcase *newBc;
   char **newAry;
   int j, oldBook, newSlot;
   newAry = (char **)smartCalloc(bc->height, sizeof(char *));
   for (j = 0; j < bc->height; j++){
      newAry[j] = (char *)smartCalloc(bc->width + 1, sizeof(char));
      strcpy(newAry[j], bc->ary[j]);
   }
   oldBook = numBooks(bc, oldShelf) - 1;
   newSlot = numBooks(bc, newShelf);
   if (oldBook < 0 || newSlot < 0){
      errorQuit("Cannot move book...exiting");
   }
   newAry[newShelf][newSlot] = newAry[oldShelf][oldBook];
   newAry[oldShelf][oldBook] = EMPTY;
   newBc = createBookcase(newAry, bc->height, bc->width);
   newBc->prev = bc;
   return newBc;
}

bool canMoveBook(bookcase *bc, int oldShelf, int newShelf){
   if (shelfEmpty(bc, oldShelf)){
      return false;
   }
   if (shelfFull(bc, newShelf)){
      return false;
   }
   return true;
}

int numBooks(bookcase *bc, int shelf){
   int i = 0;
   char c;
   do {
      c = bc->ary[shelf][i];
      if (c == EMPTY){
         return i;
      }
      i++;
   } while (i < bc->width);
   return i;
}

bool shelfEmpty(bookcase *bc, int shelf){
   if (bc->ary[shelf][0] == EMPTY){
      return true;
   }
   return false;
}

bool shelfFull(bookcase *bc, int shelf){
   int i = 0;
   char c;
   do {
      c = bc->ary[shelf][i];
      if (c == EMPTY){
         return false;
      }
      i++;
   } while (i < bc->width);
   return true;
}

bookcase *createBookcase(char **ary, int height, int width){
   bookcase *bc = (bookcase *) smartCalloc(1, sizeof(bookcase));
   bc->ary = ary;
   bc->height = height;
   bc->width = width;
   bc->prev = NULL;
   return bc;
}

void freeProblem(problem *p){
   layer *tmp, *current;
   current = (*p).start;
   while (current->next != NULL){
      tmp = current->next;
      freeLayer(&current);
      current = tmp;
   }
   freeLayer(&current);
}

void freeLayer(layer **y){
   int i;
   for (i = 0; i < (*y)->size; i++){
      freeBookcase(&((*y)->siblings[i]));
   }
   free((*y)->siblings);
   free(*y);
}

void freeBookcase(bookcase **bc){
   int j;
   for (j = 0; j < (*bc)->height; j++){
      free((*bc)->ary[j]);
   }
   free((*bc)->ary);
   free(*bc);
}

void *smartCalloc(int quant, int size){
   void *v = calloc(quant, size);
   if (v == NULL){
      errorQuit("Couldn't allocate memory...exiting");
   }
   return v;
}

void errorQuit(char *message){
   fprintf(stderr, "%s\n", message);
   exit(EXIT_FAILURE);
}

index bookToIndex(book b){
   b = toupper(b);
   switch(b){
      case 'K':
         return blk;
      case 'R':
         return rd;
      case 'G':
         return grn;
      case 'Y':
         return ylw;
      case 'B':
         return bl;
      case 'M':
         return mgnt;
      case 'C':
         return cyn;
      case 'W':
         return wht;
      case '.':
         return empty;
      default: /*exit here instead of is valid?*/
         return -1;
   }
}

neillcol bookToColor(book b){
   b = toupper(b);
   switch(b){
      case 'K':
         return black;
      case 'R':
         return red;
      case 'G':
         return green;
      case 'Y':
         return yellow;
      case 'B':
         return blue;
      case 'M':
         return magenta;
      case 'C':
         return cyan;
      default:
         return white;
   }
}

int exponent(int val, int exp){
   int i, solution = 1;
   if (exp == 0){
      return 1;
   }
   for (i = exp; i > 0; i--){
      solution *= val;
   }
   return solution;
}
