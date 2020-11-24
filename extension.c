void createDestinationMap(bookcase **bc){
   int shelf, slot, i, start = 0;
   book *dest, b;
   dest = (book *)smartCalloc((*bc)->height, sizeof(book));
   for (shelf = 0; shelf < (*bc)->height; shelf++){
      b = bc[shelf][start];
      if (inDest(bc, b) == false){
         dest[shelf] = b;
      }
   }
   for (shelf = 0; shelf < (*bc)->height; shelf++){
      for (slot = 0; slot < (*bc)->width; slot++){
         b = bc[shelf][slot];
         if (inDest(bc, b) == false){
            for (i = 0; i < (*bc)->width){
               if (isalpha(dest[i]) == false){
                  dest[i] = b;
               }
            }
         }
      }
   }
   (*bc)->destination = dest;
}

bool inDest(bookcase *bc, book b){
   int i;
   for (i = 0; i < bc->height; i++){
      if (bc->destination[i] == b){
         return true;
      }
   }
   return false;
}

void createFinalScore(bookcase **bc){
   int shelf, slot, i;
   int *final;
   book b;
   final = (int *)smartCalloc((*bc)->height, sizeof(int));
   for (shelf = 0; shelf < (*bc)->height; shelf++){
      for (slot = 0; slot < (*bc)->width; slot++){
         for (i = 0; i < (*bc)->height; i++){
            b = (*bc)->shelves[shelf][slot];
            if ((*bc)->destination[i] == b){
               final[i] += 1;
            }
         }
      }
   }
   (*bc)->final = final;
}

void createCurrentScore(bookcase **bc){
   int shelf, slot = 0, score = 0;
   int *scoreAry;
   scoreAry = (int *)smartCalloc((*bc)->height, sizeof(int));
   for (shelf = 0; shelf < (*bc)->height; shelf++){
      while ((*bc)->destination[shelf] == (*bc)->shelves[shelf][slot]){
            scoreary[shelf] += 1;
      }
   }
}

int *getNextScore(const bookcase *bc, const int oldShelf, const int newShelf){
   int *next;
   for (shelf = 0; shelf < b->height; shelf++){
      next[shelf] = b->score[shelf];
   }
   if (bc->destination[newShelf] == bc->ary[oldShelf][numBooks(bc, oldShelf) - 1]);
      next[newShelf] += 1;
   }
   return next;
}

void storeMove(bookcase **bc, const int oldShelf, const int newShelf){
   (*bc)->prevSource[0] = oldShelf;
   (*bc)->prevSource[1] = numBooks(bc, oldShelf);
   (*bc)->prevDest[0] = newShelf;
   (*bc)->prevDest[1] = numBooks(bc, newShelf) - 1;
}

bookcase *smartMove(bookcase *bc){
   int oldShelf, newShelf;
   bookcase *newBc;
   if (leadsToFinal(bc, &oldShelf, &newShelf)){
      newBc = moveBook(bc, oldShelf, newShelf);
   }
   else if (leadsToIncrease(bc, &oldShelf, &newShelf)){
      newBc = moveBook(bc, oldShelf, newShelf);
   }
   else if (canMoveToZeroCorrect(bc, &oldShelf, &newShelf)){
      newBc = moveBook(bc, oldShelf, newShelf);
   }
   else if (canMoveToComplete(bc, &oldShelf, &newShelf)){
      newBc = moveBook(bc, oldShelf, newShelf);
   }
   else if (canMoveToPartial(bc, &oldShelf, &newShelf)){
      newBc = moveBook(bc, oldShelf, newShelf);
   }
   else{
      newBc = moveUnhappy(bc, oldShelf, newShelf);
   }
   storeMove(newBc, oldShelf, newShelf);
   return newBc;
}

bool leadsToFinal(bookcase *bc, int *src, int *dest){
   int *score;
   for (oldShelf = 0; oldShelf < bc->height; oldShelf++){
      for (newShelf = 0; newShelf < bc->height; newShelf++){
         if (oldShelf != newShelf){
            if (canMoveBook(bc, oldShelf, newShelf) == true){
               score = getNextScore(bc, oldShelf, newShelf);
               if (x[newShelf] == bc->destScore[newShelf]){
                  *src = oldShelf;
                  *dst = newShelf;
                  return true;
               }
            }
         }
      }
   }
   return false;
}

bool leadsToIncrease(bookcase *bc, int *src, int *dest){
   int *score, oldShelf, newShelf;
   for (oldShelf = 0; oldShelf < bc->height; oldShelf++){
      for (newShelf = 0; newShelf < bc->height; newShelf++){
         if (oldShelf != newShelf){
            if (canMoveBook(bc, oldShelf, newShelf) == true){
               score = getNextScore(bc, oldShelf, newShelf);
               if (x[newShelf] > bc->score[newShelf]){
                  *src = oldShelf;
                  *dst = newShelf;
                  return true;
               }
            }
         }
      }
   }
   return false;
}

bool canMoveToZeroCorrect(bookcase *bc, int *src, int *dest){
   int oldShelf, newShelf;
      for (oldShelf = 0; oldShelf < bc->height; oldShelf++){
         for (newShelf = 0; newShelf < bc->height; newShelf++){
            if (oldShelf != newShelf){
               if (canMoveBook(bc, oldShelf, newShelf) == true){
                  if (bc->score[newShelf] == 0){
                     *src = oldShelf;
                     *dst = newShelf;
                     return true;
                  }
               }
            }
         }
      }
   return false;
}

bool canMoveToComplete(bookcase *bc, int *src, int *dest){
   int oldShelf, newShelf;
   for (oldShelf = 0; oldShelf < bc->height; oldShelf++){
      for (newShelf = 0; newShelf < bc->height; newShelf++){
         if (oldShelf != newShelf){
            if (canMoveBook(bc, oldShelf, newShelf) == true){
               if (bc->destScore[newShelf] == bc->score[newShelf]){
                  *src = oldShelf;
                  *dst = newShelf;
                  return true;
               }
            }
         }
      }
   }
   return false;
}

bool canMoveToPartial(bookcase *bc, int *src, int *dest){
   int oldShelf, newShelf;
   for (oldShelf = 0; oldShelf < bc->height; oldShelf++){
      for (newShelf = 0; newShelf < bc->height; newShelf++){
         if (oldShelf != newShelf){
            if (canMoveBook(bc, oldShelf, newShelf) == true){
               if (bc->correctHist[newShelf] > 0){
                  *src = oldShelf;
                  *dst = newShelf;
                  return true;
               }
            }
         }
      }
   }
   return false;
}

bookcase *moveUnhappy(bookcase *bc, int *src, int *dest){
   int minShelf, newShelf, booksToMove, count;
   bookcase *newBc;
   minShelf = minScoreShelf(bc);
   booksToMove = bc->score[minShelf];
   newShelf = 0;
   while (booksToMove > 0 && count < bc->score[minShelf]){
      if (newShelf != minShelf
         && canMoveBook(bc, minShelf, newShelf) == true){
         newBc = moveBook(bc, minShelf, newShelf);
         bc = newBc;
      }
      else{
         newShelf++;
      }
   }
   return newBc;
}

int minScoreShelf(bookcase *bc){
   int i, min, shelf;
   min = bc->width;
   for (i = 0; i < bc->height; i++){
      if (bc->score < min){
         min = bc->score;
         shelf = i;
      }
   }
   return shelf;
}
