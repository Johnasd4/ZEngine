/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_task.h"

#pragma warning(disable: 26495)

namespace zengine {

static Void EmptyOperateFuncForZTask(ZTask* _task_ptr, Void* _param_ptr) noexcept {
    // discard unused param warning
    (Void)_task_ptr;
    (Void)_param_ptr;
}

ZTask::ZTask() noexcept
    : SuperType_()
    , state_(ZTaskStateEnum::kNoTask) 
    , operate_func_ptr_(EmptyOperateFuncForZTask)
    , ret_val_ptr_(nullptr)
{}

ZTask::ZTask(ZTask&& _task) noexcept 
    : SuperType_(std::move(_task)) 
{
    MoveP(std::move(_task));
}

ZTask::~ZTask() noexcept { Clear(); }

ZTask& ZTask::operator=(ZTask&& _task) noexcept {
    if (this == &_task) {
        return *this;
    }
    SuperType_::operator=(std::move(_task));

    Clear();
    MoveP(std::move(_task));
    return *this;
}

Void ZTask::Clear() noexcept {
    if (state_ == ZTaskStateEnum::kTaskSet) {
        state_ = ZTaskStateEnum::kFinished;
    }
    if (state_ == ZTaskStateEnum::kFinished) {
        operate_func_ptr_(this, nullptr);
    }
}

NODISCARD ReturnType ZTask::Run() noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ == ZTaskStateEnum::kTaskSet,
        error_code::kZTaskErrorCode_StateError,
        "Task state error! state: {} expect state: {}",
        static_cast<Int32>(state_), static_cast<Int32>(ZTaskStateEnum::kTaskSet)
    );

    operate_func_ptr_(this, nullptr);
    return ret_val;
}

static Void EmptyOperateFuncForZRepeatTask(ZRepeatTask* _task_ptr, Void* _param_ptr) noexcept {
    // discard unused param warning
    (Void)_task_ptr;
    (Void)_param_ptr;
}

ZRepeatTask::ZRepeatTask() noexcept
    : SuperType_()
    , state_(ZTaskStateEnum::kNoTask) 
    , operate_func_ptr_(EmptyOperateFuncForZRepeatTask)
{}

ZRepeatTask::ZRepeatTask(ZRepeatTask&& _task) noexcept 
    : SuperType_(std::move(_task)) 
{
    MoveP(std::move(_task));
}

ZRepeatTask::~ZRepeatTask() noexcept { Clear(); }

ZRepeatTask& ZRepeatTask::operator=(ZRepeatTask&& _task) noexcept {
    if (this == &_task) {
        return *this;
    }
    SuperType_::operator=(std::move(_task));

    Clear();
    MoveP(std::move(_task));
    return *this;
}

Void ZRepeatTask::Clear() noexcept {
    if (state_ == ZTaskStateEnum::kTaskSet) {
        state_ = ZTaskStateEnum::kFinished;
    }
    if (state_ == ZTaskStateEnum::kFinished) {
        operate_func_ptr_(this, nullptr);
    }
}

NODISCARD ReturnType ZRepeatTask::Run() noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        state_ == ZTaskStateEnum::kTaskSet,
        error_code::kZTaskErrorCode_StateError,
        "Task state error! state: {} expect state: {}",
        static_cast<Int32>(state_), static_cast<Int32>(ZTaskStateEnum::kTaskSet)
    );

    operate_func_ptr_(this, nullptr);
    return ret_val;
}

} // zengine

#pragma warning(default: 26495)

