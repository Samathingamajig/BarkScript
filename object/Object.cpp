#include "object.h"
#include <math.h>
#include <sstream>
#include "../interpreter/interpreter.h"

bool didOverflow(const double& value) {
    return std::numeric_limits<double>::max() < value;
}

bool didUnderflow(const double& value) {
    return -std::numeric_limits<double>::max() > value;
}

template <class T>
RuntimeResult notSupported(RuntimeResult& rt, const T& self, spObject other, const std::string& function, const std::string& extra) {
    return rt.failure(TypeError(self->positionStart, other->positionEnd, function + " is not supported between the types \"" + self->type + "\" and \"" + other->type + "\"!" + (extra.size() > 0 ? " (" + extra + ")" : ""), self->context));
}

template <class T>
RuntimeResult notSupportedNoRT(const T& self, spObject other, const std::string&& function, const std::string& extra) {
    RuntimeResult rt;
    return notSupported(rt, self, other, function, extra);
}

template <class T>
RuntimeResult notSupportedNoRT(const T& self, const std::string&& function, const std::string& extra) {
    RuntimeResult rt;
    return rt.failure(TypeError(self->positionStart, self->positionEnd, function + " is not supported for the type \"" + self->type + "\"!" + (extra.size() > 0 ? " (" + extra + ")" : ""), self->context));
}

void Object::setPosition(const Position& positionStart, const Position& positionEnd) {
    this->positionStart = positionStart;
    this->positionEnd = positionEnd;
}

void Object::setContext(const spContext& context) {
    this->context = context;
}

RuntimeResult Object::unary_bang() const {
    return RuntimeResult().success(Boolean(!this->to_bool()));
}

RuntimeResult Object::toOther(spObject other) const {
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

RuntimeResult Object::toNull() const {
    return RuntimeResult().success(Null());
}

Number::Number(const double& value, const bool sign) {
    this->type = "Number";
    this->doubleValue = value;
    this->isPureDouble = true;
    if (value < 0.0) this->sign = -0;
    if (value == 0) this->isPureZero = true;
}

Number::Number(const std::string& value, const bool sign) {
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

std::string Number::to_string() const {
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

bool Number::to_bool() const {
    return !(this->isPureZero || this->isNaN);
}

spObject Number::copy() const {
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
            return notSupported(rt, this, other, "binary_plus");
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Number("NaN"));
    if (this->isInfinity && other->isInfinity) {
        if (this->sign == other->sign) {
            return rt.success(Number("Infinity", this->sign));
        } else {
            // Infinity + -Infinity, and -Infinity + Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(Number("NaN"));
        }
    }
    if (this->isInfinity || other->isInfinity)
        return rt.success(Number("Infinity", this->isInfinity ? this->sign : other->sign));
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
            return notSupported(rt, this, other, "binary_minus");
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Number("NaN"));
    if (this->isInfinity && other->isInfinity) {
        if (this->sign != other->sign) {
            return rt.success(Number("Infinity", this->sign));
        } else {
            // Infinity - Infinity, and -Infinity - -Infinity, are both proven impossible
            // so we need to return NaN
            return rt.success(Number("NaN"));
        }
    }
    if (this->isInfinity || other->isInfinity)
        return rt.success(Number("Infinity", this->isInfinity ? this->sign : !other->sign));
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
            return notSupported(rt, this, other, "binary_asterisk");
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
            return notSupported(rt, this, other, "binary_f_slash");
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
            return notSupported(rt, this, other, "binary_double_asterisk");
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
            return notSupported(rt, this, other, "binary_double_f_slash");
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

RuntimeResult Number::binary_double_equal(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            rt.error = nullptr;
            return rt.success(Boolean(false));
        } else {
            other = tempOther;
        }
    }

    return rt.success(Boolean(
        (this->doubleValue == other->doubleValue)
        && (this->sign == other->sign)
        && (this->isInfinity == other->isInfinity)
        && (this->isNaN == other->isNaN))
    );
}

