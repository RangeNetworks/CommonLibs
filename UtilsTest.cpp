#include <string>
#include <stdio.h>
#include "Configuration.h"
#include "Utils.h"
ConfigurationTable gConfig;	// Needed for logger.
using namespace std;


struct foo : public RefCntBase {
	string id;
	foo(string wid) : id(wid) { printf("create foo %s %p\n",id.c_str(),this); }
	~foo() { printf("delete foo %s %p\n",id.c_str(),this); }
};

typedef RefCntPointer<foo> fooref;

RefCntPointer<foo> refize(foo*f)
{
	fooref ref(f);
	return ref;
}

void test1()	// Basic test.
{
	foo *b = new foo("b");
	fooref bref(b);
	bref.free();	// Should free b.

}

void test2()	// assignment
{
	foo *c = new foo("c");
	fooref cref = refize(c);

	foo *d = new foo("d");
	fooref dref = d;
	fooref dref2 = d;

	dref = c;
	printf("here\n");
	// d and c deleted when test2 returns.
}

void test3()	// assignment
{
	foo *c = new foo("c");
	fooref cref = refize(c);

	foo *d = new foo("d");
	fooref dref = d;

	dref = c;
	printf("here\n");
	// d and c deleted when test2 returns.
}

void refcntpointertest()
{
	test1();
	test2();
	test3();
}

// Throw spaces into the string.
string messup(string in)
{
	string result;
	for (string::iterator it = in.begin(); it != in.end(); it++) {
		if (rand() & 1) result.push_back(' ');
		result.push_back(*it);
	}
	return result;
}

void asciiEncoderTest(bool verbose)
{
	char tbuf[102];		// binary data to try encoding and then decoding.
	int numtests = 0;
	printf("asciiEncoderTest...\n");
	for (unsigned len = 0; len < 100; len++) {	// Length of binary data.
		for (unsigned testno = 0; testno < 100; testno++) {
			numtests++;
			for (unsigned charno = 0; charno < len; charno++) {
				tbuf[charno] = rand();
			}
			string binaryinput = string(tbuf,len);

			for (unsigned type = 0; type < 2; type++) {
				const char *method = type ? "base64" : "hex";
				string errorMessage;
				string encoded = encodeToString(tbuf,len,method,errorMessage);
				if (testno & 1) encoded = messup(encoded);
				string decoded = decodeToString(encoded.data(),encoded.size(),method,errorMessage);

				// Check our results.
				if (decoded.size() != len) { printf("FAIL %s len=%d size=%d encoded=%s\n",method,len,decoded.size(),encoded.c_str()); }
				if (binaryinput != decoded) { printf("FAIL %s testno=%d len=%d encoded=%s\n",method,testno,len,encoded.c_str()); }
				// Paranoid double check manually:
				for (unsigned j = 0; j < len; j++) {
					if (tbuf[j] != decoded[j]) { printf("FAIL %s len=%d j=%d\n",method,len,j); }
				}
				if (errorMessage.size()) {
					printf("%s error message=%s\n",method,errorMessage.c_str());
				}
				if (verbose) printf("TEST %s len=%d test=%d encoded=%s\n",method,len,testno,encoded.c_str());
			}

			/****
			{	// Hex encoder/decoder tests.
				string errorMessage = "";
				string hexencoded = encodeToString(tbuf,len,"hex",errorMessage);
				if (hexencoded.size() != 2 * len) {
					printf("FAIL hex encoded size=%d expected=%d\n",hexencoded.size(),2*len);
				}
				string hexdecoded = decodeToString(hexencoded.data(),hexencoded.size(),"hex",errorMessage);
				//printf("input size=%d hexencoded size=%d hexdecoded size=%d %s\n",len,hexencoded.size(),hexdecoded.size(),hexencoded.c_str());
				string e2 = encodeToString(hexdecoded.data(),hexdecoded.size(),"hex",errorMessage);
				//printf("in=%s out=%s\n",hexencoded.c_str(),e2.c_str());
				if (e2 != hexencoded) {
					printf("FAIL hex here\n");
				}
				if (binaryinput != hexdecoded) { printf("FAIL hex testno=%d len=%d\n",testno,len); }
				if (errorMessage.size()) {
					printf("base64 error message=%s\n",errorMessage.c_str());
				}
			}
			****/
		}
	}
	printf("PASS %d tests\n",numtests);
}


int main(int argc, char **argv)
{
	bool verbose = argc > 1;
	printf("Utils Test starting.\n");
	refcntpointertest();
	asciiEncoderTest(verbose);
}
