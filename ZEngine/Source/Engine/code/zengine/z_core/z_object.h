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
#pragma once

#include "drive.h"

#include "f_memory_pool.h"

namespace zengine {

/*
    The base class of almost everything.
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
    WARNING: Adding any member will cause fatal error!!!!!!!
*/
class ZObject {
public:
    /*
        Overwrite the new and delete operator to use the memory from the memorypool.
    */
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size) noexcept {
        return memory_pool::ApplyMemory(static_cast<SizeType>(_size));
    }
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size, Void* _memory_ptr) noexcept {
        return _memory_ptr;
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* _memory_ptr) noexcept {
        memory_pool::ReleaseMemory(_memory_ptr);
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* _delete_memory, Void* _memory_ptr) noexcept {}

    FORCEINLINE constexpr const ZObject& operator=(const ZObject& _obj) noexcept { return *this; }
    FORCEINLINE constexpr const ZObject& operator=(ZObject&& _obj) noexcept { return *this; }

    NODISCARD FORCEINLINE Bool operator==(const ZObject& _obj) noexcept {
        return this == &_obj;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZObject& _obj) noexcept {
        return this != &_obj;
    }

protected:
    FORCEINLINE constexpr ZObject() noexcept {}
    FORCEINLINE constexpr ZObject(const ZObject& _obj) noexcept {}
    FORCEINLINE constexpr ZObject(ZObject&& _obj) noexcept {}

    FORCEINLINE constexpr ~ZObject() noexcept {}

private:
    static Void* operator new[](SizeType) = delete;
    static Void* operator new[](SizeType, Void*) = delete;
    static Void operator delete[](Void*) = delete;
};

}//zengine