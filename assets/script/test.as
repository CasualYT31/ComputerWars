void main(string &in param, int8 negative, uint64 positive, bool t) {
	info(param + "," + negative + "," + positive + "," + t);
	info("" + getJoystickID());
	setJoystickID(1);
	info("" + getJoystickAxisThreshold());
	setJoystickAxisThreshold(89.6);
	info("" + getJoystickAxisThreshold());
	loadUIConfig();
	info("" + getJoystickAxisThreshold());
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