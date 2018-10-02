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
int binary_Search(int array[], int l, int r, int x);

char *text;
int delimiter_number;

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
    
    
    /* open file */
    int fgetc_result;
    long file_size;
    int char_frequency[128] = {0};
    FILE *file = fopen(text_file, "r");
    fseek(file, 0L, SEEK_END);


    /* need to change !!!!!!*/
    file_size = ftell(file) - 1;
    
    fseek(file, 0L, SEEK_SET);
    printf("file size id %ld\n", file_size);
    text = malloc(file_size * sizeof(char));

    i = 0;
    while ((fgetc_result = fgetc(file)) != EOF && i < file_size) {
        // printf("%d\n", fgetc_result);
        if ((char) fgetc_result == delimiter) {
            // printf("At position %d\n", i);
            text[i] = 0;
            char_frequency[0]++;
        } else {
            text[i] = fgetc_result;
            char_frequency[fgetc_result]++;
        }
        i++;
    }
    fclose(file);
    printf("text file is %s\n", text);



    /* find half interupt to sperate to two files */
    int* half_interupt = find_interupt(char_frequency, file_size/2 + 1);
    if (half_interupt[0] < 0) {
        printf("find_interupt bug!!!!!\n");
        exit(-1);
    } else {
        printf("interupy char is:%c\n", (char)half_interupt[0]);
    }
    



    /* separate to two arrays */
    // first file
    int j = 0;

    int *delimiter_position = malloc(char_frequency[0] * sizeof(int));
    int delimiter_index = 0;

    int index_array_length_1 = half_interupt[1];  // the first index array length
    printf("index array 1 length is %d\n", index_array_length_1);
    int *index_array = malloc(index_array_length_1 * sizeof(int));
    for (i = 0; i < file_size; i++) {
        if (text[i] <= (char)half_interupt[0]) {
            if (text[i] == 0) {
                delimiter_position[delimiter_index] = i;
                delimiter_index++;
            }
            index_array[j] = i;
            j++;
            if (j == index_array_length_1) {
                if (delimiter_index != char_frequency[0]) {
                    printf("delimiter bug!!!!\n");
                }
                break;
            }
        }
    }
    printf("first sort result:\n");
    BWT_sort(index_array, index_array_length_1);
    char *file_name_1 = malloc(strlen(temp_fold_path) + 10 + 1);
    strcpy(file_name_1, temp_fold_path);
    strcat(file_name_1, "/temp1.txt");
    
    FILE *file_temp1 = fopen(file_name_1, "w");
    fwrite(index_array, sizeof(int), index_array_length_1, file_temp1);
    fclose(file_temp1);
    
    // second file
    int index_array_length_2 = file_size - index_array_length_1;
    printf("index array 2 length is %d\n", index_array_length_2);
    j = 0;
    for (i = 0; i < file_size; i++) {
        if (text[i] > (char)half_interupt[0]) {
            index_array[j] = i;
            j++;
//            printf("%d\n", i);
            if (j == index_array_length_2) {
                break;
            }
        }
    }
    printf("second sort result:\n");
    BWT_sort(index_array, index_array_length_2);
    char *file_name_2 = malloc(strlen(temp_fold_path) + 10 + 1);
    strcpy(file_name_2, temp_fold_path);
    strcat(file_name_2, "/temp2.txt");
    FILE *file_temp2 = fopen(file_name_2, "w");
    fwrite(index_array, sizeof(int), index_array_length_2, file_temp2);
    fclose(file_temp2);


    // write positional file
    // printf("delimiter position array:\n");
    // for (i = 0; i < char_frequency[0]; i++) {
    //     printf("%d ", delimiter_position[i]);
    // }
    // printf("\n");

    char *positional_file_name = malloc(strlen(temp_fold_path) + 13 + 1);
    strcpy(positional_file_name, temp_fold_path);
    strcat(positional_file_name, "/position.aux");
    FILE *positional_file = fopen(positional_file_name, "w");
    int position_index;

    // read from temp file

    FILE *output_file = fopen(bwt_result, "w");
    file_temp1 = fopen(file_name_1, "r");
    printf("read temp file1:\n");
    fread(index_array, sizeof(int), index_array_length_1, file_temp1);
    
    for (i = 0; i < index_array_length_1; i++) {
//        printf("%d", index_array[i]);
        if (index_array[i] == 0) {              // the first suffix array element.
            if (text[file_size-1] == 0) {       // if it is delimiter
                fwrite(&delimiter, sizeof(char), 1, output_file); 
                if ((position_index = binary_Search(delimiter_position, 0, char_frequency[0]-1, file_size-1)) == -1) {
                    printf("binary search error!!\n");
                    exit(1);
                } else {
                    fwrite(&position_index, sizeof(int), 1, positional_file);
                }
                // printf(" position %ld: %c", file_size-1, delimiter);
                
            } else {                            // ordinary text
                fwrite(&text[index_array[i]-1], sizeof(char), 1, output_file);
                // printf("%c", text[index_array[i]-1]);
            }
        } else {
            if (text[index_array[i]-1] == 0) {
                if ((position_index = binary_Search(delimiter_position, 0, char_frequency[0]-1, index_array[i]-1)) == -1) {
                    printf("binary search error!!\n");
                    exit(1);
                } else {
                    fwrite(&position_index, sizeof(int), 1, positional_file);
                }

                fwrite(&delimiter, sizeof(char), 1, output_file);
                // printf(" postion %d: %c", index_array[i]-1, delimiter);
            } else {
                fwrite(&text[index_array[i]-1], sizeof(char), 1, output_file);
                // printf("%c", text[index_array[i]-1]);
            }
        }
    }
    printf("\n");

    file_temp2 = fopen(file_name_2, "r");
    fread(index_array, sizeof(int), index_array_length_2, file_temp2);
    printf("read temp file2:\n");
    for (i = 0; i < index_array_length_2; i++) {
    //    printf("%d", index_array[i]);
        if (index_array[i] == 0) {
            if (text[file_size-1] == 0) {
                if ((position_index = binary_Search(delimiter_position, 0, char_frequency[0]-1, file_size-1)) == -1) {
                    printf("binary search error!!\n");
                    exit(1);
                } else {
                    fwrite(&position_index, sizeof(int), 1, positional_file);
                }

                fwrite(&delimiter, sizeof(char), 1, output_file);
                // printf(" position %ld: %c", file_size-1, delimiter);
                
            } else {
                fwrite(&text[index_array[i]-1], sizeof(char), 1, output_file);
                // printf("%c", text[index_array[i]-1]);
            }
        } else {
            if (text[index_array[i]-1] == 0) {
                if ((position_index = binary_Search(delimiter_position, 0, char_frequency[0]-1, index_array[i]-1)) == -1) {
                    printf("binary search error!!\n");
                    exit(1);
                } else {
                    fwrite(&position_index, sizeof(int), 1, positional_file);
                }

                fwrite(&delimiter, sizeof(char), 1, output_file);
                // printf(" position %d: %c", index_array[i]-1, delimiter);
            } else {
                fwrite(&text[index_array[i]-1], sizeof(char), 1, output_file);
                // printf("%c", text[index_array[i]-1]);
            }
        }
    }
    printf("\n");

    return 0;

}








