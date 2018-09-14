//
//  main.c
//  assignment_2
//
//  Created by Wujian on 11/9/18.
//  Copyright © 2018 Wujian. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define True 1
#define False 0


int qsort_compare(const void *a, const void *b);
void BWT_sort(int *index_array, int size);
int *find_interupt(int *char_frequency, int size);
char *text;

int main(int argc, const char * argv[]) {
    /* analyze input arguments */
    int i;
    char delimiter;
    
    if (strlen(argv[1]) == 2)
        delimiter = '\n';
    else
        delimiter = (char)argv[1][0];
//    printf("delimiter is %d %c\n", delimiter, delimiter);
    const char *temp_fold_path = argv[2];
    const char *text_file = argv[3];
    const char *bwt_result = argv[4];
//    printf("%d, %s, %s, %s\n", delimiter, temp_fold_path, text_file, bwt_result);
    
    
    /* open file */
    int fgetc_result, delimiter_number;
    long file_size;
    int char_frequency[128] = {0};
    
    FILE *file = fopen(text_file, "r");
    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    printf("file size id %d\n", file_size);
    text = malloc(file_size * sizeof(char));

    i = 0;
    while ((fgetc_result = fgetc(file)) != EOF) {
        // printf("%d\n", fgetc_result);
        if ((char) fgetc_result == delimiter) {
            text[i] = 0;
            char_frequency[0]++;
        } else {
            text[i] = fgetc_result;
            char_frequency[fgetc_result]++;
        }
        i++;
    }
    fclose(file);
    // for (i = 0; i < file_size; i++) {
    //     printf("%d\n", text[i]);
    // }
    printf("text file is %s\n", text);



    /* find half interupt to sperate to two files */
    int* half_interupt = find_interupt(char_frequency, file_size);
    if (half_interupt[0] < 0) {
        printf("find_interupt bug!!!!!\n");
        exit(-1);
    } else {
        printf("interupy char is:%c\n", (char)half_interupt[0]);
    }
    



    /* separate to two arrays */
    // first file
    int j = 0;
    int interupt_number = 0;
    int index_array_length = (int)file_size/2 + 1;
    printf("index array 1 length is %d\n", index_array_length);
    int *index_array1 = malloc(index_array_length * sizeof(int));
    for (i = 0; i < file_size; i++) {
        if (text[i] < (char)half_interupt[0]) {
            index_array1[j] = i;
            j++;
//            printf("%d\n", i);
            if (j == index_array_length) {
                break;
            }
        } else if (text[i] == (char)half_interupt[0]) {
            if (interupt_number < (char)half_interupt[1]) {
                index_array1[j] = i;
                j++;
                interupt_number++;
//                printf("%d\n", i);
                if (j == index_array_length) {
                    break;
                }
            }
        }
    }
    printf("first sort result:\n");
    BWT_sort(index_array1, index_array_length);
    char *file_name_1 = malloc(strlen(temp_fold_path) + 10 + 1);
    strcpy(file_name_1, temp_fold_path);
    strcat(file_name_1, "/temp1.txt");
    
    file = fopen(file_name_1, "w");
    fwrite(index_array1, sizeof(int), index_array_length, file);
    fclose(file);
    
    // second file
    index_array_length = file_size - index_array_length;
    printf("index array 2 length is %d\n", index_array_length);
    j = 0;
    interupt_number = 0;
    for (i = 0; i < file_size; i++) {
        if (text[i] > (char)half_interupt[0]) {
            index_array1[j] = i;
            j++;
//            printf("%d\n", i);
            if (j == index_array_length) {
                break;
            }
        } else if (text[i] == (char)half_interupt[0]) {
            if (interupt_number >= (char)half_interupt[1]) {
                index_array1[j] = i;
                j++;
//                printf("%d\n", i);
                if (j == index_array_length) {
                    break;
                }
            }
            interupt_number++;
        }
    }
    printf("second sort result:\n");
    BWT_sort(index_array1, index_array_length);
    char *file_name_2 = malloc(strlen(temp_fold_path) + 10 + 1);
    strcpy(file_name_2, temp_fold_path);
    strcat(file_name_2, "/temp2.txt");
    file = fopen(file_name_2, "w");
    fwrite(index_array1, sizeof(int), index_array_length, file);
    fclose(file);

    
    return 0;

}

int* find_interupt(int *char_frequency, int size) {
    int *return_value = malloc(2 * sizeof(int));
    int i;
    int half = size / 2 + 1;
    for (i = 0; i < 128; i++) {
        half = half - char_frequency[i];
        if (half <= 0) {
            return_value[0] = i;
            return_value[1] = char_frequency[i] + half;
            return return_value;
        }
    }
    return_value[0] = -1;
    return_value[1] = 0;
    return return_value;
}


void BWT_sort(int *index_array, int size) {
    int i = 0;
    
//    for (i = 0; i < size; i++) {
//        printf("%d\n", index_array[i]);
//    }

    qsort(index_array, size, sizeof(int), qsort_compare);
    for (i = 0; i < size; i++) {
//        if (index_array[i] == 0)
//            printf("%c", text[size-1]);
//        else
//            printf("%c", text[index_array[i]-1]);
        printf("%d", index_array[i]);
//        printf("%s\n", text+index_array[i]);
    }
    printf("\n");
}

int qsort_compare(const void *Ina, const void *Inb) {
    int *a = (int *)Ina;
    int *b = (int *)Inb;
    return strcmp((char *)text+*a, (char *)text+*b);
}
