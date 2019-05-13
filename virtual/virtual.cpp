#include <iostream>

class Base
{
public:
	virtual void f() { 
		std::cout << "in BASE::f() \n"; 
		g(); 
	};
	virtual void g() {
		std::cout << "in BASE::g() \n";
	};
};

class Derived : public Base
{
public:

	virtual void f() { 
		std::cout << "in Derived::f() \n"; 
		//Base::f(); 
	};
	virtual void g() {
		std::cout << "in Derived::g() \n";
	};
};

int main()
{
	Base* pBase = new Derived;
	pBase->f();
	//pBase->g();
	return 0;
}