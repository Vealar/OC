//
// Created by Александр Артемьев on 16.03.2024.
//
#include <ctype.h>
// Переворот слова
void rollWord(int a,int b,char* buffer){
    char *start = &buffer[a];
    char *end = &buffer[b];
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        ++start;
        --end;
    }
}
// Переворот текста
void rollText(int length,char* buffer)
{
    int left = 0;
    while (left < length) {
        if (!isalpha(buffer[left])) {
            ++left;
            continue;
        }
        int right = left + 1;
        while (right < length && isalpha(buffer[right]) == 1) {
            ++right;
        }
        rollWord(left, right - 1,buffer);
        left = right + 1;
    }
}