
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define OCC_SIZE 1000

int extract_bwt_file_name(const char *bwt_file_path);
int Occ(char c, int num, FILE* occ_file, FILE* bwt_file);
int backward_search(int *First, int *Last, char delimiter, const char *P, int *char_frequency, FILE* occ_file, FILE* bwt_file);
int get_next_char_frequency(int *char_frequency, char c);
int search_to_delimiter(int *char_frequency, char delimiter, int *num, FILE* occ_file, FILE* bwt_file, FILE *delimiter_position_file);

int record_printing(int *char_frequency, int num, char *result, FILE* occ_file, FILE* bwt_file);

int cmpfunc (const void * a, const void * b);


char delimiter;

int main(int argc, const char * argv[]) {
    if (strlen(argv[1]) == 2)
        delimiter = '\n';
    else
        delimiter = (char)argv[1][0];

    const char *bwt_result = argv[2];
    const char *temp_file_path_1 = argv[3];
    const char *argument = argv[4];
    const char *search_content = argv[5];

    char *temp_file_path = malloc(strlen(temp_file_path_1) + 1);
    sprintf(temp_file_path, "%s/", temp_file_path_1);

    // open occ file
    char *occ_file_path = malloc(strlen(temp_file_path) + 16);
    sprintf(occ_file_path, "%ssearch_file.tmp", temp_file_path);
    FILE *occ_file;
    // printf("occ_file_path is %s\n", occ_file_path);


    // open delimiter position file
    char *delimiter_position_file_path = malloc(strlen(bwt_result) + 12 + 1);
    sprintf(delimiter_position_file_path, "%sposiotion.aux", bwt_result);
    FILE *delimiter_position_file = fopen(delimiter_position_file_path, "r");
    // printf("delimiter_position_file_path is %s\n", delimiter_position_file_path);

    // open bwt file
    FILE *bwt_file = fopen(bwt_result, "r");

    int char_frequency[128] = {0};
    int fgetc_result;
    int occ_temp_count = 0;
    int i = 0;


    if (access(occ_file_path, F_OK) != -1) {
        occ_file = fopen(occ_file_path, "r");
        fseek(occ_file, -128 * sizeof(int), SEEK_END);
        fread(char_frequency, sizeof(int), 128, occ_file);
    } else {
        occ_file = fopen(occ_file_path, "w");
        while ((fgetc_result = fgetc(bwt_file)) != EOF) {
            if ((char) fgetc_result == delimiter) {
                char_frequency[0]++;
            } else {
                char_frequency[fgetc_result]++;
            }
            occ_temp_count++;
            if (occ_temp_count == OCC_SIZE) {
                fwrite(char_frequency, sizeof(int), 128, occ_file);
                occ_temp_count = 0;
                // printf("test\n");
            }
        }
        if (occ_temp_count) {
            fwrite(char_frequency, sizeof(int), 128, occ_file);
        }
        fclose(occ_file);
        occ_file = fopen(occ_file_path, "r");
    }
    

    int temp_sum = char_frequency[0];
    int temp, last_character;

    // for (i = 0; i < 128; i++) {
    //     if (char_frequency[i]) {
    //         printf("frequency of char %c is %d\n", (char)i, char_frequency[i]);
    //     }
    // }

    char_frequency[0] = 0;


    printf("\n");

    for (i = 0; i < 128; i++) {
        if (char_frequency[i]) {
            temp = char_frequency[i];
            char_frequency[i] = temp_sum;
            temp_sum = temp_sum + temp;
            printf("frequency of char %c is %d\n", (char)i, char_frequency[i]);
            last_character = i;
        }
    }
    char_frequency[last_character + 1] = temp_sum;
    // printf("frequency of char %c is %d\n", last_character + 1, temp_sum);

    if ((strcmp(argument, "-m") == 0) || (strcmp(argument, "-a") == 0) || (strcmp(argument, "-n") == 0)) {
        int First = 0;
        int Last = 0;
        char search_result_char = backward_search(&First, &Last, delimiter,search_content, char_frequency, occ_file, bwt_file);
        if (Last == -1) {
            printf("get_next_char_frequency bug!!\n");
            exit(1);
        }
        if (Last >= First) {
            // printf("search reasult is %d\n", Last - First + 1);
            // printf("Frist is %d, Last is %d\n", First, Last);
            // printf("Last char is %c\n", search_result_char);
            ;
        } else {
            printf("0\n");
            return 0;
        }
        // print -m result
        if (strcmp(argument, "-m") == 0) {
            printf("%d\n", Last - First + 1);
            return 0;
        }
        
        int *result_array = malloc((Last - First + 1) * sizeof(int));
        int temp_index = 0;
        for (i = First; i <= Last; i++) {
            int temp = i;
            int a = search_to_delimiter(char_frequency, delimiter, &temp, occ_file, bwt_file, delimiter_position_file);
            result_array[temp_index] = a;
            temp_index++;
        }
        qsort(result_array, Last - First + 1, sizeof(int), cmpfunc);
        // printf("back ward result is:\n");
        // for (i = 0; i < Last - First + 1; i++) {
        //     printf("%d ", result_array[i]);
        // }
        // printf("\n");

        if ((strcmp(argument, "-a") == 0)) {
            // printf("-a result:\n");
            int last = -1;
            for (i = 0; i < Last - First + 1; i++) {
                if (result_array[i] != last) {
                    printf("%d\n", result_array[i]);
                }
                last = result_array[i];
            }
            return 0;
        } else {
            // printf("-n result:\n");
            int last = -1;
            int result = 0;
            for (i = 0; i < Last - First + 1; i++) {
                if (result_array[i] != last) {
                   result++;
                }
                last = result_array[i];
            }
            printf("%d\n", result);
            return 0;
        }
        
    } else if ((strcmp(argument, "-i") == 0)) {
        int search_content_length = strlen(search_content);
        int current_num = 1;
        int *range = calloc(2, sizeof(int));
        int current_length = 0;
        
        for (i = search_content_length - 1; i >= 0; i--) {
            if (search_content[i] != ' ') {
                // printf("search_content[i] is %d\n", search_content[i]);
                range[current_num] = range[current_num] + (search_content[i] - '0') * (int)pow((double)10, (double)current_length);
                current_length++;
            } else {
                current_length = 0;
                current_num = 0;
            }
        }
        printf("start is %d, end is %d\n", range[0], range[1]);
        
        int j;
        char *result = malloc(6000 * sizeof(char));
        
        for (i = range[0]; i <= range[1]; i++) {
           
            int length = record_printing(char_frequency, i, result, occ_file, bwt_file);
            printf("the number of %d record: \n", i);
            for (j = length - 2; j >= 0; j--) {
                printf("%c", result[j]);
            }
            printf("\n");
            printf("\n");
        }
        
    }


        

    return 0;
}

