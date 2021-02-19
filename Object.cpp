#include "object.h"
#include <math.h>
#include <sstream>
#include "interpreter.h"

bool didOverflow(double value) {
    return std::numeric_limits<double>::max() < value;
}

bool didUnderflow(double value) {
    return -std::numeric_limits<double>::max() > value;
}

template <class T>
RuntimeResult notImplemented(RuntimeResult rt, T self, spObject other, std::string function) {
    return rt.failure(makeSharedError(RuntimeError(self->positionStart, other->positionEnd, function + " is not implemented between " + self->type + " and " + other->type + "!", self->context)));
}

void Object::setPosition(Position positionStart, Position positionEnd) {
    this->positionStart = positionStart;
    this->positionEnd = positionEnd;
}

void Object::setContext(spContext context) {
    this->context = context;
}

Number::Number(double value, bool sign) {
    this->type = "Number";
    this->doubleValue = value;
    this->isPureDouble = true;
    if (value < 0.0) this->sign = -0;
    if (value == 0) this->isPureZero = true;
}

Number::Number(std::string value, bool sign) {
    this->type = "Number";
    this->sign = sign;
    if (value == "Infinity") {
        this->isInfinity = true;
    } else if (value == "NaN") {
        this->isNaN = true;
    } else {
        try {
            this->doubleValue = std::stod(value);
            this->isPureDouble = true;
            if (this->doubleValue == 0) this->isPureZero = true;
        } catch (const std::out_of_range&) {
            this->isInfinity = true;
        }
    }
}

std::string Number::to_string() {
    // https://stackoverflow.com/a/16606128/12101554
    if (isInfinity) {
        std::string sign;
        if (this->sign == -0) sign += "-";
        return sign + "Infinity";
    } else if (isNaN) {
        return "NaN";
    }
    std::ostringstream out;
    out.precision(16);
    out << doubleValue;
    return out.str();
}

RuntimeResult Number::binary_plus(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) return notImplemented(rt, this, other, "binary_plus");

    if (this->isNaN || other->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity || other->isInfinity) {
        if (this->sign == other->sign) {
            return rt.success(makeSharedObject(Number("Infinity", this->sign)));
        } else {
            // Infinity + -Infinity, and -Infinity + Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(makeSharedObject(Number("NaN", 0)));
        }
    }
    double result = this->doubleValue + other->doubleValue;
    if (didOverflow(result)) return rt.success(makeSharedObject(Number("Infinity", +1)));
    if (didUnderflow(result)) return rt.success(makeSharedObject(Number("Infinity", -0)));
    return rt.success(makeSharedObject(Number(result)));
}

RuntimeResult Number::binary_minus(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) return notImplemented(rt, this, other, "binary_minus");

    if (this->isNaN || other->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity || other->isInfinity) {
        if (this->sign != other->sign) {
            return rt.success(makeSharedObject(Number("Infinity", this->sign)));
        } else {
            // Infinity - Infinity, and -Infinity - -Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(makeSharedObject(Number("NaN")));
        }
    }
    double result = this->doubleValue - other->doubleValue;
    if (didOverflow(result)) return rt.success(makeSharedObject(Number("Infinity", +1)));
    if (didUnderflow(result)) return rt.success(makeSharedObject(Number("Infinity", -0)));
    return rt.success(makeSharedObject(Number(result)));
}

RuntimeResult Number::binary_asterisk(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) return notImplemented(rt, this, other, "binary_asterisk");

    if (this->isNaN || other->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity || other->isInfinity) {
        if (this->isPureZero || other->isPureZero) return rt.success(makeSharedObject(Number("NaN")));
        // a == b is the same as !(a ^ b)
        // this->sign == other->sign
        //       0    ==         0    -> 1
        //       0    ==         1    -> 0
        //       1    ==         0    -> 0
        //       1    ==         1    -> 1
        return rt.success(makeSharedObject(Number("Infinity", this->sign == other->sign)));
    }
    double result = this->doubleValue * other->doubleValue;
    if (didOverflow(result)) return rt.success(makeSharedObject(Number("Infinity", +1)));
    if (didUnderflow(result)) return rt.success(makeSharedObject(Number("Infinity", -0)));
    return rt.success(makeSharedObject(Number(result)));
}

RuntimeResult Number::binary_f_slash(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) return notImplemented(rt, this, other, "binary_f_slash");

    if (other->isPureZero) return rt.failure(makeSharedError(RuntimeError(other->positionStart, other->positionEnd, "Division by 0", this->context)));
    if (this->isNaN || other->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity && other->isInfinity) return rt.success(makeSharedObject(Number("NaN")));
    if (other->isInfinity) return rt.success(makeSharedObject(Number(0, this->sign == other->sign)));
    if (this->isInfinity) return rt.success(makeSharedObject(Number("Infinity", this->sign == other->sign)));
    double result = this->doubleValue / other->doubleValue;
    if (didOverflow(result)) return rt.success(makeSharedObject(Number("Infinity", +1)));
    if (didUnderflow(result)) return rt.success(makeSharedObject(Number("Infinity", -0)));
    return rt.success(makeSharedObject(Number(result)));
}

RuntimeResult Number::binary_double_asterisk(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) return notImplemented(rt, this, other, "binary_double_asterisk");

    if (this->isNaN || other->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (other->isPureZero) return rt.success(makeSharedObject(Number(1)));
    if (other->isInfinity && other->sign == +1) return rt.success(makeSharedObject(Number("Infinity")));
    if (other->isInfinity && other->sign == -0) return rt.success(makeSharedObject(Number(0)));
    if (this->isInfinity && other->sign == +1) return rt.success(makeSharedObject(Number("Infinity", this->sign)));
    if (this->isInfinity && other->sign == -0) return rt.success(makeSharedObject(Number(0)));
    double result = std::pow(this->doubleValue, other->doubleValue);
    if (didOverflow(result)) return rt.success(makeSharedObject(Number("Infinity", +1)));
    if (didUnderflow(result)) return rt.success(makeSharedObject(Number("Infinity", -0)));
    return rt.success(makeSharedObject(Number(result)));
}

RuntimeResult Number::binary_double_f_slash(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) return notImplemented(rt, this, other, "binary_double_f_slash");

    if (other->isPureZero) return rt.failure(makeSharedError(RuntimeError(other->positionStart, other->positionEnd, "Floored division by 0", this->context)));
    spObject normalDivisionResult = rt.registerRT(this->binary_f_slash(other));
    if (rt.hasError()) return rt;
    if (!normalDivisionResult->isPureDouble) return rt;
    double result = std::floor(normalDivisionResult->doubleValue);
    if (didOverflow(result)) return rt.success(makeSharedObject(Number("Infinity", +1)));
    if (didUnderflow(result)) return rt.success(makeSharedObject(Number("Infinity", -0)));
    return rt.success(makeSharedObject(Number(result)));
}

RuntimeResult Number::unary_plus() {
    RuntimeResult rt;

    if (isNaN) return rt.success(makeSharedObject(Number("NaN")));
    else if (isInfinity) return rt.success(makeSharedObject(Number("Infinity", sign)));
    return rt.success(makeSharedObject(Number(this->doubleValue)));
}

RuntimeResult Number::unary_minus() {
    RuntimeResult rt;

    if (isNaN) return rt.success(makeSharedObject(Number("NaN")));
    else if (isInfinity) return rt.success(makeSharedObject(Number("Infinity", !sign)));
    return rt.success(makeSharedObject(Number(this->doubleValue * -1)));
}
