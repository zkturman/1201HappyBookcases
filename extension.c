#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "neillsimplescreen.h"

#define BOOKDIMEN 4
#define WAITTIME 1.5
#define EMPTY '.'
#define BLANKCELL ' '
#define NUMTYPES 9
#define BUFFER 50
#define MAXPARAMS 3
#define MINPARAMS 2
#define MAXSHELVES 9
#define REALLOC_PLUS 5
#define ARRAYCAP 7500000
#define SEARCHMAX 24
#define SEARCHRESET 6

enum index {invalid = -1, empty, blk, rd, grn, ylw, bl, mgnt, cyn, wht};
typedef enum index index;

enum bool {false, true};
typedef enum bool bool;

typedef char book;

struct bookcase {
   book **shelves;
   int height;
   int width;
   book *destination;
   book score;
   book final;
   struct bookcase *prev;
   struct bookcase *next;
};
typedef struct bookcase bookcase;

struct layer{
   bookcase **siblings;
   bookcase *solution;
   int size;
   int generation;
   struct layer *next;
};
typedef struct layer layer;

struct problem {
   layer *start;
   bookcase *solution;
   bookcase *puzzle;
   bool noSolution;
   int moves;
};
typedef struct problem problem;

/*Prints the number of moves needed to make a bookcase happy. The count starts
from 1. E.g., if no moves are needed to make a bookcase happy, the number 1 will
be printed. When verbose is true, the moves from start to solution will be
printed to screen.*/
void printSolution(problem p, bool verbose);

/*Prints the steps taken for the solution*/
void printVerbose(problem p);

/*Prints the contents of a bookcase to screen using neillsimplescreen for
coloring. A blank line is printed at the end of each bookcase.*/
void printBookcase(bookcase *bc);

/*Prints a horizontal border for a bookcase color white*/
void printCaseHorizontalBorder(int width);

/*Prints a horizontal border for all books on a shelf of color black*/
void printBookHorizontalBorder(int width);

/*Prints a horizontal border of a specified color for a width that is plus two
to include corners of a box. The line prints to the terminal window.*/
void printHorizontalBorder(int width, neillcol n);

/*Prints a blank cell of a specified color to the terminal window*/
void printBlankCell(neillcol n);

/*Uses neillsimplescreen to print a book with its corresponding color.
For black books, the background color is changed to white.*/
void printBook(book b);

/*Converts the contents of a .bc file into a bookcase. The function returns a
bookcase if the file is correctly formatted. Otherwise, the program will exit.*/
bookcase *readFile(const char *filename);

/*Reads the lines of a bookshelf in a bookcase file and assigns them to an
array of books. Side effects: sets books, size, and shelfFactor.*/
void scanShelves(char *str, book ***books, int *size, int *shelfFactor);

/*Reads topline of a bookcase file and verifies the right number of inputs.
Side effects: sets height, width, and guess*/
void scanParameters(char *str, int *height, int *width, int *guess);

/*Generates a filestream and handles any errors opening the file.*/
FILE *smartFileOpen(const char *filename);

/*Returns true if a file has valid formatting. The line count should be greater
than zero. The height and weight should each fall between 0 and 9. There should
be no books after an EMPTY gap. The parameters of the books should match those
specified in height and width.*/
bool isValid(book **books, int height, int width, int lineCount);

/*Returns true if books never follow EMPTY gaps for any shelf in an array*/
bool mindTheGap(book **books, int height, int width);

/*Returns true if a given height matches a file's line count minus one*/
bool isValidHeight(int height, int lineCount);

/*Returns true if all shelves of books in an array match a given width*/
bool isValidWidth(book **books, int height, int width);

/*Returns true if all books in an array are defined in enum index*/
bool validColors(book **books, int height, int width);

/*Returns true if this bookcase cannot be made happy*/
bool isDoomed(bookcase *bc, int numTypes);

/*Generates a histogram for each type of book present on a bookcase*/
int *bookcaseHist(bookcase *bc, int numTypes);

/*Builds solution steps in reverse to make a doubly linked list*/
bookcase *buildReverse(bookcase **bc);

