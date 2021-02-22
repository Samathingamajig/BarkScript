#include "object.h"
#include <math.h>
#include <sstream>
#include "../interpreter/interpreter.h"

bool didOverflow(double value) {
    return std::numeric_limits<double>::max() < value;
}

bool didUnderflow(double value) {
    return -std::numeric_limits<double>::max() > value;
}

template <class T>
RuntimeResult notImplemented(RuntimeResult rt, T self, spObject other, std::string function, std::string extra = "") {
    extra = extra.size() > 0 ? " (" + extra + ")" : "";
    return rt.failure(RuntimeError(self->positionStart, other->positionEnd, function + " is not implemented between " + self->type + " and " + other->type + "!" + extra, self->context));
}

void Object::setPosition(Position positionStart, Position positionEnd) {
    this->positionStart = positionStart;
    this->positionEnd = positionEnd;
}

void Object::setContext(spContext context) {
    this->context = context;
}

RuntimeResult Object::toOther(spObject other) {
    std::string type = other->type;

    if (type == "Number") {
        return this->toNumber();
    } else if (type == "Boolean") {
        return this->toBoolean();
    } else if (type == "Null") {
        return this->toNull();
    } else {
        return RuntimeResult().failure(RuntimeError(this->positionStart, this->positionEnd, "No conversion from " + this->type + " to " + other->type + " exists!", this->context));
    }
}

RuntimeResult Object::toNull() {
    return RuntimeResult().success(Null());
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

spObject Number::copy() {
    spObject other = Number();
    other->doubleValue = this->doubleValue;
    other->sign = this->sign;
    other->isPureDouble = this->isPureDouble;
    other->isPureZero = this->isPureZero;
    other->isInfinity = this->isInfinity;
    other->isNaN = this->isNaN;
    other->setPosition(this->positionStart, this->positionEnd);
    other->setContext(this->context);
    return other;
}

RuntimeResult Number::binary_plus(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notImplemented(rt, this, other, "binary_plus", rt.error->details);
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Number("NaN"));
    if (this->isInfinity || other->isInfinity) {
        if (this->sign == other->sign) {
            return rt.success(Number("Infinity", this->sign));
        } else {
            // Infinity + -Infinity, and -Infinity + Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(Number("NaN", 0));
        }
    }
    double result = this->doubleValue + other->doubleValue;
    if (didOverflow(result)) return rt.success(Number("Infinity", +1));
    if (didUnderflow(result)) return rt.success(Number("Infinity", -0));
    return rt.success(Number(result));
}

RuntimeResult Number::binary_minus(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notImplemented(rt, this, other, "binary_minus", rt.error->details);
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Number("NaN"));
    if (this->isInfinity || other->isInfinity) {
        if (this->sign != other->sign) {
            return rt.success(Number("Infinity", this->sign));
        } else {
            // Infinity - Infinity, and -Infinity - -Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(Number("NaN"));
        }
    }
    double result = this->doubleValue - other->doubleValue;
    if (didOverflow(result)) return rt.success(Number("Infinity", +1));
    if (didUnderflow(result)) return rt.success(Number("Infinity", -0));
    return rt.success(Number(result));
}

RuntimeResult Number::binary_asterisk(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notImplemented(rt, this, other, "binary_asterisk", rt.error->details);
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Number("NaN"));
    if (this->isInfinity || other->isInfinity) {
        if (this->isPureZero || other->isPureZero) return rt.success(Number("NaN"));
        // a == b is the same as !(a ^ b)
        // this->sign == other->sign
        //       0    ==         0    -> 1
        //       0    ==         1    -> 0
        //       1    ==         0    -> 0
        //       1    ==         1    -> 1
        return rt.success(Number("Infinity", this->sign == other->sign));
    }
    double result = this->doubleValue * other->doubleValue;
    if (didOverflow(result)) return rt.success(Number("Infinity", +1));
    if (didUnderflow(result)) return rt.success(Number("Infinity", -0));
    return rt.success(Number(result));
}

RuntimeResult Number::binary_f_slash(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notImplemented(rt, this, other, "binary_f_slash", rt.error->details);
        } else {
            other = tempOther;
        }
    }

    if (other->isPureZero) return rt.failure(RuntimeError(other->positionStart, other->positionEnd, "Division by 0", this->context));
    if (this->isNaN || other->isNaN) return rt.success(Number("NaN"));
    if (this->isInfinity && other->isInfinity) return rt.success(Number("NaN"));
    if (other->isInfinity) return rt.success(Number(0, this->sign == other->sign));
    if (this->isInfinity) return rt.success(Number("Infinity", this->sign == other->sign));
    double result = this->doubleValue / other->doubleValue;
    if (didOverflow(result)) return rt.success(Number("Infinity", +1));
    if (didUnderflow(result)) return rt.success(Number("Infinity", -0));
    return rt.success(Number(result));
}

RuntimeResult Number::binary_double_asterisk(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notImplemented(rt, this, other, "binary_double_asterisk", rt.error->details);
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Number("NaN"));
    if (other->isPureZero) return rt.success(Number(1));
    if (other->isInfinity && other->sign == +1) return rt.success(Number("Infinity"));
    if (other->isInfinity && other->sign == -0) return rt.success(Number(0));
    if (this->isInfinity && other->sign == +1) return rt.success(Number("Infinity", this->sign));
    if (this->isInfinity && other->sign == -0) return rt.success(Number(0));
    double result = std::pow(this->doubleValue, other->doubleValue);
    if (didOverflow(result)) return rt.success(Number("Infinity", +1));
    if (didUnderflow(result)) return rt.success(Number("Infinity", -0));
    return rt.success(Number(result));
}

