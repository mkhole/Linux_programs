#include <stdio.h>
#include <stdint.h>


uint32_t byte_swap(uint32_t x) {
	return ((x & 0xff000000) >> 24 | (x & 0xff0000) >> 8 | (x & 0xff00) << 8| (x & 0xff) << 24);
}

void show_bytes(uint32_t *x) {
	uint8_t *p = (uint8_t *)x;
	for(int i=0; i<4; i++)
		printf("%p %02x\n", p, *p++);
}

int main() {
	uint32_t a = 0x12345678;

	printf("former:\n");
	show_bytes(&a);

	printf("latter:\n");
	a = byte_swap(a);
	show_bytes(&a);

	return 0;
}
