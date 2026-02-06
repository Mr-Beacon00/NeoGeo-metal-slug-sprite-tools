#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// read 1 byte from file
uint8_t read_uint8(FILE *file) {
    uint8_t value;
    if (fread(&value, sizeof(uint8_t), 1, file) != 1) {
        perror("failed to read byte");
        exit(EXIT_FAILURE);
    }
    return value;
}

// read 2 bytes from file
uint16_t read_uint16(FILE *file) {
    uint16_t value;
    if (fread(&value, sizeof(uint16_t), 1, file) != 1) {
        perror("failed to read byte");
        exit(EXIT_FAILURE);
    }
    return value;
}

// read pal file and return the data
void read_pal(uint16_t addr, char *pal_index, uint8_t *buffer) {
    FILE *file = fopen(pal_index, "rb");
    if (!file) {
        perror("failed to open pal file");
        exit(EXIT_FAILURE);
    }

    if (fseek(file, addr, SEEK_SET) != 0) {
        perror("fseek failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (fread(buffer, 1, 2, file) != 2) {
        perror("fread failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input index file> <input pal file> <output file>\n", argv[0]);
        return 1;
    }

    char *input_index_file = argv[1];
    char *input_pal_file = argv[2];
    char *output_file = argv[3];

    FILE *file = fopen(input_index_file, "rb");
    if (!file) {
        perror("failed to open pal index");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calculate the total number of entries
    long num_entries = file_size / 4;

    // Allocate buffer to hold all the data
    uint8_t *buffer = (uint8_t *)malloc(num_entries * 2);
    if (!buffer) {
        perror("failed to allocate buffer");
        fclose(file);
        return EXIT_FAILURE;
    }

    uint8_t *current_buffer_pos = buffer;

    for (long i = 0; i < file_size; i += 4) {
        uint8_t index1 = read_uint8(file);
        uint8_t index2 = read_uint8(file);
        fseek(file, 1, SEEK_CUR);
        uint8_t index3 = read_uint8(file);
        uint16_t result = index1 * 0x40 + index2 * 0x800 + index3 * 0x02;

        read_pal(result, input_pal_file, current_buffer_pos);
        current_buffer_pos += 2;
    }

    fclose(file);

    // Write the entire buffer to the output file
    FILE *output_name = fopen(output_file, "wb");
    if (!output_name) {
        perror("output failed");
        free(buffer);
        return EXIT_FAILURE;
    }

    if (fwrite(buffer, 1, num_entries * 2, output_name) != num_entries * 2) {
        perror("fwrite failed");
    }

    fclose(output_name);
    free(buffer);

    return EXIT_SUCCESS;
}
