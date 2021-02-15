#include "object.h"
#include "interpreter.h"
void Object::setPosition(Position positionStart, Position positionEnd) {
    this->positionStart = positionStart;
    this->positionEnd = positionEnd;
}

void Object::setContext(spContext context) {
    this->context = context;
}

std::string Object::to_string() {
    return "Not implemented! " + type;
}

Number::Number(double value, bool sign) {
    this->type = "Number";
    this->doubleValue = value;
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

RuntimeResult Number::binary_plus(spObject object) {
    RuntimeResult rt;

    if (this->isNaN || object->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity || object->isInfinity) {
        if (this->sign == object->sign) {
            return rt.success(makeSharedObject(Number("Infinity", this->sign)));
        } else {
            // Infinity + -Infinity, and -Infinity + Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(makeSharedObject(Number("NaN", 0)));
        }
    }
    return rt.success(makeSharedObject(Number(this->doubleValue + object->doubleValue)));
}

RuntimeResult Number::binary_minus(spObject object) {
    RuntimeResult rt;

    if (this->isNaN || object->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity || object->isInfinity) {
        if (this->sign != object->sign) {
            return rt.success(makeSharedObject(Number("Infinity", this->sign)));
        } else {
            // Infinity - Infinity, and -Infinity - -Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(makeSharedObject(Number("NaN")));
        }
    }
    return rt.success(makeSharedObject(Number(this->doubleValue - object->doubleValue)));
}

RuntimeResult Number::binary_asterisk(spObject object) {
    RuntimeResult rt;

    if (this->isNaN || object->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity || object->isInfinity) {
        if (this->isPureZero || object->isPureZero) return rt.success(makeSharedObject(Number("NaN")));
        // a == b is the same as !(a ^ b)
        // this->sign == object->sign
        //       0    ==         0    -> 1
        //       0    ==         1    -> 0
        //       1    ==         0    -> 0
        //       1    ==         1    -> 1
        return rt.success(makeSharedObject(Number("Infinity", this->sign == object->sign)));
    }
    return rt.success(makeSharedObject(Number(this->doubleValue * object->doubleValue)));
}

RuntimeResult Number::binary_f_slash(spObject object) {
    RuntimeResult rt;

    if (this->isNaN || object->isNaN) return rt.success(makeSharedObject(Number("NaN")));
    if (this->isInfinity && object->isInfinity) return rt.success(makeSharedObject(Number("NaN")));
    if (object->isPureZero) return rt.failure(makeSharedError(RuntimeError(object->positionStart, object->positionEnd, "Division by 0", this->context)));
    if (object->isInfinity) return rt.success(makeSharedObject(Number(0, this->sign == object->sign)));
    if (this->isInfinity) return rt.success(makeSharedObject(Number("Infinity", this->sign == object->sign)));
    return rt.success(makeSharedObject(Number(this->doubleValue / object->doubleValue)));
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