/*Uses a breadth-first search to generate a happy bookcase. The search continues
until a solution is found or the steps taken meets SEARCHRESET. Then a best
bookcase is used as an intermediate starting point to continue searching. The
search will quit if SEARCHMAX steps are made.*/
void makeHappy(problem *p);

/*Returns the first bookcase found with the smallest difference between its
score and final*/
bookcase *findBestSibling(layer *y, int final);

/*Resets the siblings of a layer to only the input bookcase. All other siblings
are freed.*/
void resetBreadth(layer **y, bookcase *bc);

/*Initializes a problem using a bookcase. Generates a layer used as the start
when generating a solution for a bookcase.*/
problem createProblem(bookcase *bc);

/*Generates all possible children moves from the siblings inside a layer. The
function checks for valid moves and quits if a solution is found.*/
bool createChildren(layer **y);

/*Initializes a layer pointer using an input bookcase*/
layer *createLayer(layer **prev, bookcase *bc);

/*Returns true if each shelf has a single color and if any given color is only
found on a single shelf*/
bool isHappy(bookcase *bc);

/*Returns true if each color only occurs on a single shelf*/
bool isolatedColors(bookcase *bc);

/*Returns true if the specified book exists on the shelf*/
bool isBookOnShelf(book *shelf, book b);

/*Returns true if all books on the shelf are equal OR if the shelf is empty*/
bool uniformShelf(bookcase *bc, int shelf);

/*Returns a new bookcase with the last book on the old shelf moved to the
first available position on the new shelf*/
bookcase *moveBook(bookcase *bc, int oldShelf, int newShelf);

/*Returns true for the following conditions:*/
/*oldShelf is non-empty, containing something other than '.'*/
/*newShelf is non-full, containing at least one '.'*/
bool canMoveBook(bookcase *bc, int oldShelf, int newShelf);

/*Returns the number of books on a specified shelf*/
int numBooks(bookcase *bc, int shelf);

/*Determines if a shelf for a bookcase is empty. Empty means there
are only empty spaces('.') on the shelf.*/
bool shelfEmpty(bookcase *bc, int shelf);

/*Determines if a given shelf for a bookcase is full. Full means there are
no empty spaces('.') on the shelf.*/
bool shelfFull(bookcase *bc, int shelf);

/*Initializes a bookcase pointer using an input ary that is height x width*/
bookcase *createBookcase(char **ary, int height, int width);

/*Returns the total number of books in bookcase*/
int createFinalScore(bookcase *bc);

/*Returns the number of books in the correct position for a solution*/
int createCurrentScore(bookcase *bc);

/*Creates a solution map of colors. Each map index corresponds to a shelf in
a bookcase. The values in a map indicate the color of each shelf in a solution.
First, it picks colors in each shelf's first gap, then loops for the rest.*/
book *createDestinationMap(book **books, int height, int width);

/*Returns true if a book is already in the solution map*/
bool inDestination(book *map, int size, book b);

/*Frees allocated memory for a struct pyramid*/
void freeProblem(problem *p);

/*Frees allocated memory for a struct layer*/
void freeLayer(layer **y);

/*Frees allocated memory for a struct bookcase*/
void freeBookcase(bookcase **bc);

/*Allocates memory or exits the program with an error if the requested
memory couldn't be allocated*/
void *smartCalloc(int quant, int size);

/*Prints a string to stderr, and exits the program*/
void errorQuit(char *message);

/*Uses an enum to index the different types of books. Note, weird names are used
 to avoid conflicts with neillcol.*/
index bookToIndex(book b);

/*Used to get foreground color based on a type of book*/
neillcol bookToColor(book b);

/*Returns a value raised to a specified exponent*/
int exponent(int val, int exp);

/*Tests all non-printing and non-scanning functions*/
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
   freeProblem(&p);
   return 0;
}

void printSolution(problem p, bool verbose){
   if (p.noSolution == true){
      fprintf(stdout, "No solution?\n\n");
   }
   else{
      if (verbose == true){
         printVerbose(p);
      }
   }
}

void printVerbose(problem p){
   bookcase *current;
   int moves = 1;
   current = p.puzzle;
   neillclrscrn();
   while(current != NULL){
      neillcursorhome();
      fprintf(stdout, "Total moves: %d\n", moves++);
      printBookcase(current);
      current = current->next;
      neillbusywait(WAITTIME);
   }
   neillreset();
}