RuntimeResult Number::binary_bang_equal(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            rt.error = nullptr;
            return rt.success(Boolean(true));
        } else {
            other = tempOther;
        }
    }

    return rt.success(Boolean(
        (this->doubleValue != other->doubleValue)
        || (this->sign != other->sign)
        || (this->isInfinity != other->isInfinity)
        || (this->isNaN != other->isNaN))
    );
}

RuntimeResult Number::binary_less_than(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notSupported(rt, this, other, "binary_less_than");
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Boolean(false));
    if ((this->isInfinity && this->sign == -0) && (other->isInfinity && other->sign == +1)) return rt.success(Boolean(true));
    if (this->isInfinity || other->isInfinity) return rt.success(Boolean(false));
    if (this->isPureZero && other->isPureZero) return rt.success(Boolean(false));
    return rt.success(Boolean(this->doubleValue < other->doubleValue));
}

RuntimeResult Number::binary_less_than_equal(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notSupported(rt, this, other, "binary_less_than_equal");
        } else {
            other = tempOther;
        }
    }

    spObject lessThan = rt.registerRT(this->binary_less_than(other));
    if (rt.hasError()) return rt;
    if (!lessThan->isPureZero) return rt.success(lessThan);
    return this->binary_double_equal(other);
}

RuntimeResult Number::binary_greater_than(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notSupported(rt, this, other, "binary_greater_than");
        } else {
            other = tempOther;
        }
    }

    if (this->isNaN || other->isNaN) return rt.success(Boolean(false));
    if ((this->isInfinity && this->sign == +1) && (other->isInfinity && other->sign == -0)) return rt.success(Boolean(true));
    if (this->isInfinity || other->isInfinity) return rt.success(Boolean(false));
    if (this->isPureZero && other->isPureZero) return rt.success(Boolean(false));
    return rt.success(Boolean(this->doubleValue > other->doubleValue));
}

RuntimeResult Number::binary_greater_than_equal(spObject other) {
    RuntimeResult rt;

    if (other->type != objecttypes::Number) {
        spObject tempOther = rt.registerRT(other->toNumber());
        if (rt.hasError()) {
            return notSupported(rt, this, other, "binary_greater_than_equal");
        } else {
            other = tempOther;
        }
    }

    spObject greaterThan = rt.registerRT(this->binary_greater_than(other));
    if (rt.hasError()) return rt;
    if (!greaterThan->isPureDouble) return rt.success(greaterThan);
    return this->binary_double_equal(other);
}

RuntimeResult Number::toNumber() const {
    return RuntimeResult().success(Number(*this));
}

RuntimeResult Number::toBoolean() const {
    return RuntimeResult().success(Boolean(this->to_bool()));
}

Boolean::Boolean(bool value) {
    this->type = "Boolean";
    this->isPureDouble = true;
    this->isPureZero = !value;
    this->doubleValue = value;
}

std::string Boolean::to_string() const {
    return this->isPureZero ? "false" : "true";
}

bool Boolean::to_bool() const {
    return !this->isPureZero;
}

spObject Boolean::copy() const {
    return Boolean(this->doubleValue);
}

RuntimeResult Boolean::toNumber() const {
    return RuntimeResult().success(Number(this->doubleValue));
}

RuntimeResult Boolean::toBoolean() const {
    return RuntimeResult().success(Boolean(!this->isPureZero));
}

std::string Null::to_string() const {
    return "null";
}

bool Null::to_bool() const {
    return false;
}

spObject Null::copy() const {
    return Null();
}

RuntimeResult Null::binary_double_equal(spObject other) {
    return RuntimeResult().success(Boolean(other->type == "Null"));
}

RuntimeResult Null::binary_bang_equal(spObject other) {
    return RuntimeResult().success(Boolean(other->type != "Null"));
}
