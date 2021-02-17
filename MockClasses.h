#ifndef ADVANCED_EX4_MOCKCLASSES_H
#define ADVANCED_EX4_MOCKCLASSES_H

class A {
public:
    bool prop;
    virtual ~A() = default;

    A(bool prop = true) : prop(prop) {}
};

class B : public A {
public:
    B(bool prop = true) : A(prop) {}
};

class C : public A {
public:
    C(bool prop = true) : A(prop) {}
};

#endif //ADVANCED_EX4_MOCKCLASSES_H
