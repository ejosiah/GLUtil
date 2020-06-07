struct stack {
	int size;
	int top;
	int data[256];
};

bool empty(stack stack) {
	return stack.size == 0;
}

void init(inout stack stack) {
	stack.top = 0;
	stack.size = 0;
}

void push(inout stack stack, int elm) {
	stack.top = stack.size;
	stack.size += 1;
	stack.data[stack.top] = elm;
}

int pop(inout stack stack) {
	if (empty(stack)) return -1;
	int elm = stack.data[stack.top];
	stack.size = stack.top;
	stack.top -= 1;
	return elm;
}

int peek(inout stack stack) {
	if (empty(stack)) return -1;
	return stack.data[stack.top];
}