int* find_interupt(int *char_frequency, int size) {
    int *return_value = malloc(2 * sizeof(int));
    int i;
    int temp = size;
    for (i = 0; i < 128; i++) {
        temp = temp - char_frequency[i];
        // if (char_frequency[i])
        //     printf("%c frequency is %d, left half is %d\n", i, char_frequency[i], temp);
        if (temp <= 0) {
            return_value[0] = i;
            return_value[1] = size - temp;
            return return_value;
        }
    }
    return_value[0] = -1;
    return_value[1] = 0;
    return return_value;
}




void BWT_sort(int *index_array, int size) {
    
//    for (i = 0; i < size; i++) {
//        printf("%d\n", index_array[i]);
//    }

    qsort(index_array, size, sizeof(int), qsort_compare);
    // for (i = 0; i < size; i++) {
//        if (index_array[i] == 0)
//            printf("%c", text[size-1]);
//        else
//            printf("%c", text[index_array[i]-1]);
        // printf("%d ", index_array[i]);
//        printf("%s\n", text+index_array[i]);
    // }
    printf("\n");
}

int qsort_compare(const void *Ina, const void *Inb) {
    int *a = (int *)Ina;
    int *b = (int *)Inb;
    if ((char)text[*a] == (char)delimiter_number && ((char)text[*b] == (char)delimiter_number)) {
        return (*a - *b);
    }
    return strcmp((char *)text+*a, (char *)text+*b);
}

int binary_Search(int array[], int l, int r, int x) {
    // printf("search position is %d\n", x);
   while (l <= r) 
    { 
        int m = l + (r-l)/2; 
        // Check if x is present at mid 
        if (array[m] == x) 
            return m + 1; 
        // If x greater, ignore left half 
        if (array[m] < x) 
            l = m + 1; 
        // If x is smaller, ignore right half 
        else
            r = m - 1; 
    } 
    return -1;
}