void printBookcase(bookcase *bc){
   int shelf, gap, i;
   printCaseHorizontalBorder(bc->width * BOOKDIMEN);
   for (shelf = 0; shelf < bc->height; shelf++){
      printBookHorizontalBorder(bc->width * BOOKDIMEN);
      for (i = 0; i < BOOKDIMEN; i++){
         printBlankCell(white);
         for (gap = 0; gap < bc->width; gap++){
            printBook(bc->shelves[shelf][gap]);
         }
         printBlankCell(white);
         fprintf(stdout, "\n");
      }
      printBookHorizontalBorder(bc->width * BOOKDIMEN);
   }
   printCaseHorizontalBorder(bc->width * BOOKDIMEN);
   fprintf(stdout, "\n");

}

void printCaseHorizontalBorder(int width){
   printBlankCell(white);
   printHorizontalBorder(width, white);
   printBlankCell(white);
   fprintf(stdout, "\n");
}

void printBookHorizontalBorder(int width){
   printBlankCell(white);
   printHorizontalBorder(width, black);
   printBlankCell(white);
   fprintf(stdout, "\n");
}

void printHorizontalBorder(int width, neillcol n){
   int i;
   for (i = 0; i < width; i++){
      printBlankCell(n);
   }
}

void printBook(book b){
   neillcol nc;
   int i;
   nc = bookToColor(b);
   printBlankCell(black);
   if (nc == black){
      neillbgcol(white);
   }
   neillfgcol(nc);
   for (i = 1; i < BOOKDIMEN - 1; i++){
      fprintf(stdout, "%c", b);
   }
   printBlankCell(black);
   neillfgcol(white);
   neillbgcol(black);
}

void printBlankCell(neillcol n){
   neillbgcol(n);
   fprintf(stdout, "%c", BLANKCELL);
   neillbgcol(black);
}

bookcase *readFile(const char *filename){
   book **books;
   char str[BUFFER];
   FILE *fp;
   int shelves, gaps, guess, lineCount = 0;
   int size = 0, shelfFactor = MAXSHELVES;
   bookcase *bc;
   books = (book **)smartCalloc(shelfFactor, sizeof(char *));
   fp = smartFileOpen(filename);
   while ((fgets(str, BUFFER, fp)) != NULL){
      lineCount++;
      if (lineCount == 1){
         scanParameters(str, &shelves, &gaps, &guess);
      }
      else{
         scanShelves(str, &books, &size, &shelfFactor);
      }
   }
   if (isValid(books, shelves, gaps, lineCount) == false){
      errorQuit("Invalid file formatting... exiting\n");
   }
   bc = createBookcase(books, shelves, gaps);
   fclose(fp);
   return bc;
}

void scanShelves(char *str, book ***books, int *size, int *shelfFactor){
   if (sscanf(str, "%s\n", str) != 1){
      errorQuit("Could not read shelf of books... exiting\n");
   }
   if (*size >= *shelfFactor){
      *shelfFactor += REALLOC_PLUS;
      *books = realloc(*books, *shelfFactor * sizeof(char *));
   }
   (*books)[*size] = (book *)smartCalloc(strlen(str) + 1, sizeof(char));
   strcpy((*books)[(*size)++], str);
}

void scanParameters(char *str, int *height, int *width, int *guess){
   if (sscanf(str, "%d %d %d\n", height, width, guess) != MAXPARAMS){
      if (sscanf(str, "%d %d\n", height, width) != MINPARAMS){
         errorQuit("Could not read parameters from file... exiting\n");
      }
   }
}

FILE *smartFileOpen(const char *filename){
   FILE *fp;
   if ((fp = fopen(filename, "r")) == NULL){
      errorQuit("Could not open file...exiting\n");
   }
   return fp;
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
   if (mindTheGap(books, height, width) == false){
      return false;
   }
   return true;
}

