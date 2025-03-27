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
#ifndef Z_CORE_INTERNAL_D_MACRO_H_
#define Z_CORE_INTERNAL_D_MACRO_H_


#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
#define DLLIMPORT __declspec(dllimport)
#endif

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif

/*
    Checks if the state is in thes given states.
    use | to combine the check states.
*/
#ifndef IN_STATE
#define IN_STATE(_state, _check_states) ((_state) & (_check_states))
#endif //IN_STATE

#endif // !Z_CORE_INTERNAL_D_MACRO_H_