# README

## encode

Firstyly, I read the original file and store it in a char array and change all the delimiter to the char 0, so that when I sort the sentence, the delimiter has the highest priority. Then, according to the frequency of each character, I split the file into three small temporary files and after sorting these three files, I can merge them directly without comparation.  

The sort function I used is `qsort()` in c library which has been optimised and has a good performance. The only problem is that when I pass the arguments to `qsort() ` , it would automatically make a copy of the arguments, which would consume a large amount of memory and this is why I split into three files rather than two.  Sort is according to the lexicographic order and the only different is when I compare two delimiters, the former one is smaller(eg. The delimiter of the first sentence is smaller than the second one).

The sorted file is the Suffix array so I need to miners one for each position index and the result would be the BWT result for the original file.

I also keep a position file for searching. The content is the original sentence sequency of the delimiter in BWT result. 

## search

The first two steps before doing search is make the frequency table and occurrence table. 

The frequency table can be built by visiting BWT file once and count the frequency of each characters.

For the occurrence table, I store it in searching temporary file for every 1000 characters. When I need the occurrence number of one character, I just need to get the result from  temporary file and count the number from the left result.

For  `-m`,  according to the backward-search algorithm, the `Last - First + 1` would be the final result. 

For `-n`, `-a`,  I need to continue backward-search until reaching the delimiter. According to the position file created in encoding, I can know this delimiter is the result of witch sentence and store the result in a integer array.

For `-n`, the result is the number of non-duplicate number in this integer array. 

For `-a`, I just need to print all the non-duplicate number in increasing order.

For `-i`, because the sequence of BWT result is same as the sequence of original sentence, I just need to do the backward-search according the arguments and output the result in reverse order.





