#include <iostream>
#include <cmath>
#include <ctime>
#include <stdlib.h>

using namespace std;

typedef unsigned long long DWORD64;

DWORD64 prng_key;

int randVal(void)
{
	double res1;
	double d2;
	double s_val;
	DWORD64 prod;

	d2 = prng_key;
	prod = prng_key * 0x64236735;

	s_val = sin(d2);

	res1 = (( ((prod + s_val) * d2) + 0.737565675 ) * d2 );
	res1+= log(d2);

	*(double*)&prng_key = res1;

	return abs((int)prng_key);
}

unsigned long long get_timestamp() {
	time_t t = time(0);   // get time now
    struct tm *now = gmtime(&t);
    now->tm_sec = 0;
    now->tm_min = 0;
    now->tm_hour = 0;

    return mktime(now);
}

void print_domains() {
	string tldList[5] = { "com", "net", "org", "info", "biz" };

	unsigned long long timestamp = get_timestamp();
	unsigned long long filetime = (timestamp + 11644473600LL) * 10000000LL;

	prng_key = filetime * 0x463DA5676LL / 0x058028E44000LL + 0x0B46A7637LL;

	for(int i = 0; i < 250; i++) {
		int n = (randVal() % 4) + 8;
		for(int j = 0; j < n; j++) {
			cout << (char)((randVal() % 26) + 97);
		}
		cout << ".";
		cout << tldList[randVal() % 5] << endl;
	}
}

int main() {
	print_domains();
}
