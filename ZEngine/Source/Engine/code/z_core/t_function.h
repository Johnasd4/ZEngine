/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "internal/z_drive.h"

#include "m_log.h"
#include "t_smart_pointer.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum TFunctionErrorCode : ReturnType {
    kTFunctionErrorCode_LinkError = kErrorCodeBase_TFunction,
    kTFunctionErrorCode_SystemError,
    kTFunctionErrorCode_NullptrParam,
    kTFunctionErrorCode_ParamOutOfRange,
    kTFunctionErrorCode_FunctionNotExist,
};
}//error_code
}//zengine

namespace zengine {

/*
    Function class, same as std::function. Use zengine's memory pool.
    Template: TFunction<_ReturnType(_Arg1, _Arg2, _Arg3...)> = _func.
    [_func] can be basic function or non-capture lambda expression. 
*/
template<typename _Signature>
class TSimpleFunction : public ZObject {
    static_assert(!kSameType<_Signature, _Signature>, "TSimpleFunction signature not valid!");
};

template<typename _ReturnType, typename... Args>
class TSimpleFunction<_ReturnType(Args...)> : public ZObject {
public:
    using FunctionType_ = _ReturnType(*)(Args...);
    FORCEINLINE TSimpleFunction() noexcept : SuperType_(), func_ptr_(nullptr) {}

    FORCEINLINE TSimpleFunction(FunctionType_ _origin_func) noexcept
        : SuperType_()
        , func_ptr_(_origin_func) {}

    TSimpleFunction(const TSimpleFunction& _func) noexcept
        : SuperType_()
        , func_ptr_(_func.func_ptr_) {}
    TSimpleFunction(TSimpleFunction&& _func) noexcept
        : SuperType_()
        , func_ptr_(_func.func_ptr_) {}

    TSimpleFunction& operator=(const TSimpleFunction& _func) noexcept {
        func_ptr_ = _func.func_ptr_;
        return *this;
    }
    TSimpleFunction& operator=(TSimpleFunction&& _func) noexcept {
        func_ptr_ = _func.func_ptr_;
        return *this;
    }

    _ReturnType operator()(Args... args) const noexcept {
        if (!func_ptr_) {
            Z_LOG_ERROR(
                error_code::kTFunctionErrorCode_FunctionNotExist, 0, 
                L"Simple function not exist, can not execute!"
            );
            return _ReturnType();
        }
        return func_ptr_(std::forward<Args>(args)...);
    }

protected:
    using SuperType_ = ZObject;

private:
    FunctionType_ func_ptr_;
};

/*
    Function class, same as std::function. Use zengine's memory pool.
    Template: TFunction<_ReturnType(_Arg1, _Arg2, _Arg3...)> = _func.
    [_func] can be function, lambda expreesion or functor. 
*/
template<typename _Signature>
class TFunction : public ZObject {
    static_assert(!kSameType<_Signature, _Signature>, "TFunction signature not valid!");
};

template<typename _ReturnType, typename... ArgTypes>
class TFunction<_ReturnType(ArgTypes...)> : public ZObject {
public:
    FORCEINLINE TFunction() noexcept : SuperType_(), func_() {}

    template<typename _OriginFunc, typename = std::enable_if<kNotType<TFunction, _OriginFunc>>::type>
    FORCEINLINE TFunction(_OriginFunc&& _origin_func) noexcept
        : SuperType_()
        , func_(MakeUnique<FunctionImplP_<std::decay_t<_OriginFunc>>>(std::forward<_OriginFunc>(_origin_func))) {}

    TFunction(const TFunction& _func) noexcept
        : SuperType_()
        , func_(_func.func_ ? _func.func_->Clone() : nullptr) {}
    TFunction(TFunction&& _func) noexcept
        : SuperType_()
        , func_(std::move(_func.func_)) {}

    TFunction& operator=(const TFunction& _func) noexcept {
        func_ = _func.func_ ? _func.func_->Clone() : nullptr;
        return *this;
    }
    FORCEINLINE TFunction& operator=(TFunction&& _func) noexcept {
        func_ = std::move(_func.func_);
        return *this;
    }
    TFunction& operator=(NullptrType _nullptr) noexcept {
        func_.Reset();
        return *this;
    }
    template<typename _OriginFunc, typename = std::enable_if<kNotType<TFunction, _OriginFunc>>::type>
    TFunction& operator=(_OriginFunc&& _origin_func) noexcept {
        func_ = std::move(
            MakeUnique<FunctionImplP_<std::decay_t<_OriginFunc>>>(std::forward<_OriginFunc>(_origin_func))
        );
        return *this;
    }

    _ReturnType operator()(ArgTypes... args) const noexcept {
        if (!func_) {
            Z_LOG_ERROR(error_code::kTFunctionErrorCode_FunctionNotExist, 0, L"Function not exist, can not execute!");
            return _ReturnType();
        }
        return func_->Execute(std::forward<ArgTypes>(args)...);
    }


    FORCEINLINE operator Bool() const noexcept {
        return func_.operator Bool();
    }

protected:
    using SuperType_ = ZObject;

private:
    class FunctionBaseP_ : public ZObject {
    public:
        FORCEINLINE FunctionBaseP_() noexcept : SuperType_() {}
        virtual _ReturnType Execute(ArgTypes... args) const noexcept = 0;
        virtual TUniquePointer<FunctionBaseP_> Clone() const noexcept = 0;

    protected:
        using SuperType_ = ZObject;
    };

    template<typename _DecayFunction>
    class FunctionImplP_ : public FunctionBaseP_ {
    public:
        template<typename _OriginFunc>
        FORCEINLINE FunctionImplP_(_OriginFunc&& _func) noexcept 
            : SuperType_(), decay_func_(std::forward<_OriginFunc>(_func)) {}

        virtual _ReturnType Execute(ArgTypes... args) const noexcept {
            return decay_func_(std::forward<ArgTypes>(args)...);
        }

        virtual TUniquePointer<FunctionBaseP_> Clone() const noexcept {
            return MakeUnique<FunctionImplP_>(decay_func_);
        }

    protected:
        using SuperType_ = FunctionBaseP_;

    private:
        _DecayFunction decay_func_;
    };

    TUniquePointer<FunctionBaseP_> func_;
};

}//zengine