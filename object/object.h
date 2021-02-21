#pragma once
#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include <memory>
#include "../token/token.h"
#include "../position/position.h"
#include "../context/context.h"
#include "../interpreter/interpreter.h"

namespace objecttypes {
    using namespace std;

    const string Number = "Number";
}

struct Object;

typedef std::shared_ptr<Object> spObject;

// Polymorphism without having to cast to unknown types later on
// https://stackoverflow.com/a/42539569/12101554
// https://ideone.com/4jdhfZ
template<class ObjectType>
spObject makeSharedObject(ObjectType&& object) {
    return std::make_shared<std::remove_reference_t<ObjectType>>(std::forward<ObjectType>(object));
}

struct RuntimeResult;

struct Object {
    std::string stringValue;
    double doubleValue = 0.0;
    bool sign = +1;
    bool isInfinity = false;
    bool isNaN = false;
    bool isPureDouble = false;
    bool isPureZero = false;

    std::string type = "UNKNOWN_OBJECT";
    Position positionStart;
    Position positionEnd;
    spContext context;

    void setPosition(Position positionStart, Position positionEnd);
    void setContext(spContext context);

    std::string virtual to_string() { return "to_string is not implemented for type " + this->type; };
    spObject virtual copy() = 0;

    RuntimeResult virtual binary_plus(spObject other) { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, other->positionEnd, "binary_plus for " + this->type + " is not implemented!", this->context))); };
    RuntimeResult virtual binary_minus(spObject other) { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, other->positionEnd, "binary_minus for " + this->type + " is not implemented!", this->context))); };
    RuntimeResult virtual binary_asterisk(spObject other) { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, other->positionEnd, "binary_asterisk for " + this->type + " is not implemented!", this->context))); };
    RuntimeResult virtual binary_f_slash(spObject other) { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, other->positionEnd, "binary_f_slash for " + this->type + " is not implemented!", this->context))); };
    RuntimeResult virtual binary_double_asterisk(spObject other) { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, other->positionEnd, "binary_double_asterisk for " + this->type + " is not implemented!", this->context))); };
    RuntimeResult virtual binary_double_f_slash(spObject other) { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, other->positionEnd, "binary_double_f_slash for " + this->type + " is not implemented!", this->context))); };

    RuntimeResult virtual unary_plus() { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, this->positionEnd, "unary_plus for " + this->type + " is not implemented!", this->context))); };
    RuntimeResult virtual unary_minus() { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, this->positionEnd, "unary_minus for " + this->type + " is not implemented!", this->context))); };

    RuntimeResult toOther(spObject other);
    RuntimeResult virtual toNumber() { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, this->positionEnd, "toNumber for " + this->type + " is not implemented!", this->context))); }
    RuntimeResult virtual toBoolean() { return RuntimeResult().failure(makeSharedError(RuntimeError(this->positionStart, this->positionEnd, "toBoolean for " + this->type + " is not implemented!", this->context))); }
    RuntimeResult toNull();
};

struct Number : Object {
    Number() { this->type = "Number"; };
    Number(double value, bool sign = +1);
    Number(std::string value, bool sign = +1);

    std::string to_string() override;
    spObject copy() override;

    RuntimeResult binary_plus(spObject other) override;
    RuntimeResult binary_minus(spObject other) override;
    RuntimeResult binary_asterisk(spObject other) override;
    RuntimeResult binary_f_slash(spObject other) override;
    RuntimeResult binary_double_asterisk(spObject other) override;
    RuntimeResult binary_double_f_slash(spObject other) override;

    RuntimeResult unary_plus() override;
    RuntimeResult unary_minus() override;

    RuntimeResult toNumber() override;
    RuntimeResult toBoolean() override;
};

struct Boolean : Number {
    Boolean() { this->type = "Boolean"; this->isPureZero = 0; this->isPureDouble = true; }
    Boolean(bool value);

    std::string to_string() override;
    spObject copy() override;

    RuntimeResult toNumber() override;
    RuntimeResult toBoolean() override;
};

struct Null : Object {
    Null() { this->type = "Null"; }

    std::string to_string() override;
    spObject copy() override;

    RuntimeResult binary_plus(spObject other) override;
    RuntimeResult binary_minus(spObject other) override;
    RuntimeResult binary_asterisk(spObject other) override;
    RuntimeResult binary_f_slash(spObject other) override;
    RuntimeResult binary_double_asterisk(spObject other) override;
    RuntimeResult binary_double_f_slash(spObject other) override;

    RuntimeResult unary_plus() override;
    RuntimeResult unary_minus() override;

    RuntimeResult toNumber() override;
    RuntimeResult toBoolean() override;
};

#endif // !OBJECT_H
