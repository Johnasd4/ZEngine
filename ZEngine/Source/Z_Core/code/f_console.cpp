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
#define CORE_DLLFILE

#include "f_console.h"

#include "z_core/z_mutex.h"

namespace zengine {
namespace console {

namespace internal {

/*
    Singleton class that contains the console settings.
*/
class ZPrintManager : public ZObject {
public:
    static Void SetColour(PrintTextColourEnum _text_colour, PrintBackgroundColourEnum _background_colour) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        print_manager.text_colour_ = _text_colour;
        print_manager.background_colour_ = _background_colour;
        //Changes the console output colour.
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE), (PrintColourType)_text_colour | (PrintColourType)_background_colour);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(const Char* _format, ArgListType _args) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        vprintf(_format, _args);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(const WChar* _format, ArgListType _args) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        vwprintf(_format, _args);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(
        PrintTextColourEnum _text_colour, 
        PrintBackgroundColourEnum _background_colour, 
        const Char* _format, 
        ArgListType _args
    ) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                                (PrintColourType)_text_colour | (PrintColourType)_background_colour);
        vprintf(_format, _args);
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE), 
            (PrintColourType)print_manager.text_colour_ | (PrintColourType)print_manager.background_colour_);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(
        PrintTextColourEnum _text_colour, 
        PrintBackgroundColourEnum _background_colour, 
        const WChar* _format, 
        ArgListType _args
    ) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                                (PrintColourType)_text_colour | (PrintColourType)_background_colour);
        vwprintf(_format, _args);
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE), 
            (PrintColourType)print_manager.text_colour_ | (PrintColourType)print_manager.background_colour_);
        print_manager.print_mutex_.Unlock();
    }

protected:
    using SuperType_ = ZObject;

private:
    static constexpr PrintTextColourEnum kDefaultTextColour = kPrintTextColour_LightWhite;
    static constexpr PrintBackgroundColourEnum kDefaultBackgroundColour = kPrintBackgroundColour_DarkBlack;

    NODISCARD static ZPrintManager& InstanceP() {
        static ZPrintManager instance;
        return instance;
    }

    ZPrintManager() : SuperType_(), text_colour_(kDefaultTextColour), background_colour_(kDefaultBackgroundColour) {}

    PrintTextColourEnum text_colour_;
    PrintBackgroundColourEnum background_colour_;
    ZMutex print_mutex_;
};

}//internal

/*
    Sets language.
*/
CORE_DLLAPI Void SetLanguage(LanguageType _language) noexcept {
    std::setlocale(LC_ALL, _language);
}

CORE_DLLAPI Void SetPrintColour(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour
) noexcept {
    internal::ZPrintManager::SetColour(_text_colour, _background_colour);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and 
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const Char * _format, ...) noexcept {
    ArgListType args;
    va_start(args, _format);
    internal::ZPrintManager::Print(_format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const Char* _format, ArgListType _args) noexcept {
    internal::ZPrintManager::Print(_format, _args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const WChar* _format, ...) noexcept {
    ArgListType args;
    va_start(args, _format);
    internal::ZPrintManager::Print(_format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const WChar* _format, ArgListType _args) noexcept {
    internal::ZPrintManager::Print(_format, _args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour,
    const Char* format, 
    ...
) noexcept{
    ArgListType args;
    va_start(args, format);
    internal::ZPrintManager::Print(_text_colour, _background_colour, format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour,
    const Char* _format, 
    ArgListType _args
) noexcept{
    internal::ZPrintManager::Print(_text_colour, _background_colour, _format, _args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour,
    const WChar* _format, 
    ...
) noexcept{
    ArgListType args;
    va_start(args, _format);
    internal::ZPrintManager::Print(_text_colour, _background_colour, _format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(
    PrintTextColourEnum _text_colour, 
    PrintBackgroundColourEnum _background_colour,
    const WChar* _format, 
    ArgListType _args
) noexcept{
    internal::ZPrintManager::Print(_text_colour, _background_colour, _format, _args);
}

}//console
}//zengine
