//
// Created by denist on 2/6/23.
//

#ifndef VM_VALUE_H
#define VM_VALUE_H

class Value {
public:
    enum class value_t {opnd};

    value_t GetType() const {
        return value_type_;
    }

    void SetType(value_t type) {
        value_type_ = type;
    }

    virtual ~Value() {}

protected:
    Value(value_t value_type) : value_type_(value_type) {}

    Value(const Value &other) : value_type_(other.value_type_) {};
    Value &operator=(const Value &other) = delete;

    Value(Value &&other) = delete;
    Value &operator=(Value &&other) = delete;

private:
    value_t value_type_;
};
#endif //VM_VALUE_H