bool mindTheGap(book **books, int height, int width){
   int shelf, gap;
   bool atEnd = false;
   for (shelf = 0; shelf < height; shelf++){
      for (gap = 0; gap < width; gap++){
         if (books[shelf][gap] == EMPTY){
            atEnd = true;
         }
         else{
            if (atEnd == true){
               return false;
            }
         }
      }
      atEnd = false;
   }
   return true;
}

bool isValidHeight(int height, int lineCount){
   return height == (lineCount - 1);
}

bool isValidWidth(book **books, int height, int width){
   int shelf;
   for (shelf = 0; shelf < height; shelf++){
      if ((int)strlen(books[shelf]) != width){
         return false;
      }
   }
   return true;
}

bool validColors(book **books, int height, int width){
   int shelf, gap;
   for (shelf = 0; shelf < height; shelf++){
      for (gap = 0; gap < width; gap++){
         if (bookToIndex(books[shelf][gap]) == invalid){
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
   int *hist, shelf, gap;
   index n;
   hist = (int *) smartCalloc(numTypes, sizeof(int));
   for (shelf = 0; shelf < bc->height; shelf++){
      for (gap = 0; gap < bc->width; gap++){
         n = bookToIndex(bc->shelves[shelf][gap]);
         hist[n] += 1;
      }
   }
   return hist;
}

bookcase *buildReverse(bookcase **bc){
   bookcase *current, *tmp;
   current = *bc;
   while (current->prev != NULL){
      tmp = current;
      current = current->prev;
      current->next = tmp;
   }
   return current;
}

void makeHappy(problem *p){
   layer *y;
   bookcase *bc, *bestBookCase;
   int counter = 0;
   y = p->start;
   bc = y->siblings[0];
   if (isDoomed(bc, NUMTYPES)){
      p->noSolution = true;
      return;
   }
   else if (isHappy(bc)){
      y->solution = bc;
   }
   else{
      while (y->solution == NULL && counter < SEARCHMAX){
         createChildren(&y);
         y = y->next;
         counter++;
         if (counter % SEARCHRESET == 0 || y->size > ARRAYCAP){
            bestBookCase = findBestSibling(y, bc->final);
            resetBreadth(&y, bestBookCase);
         }
      }
   }
   if (counter >= SEARCHMAX){
      p->noSolution = true;
   }
   else{
      p->moves = counter + 1;
      p->solution = y->solution;
      p->puzzle = buildReverse(&p->solution);
   }
}

bookcase *findBestSibling(layer *y, int final){
   int min, i = 0, curScore;
   bookcase *minBc;
   minBc = y->siblings[i];
   min = final - minBc->score;
   for (i = 0; i < y->size; i++){
      curScore = y->siblings[i]->score;
      if (final - curScore < min){
         minBc = y->siblings[i];
      }
   }
   return minBc;
}

void resetBreadth(layer **y, bookcase *bc){
   int i;
   for (i = 0; i < (*y)->size; i++){
      if ((*y)->siblings[i] != bc){
         freeBookcase(&(*y)->siblings[i]);
      }
   }
   (*y)->siblings[0] = bc;
   (*y)->siblings = realloc((*y)->siblings, sizeof(bookcase *));
   (*y)->size = 1;
   (*y)->generation = 0;
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
   int shelfLoop1, shelfLoop2, i, counter = 0;
   bookcase *bc, *newSib;
   bc = (*y)->siblings[counter];
   nextY = createLayer(y, bc);
   for (i = 0; i < (*y)->size; i++){
      for (shelfLoop1 = 0; shelfLoop1 < bc->height; shelfLoop1++){
         for (shelfLoop2 = 0; shelfLoop2 < bc->height; shelfLoop2++){
            if (shelfLoop1 != shelfLoop2
               && canMoveBook((*y)->siblings[i], shelfLoop1, shelfLoop2)){
               newSib = moveBook((*y)->siblings[i], shelfLoop1, shelfLoop2);
               nextY->siblings[counter] = newSib;
               if (isHappy(newSib)){
                  nextY->solution = newSib;
                  counter++;
                  nextY->siblings = realloc(nextY->siblings, sizeof(bookcase *) * counter);
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
   int shelf;
   for (shelf = 0; shelf < bc->height; shelf++){
      if (uniformShelf(bc, shelf) == false){
         return false;
      }
   }
   if (isolatedColors(bc) == false){
      return false;
   }
   return true;
}

bool isolatedColors(bookcase *bc){
   int shelfLoop1, shelfLoop2, gap;
   book b;
   for (shelfLoop1 = 0; shelfLoop1 < bc->height; shelfLoop1++){
      for (gap = 0; gap < bc->width; gap++){
         b = bc->shelves[shelfLoop1][gap];
         for (shelfLoop2 = 0; shelfLoop2 < bc->height; shelfLoop2++){
            if (b != '.' && (shelfLoop2 != shelfLoop1)){
               if (isBookOnShelf(bc->shelves[shelfLoop2], b) == true){
                  return false;
               }
            }
         }
      }
   }
   return true;
}

bool isBookOnShelf(book *shelf, book b){
   int gap = 0;
   while (shelf[gap] != '\0'){
      if(shelf[gap] == b){
         return true;
      }
      gap++;
   }
   return false;
}

bool uniformShelf(bookcase *bc, int shelf){
   int gap = 0;
   book b, curBook;
   b = curBook = bc->shelves[shelf][gap];
   while (curBook != EMPTY && curBook != '\0'){
      if ( b!= curBook){
         return false;
      }
      gap++;
      curBook = bc->shelves[shelf][gap];
   }
   return true;
}

bookcase *moveBook(bookcase *bc, int oldShelf, int newShelf){
   bookcase *newBc;
   char **newShelves;
   int curShelf, oldBook, newGap;
   newShelves = (char **)smartCalloc(bc->height, sizeof(char *));
   for (curShelf = 0; curShelf < bc->height; curShelf++){
      newShelves[curShelf] = (char *)smartCalloc(bc->width + 1, sizeof(char));
      strcpy(newShelves[curShelf], bc->shelves[curShelf]);
   }
   oldBook = numBooks(bc, oldShelf) - 1;
   newGap = numBooks(bc, newShelf);
   if (oldBook < 0 || newGap < 0){
      errorQuit("Cannot move book...exiting");
   }
   newShelves[newShelf][newGap] = newShelves[oldShelf][oldBook];
   newShelves[oldShelf][oldBook] = EMPTY;
   newBc = createBookcase(newShelves, bc->height, bc->width);
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
   int gap = 0;
   book b;
   do {
      b = bc->shelves[shelf][gap];
      if (b == EMPTY){
         return gap;
      }
      gap++;
   } while (gap < bc->width);
   return gap;
}

bool shelfEmpty(bookcase *bc, int shelf){
   if (bc->shelves[shelf][0] == EMPTY){
      return true;
   }
   return false;
}

bool shelfFull(bookcase *bc, int shelf){
   int gap = 0;
   book b;
   do {
      b = bc->shelves[shelf][gap];
      if (b == EMPTY){
         return false;
      }
      gap++;
   } while (gap < bc->width);
   return true;
}

bookcase *createBookcase(book **books, int height, int width){
   bookcase *bc = (bookcase *) smartCalloc(1, sizeof(bookcase));
   bc->shelves = books;
   bc->height = height;
   bc->width = width;
   bc->destination = createDestinationMap(books, height, width);
   bc->score = createCurrentScore(bc);
   bc->final = createFinalScore(bc);
   bc->prev = NULL;
   return bc;
}

int createCurrentScore(bookcase *bc){
   int shelf, gap;
   int score = 0;
   for (shelf = 0; shelf < bc->height; shelf++){
      for (gap = 0; gap < bc->width; gap++){
         if (bc->destination[shelf] == bc->shelves[shelf][gap]){
            score += 1;
         }
      }
   }
   return score;
}

int createFinalScore(bookcase *bc){
   int i, shelf, gap;
   int final = 0;
   for (i = 0; i < bc->height; i++){
      for (shelf = 0; shelf < bc->height; shelf++){
         for (gap = 0; gap < bc->width; gap++){
            if (bc->destination[i] == bc->shelves[shelf][gap]){
               final += 1;
            }
         }
      }
   }
   return final;
}

book *createDestinationMap(book **books, int height, int width){
   book *map, b;
   int shelf, i, gap = 0;
   map = (book *)smartCalloc(height, sizeof(book));
   for(shelf = 0; shelf < height; shelf++){
      if (isalpha(books[shelf][gap]) != 0){
         if (inDestination(map, height, books[shelf][gap]) == false){
            map[shelf] = books[shelf][gap];
         }
      }
   }
   for(shelf = 0; shelf < height; shelf++){
      for (gap = 0; gap < width; gap++){
         b = books[shelf][gap];
         if (inDestination(map, height, b) == false
            && isalpha(b) != 0){
            for (i = 0; i < height; i++){
               if (isalpha(map[i]) != 0){
                  map[i] = b;
               }
            }
         }
      }
   }
   return map;
}

bool inDestination(book *map, int size, book b){
   int i;
   b = toupper(b);
   for (i = 0; i < size; i++){
      if (map[i] == b){
         return true;
      }
   }
   return false;
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
   int shelf;
   for (shelf = 0; shelf < (*bc)->height; shelf++){
      free((*bc)->shelves[shelf]);
   }
   free((*bc)->shelves);
   free((*bc)->destination);
   free(*bc);
}

void *smartCalloc(int quant, int size){
   void *block = calloc(quant, size);
   if (block == NULL){
      errorQuit("Couldn't allocate memory...exiting");
   }
   return block;
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
      default:
         return invalid;
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

void test(){
   book **aryTest1, **emptyBooks, *destMap;
   bookcase *b1, *b2, *b3, *b4, *easyBc, *emptyBc;
   layer *y1, *y2;
   problem p;
   int j, score, *hist;
   assert(bookToIndex('c') == cyn);
   assert(bookToIndex('b') == bl);
   assert(bookToIndex('B') == bl);
   assert(bookToIndex('k') == blk);
   assert(bookToIndex('.') == empty);
   assert(bookToIndex(' ') == invalid);

   aryTest1 = (book **) smartCalloc(3, sizeof(book *));
   /*should quit instead if smartCalloc fails*/
   assert(aryTest1 != NULL);

   for (j = 0; j < 3; j++){
      aryTest1[j] = (book *) smartCalloc(4, sizeof(book));
   }
   strcpy(aryTest1[0], "RB.");
   strcpy(aryTest1[1], "Y..");
   strcpy(aryTest1[2], "BR.");

   destMap = (book *)smartCalloc(4, sizeof(book));
   strcpy(destMap, "BY.");
   assert(inDestination(destMap, 3, 'B') == true);
   assert(inDestination(destMap, 3, 'b') == true);
   assert(inDestination(destMap, 3, 'Z') == false);
   assert(inDestination(destMap, 3, '.') == true);
   free(destMap);
   destMap = createDestinationMap(aryTest1, 3, 3);
   assert(destMap[0] == 'R');
   assert(destMap[1] == 'Y');
   assert(destMap[2] == 'B');

   b1 = createBookcase(aryTest1, 3, 3);
   assert(strcmp(b1->shelves[0], aryTest1[0]) == 0);
   assert(b1->height == 3);
   assert(b1->width == 3);
   score = createCurrentScore(b1);
   assert(score = 3);
   score = createFinalScore(b1);
   assert(score == 5);

   b1->shelves[0][2] = 'B';
   assert(shelfFull(b1, 0) == true);
   assert(shelfFull(b1, 1) == false);

   b1->shelves[1][0] = '.';
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
   assert(strcmp(b2->shelves[2], "B..") == 0);
   assert(strcmp(b2->shelves[1], "R..") == 0);
   b3 = moveBook(b2, 1, 2);
   assert(strcmp(b1->shelves[1], b3->shelves[1]) == 0);
   assert(strcmp(b1->shelves[2], b3->shelves[2]) == 0);

   /*make sure uniform shelves return true, including empty shelves*/
   assert(uniformShelf(b1, 1) == true);
   assert(uniformShelf(b2, 2) == true);
   assert(uniformShelf(b2, 1) == true);
   assert(uniformShelf(b1, 0) == false);
   assert(uniformShelf(b2, 0) == false);

   assert(isBookOnShelf(b1->shelves[0], 'R') == true);
   assert(isBookOnShelf(b1->shelves[0], 'B') == true);
   assert(isBookOnShelf(b1->shelves[0], 'Y') == false);
   assert(isBookOnShelf(b2->shelves[2], 'B') == true);

   /*test isolatedColors*/
   strcpy(b2->shelves[0], "...");
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
   assert(p.start->next->size == 6);
   resetBreadth(&(p.start), b2);
   freeProblem(&p);

   /*for easier tracking: b1 = "RBB"
                               "..."
                               "BR."
   */
   assert(b1->score == 3);
   /*try finding solutions various steps away*/
   b2 = moveBook(b1, 0, 1);
   b3 = moveBook(b2, 0, 1);
   easyBc = moveBook(b3, 2, 0);
   assert(easyBc->score == 4);
   buildReverse(&easyBc);
   /*testing chain rebuilt backwards*/
   assert(strcmp(b3->shelves[0], easyBc->prev->shelves[0]) == 0);
   assert(strcmp(b3->shelves[1], easyBc->prev->shelves[1]) == 0);
   assert(strcmp(b3->shelves[2], easyBc->prev->shelves[2]) == 0);
   p = createProblem(easyBc);
   p.puzzle = buildReverse(&easyBc);
   assert(p.puzzle == b1);
   makeHappy(&p);
   assert(p.moves >= 2);
   y1 = p.start->next;
   b4 = findBestSibling(y1, easyBc->final);
   assert(b4->score == easyBc->final);

   freeProblem(&p);
   p = createProblem(b3);
   makeHappy(&p);
   assert(p.moves >= 3);
   freeProblem(&p);
   p = createProblem(b1);
   makeHappy(&p);
   assert(p.moves >= 5);
   assert(isHappy(p.solution) == true);
   assert(isHappy(p.solution->prev) == false);

   /*test colors and solvability*/
   hist = bookcaseHist(p.solution, NUMTYPES);
   assert(hist[bl] == 3);
   assert(hist[rd] == 2);
   free(hist);
   strcpy(b1->shelves[1], "YW.");
   hist = bookcaseHist(b1, NUMTYPES);
   assert(hist[ylw] == 1);
   assert(hist[wht] == 1);
   assert(hist[blk] == 0);
   assert(isDoomed(b1, NUMTYPES) == true);
   assert(validColors(b1->shelves, 3, 3) == true);
   free(hist);
   strcpy(b1->shelves[1], "RRR");
   hist = bookcaseHist(b1, NUMTYPES);
   assert(isDoomed(b1, NUMTYPES) == true);
   assert(validColors(b1->shelves, 3, 3) == true);
   free(hist);
   hist = bookcaseHist(b1, NUMTYPES);
   strcpy(b1->shelves[1], "...");
   assert(isDoomed(b1, NUMTYPES) == false);
   assert(validColors(b1->shelves, 3, 3) == true);

   strcpy(b1->shelves[1], "XXX");
   assert(validColors(b1->shelves, 3, 3) == false);

   /*test valid widths*/
   assert(isValidWidth(b1->shelves, 3, 3) == true);
   assert(isValidWidth(b1->shelves, 3, 2) == false);

   /*height will be one fewer than lineCount*/
   assert(isValidHeight(b1->height, 4) == true);
   assert(isValidHeight(b1->height, 3) == false);

   strcpy(b1->shelves[1], "R.R");
   assert(mindTheGap(b1->shelves, 3, 3) == false);
   strcpy(b1->shelves[1], "RXR");
   assert(mindTheGap(b1->shelves, 3, 3) == true);

   assert(isValid(b1->shelves, 3, 3, 4) == false);
   strcpy(b1->shelves[1], "RRR");
   assert(isValid(b1->shelves, 3, 3, 4) == true);
   assert(isValid(b1->shelves, 2, 3, 4) == false);
   assert(isValid(b1->shelves, 3, 2, 4) == false);
   assert(isValid(b1->shelves, 3, 3, 3) == false);
   assert(isValid(b1->shelves, -1, 3, 3) == false);

   assert(bookToColor('k') == black);
   assert(bookToColor('K') == black);
   assert(bookToColor('b') == blue);

   free(destMap);
   free(hist);
   freeBookcase(&b2);
   freeBookcase(&emptyBc);
   freeProblem(&p);
}
