#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
// swap palette bits to tileggd compatible
original bits: abcdefghijklmnop
swaped bits: aefghbijklcmnopd
*/

uint16_t swap_bits(uint16_t value) {
    uint16_t swapped = 0;
    swapped |= ((value >> 15) & 1) << 15;
    swapped |= ((value >> 10) & 1) << 14;
    swapped |= ((value >> 5) & 1) << 13;
    swapped |= ((value >> 0) & 1) << 12;
    swapped |= ((value >> 14) & 1) << 11;
    swapped |= ((value >> 13) & 1) << 10;
    swapped |= ((value >> 12) & 1) << 9;
    swapped |= ((value >> 11) & 1) << 8;
    swapped |= ((value >> 9) & 1) << 7;
    swapped |= ((value >> 8) & 1) << 6;
    swapped |= ((value >> 7) & 1) << 5;
    swapped |= ((value >> 6) & 1) << 4;
    swapped |= ((value >> 4) & 1) << 3;
    swapped |= ((value >> 3) & 1) << 2;
    swapped |= ((value >> 2) & 1) << 1;
    swapped |= ((value >> 1) & 1) << 0;
    return swapped;
}

// reverse swap
uint16_t swap_bits_2(uint16_t value) {
    uint16_t swapped = 0;
    swapped |= ((value >> 15) & 1) << 15;
    swapped |= ((value >> 11) & 1) << 14;
    swapped |= ((value >> 10) & 1) << 13;
    swapped |= ((value >> 9) & 1) << 12;
    swapped |= ((value >> 8) & 1) << 11;
    swapped |= ((value >> 14) & 1) << 10;
    swapped |= ((value >> 7) & 1) << 9;
    swapped |= ((value >> 6) & 1) << 8;
    swapped |= ((value >> 5) & 1) << 7;
    swapped |= ((value >> 4) & 1) << 6;
    swapped |= ((value >> 13) & 1) << 5;
    swapped |= ((value >> 3) & 1) << 4;
    swapped |= ((value >> 2) & 1) << 3;
    swapped |= ((value >> 1) & 1) << 2;
    swapped |= ((value >> 0) & 1) << 1;
    swapped |= ((value >> 12) & 1) << 0;
    return swapped;
}

int main(int argc, char *argv[]) {
    if (!(argc == 4)) {
        fprintf(stderr, "Usage: %s <input file> <output file> <swap/rev>\n", argv[0]);
        return 1;
    }
    char *input_file = argv[1];
    char *output_file = argv[2];
    char *operat = argv[3];
    FILE *infile = fopen(input_file, "rb");
    if (!infile) {
        perror("Error opening input file");
        return 1;
    }

    FILE *outfile = fopen(output_file, "wb");
    if (!outfile) {
        perror("Error opening output file");
        fclose(infile);
        return 1;
    }

    uint16_t buffer;
    size_t bytes_read;

    while ((bytes_read = fread(&buffer, sizeof(buffer), 1, infile)) == 1) {
		uint16_t swapped_value;
		if (strcmp(operat, "swap")) {
			swapped_value = swap_bits(buffer);
		} else if (operat, "rev") {
			swapped_value = swap_bits_2(buffer);
		} else {
			printf("nah\n");
		}

        fwrite(&swapped_value, sizeof(swapped_value), 1, outfile);
    }

    fclose(infile);
    fclose(outfile);

    return 0;
}