int backward_search(int *First, int *Last, char delimiter, const char *P, int *char_frequency, FILE* occ_file, FILE* bwt_file) {
    int i = strlen(P);
    char c = P[i - 1];

    if (c == delimiter)
        c = 0;

    *First = char_frequency[c] + 1;
    *Last = get_next_char_frequency(char_frequency, c);

    while ((*First <= *Last) && (i >= 2)) {
        // printf("i = %d\n", i);
        // printf("c = %c\n", c);
        // printf("First = %d\n", *First);
        // printf("Last = %d\n", *Last);
        // printf("(Last - First + 1) = %d\n", *Last - *First + 1);
        // printf("\n");
        c = P[i - 2];
        if (c == delimiter)
            c = 0;

        *First = char_frequency[c] + Occ(c, *First - 1, occ_file, bwt_file) + 1;
        *Last = char_frequency[c] + Occ(c, *Last, occ_file, bwt_file);
        i--;
    }
    // printf("i = %d\n", i);
    // printf("c = %c\n", c);
    // printf("First = %d\n", *First);
    // printf("Last = %d\n", *Last);
    // printf("(Last - First + 1) = %d\n", *Last - *First + 1);
    // printf("\n");
    return c;
}

int record_printing(int *char_frequency, int num, char *result, FILE* occ_file, FILE* bwt_file) {
    int delimiter_number = get_next_char_frequency(char_frequency, 0);
    // printf("Record_printing: num is %d\n", num);
   
    fseek(bwt_file, num - 1, SEEK_SET);
    // printf("test\n");
    char temp_c;
    
    fread(&temp_c, sizeof(char), 1, bwt_file);
    result[0] = temp_c;
    int length = 1;
    
    // printf("Record: temp c is %c\n", temp_c);
    num = char_frequency[temp_c] + Occ(temp_c, num - 1, occ_file, bwt_file) + 1;
    
    // printf("Record: num is %d\n", num);
    while (num > delimiter_number) {
        fseek(bwt_file, num - 1, SEEK_SET);
        fread(&temp_c, sizeof(char), 1, bwt_file);
        result[length] = temp_c;
        length++;
        // printf("Record: temp c is %c\n", temp_c);
        if (temp_c == delimiter) {
            temp_c = 0;
        }
        num = char_frequency[temp_c] + Occ(temp_c, num - 1, occ_file, bwt_file) + 1;
        // printf("Record: num is %d\n", num);
    }
    return length;

    
}

