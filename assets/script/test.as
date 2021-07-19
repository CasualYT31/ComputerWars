enum TestingEnum {
	Val1,
	Val2
}

void main(string &in param, int8 negative, uint64 positive, bool t) {
	info(param + "," + negative + "," + positive + "," + t);
	
	joystick_axis test;
	test.axis = Val2;
	test.direction = -10000;
	info(test.axis + ", " + test.direction);
}

void add(int a, int b) {
	// printno(a+b);
}

void printFloat(float a) {
	// printfloat(a);
}

void settings_Button1_Pressed() {
	info("I was pressed! And I was changed!\n");
}