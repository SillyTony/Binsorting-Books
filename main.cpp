#include <stdio.h>
#include <cctype>
#include <time.h>
#include <string.h>

void getAllWords(FILE* input,FILE* output, int& wordCount, int wordAmount[],int& paragraphCount, int& sentenceCount, int& vowelCount);
void binSort();
void findPopularWords(FILE* input, FILE *popularWords[], FILE *frequencyCountFile, int popularWordCount[]);
void print(const double &speed, const int &totalWords, const int &paragraphs, const int &sentences, int countPerAlphabet[], const double &readingLevel, FILE* popularWord[], int popularWordCount[]);

int main()
{
    int wordCount = 0, paragraphCount = 1, sentenceCount = 0, vowelCount = 0, popularWord[11] = {}, wordAmount[26] = {};
    double time = 0, readingLevel = 0;
    char fileName[20];
    time_t start, end;
    FILE* input,* output,* frequentCount,* popularWords[11];

    scanf("%s",fileName);
    start = clock();
    input = fopen(fileName,"r");
    output = fopen("output.txt","w");
    frequentCount = fopen("frequent.txt","w");

    for(int i = 0; i < 11; ++i)
    {
        sprintf(fileName,"popularWord_%i.txt",i);
        popularWords[i] = fopen(fileName,"w+");
    }

    if(!input)
        printf("File is not in the directory.\n");
    else
    {
        getAllWords(input,output,wordCount,wordAmount,paragraphCount,sentenceCount,vowelCount);
        binSort();
        fclose(input);
        findPopularWords(input,popularWords,frequentCount,popularWord);
        end = clock();
        time = double(end-start)/CLOCKS_PER_SEC;
        readingLevel = (((.39 * double(wordCount)/sentenceCount) + (11.8 * (double(vowelCount)/wordCount))) - 15.59);
        print(time, wordCount, paragraphCount,sentenceCount,wordAmount,readingLevel,popularWords,popularWord);
    }
    fclose(input);
    fclose(output);
    fclose(frequentCount);
    for(int i = 0; i < 11; ++i)
        fclose(popularWords[i]);
    return 0;
}

void getAllWords(FILE* input,FILE* output, int& wordCount, int wordAmount[],int& paragraphCount, int& sentenceCount, int& vowelCount)
{
    char letter, word[25], buffer[25];
    int i, j;

    while(!feof(input))
    {
        fscanf(input,"%c",&letter);
        if(letter == '\n')
        {
            fscanf(input,"%c",&letter);
            (letter == '\n' && ++paragraphCount);
        }
        if(isalpha(letter))
        {
            ungetc(letter,input);
            fscanf(input,"%s",buffer);
            j = 0;
            while(buffer[j])
            {
                if(buffer[j] == '.' ||buffer[j] == '!' ||buffer[j] == '?')
                    ++sentenceCount;
                ++j;
            }
            sscanf(buffer,"%[a-zA-Z]",word);
            i = 0;
            word[0] = tolower(word[0]);
            while(word[i])
            {
                if(word[i] == 'a' || word[i] == 'e' || word[i] == 'i' || word[i] == 'o' || word[i] == 'u')
                    ++vowelCount;
                ++i;
            }
            ++wordCount;
            word[0] = toupper(word[0]);
            ++wordAmount[word[0]-65];
            fprintf(output,"%s\n",word);
        }
    }
}

void binSort()
{
    FILE *buckets[27], *container;
    char word[25] = {}, fileName[20] = {};

    for(int i = 5; i > -1; --i)
    {
        container = fopen("output.txt", "r");
        for(int j = 0; j < 27; ++j)
        {
            sprintf(fileName,"%c.txt",'@'+j);
            buckets[j] = fopen(fileName, "w+");
        }
        while(!feof(container))
        {
            fscanf(container, "%s", word);
            if(strlen(word) > i)
                fprintf(buckets[toupper(word[i]) - '@'], "%s ", word);
            else
                fprintf(buckets[0], "%s ", word);
        }
        fclose(container);
        container = fopen("output.txt", "w");
        for(int j = 0; j < 27; ++j)
        {
            rewind(buckets[j]);
            while(!feof(buckets[j]))
            {
                fscanf(buckets[j], "%s", word);
                fprintf(container, "%s ", word);
            }
        }
    }
    for(int i = 0; i < 27; ++i)
         fclose(buckets[i]);
    fclose(container);
}

void findPopularWords(FILE* input,FILE* popularWords[],FILE* frequencyCountFile,int popularWordCount[])
{
    int frequencyCount = 0, temp = 0;
    char wordTwo[25] = {}, word[25] = {}, buffer[25] = {};

    input = fopen("output.txt","r");
    fscanf(input,"%s",word);
    while(!feof(input))
    {
        frequencyCount = 1;
        fscanf(input,"%s",wordTwo);
        while(!strcmp(word,wordTwo))
        {
            ++frequencyCount;
            fscanf(input,"%s",wordTwo);
            if(feof(input))
                break;
        }
        popularWordCount[10] = frequencyCount;
        for(int i = 9; i > -1; --i)
        {
            if(popularWordCount[i+1] > popularWordCount[i])
            {
                rewind(popularWords[i]);
                rewind(popularWords[i+1]);
                temp = popularWordCount[i];
                popularWordCount[i] = popularWordCount[i+1];
                popularWordCount[i+1] = temp;
                fscanf(popularWords[i],"%s",buffer);
                rewind(popularWords[i]);
                fprintf(popularWords[i],"%s ",word);
                fprintf(popularWords[i+1],"%s ",buffer);
            }
            else
                break;
        }
        fprintf(frequencyCountFile,"%s %i\n",word, frequencyCount);
        strncpy(word,wordTwo,35);
    }
}

void print(const double& speed, const int& totalWords, const int& paragraphs, const int& sentences, int countPerAlphabet[], const double& readingLevel, FILE* popularWord[], int popularWordCount[])
{
    char buffer[20];
    printf("Total time: %f\nWord Count:%i\nParagraph Count: %i\nSentence Count: %i\nReading Level %f\n",speed,totalWords,paragraphs,sentences,readingLevel);
    for(int i = 0; i < 26; ++i)
        printf("Letter %c %i\n",i+65,countPerAlphabet[i]);
    printf("Most popular words.\n");
    for(int i = 0; i < 10; ++i)
    {
        rewind(popularWord[i]);
        fscanf(popularWord[i],"%s",buffer);
        printf("%s %i\n",buffer,popularWordCount[i]);
    }
}
