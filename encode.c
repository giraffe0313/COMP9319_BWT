//  main.c
//  assignment_2
//
//  Created by Wujian on 11/9/18.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define True 1
#define False 0

struct divid_structure {
    int length;
    char element;
};

int qsort_compare(const void *a, const void *b);
void BWT_sort(int *index_array, int size);
struct divid_structure *find_interupt(int *char_frequency, int size);
int binary_Search(int array[], int l, int r, int x);
void temp_file_write(char **temp_file_name, int *delimiter_position, int file_number, struct divid_structure* third_interupt
                    , int file_size, int delimiter_number, const char *temp_fold_path);

void temp_file_read(char **temp_file_name, const char *bwt_result, struct divid_structure* third_interupt, 
                    int file_size, char delimiter, int *delimiter_position, int delimiter_number, 
                    const char *temp_fold_path);

int extract_bwt_file_name(const char *bwt_file_path);

char *text;
int delimiter_number;
char delimiter;

int main(int argc, const char * argv[]) {
    /* analyze input arguments */
    int i;
    
    if (strlen(argv[1]) == 2)
        delimiter = '\n';
    else
        delimiter = (char)argv[1][0];
    const char *temp_fold_path_temp = argv[2];
    const char *text_file = argv[3];
    const char *bwt_result = argv[4];

    char *temp_fold_path = malloc(strlen(temp_fold_path_temp) + 1);
    sprintf(temp_fold_path, "%s/", temp_fold_path_temp);
    
    
    /* open file */
    int fgetc_result;
    long file_size;
    int char_frequency[128] = {0};
    FILE *file = fopen(text_file, "r");
    fseek(file, 0L, SEEK_END);


    /* need to change !!!!!!*/
    file_size = ftell(file);
    
    fseek(file, 0L, SEEK_SET);
    text = malloc(file_size * sizeof(char));

    // read file
    i = 0;
    while ((fgetc_result = fgetc(file)) != EOF && i < file_size) {
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


    /* find half interupt to sperate to two files */
    struct divid_structure* third_interupt = find_interupt(char_frequency, file_size);
    if (third_interupt[0].element < 0) {
        printf("find_interupt bug!!!!!\n");
        exit(-1);
    }
    


    // first file
    int j = 0;
    char **temp_file_name = calloc(3, sizeof(char *));
    // printf("Malloc: delimiter_position is%d, type: int\n", char_frequency[0]);
    int *delimiter_position = malloc(char_frequency[0] * sizeof(int));
    int delimiter_index = 0;

    // write sort result to three temp files
    temp_file_write(temp_file_name, delimiter_position, 0, third_interupt, file_size, char_frequency[0], temp_fold_path);
    temp_file_write(temp_file_name, delimiter_position, 1, third_interupt, file_size, char_frequency[0], temp_fold_path);
    temp_file_write(temp_file_name, delimiter_position, 2, third_interupt, file_size, char_frequency[0], temp_fold_path);
    
    // merge result to one final file
    temp_file_read(temp_file_name, bwt_result, third_interupt, file_size, delimiter, delimiter_position, char_frequency[0], temp_fold_path);
    
    free(text);
    free(delimiter_position);

    // delete temp file
    for (i = 0; i < 3; i++) {
        if (temp_file_name[i]) {
            remove(temp_file_name[i]);
            free(temp_file_name[i]);
        }
    }
    free(temp_file_name);

    return 0;

}

struct divid_structure* find_interupt(int *char_frequency, int size) {
    struct divid_structure *return_value = calloc(3, sizeof(struct divid_structure));
    int i;
    int temp = size/3 + 1;

    for (i = 0; i < 128; i++) {
        temp = temp - char_frequency[i];
        if (temp <= 0) {
            return_value[0].element = (char) i;
            return_value[0].length = size/3 + 1 - temp;
            break;
        }
    }
    int max_length = size/3 + 1 - temp;
    temp = (size - return_value[0].length)/2 + 1;
     for (i = return_value[0].element + 1; i < 128; i++) {
        temp = temp - char_frequency[i];
        if (temp <= 0) {            
            return_value[1].element = (char) i;
            return_value[1].length = (size - return_value[0].length)/2 + 1 - temp;
            max_length = max_length < return_value[1].length?return_value[1].length:max_length;
            int third = size - return_value[0].length - return_value[1].length;
            max_length = max_length < third ? third : max_length;
            return_value[2].length = size - return_value[0].length - return_value[1].length;
            return_value[2].element = (char) 127;
            return return_value;
        }
    }
    
    return_value[0].element = -1;
    return_value[0].length = 0;
    return return_value;
}




void BWT_sort(int *index_array, int size) {
    qsort(index_array, size, sizeof(int), qsort_compare);
}

int qsort_compare(const void *Ina, const void *Inb) {
    int *a = (int *)Ina;
    int *b = (int *)Inb;
    if ((char)text[*a] == 0 && ((char)text[*b] == 0)) {
        return (*a - *b);
    }
    int i = 0;

    while ((text[*a + i] != 0) && (text[*b + i] != 0)) {
        if (text[*a + i] < text[*b + i]) {
            return -1;
        } else if (text[*a + i] > text[*b + i]) {
            return 1;
        }
        i++;
    }
    if (text[*a + i] == 0 && text[*b + i] == 0) {
        return (*a - *b);
    }

    if (text[*a + i] == '\0') {
        return -1;
    } else {
        return 1;
    } 
    return 0;
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


void temp_file_write(char **temp_file_name, int *delimiter_position, int file_number, struct divid_structure* third_interupt
                    , int file_size, int delimiter_number, const char *temp_fold_path) {
    int j = 0;
    int i = 0;
    int index_array_length_1 = third_interupt[file_number].length;
    // printf("Temp_file_write: index_array_length_1 is %d, elements is %c\n", index_array_length_1, third_interupt[file_number].element);

    int *index_array = malloc(index_array_length_1 * sizeof(int));
    int delimiter_index = 0;
    if (third_interupt[file_number].length <= 0) {
        return;
    }

    for (i = 0; i < file_size; i++) {
        if (text[i] <= (char)third_interupt[file_number].element) {
            if (file_number == 1 && text[i] <= (char)third_interupt[0].element) {
                continue;
            }
            if (file_number == 2 && text[i] <= (char)third_interupt[1].element) {
                continue;
            }
            if (text[i] == 0) {
                delimiter_position[delimiter_index] = i;
                delimiter_index++;
            }
            index_array[j] = i;
            j++;
            if (j == index_array_length_1) {
                break;
            }
        }
    }

    BWT_sort(index_array, index_array_length_1);
    char *file_name_1 = malloc(strlen(temp_fold_path) + 10 + 1);
    sprintf(file_name_1, "%stemp%d.txt", temp_fold_path, file_number);
    
    FILE *file_temp1 = fopen(file_name_1, "w");
    fwrite(index_array, sizeof(int), index_array_length_1, file_temp1);
    fclose(file_temp1);
    free(index_array);
    temp_file_name[file_number] = file_name_1;
}




void temp_file_read(char **temp_file_name, const char *bwt_result, struct divid_structure* third_interupt, 
                    int file_size, char delimiter, int *delimiter_position, int delimiter_number, 
                    const char *temp_fold_path) {    
    FILE *output_file = fopen(bwt_result, "w");
    int i = 0;
    int file_number = 0;
    int position_index;
    int index_array_length = third_interupt[2].length;
    for (i = 0; i < 3; i ++) {
        if (third_interupt[i].length > index_array_length) {
            index_array_length = third_interupt[i].length;
        }
    }

    int *index_array = malloc(index_array_length * sizeof(int));

    char *positional_file_name = malloc(strlen(bwt_result) + 12 + 1);
    sprintf(positional_file_name, "%sposiotion.aux", bwt_result);
    FILE *positional_file = fopen(positional_file_name, "w");
    
    
    for (; file_number < 3; file_number++) {
        if (!temp_file_name[file_number]) return;
        FILE *file_temp = fopen(temp_file_name[file_number], "r");
        
        fread(index_array, sizeof(int), third_interupt[file_number].length, file_temp);
        
        for (i = 0; i < third_interupt[file_number].length; i++) {
            if (index_array[i] == 0) {              // the first suffix array element.
                if (text[file_size-1] == 0) {       // if it is delimiter
                    fwrite(&delimiter, sizeof(char), 1, output_file); 
                    if ((position_index = binary_Search(delimiter_position, 0, delimiter_number-1, file_size-1)) == -1) {
                        printf("binary search error!!\n");
                        exit(1);
                    } else {
                        fwrite(&position_index, sizeof(int), 1, positional_file);
                    }               
                } else {                            // ordinary text
                    fwrite(&text[file_size-1], sizeof(char), 1, output_file);
                }
                
            } else {
                if (text[index_array[i]-1] == 0) {
                    if ((position_index = binary_Search(delimiter_position, 0, delimiter_number-1, index_array[i]-1)) == -1) {
                        printf("binary search error!!\n");
                        exit(1);
                    } else {
                        fwrite(&position_index, sizeof(int), 1, positional_file);
                    }

                    fwrite(&delimiter, sizeof(char), 1, output_file);
                } else {
                    fwrite(&text[index_array[i]-1], sizeof(char), 1, output_file);
                }
            } 
        }
    }
    free(index_array);
}

int extract_bwt_file_name(const char *bwt_file_path) {
    int i = strlen(bwt_file_path) - 1;
    while (i >= 0) {
        if (bwt_file_path[i] == '/')
            return i + 1;
        i--;
    }
    return i;
}