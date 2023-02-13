//
// Created by denist on 2/6/23.
//

#ifndef VM_USER_H
#define VM_USER_H
#include <memory>

#include "operands.h"

template <class ...Elt>
concept IsPackBaseOfSimpleOperand = (std::derived_from<Elt, SimpleOperand> && ...);

template <class It>
concept Iterator = requires () {
    typename It::iterator_category;
    typename It::pointer;
    typename It::reference;
    typename It::value_type;
    typename It::difference_type;
};

template <class It>
concept InputIterator = Iterator<It> &&
        std::derived_from<typename It::iterator_tag, std::input_iterator_tag>;

template <class It>
concept RandomIterator = InputIterator<It> &&
        std::derived_from<typename It::iterator_tag, std::random_access_iterator_tag>;

template <class OpndType, size_t opnd_num = std::numeric_limits<size_t>::max()>
class User : public IReg {
public:

    using OperandType = OpndType;

    const OpndType *GetOpndAt(size_t idx) const {
        return opnds_.at(idx).get();
    };

    OpndType *GetOpndAt(size_t idx) {
        return opnds_.at(idx).get();
    }

    const OpndType *GetOpnd(size_t idx) const {
        return opnds_[idx].get();
    };

    OpndType *GetOpnd(size_t idx) {
        return opnds_[idx].get();
    }

    size_t GetOpndsSize() {
        return opnds_.size();
    }

protected:
    template <class ...Elt>
    /*requires IsPackBaseOfSimpleOperand<Elt...>*/
    User(prim_type p_type, uint8_t bit_lenght, size_t idx, std::unique_ptr<Elt>&&... opnds) :
    IReg(p_type, bit_lenght, idx), opnds_{std::forward<std::unique_ptr<Elt>>(opnds)...} {}

    template <class ...Elt>
    /*requires IsPackBaseOfSimpleOperand<Elt...>*/
    User(prim_type p_type, uint8_t bit_lenght, size_t idx, Elt *... opnds) :
    IReg(p_type, bit_lenght, idx), opnds_{std::forward<std::unique_ptr<Elt>>(std::unique_ptr<Elt>(opnds))...} {}

    template <InputIterator It>
    User(It begin, It end, prim_type p_type, uint8_t bit_lenght, size_t idx) : IReg(p_type, bit_lenght, idx) {
        opnds_.resize(end-begin);
        std::copy(begin, end, opnds_.begin());
    }

    User(const User &other) = delete;
    User& operator=(const User &other) = delete;

    User(User &&another) = delete;
    User&& operator=(User&&) = delete;

private:
    std::array<std::unique_ptr<OpndType>, opnd_num> opnds_;
};

template <class OpndType>
class User<OpndType, std::numeric_limits<size_t>::max()> : public IReg {
public:

    using OperandType = OpndType;

    const OpndType *GetOpndAt(size_t idx) const {
        return opnds_.at(idx).get();
    };

    OpndType *GetOpndAt(size_t idx) {
        return opnds_.at(idx).get();
    }

    const OpndType *GetOpnd(size_t idx) const {
        return opnds_[idx].get();
    };

    OpndType *GetOpnd(size_t idx) {
        return opnds_[idx].get();
    }

    size_t GetOpndsSize() {
        return opnds_.size();
    }

protected:
    template <class ...Elt>
    /*requires IsPackBaseOfSimpleOperand<Elt...>*/
    User(prim_type p_type, uint8_t bit_lenght, size_t idx, std::unique_ptr<Elt>&&... opnds) :
    IReg(p_type, bit_lenght, idx) {
        std::array<std::unique_ptr<std::common_type_t<Elt...>>, sizeof...(Elt)> buf{std::move(opnds)...};
        opnds_.reserve(sizeof...(Elt));
        std::move(buf.begin(), buf.end(), std::back_insert_iterator(opnds_));
    }

    template <class ...Elt>
    /*requires IsPackBaseOfSimpleOperand<Elt...>*/
    User(prim_type p_type, uint8_t bit_lenght, size_t idx, Elt *... opnds) :
    IReg(p_type, bit_lenght, idx), opnds_{std::forward<std::unique_ptr<Elt>>(std::unique_ptr<Elt>(opnds))...} {}

//    template <InputIterator It>
//    requires It::value_type ==
//    User(It begin, It end, prim_type p_type, uint8_t bit_lenght, size_t idx) :
//    IReg(p_type, bit_lenght, idx) {
//        std::copy(begin, end, std::back_insert_iterator(opnds_));
//    }
//
//    template <RandomIterator It>
//    User(It begin, It end, prim_type p_type, uint8_t bit_lenght, size_t idx) :
//    IReg(p_type, bit_lenght, idx){
//        opnds_.resize(end-begin);
//        std::copy(begin, end, opnds_.begin());
//    }

    User(const User&) = delete;
    User& operator=(const User&) = delete;

    User(User &&another) = delete;
    User&& operator=(User&&) = delete;

private:
    std::vector<std::unique_ptr<SimpleOperand>> opnds_;
};

#endif //VM_USER_H
