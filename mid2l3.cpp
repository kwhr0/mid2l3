#include <cstdio>
#include <string>
#include <vector>

enum {
	TYPE_BASIC, TYPE_DATA, TYPE_ML
};

static int type, bodycount, sum;
static FILE *fo;

static void put(int c) {
	putc(c, fo);
	sum += c;
}

static void header(const char *filename, int _type) {
	int i;
	type = _type;
	for (i = 0; i < 90; i++) put(0xff);
	put(1);
	put(0x3c);
	sum = 0;
	put(0);
	put(0x14);
	for (i = 0; i < 8 && filename[i] > ' '; i++) put(filename[i]);
	while (i++ < 8) put(' ');
	put(type);
	put(type == TYPE_ML ? 0 : 0xff);
	put(type == TYPE_ML ? 0 : 0xff);
	for (i = 0; i < 9; i++) put(0);
	put(sum);
	for (i = 0; i < 4; i++) put(0);
}

static void footer() {
	int i;
	for (i = type == TYPE_ML ? 10 : 90; i > 0; i--) put(0xff);
	put(1);
	put(0x3c);
	put(0xff);
	put(0);
	put(0xff);
	for (i = 0; i < 4; i++) put(0);
}

static void body(const char *buf, int n) {
	int i;
	for (i = type == TYPE_ML && bodycount ? 10 : 90; i > 0; i--) put(0xff);
	put(1);
	put(0x3c);
	sum = 0;
	put(1);
	put(n);
	for (i = 0; i < n; i++) put(buf[i]);
	put(sum);
	for (i = 0; i < 4; i++) put(0);
}

static void writefile(const char *filename, int type, const char *buf, int len) {
	header(filename, type);
	for (int i = 0; i < len; i += 255)
		body(&buf[i], len - i < 255 ? len - i : 255);
	footer();
}

static void makeML(std::vector<char> &ml, int adr) {
	FILE *fi;
	if (!(fi = fopen("a.out", "rb"))) return;
	int c, i;
	for (i = 0; i < 5; i++) ml.push_back(0);
	for (i = 0; (c = getc(fi)) != EOF; i++)
		ml.push_back(c);
	fclose(fi);
	ml[1] = i >> 8;
	ml[2] = i;
	ml[3] = adr >> 8;
	ml[4] = adr;
	ml.push_back(0xff);
	ml.push_back(0);
	ml.push_back(0);
	ml.push_back(adr >> 8);
	ml.push_back(adr);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: mid2l3 <midi file>\n");
		return 1;
	}
	FILE *fi = fopen(argv[1], "rb");
	if (!fi) {
		fprintf(stderr, "cannot open MIDI file.\n");
		return 1;
	}
	std::vector<char> ml, data;
	int c;
	while ((c = getc(fi)) != EOF)
		data.push_back(c);
	fclose(fi);
	std::string path = getenv("HOME");
	if (!(fo = fopen((path + "/Desktop/music.l3").c_str(), "wb"))) return 1;
	std::string basic = "10 CLEAR,&HC000:LOADM \"CAS0:ML\":OPEN \"I\",#1,\"CAS0:TEST\":EXEC &HC000:CLOSE #1\r";
	writefile("MUSIC", TYPE_BASIC, &basic[0], basic.size());
	makeML(ml, 0xc000);
	if (!ml.size()) {
		fprintf(stderr, "cannot make machine language part.\n");
		return 1;
	}
	writefile("ML", TYPE_ML, &ml[0], ml.size());
	writefile("TEST", TYPE_DATA, &data[0], data.size());
	fclose(fo);
	return 0;
}
