#include "arraysum.h"

int sum(int inputArray[], int size) {
	int result = 0;
	for (int i = 0; i < size; ++i) {
		result += inputArray[i];
	}
	return result;
}
