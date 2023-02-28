//
// Created by denist on 2/6/23.
//

#ifndef VM_USER_H
#define VM_USER_H
#include <memory>

#include "operands.h"

template <class Type, class ...Elt>
concept IsPackDerivedFromSameType = (std::derived_from<Elt, Type> && ...);

template <class Type, class ...Elt>
concept IsPackSameType = (std::same_as<Type, Elt> && ...);

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
class User {
public:

    using OperandType = OpndType;

    const IReg *GetResReg() const {
        return res_.get();
    }

    IReg *GetResReg() {
        return res_.get();
    }

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

    void SetOpnd(size_t idx, OpndType *opnd) {
        opnds_[idx] = std::unique_ptr<OpndType>(opnd);
    }

    void SetOpndAt(size_t idx, OpndType *opnd) {
        opnds_.at(idx) = std::unique_ptr<OpndType>(opnd);
    }

protected:
    template <class ...Elt>
    requires IsPackSameType<OpndType, Elt...> || IsPackDerivedFromSameType<OpndType, Elt...>
    User(std::unique_ptr<IReg> &&res, std::unique_ptr<Elt>&&... opnds) :
    res_(std::move(res)), opnds_{std::forward<std::unique_ptr<Elt>>(opnds)...} {}

    template <class ...Elt>
    requires IsPackSameType<OpndType, Elt...> || IsPackDerivedFromSameType<OpndType, Elt...>
    User(IReg *res, Elt *... opnds) :
    res_(std::unique_ptr<IReg>(res)), opnds_{std::forward<std::unique_ptr<Elt>>(std::unique_ptr<Elt>(opnds))...} {}

    template <InputIterator It>
    requires std::same_as<typename It::value_type, OpndType>
    User(IReg *res, It begin, It end) : res_(res) {
        opnds_.resize(end-begin);
        std::copy(begin, end, opnds_.begin());
    }

    template <class Ptr>
    requires std::is_pointer_v<Ptr> && std::same_as<std::remove_pointer_t<Ptr>, OpndType>
    User(IReg *res, Ptr begin, Ptr end) : res_(res) {
        opnds_.resize(end-begin);
        std::copy(begin, end, opnds_.begin());
    }


    User(const User &rhs) {
        res_ = std::unique_ptr<IReg>(static_cast<IReg *>(rhs.res_.get()->clone()));
        size_t idx = 0;
        for(const auto &opnd : rhs.opnds_) {
            opnds_.at(idx) = std::unique_ptr<OpndType>(opnd.get()->clone());
            ++idx;
        }
    }
    User& operator=(const User &other) = delete;

    User(User &&another) = delete;
    User&& operator=(User&&) = delete;

private:
    std::unique_ptr<IReg> res_;
    std::array<std::unique_ptr<OpndType>, opnd_num> opnds_;
};

template <class OpndType>
class User<OpndType, std::numeric_limits<size_t>::max()> {
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

    size_t GetOpndsSize() const {
        return opnds_.size();
    }

    IReg *GetResReg() {
        return res_.get();
    }

    const IReg *GetResReg() const {
        return res_.get();
    }

    void SetOpnd(size_t idx, OpndType *opnd) {
        opnds_[idx] = std::unique_ptr<OpndType>(opnd);
    }

    void SetOpndAt(size_t idx, OpndType *opnd) {
        opnds_.at(idx) = std::unique_ptr<OpndType>(opnd);
    }

    void PushOpndBack(OpndType *opnd) {
        opnds_.push_back(std::unique_ptr<OpndType>(opnd));
    }

protected:
    template <class ...Elt>
    requires IsPackSameType<OpndType, Elt...> || IsPackDerivedFromSameType<OpndType, Elt...>
    User(std::unique_ptr<IReg> res, std::unique_ptr<Elt>&&... opnds) :
    res_(std::move(res)) {
        opnds_.reserve(sizeof...(Elt));
        (opnds_.emplace_back(std::move(opnds)), ...);
    }

    template <class ...Elt>
    requires IsPackSameType<OpndType, Elt...> || IsPackDerivedFromSameType<OpndType, Elt...>
    User(IReg *res, Elt *... opnds) :
    res_(std::unique_ptr<IReg>(res)) {
        opnds_.reserve(sizeof...(Elt));
        (opnds_.emplace_back(opnds), ...);
    }

    template <InputIterator It>
    requires std::same_as<typename It::value_type, OpndType>
    User(IReg *res, It begin, It end) : res_(std::unique_ptr<IReg>(res)) {
        std::copy(begin, end, std::back_insert_iterator(opnds_));
    }

    template <RandomIterator It>
    requires std::same_as<typename It::value_type, OpndType>
    User(IReg *res, It begin, It end) : res_(std::unique_ptr<IReg>(res)) {
        opnds_.resize(end-begin);
        std::copy(begin, end, opnds_.begin());
    }

    template <class Ptr>
    requires std::is_pointer_v<Ptr> && std::same_as<std::remove_cv_t<std::remove_pointer_t<Ptr>>, OpndType>
    User(IReg *res, Ptr begin, Ptr end) : res_(res) {
        opnds_.reserve(end-begin);
        for(;begin != end; ++begin) {
            std::unique_ptr<OpndType> new_ptr = std::make_unique<OpndType>(std::move(*begin));
            opnds_.emplace_back(std::move(new_ptr));
        }
    }

protected:
    User(const User &rhs) {
        res_ = std::unique_ptr<IReg>(static_cast<IReg *>(rhs.res_.get()->clone()));
        opnds_.reserve(rhs.opnds_.size());
        auto emplace_lambda = [this](const auto &opnd){
            opnds_.emplace_back(opnd.get()->clone());
        };
        std::for_each(rhs.opnds_.begin(), rhs.opnds_.end(), emplace_lambda);
    };
    User& operator=(const User&) = delete;

    User(User &&another) = delete;
    User&& operator=(User&&) = delete;

    std::unique_ptr<IReg> res_;
    std::vector<std::unique_ptr<OpndType>> opnds_;
};

#endif //VM_USER_H
