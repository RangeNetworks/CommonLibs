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



int main(int argc, char **argv)
{
	printf("Utils Test starting.\n");
	refcntpointertest();
}
