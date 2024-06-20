/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#include "f_console.h"

namespace zengine {
namespace console {

namespace internal {

/*
    Singleton class that contains the console settings.
*/
class ZPrintManager : public ZObject {
public:
    static Void SetColour(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        print_manager.text_colour_ = text_colour;
        print_manager.background_colour_ = background_colour;
        //Changes the console output colour.
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                                (PrintColourType)text_colour | (PrintColourType)background_colour);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(const Char* format, ArgListType args) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        vprintf(format, args);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(const WChar* format, ArgListType args) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        vwprintf(format, args);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour, 
                      const Char* format, ArgListType args) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                                (PrintColourType)text_colour | (PrintColourType)background_colour);
        vprintf(format, args);
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE), 
            (PrintColourType)print_manager.text_colour_ | (PrintColourType)print_manager.background_colour_);
        print_manager.print_mutex_.Unlock();
    }

    static Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour, 
                      const WChar* format, ArgListType args) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();

        print_manager.print_mutex_.Lock();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                                (PrintColourType)text_colour | (PrintColourType)background_colour);
        vwprintf(format, args);
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE), 
            (PrintColourType)print_manager.text_colour_ | (PrintColourType)print_manager.background_colour_);
        print_manager.print_mutex_.Unlock();
    }

protected:
    using SuperType = ZObject;

private:
    static constexpr PrintTextColourEnum kDefaultTextColour = kPrintTextColourLightWhite;
    static constexpr PrintBackgroundColourEnum kDefaultBackgroundColour = kPrintBackgroundColourDarkBlack;

    NODISCARD static ZPrintManager& InstanceP() {
        static ZPrintManager instance;
        return instance;
    }

    ZPrintManager() : SuperType(), text_colour_(kDefaultTextColour), background_colour_(kDefaultBackgroundColour) {}

    PrintTextColourEnum text_colour_;
    PrintBackgroundColourEnum background_colour_;
    ZMutex print_mutex_;
};

}//internal

CORE_DLLAPI Void SetPrintColour(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour) noexcept {
    internal::ZPrintManager::SetColour(text_colour, background_colour);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and 
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const Char * format, ...) noexcept {
    ArgListType args;
    va_start(args, format);
    internal::ZPrintManager::Print(format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const Char* format, ArgListType args) noexcept {
    internal::ZPrintManager::Print(format, args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const WChar* format, ...) noexcept {
    ArgListType args;
    va_start(args, format);
    internal::ZPrintManager::Print(format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(const WChar* format, ArgListType args) noexcept {
    internal::ZPrintManager::Print(format, args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const Char* format, ...) noexcept{
    ArgListType args;
    va_start(args, format);
    internal::ZPrintManager::Print(text_colour, background_colour, format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const Char* format, ArgListType args) noexcept{
    internal::ZPrintManager::Print(text_colour, background_colour, format, args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const WChar* format, ...) noexcept{
    ArgListType args;
    va_start(args, format);
    internal::ZPrintManager::Print(text_colour, background_colour, format, args);
    va_end(args);
}

/*
    Use it as the same as printf, it's thread safe. You can add text colour and
    background colour infront of the format to change the colour only for this
    output.
*/
CORE_DLLAPI Void Print(PrintTextColourEnum text_colour, PrintBackgroundColourEnum background_colour,
                       const WChar* format, ArgListType args) noexcept{
    internal::ZPrintManager::Print(text_colour, background_colour, format, args);
}

}//console
}//zengine
