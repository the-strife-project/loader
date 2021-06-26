extern "C" void _start() {
	asm volatile("xchgw %bx, %bx");
	while(true);
}
