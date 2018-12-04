int Min(const int a, const int b){
	return a < b ? a : b;
}

int Max(const int a, const int b){
	return a > b ? a : b;
}

int Abs(const int a){
	return a < 0 ? -a : a;
}

int Clamp(const int a, const int min, const int max){
	return Min(max, Max(min, a));
}

int CopySign(const int value, const int sign){
	return (value < 0) ^ (sign < 0) ? -value : value;
}

//time
unsigned int TimerSet(const unsigned int length){
	return GetMasterClock() + length;
}

bool TimerPassed(const unsigned int timer){
	return timer <= GetMasterClock();
}


//rng
char Gen0To6(void){
	const static char random[] = {1, 6, 4, 5, 4, 1, 2, 1, 5, 4, 5, 6, 3, 2, 5, 4, 5, 0, 2, 1, 2, 1, 6, 4, 5, 5, 0, 3, 4, 3, 5, 1, 5, 4, 3, 2, 6, 4, 3, 5, 3, 1, 0, 4, 0, 4, 6, 2, 3, 3, 4, 2, 0, 3, 2, 4, 3, 6, 3, 3, 1, 4, 6, 4, 4, 0, 4, 2, 4, 6, 4, 4, 3, 0, 5, 3, 3, 6, 2, 2, 6, 1, 5, 4, 1, 2, 2, 5, 3, 4, 1, 1, 6, 6, 5, 5, 4, 6, 6, 5, 2, 5, 2, 3, 5, 5, 4, 3, 2, 5, 0, 1, 1, 2, 4, 1, 5, 2, 1, 6, 3, 6, 4, 1, 3, 6, 5, 2, 5, 4, 6, 3, 0, 0, 5, 0, 4, 1, 0, 2, 5, 3, 0, 0, 0, 3, 0, 3, 2, 5, 4, 2, 1, 1, 4, 4, 1, 4, 3, 1, 5, 0, 6, 1, 3, 6, 5, 3, 3, 4, 1, 6, 3, 2, 3, 4, 4, 4, 4, 3, 2, 0, 0, 4, 2, 4, 2, 5, 1, 5, 5, 0, 3, 5, 3, 5, 6, 4, 6, 4, 2, 1, 4, 2, 1, 1, 4, 5, 3, 4, 2, 2, 0, 3, 6, 2, 0, 2, 3, 3, 4, 3, 5, 3, 4, 5, 0, 1, 1, 0, 6, 4, 6, 6, 2, 3, 5, 5, 1, 2, 2, 4, 4, 4, 1, 2, 5, 2, 6, 6, 0, 5, 3, 3, 3};
	static unsigned char index = 0;
	return random[(index++ + GetMasterClock()) % 255];
}