RuntimeResult Number::binary_double_f_slash(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notImplemented(rt, this, other, "binary_double_f_slash", rt.error->details);
        } else {
            other = tempOther;
        }
    }

    if (other->isPureZero) return rt.failure(RuntimeError(other->positionStart, other->positionEnd, "Floored division by 0", this->context));
    spObject normalDivisionResult = rt.registerRT(this->binary_f_slash(other));
    if (rt.hasError()) return rt;
    if (!normalDivisionResult->isPureDouble) return rt;
    double result = std::floor(normalDivisionResult->doubleValue);
    if (didOverflow(result)) return rt.success(Number("Infinity", +1));
    if (didUnderflow(result)) return rt.success(Number("Infinity", -0));
    return rt.success(Number(result));
}

RuntimeResult Number::unary_plus() {
    RuntimeResult rt;

    if (isNaN) return rt.success(Number("NaN"));
    else if (isInfinity) return rt.success(Number("Infinity", sign));
    return rt.success(Number(this->doubleValue));
}

RuntimeResult Number::unary_minus() {
    RuntimeResult rt;

    if (isNaN) return rt.success(Number("NaN"));
    else if (isInfinity) return rt.success(Number("Infinity", !sign));
    return rt.success(Number(this->doubleValue * -1));
}

RuntimeResult Number::toNumber() {
    return RuntimeResult().success(*this);
}

RuntimeResult Number::toBoolean() {
    return RuntimeResult().success(Boolean(!(this->isPureZero || this->isNaN)));
}

Boolean::Boolean(bool value) {
    this->type = "Boolean";
    this->isPureDouble = true;
    this->isPureZero = !value;
    this->doubleValue = value;
}

std::string Boolean::to_string() {
    return this->isPureZero ? "false" : "true";
}

spObject Boolean::copy() {
    return Boolean(this->doubleValue);
}

RuntimeResult Boolean::toNumber() {
    return RuntimeResult().success(Number(this->doubleValue));
}

RuntimeResult Boolean::toBoolean() {
    return RuntimeResult().success(*this);
}

std::string Null::to_string() {
    return "null";
}

spObject Null::copy() {
    return Null();
}

RuntimeResult Null::binary_plus(spObject other) {
    RuntimeResult rt;
    if (other->type == "Null") {
        spObject self = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        spObject tempOther = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        return self->binary_plus(tempOther);
    }
    spObject self = rt.registerRT(this->toOther(other));
    if (rt.hasError()) return notImplemented(rt, this, other, "binary_plus", rt.error->details);
    return self->binary_plus(other);
}

RuntimeResult Null::binary_minus(spObject other) {
    RuntimeResult rt;
    if (other->type == "Null") {
        spObject self = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        spObject tempOther = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        return self->binary_minus(tempOther);
    }
    spObject self = rt.registerRT(this->toOther(other));
    if (rt.hasError()) return notImplemented(rt, this, other, "binary_minus", rt.error->details);
    return self->binary_minus(other);
}

RuntimeResult Null::binary_asterisk(spObject other) {
    RuntimeResult rt;
    if (other->type == "Null") {
        spObject self = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        spObject tempOther = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        return self->binary_asterisk(tempOther);
    }
    spObject self = rt.registerRT(this->toOther(other));
    if (rt.hasError()) return notImplemented(rt, this, other, "binary_asterisk", rt.error->details);
    return self->binary_asterisk(other);
}

RuntimeResult Null::binary_f_slash(spObject other) {
    RuntimeResult rt;
    if (other->type == "Null") {
        spObject self = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        spObject tempOther = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        return self->binary_f_slash(tempOther);
    }
    spObject self = rt.registerRT(this->toOther(other));
    if (rt.hasError()) return notImplemented(rt, this, other, "binary_f_slash", rt.error->details);
    return self->binary_f_slash(other);
}

RuntimeResult Null::binary_double_asterisk(spObject other) {
    RuntimeResult rt;
    if (other->type == "Null") {
        spObject self = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        spObject tempOther = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        return self->binary_double_asterisk(tempOther);
    }
    spObject self = rt.registerRT(this->toOther(other));
    if (rt.hasError()) return notImplemented(rt, this, other, "binary_double_asterisk", rt.error->details);
    return self->binary_double_asterisk(other);
}

RuntimeResult Null::binary_double_f_slash(spObject other) {
    RuntimeResult rt;
    if (other->type == "Null") {
        spObject self = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        spObject tempOther = rt.registerRT(this->toNumber());
        if (rt.hasError()) return rt;
        return self->binary_double_f_slash(tempOther);
    }
    spObject self = rt.registerRT(this->toOther(other));
    if (rt.hasError()) return notImplemented(rt, this, other, "binary_double_f_slash", rt.error->details);
    return self->binary_double_f_slash(other);
}

RuntimeResult Null::unary_plus() {
    return this->toNumber();
}

RuntimeResult Null::unary_minus() {
    RuntimeResult rt;
    spObject self = rt.registerRT(this->toNumber());
    if (rt.hasError()) return rt;
    return self->unary_minus();
}

RuntimeResult Null::toNumber() {
    return RuntimeResult().success(Number(0));
}

RuntimeResult Null::toBoolean() {
    return RuntimeResult().success(Boolean(false));
}
