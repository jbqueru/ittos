/*
 * Copyright 2024 Jean-Baptiste M. "JBQ" "Djaybee" Queru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * As an added restriction, if you make the program available for
 * third parties to use on hardware you own (or co-own, lease, rent,
 * or otherwise control,) such as public gaming cabinets (whether or
 * not in a gaming arcade, whether or not coin-operated or otherwise
 * for a fee,) the conditions of section 13 will apply even if no
 * network is involved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

// SPDX-License-Identifier: AGPL-3.0-or-later

#include <stdio.h>

unsigned char tga[192044];

int pix[64000];

unsigned char output[64000];

unsigned char lzoutput[64000 * 6];

int tgablue(int x, int y) {
	return tga[18 + (x + y * 320) * 3];
}

int tgagreen(int x, int y) {
	return tga[18 + (x + y * 320) * 3 + 1];
}

int tgared(int x, int y) {
	return tga[18 + (x + y * 320) * 3 + 2];
}

void main() {
	FILE* inputfile = fopen("out/gfx/jbq.tga", "rb");
	fread(tga, 1, 192044, inputfile);
	fclose(inputfile);

	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			pix[x + y * 320] = (tgared(x, y) + tgagreen(x, y) + tgablue(x, y)) * 8 / 766;
		}
	}

	for (int n = 0; n < 64000; n++) {
		output[n] = pix[n];
	}

	FILE* outputfile = fopen("out/gfx/pixels.bin", "wb");
	fwrite(output, 1, 64000, outputfile);
	fclose(outputfile);

	int s = 0;

	for (int target = 0; target < 64000; target++) {
		int beststart = 0;
		int bestlength = 0;
		for (int start = 0; start < target; start++) {
			int length;
			for (length = 0; length < 64000 - target; length++) {
				if (pix[start + length] != pix[target + length]) {
					break;
				}
				if (length >= bestlength) {
					bestlength = length;
					beststart = start;
				}
			}
		}
		if (bestlength == 0) {
//			printf("no match found for %d\n", target);
		} else {
			if (target + bestlength == 64000) {
//				printf("input overflow\n");
				bestlength--;
			}
//			printf("found match of %d pixels at %d for %d\n", bestlength, beststart, target);
		}

		lzoutput[s++] = bestlength >> 8;
		lzoutput[s++] = bestlength % 256;
		lzoutput[s++] = beststart >> 8;
		lzoutput[s++] = beststart % 256;
		lzoutput[s++] = pix[target + bestlength];
		s++;

		target += bestlength;
	}

	FILE* lzfile = fopen("out/gfx/lz.bin", "wb");
	fwrite(lzoutput, 1, s, lzfile);
	fclose(lzfile);
}
