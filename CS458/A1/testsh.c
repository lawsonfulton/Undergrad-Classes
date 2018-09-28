unsigned long get_sp(void) {
   __asm__("movl %esp,%eax");
}

void func() {
	printf("0x%x\n", get_sp());
}
void main() {
  func();
}