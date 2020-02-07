#include <stdio.h>
#include <stdint.h>


int check_is_little_endian() {
	uint32_t a = 1;
	return (*(uint8_t *)&a == 1);
}

int main() {
	uint32_t a = 0x12345678;
	uint8_t *p = (char *)&a;

	printf("%02x\n", *p);
	printf("%s\n", check_is_little_endian()?"yes":"no");

	return 0;
}