int search_to_delimiter(int *char_frequency, char delimiter, int *num, FILE* occ_file, FILE* bwt_file, FILE *delimiter_position_file) {
    int delimiter_number = get_next_char_frequency(char_frequency, 0);
    // printf("Back_ward: delimiter number is %d\n", delimiter_number);
    // printf("Back_ward: start num is %d\n", *num);
    char temp_c;
    while (*num > delimiter_number) {
        fseek(bwt_file, *num - 1, SEEK_SET);
        fread(&temp_c, sizeof(char), 1, bwt_file);
        // printf("Back_ward: temp c is %c\n", temp_c);
        if (temp_c == delimiter) {
            temp_c = 0;
        }
        *num = char_frequency[temp_c] + Occ(temp_c, *num - 1, occ_file, bwt_file) + 1;
        // printf("Back_ward: temp num is %d\n", *num);
    }
    // printf("Back_ward: final num is %d\n", *num);

    fseek(delimiter_position_file, (*num - 1) * sizeof(int), SEEK_SET);
    int the_sequence_of_delimiter;
    fread(&the_sequence_of_delimiter, sizeof(int), 1, delimiter_position_file);
    // printf("Back_ward: the_sequence_of_delimiter is %d\n", the_sequence_of_delimiter);
    if (the_sequence_of_delimiter == delimiter_number) {
        the_sequence_of_delimiter = 1;
    } else {
        ++the_sequence_of_delimiter;
    }
    return the_sequence_of_delimiter;
}



int Occ(char c, int num, FILE* occ_file, FILE* bwt_file) {
    int frequency, i, fgetc_result;
    /* need to change c to delimiter */

    if (num >= OCC_SIZE) {
        fseek(occ_file, (num/OCC_SIZE - 1) * 512 + sizeof(int) * c, SEEK_SET);
        fread(&frequency, sizeof(int), 1, occ_file);
    } else {
        frequency = 0;
    }
    fseek(bwt_file, (num/OCC_SIZE) * OCC_SIZE, SEEK_SET);
    i = 0;
    // printf("Occ: init frequency is %d\n", frequency);
    for (; i < num - (num/OCC_SIZE) * OCC_SIZE; i++) {
        if ((fgetc_result = fgetc(bwt_file)) != EOF) {
            if (fgetc_result == delimiter) {
                fgetc_result = 0;
            }
            if ((char)fgetc_result == c) {
                frequency++;
            } 
        } else {
                printf("OCC bug!!!!!");
                exit(1);
        }    
    }
    // printf("Occ: char %c in %d is %d\n", c, num, frequency);
    return frequency;
}

int get_next_char_frequency(int *char_frequency, char c) {
    int i = 0;
    for (i = c + 1; i < 128; i++) {
        if (char_frequency[i]) {
            return char_frequency[i];
        }
    }
    return -1;
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

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}