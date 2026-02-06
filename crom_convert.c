#include <stdio.h>
#include <stdint.h>

void print_bits(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i != 0) {
            printf(" ");  // Add space every 4 bits for readability
        }
    }
    printf("\n");
}

uint32_t swap_bits(uint32_t value) {
    uint32_t swapped = 0;
    int src_bits[] = {7, 15, 23, 31, 6, 14, 22, 30, 5, 13, 21, 29, 4, 12, 20, 28, 3, 11, 19, 27, 2, 10, 18, 26, 1, 9, 17, 25, 0, 8, 16, 24};
    int dest_bits[] = {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    for (int i = 0; i < 32; ++i) {
        swapped |= ((value >> src_bits[i]) & 1) << dest_bits[i];
    }

    return swapped;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input C rom 1> <input C rom 2> <output_file>\n", argv[0]);
        return 1;
    }

    const char *input_file1 = argv[1];
    const char *input_file2 = argv[2];
    const char *output_file = argv[3];

    FILE *file1 = fopen(input_file1, "rb");
    FILE *file2 = fopen(input_file2, "rb");
    FILE *outFile = fopen(output_file, "wb");

    if (file1 == NULL || file2 == NULL || outFile == NULL) {
        perror("Error opening files");
        return 1;
    }

    uint8_t byte1[2];
    uint8_t byte2[2];
    int8_t postype = 1;
    uint8_t waiting_i_clear = 0;
    uint16_t i=0;
    while (fread(byte1, 1, 2, file1) == 2 && fread(byte2, 1, 2, file2) == 2) {
        // Merge the bytes into a 32-bit integer
        uint32_t mergedValue = 0;
        mergedValue |= (uint32_t)byte1[0] << 24;
        mergedValue |= (uint32_t)byte1[1] << 16;
        mergedValue |= (uint32_t)byte2[0] << 8;
        mergedValue |= (uint32_t)byte2[1];

        // Swap the bits
        uint32_t swappedValue = swap_bits(mergedValue);
        if (i==16) {
            fseek(outFile, -128, SEEK_CUR);
            postype=-postype;
            //printf("r!!!!\n");

        }

        if (i==32) {
            i = 0;
            postype=-postype;
        }

        //printf("i and postype: %u, %d\n", i,postype);
        if (postype == 1) {
            fseek(outFile, 4, SEEK_CUR);
            fwrite(&swappedValue, sizeof(uint32_t), 1, outFile);
            //printf("section2 Value: %x\n",swappedValue);
        } else if (postype == -1) {
            fwrite(&swappedValue, sizeof(uint32_t), 1, outFile);
            fseek(outFile, 4, SEEK_CUR);
            //printf("section1 Value: %x\n",swappedValue);
        }
        // printf("i is: %u\n", i);
        i++;
    }

    // Close the files
    fclose(file1);
    fclose(file2);
    fclose(outFile);

    return 0;
}
