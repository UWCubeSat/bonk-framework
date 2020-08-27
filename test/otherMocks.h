typedef std::string String;

int FAKE_millis = 0;

int millis() {
	return FAKE_millis;
}
