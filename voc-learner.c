#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_EXCHANGE 100
#define SEPARATOR_CHAR ';'
/* Colors: */
#define RED "\033[31m"
#define GRN "\033[32m"
#define RESET "\033[0m"

typedef struct voc_elt {
  char word[50];
  char frensh[50];
} voc_elt;

/* Utility function: return 1 if the characters is an ending characters,
 * aka if the characters is different to 0 and '\n'
 */
static inline int isEndChar(char c) {
  return (c == '\n' || c == 0);
}

/* Remove trailing char on stdin */
void flushInput() {
  char c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

/* withdraw the word and the translation on the line stored in buff and update
 * `words` table
 */
void parseLine(voc_elt *words, char buff[100], int index) {
  int i = 0, iw = 0;

  // catch the word
  while (buff[i] && buff[i] != SEPARATOR_CHAR) {
    words[index].word[iw] = buff[i];
    ++i;
    ++iw;
  }
  words[index].word[iw] = 0;
  iw = 0;
  ++i;
  // catch the translation
  while (!isEndChar(buff[i])) {
    words[index].frensh[iw] = buff[i];
    ++i;
    ++iw;
  }
  words[index].frensh[iw] = 0;
}

/* Create an array of `voc_elt` using the content of the file given as parameter
 */
voc_elt *initWordsArray(FILE *f, int wordsNumber) {
  int i;
  char buff[500]; // buffer for fgets
  voc_elt *words = (voc_elt *)malloc(wordsNumber * sizeof(voc_elt));

  if (words != NULL) {
    for (i = 0; i < wordsNumber; ++i) { // withdraw words in f
      fgets(buff, 500, f);
      parseLine(words, buff, i);
    }
  } else {
    fprintf(stderr, "Error: unable to allocate words.\n");
  }

  return words;
}

/* Exchange 2 cells in a array */
void exchangeArr(int *tab, int i, int j) {
  int c = tab[i];
  tab[i] = tab[j];
  tab[j] = c;
}

/* Create a random permutation in order to randomly ask words to the user */
int *generateRandomOrder(int n) {
  int *order = (int *)malloc(n * sizeof(int));
  int k, i, j;

  // init random suite
  srand(time(NULL));

  if (order != NULL) {
    // init order
    for (k = 0; k < n; ++k) {
      order[k] = k;
    }

    // apply random permutation on order
    for (k = 0; k < MAX_EXCHANGE; ++k) {
      i = rand() % n;
      j = rand() % n;
      exchangeArr(order, i, j);
    }
  }

  return order;
}

/* return 1 if the words are the same
 * side effect: print the correct aswer with putting characters in green or in
 * red depending on the maching with userInput.
 * Moreover the fuction consider that words between parentheses are optional.
 * TODO: remove this and write a better fonctions
 */
int compareWords(char w[50], char *userInput) {
  int iw = 0, iui = 0;
  int result = 1;
  int inParent = 0;
  int compareInParent = 0;

  // clear the screen before dispalaying the answer
  system("clear");
  while (w[iw] || !isEndChar(userInput[iui])) {
    if (!inParent || compareInParent) { // enter word compareson
      if (!isEndChar(userInput[iui]) && w[iw] == userInput[iui]) {
        // print the letter in gren if it's correct
        printf("%s%c%s", GRN, w[iw], RESET);
        if (w[iw] == '(') {
          inParent = 1;
          compareInParent = 1;
        }
      } else if (w[iw] != '(') {
        result = 0;
        printf("%s%c%s", RED, w[iw], RESET); // TODO: don't print the answer this way
      } else {
        inParent = 1;
        printf("%s(%s", GRN, RESET);
      }
      ++iw;
      ++iui;
    } else {
      while (w[iw] != ')') {
        printf("%s%c%s", GRN, w[iw], RESET);
        ++iw;
      }
      ++iw;
      if (w[iw] == ' ') {
        ++iw;
      } // jump over the space as we don't want to compare two spaces
    } // we don't compare what's inside parent
  }
  printf("\n");
  if (userInput[0] == 0 || userInput[0] == '\n') {
    result = 0;
  }

  return result;
}

/* Ask the user to rewrite the correct aswer and loop util the word typed is
 * really correct (use strcmp this time)
 */
void askForRewirte(char word[50]) {
  char buff[100] = {0};
  int correctAnswer;

  do {
    system("clear");
    printf("Faulty answer, please rewrite %s%s%s:\n", GRN, word, RESET);
    fgets(buff, 100, stdin);
    correctAnswer = compareWords(word, buff);
    if (!correctAnswer) {
      getchar();
    }
  } while (!correctAnswer);
}

void printScore(int score, int total) {
  float p = (float)score / total;

  if (p > 0.5) {
    printf("score: %s%d%s\n", GRN, (int)(100 * p), RESET);
  } else {
    printf("score: %s%d%s\n", RED, (int)(100 * p), RESET);
  }
}

/**
 * MAIN LOOP
 * Ask all the words to the user and tell him if his answer is correct. If the
 * answer is not correct, dispalay the wrong par in red (and the right part in
 * green) and ask to the user to rewrite the word (this operation is repeted
 * until the word typed is correct)
 */
void runApp(voc_elt *words, int *randomOrder, int wordsNumber, int mode) {
  int i, rindex, ierror = 0;
  int correctAnswer;
  char answer[50];
  char toTranslate[50];
  char expectedAnswer[50];
  int score = 0;
  int error[500];
  char quit = 'c';

  for (i = 0; i < wordsNumber; ++i) {
    rindex = randomOrder[i];
    if (mode == 1) {
      strcpy(toTranslate, words[rindex].word);
      strcpy(expectedAnswer, words[rindex].frensh);
    } else {
      strcpy(toTranslate, words[rindex].frensh);
      strcpy(expectedAnswer, words[rindex].word);
    }
    system("clear");
    printf("Translate: %s\n\nAnswer: ", toTranslate);
    flushInput();
    fgets(answer, 50, stdin);
    correctAnswer = compareWords(expectedAnswer, answer);
    getchar();
    if (!correctAnswer) {
      error[ierror] = rindex;
      ++ierror;
      if (mode != 1)
        askForRewirte(expectedAnswer);
    } else {
      ++score;
    }
  }
  printScore(score, wordsNumber);
  printf("Hit q to quit, enything else to continue\n");
  quit = getchar();
  if (ierror != -1 && quit != 'q') {
    runApp(words, error, ierror, mode);
  }
}

/* Used to parse options in the program parameters */
void parseCommandLine(int argc, char **argv, char *fileName, int *mode) {
  int optionIndex = 1;
  int fileIndex = 2;

  if (argc == 2) {
    strcpy(fileName, argv[1]);
    *mode = 0;
  } else if (argc == 3) {
    if (argv[2][0] == '-') {
      optionIndex = 2;
      fileIndex = 1;
    }
    if (strcmp(argv[optionIndex], "-fr")) {
      *mode = 0;
    } else if (strcmp(argv[optionIndex], "-vo")) {
      *mode = 1;
    } else {
      *mode = -1;
    }
    strcpy(fileName, argv[fileIndex]);
  }
}

int main(int argc, char **argv) {
  char fileName[100];
  int mode;
  FILE *vocFile = NULL;
  int wordsNumber = 0;
  voc_elt *words = NULL;
  int *randomOrder = NULL;

  /* The user should give a file name as program parameter */
  if (argc > 1) {
    parseCommandLine(argc, argv, fileName, &mode);
    printf("%s\n", fileName);
    vocFile = fopen(fileName, "r");
    if (mode != -1 && vocFile != NULL) {
      // TODO: count the number of lines in the file manualy
      fscanf(vocFile, "%d%*c",
             &wordsNumber); // the number of lines in the file is writen on the
                            // first line in the file
      words = initWordsArray(vocFile, wordsNumber);
      randomOrder = generateRandomOrder(wordsNumber);
      if (words != NULL && randomOrder != NULL) { // the program can continue
        runApp(words, randomOrder, wordsNumber, mode);
      }
    } else {
      fprintf(stderr, "Error: unable to open %s.\n", argv[1]);
    }
  } else {
    printf("Error: wrong input.\nUsage: voc-learner -OPTION FILE_NAME\n");
  }
  if (vocFile != NULL) {
    fclose(vocFile);
  }
  // free
  free(words);
  free(randomOrder);

  return 0;
}
