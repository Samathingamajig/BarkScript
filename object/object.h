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

template <class T>
extern RuntimeResult notSupported(RuntimeResult& rt, const T& self, spObject other, const std::string& function, const std::string& extra = "");

template <class T>
extern RuntimeResult notSupportedNoRT(const T& self, spObject other, const std::string&& function, const std::string& extra = "");

template <class T>
extern RuntimeResult notSupportedNoRT(const T& self, const std::string&& function, const std::string& extra = "");

struct Object {
    std::string stringValue;
    double doubleValue = 0.0;
    bool sign = +1;
    bool isInfinity = false;
    bool isNaN = false;
    bool isPureDouble = false;
    bool isPureZero = false;
    std::vector<std::string> identifierList;
    spNode expression;

    std::string type = "UNKNOWN_OBJECT";
    Position positionStart;
    Position positionEnd;
    spContext context;

    void setPosition(const Position& positionStart, const Position& positionEnd);
    void setContext(const spContext& context);

    std::string virtual to_string() const { return "to_string is not implemented for type " + this->type; };
    bool virtual to_bool() const = 0;
    spObject virtual copy() const = 0;

    RuntimeResult virtual binary_plus(spObject other) { return notSupportedNoRT(this, other, "binary_plus"); };
    RuntimeResult virtual binary_minus(spObject other) { return notSupportedNoRT(this, other, "binary_minus"); };
    RuntimeResult virtual binary_asterisk(spObject other) { return notSupportedNoRT(this, other, "binary_asterisk"); };
    RuntimeResult virtual binary_f_slash(spObject other) { return notSupportedNoRT(this, other, "binary_f_slash"); };
    RuntimeResult virtual binary_double_asterisk(spObject other) { return notSupportedNoRT(this, other, "binary_double_asterisk"); };
    RuntimeResult virtual binary_double_f_slash(spObject other) { return notSupportedNoRT(this, other, "binary_double_f_slash"); };

    RuntimeResult virtual unary_plus() { return notSupportedNoRT(this, "unary_plus"); };
    RuntimeResult virtual unary_minus() { return notSupportedNoRT(this, "unary_minus"); };

    RuntimeResult unary_bang() const;

    RuntimeResult virtual binary_double_equal(spObject other) { return notSupportedNoRT(this, other, "binary_double_equal"); };
    RuntimeResult virtual binary_bang_equal(spObject other) { return notSupportedNoRT(this, other, "binary_bang_equal"); };
    RuntimeResult virtual binary_less_than(spObject other) { return notSupportedNoRT(this, other, "binary_less_than"); };
    RuntimeResult virtual binary_less_than_equal(spObject other) { return notSupportedNoRT(this, other, "binary_less_than_equal"); };
    RuntimeResult virtual binary_greater_than(spObject other) { return notSupportedNoRT(this, other, "binary_greater_than"); };
    RuntimeResult virtual binary_greater_than_equal(spObject other) { return notSupportedNoRT(this, other, "binary_greater_than_equal"); };

    RuntimeResult toOther(spObject other) const;
    RuntimeResult virtual toNumber() const { return notSupportedNoRT(this, "toNumber"); }
    RuntimeResult virtual toBoolean() const { return notSupportedNoRT(this, "toBoolean"); }
    RuntimeResult toNull() const;

    virtual operator spObject() = 0;
    // All children should have the code below
    //operator spObject() override {
    //    return makeSharedObject(*this);
    //}
};

struct Number : Object {
    Number() { this->type = "Number"; };
    Number(const double& value, const bool sign = +1);
    Number(const std::string& value, const bool sign = +1);

    std::string to_string() const override;
    bool to_bool() const override;
    spObject copy() const override;

    RuntimeResult binary_plus(spObject other) override;
    RuntimeResult binary_minus(spObject other) override;
    RuntimeResult binary_asterisk(spObject other) override;
    RuntimeResult binary_f_slash(spObject other) override;
    RuntimeResult binary_double_asterisk(spObject other) override;
    RuntimeResult binary_double_f_slash(spObject other) override;

    RuntimeResult unary_plus() override;
    RuntimeResult unary_minus() override;

    RuntimeResult binary_double_equal(spObject other) override;
    RuntimeResult binary_bang_equal(spObject other) override;
    RuntimeResult binary_less_than(spObject other) override;
    RuntimeResult binary_less_than_equal(spObject other) override;
    RuntimeResult binary_greater_than(spObject other) override;
    RuntimeResult binary_greater_than_equal(spObject other) override;

    RuntimeResult toNumber() const override;
    RuntimeResult toBoolean() const override;

    operator spObject() override {
        return makeSharedObject(*this);
    }
};

struct Boolean : Number {
    Boolean() { this->type = "Boolean"; this->isPureZero = 0; this->isPureDouble = true; }
    Boolean(const bool value);

    std::string to_string() const override;
    bool to_bool() const override;
    spObject copy() const override;

    RuntimeResult toNumber() const override;
    RuntimeResult toBoolean() const override;

    operator spObject() override {
        return makeSharedObject(*this);
    }
};

struct Null : Object {
    Null() { this->type = "Null"; }

    std::string to_string() const override;
    bool to_bool() const override;
    spObject copy() const override;

    RuntimeResult binary_double_equal(spObject other) override;
    RuntimeResult binary_bang_equal(spObject other) override;

    operator spObject() override {
        return makeSharedObject(*this);
    }
};

struct LambdaFunction : Object {
    LambdaFunction();
    LambdaFunction(const std::vector<std::string>& identifierList, const spNode& expression);

    std::string to_string() const override;
    bool to_bool() const override;
    spObject copy() const override;

    //RuntimeResult binary_double_equal(spObject other) override;
    //RuntimeResult binary_bang_equal(spObject other) override;

    operator spObject() override {
        return makeSharedObject(*this);
    }
};

#endif // !OBJECT_H
