void main() {
	int var = 5;
	info("Hello, World!\n");
	warn("New line character used!");
	setSoundVolume(89.5);
	error("Error in script! " + getSoundVolume());
	setMusicVolume(-9);
	warn("Music volume: " + getMusicVolume());
	// printno(2+2);
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