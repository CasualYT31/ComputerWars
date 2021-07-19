void print(const string& in str) {
	info(str);
}

void overloadedFunction(int x) {
	print("scriptanswer: " + x);
}

void overloadedFunction(float x) {
	print("scriptanswer: " + x);
}

void printInt(int x) {
	print("scriptanswer: " + x);
}

void printFloat(float x) {
	print("scriptanswer: " + x);
}

void add(int16 x, int16 y) {
	print("scriptanswer: " + (x + y));
}