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
#define SOCKET_DLLFILE

#include "f_tcp_log_output.h"

#include "z_core/log/type/z_error_log.h"
#include "z_core/log/type/z_trace_log.h"
#include "z_core/log/type/z_info_log.h"
#include "z_core/m_log.h"
#include "z_core/t_atom.h"
#include "z_core/t_fixed_memory.h"
#include "z_core/t_queue.h"
#include "z_core/z_mutex.h"
#include "z_core/z_object.h"
#include "z_core/z_sem_mutex.h"
#include "z_core/z_thread.h"

#include "z_io_context.h"
#include "z_tcp_client.h"
#include "z_tcp_server.h"

namespace zengine {
namespace socket {

class ZTCPLogServer : ZObject {
public:
    static ZTCPLogServer& Instance() noexcept {
        static ZTCPLogServer log_server;
        return log_server;
    }

    static Void ServerOutputErrorLog(
        const zengine::log::ZLog* _log_ptr,
        const zengine::log::ZLog::OutputString_& _output_str
    ) noexcept {
        const zengine::log::ZErrorLog* error_log_ptr = static_cast<const zengine::log::ZErrorLog*>(_log_ptr);
        TCPLogOutputReplyLogData* log_data_ptr = new TCPLogOutputReplyLogData();
        log_data_ptr->log_type_ = zengine::log::kLogType_Error;
        log_data_ptr->log_time_ = error_log_ptr->LogTime();
        log_data_ptr->log_string_ = _output_str;
        Instance().log_buffer_ptr_queue_.Push(log_data_ptr);
        Instance().LogServerBufferResetCheckP();
    }

    static Void ServerOutputTraceLog(
        const zengine::log::ZLog* _log_ptr,
        const zengine::log::ZLog::OutputString_& _output_str
    ) noexcept {
        const zengine::log::ZTraceLog* trace_log_ptr = static_cast<const zengine::log::ZTraceLog*>(_log_ptr);
        TCPLogOutputReplyLogData* log_data_ptr = new TCPLogOutputReplyLogData();
        log_data_ptr->log_type_ = zengine::log::kLogType_Trace;
        log_data_ptr->log_time_ = trace_log_ptr->LogTime();
        log_data_ptr->log_string_ = _output_str;
        Instance().log_buffer_ptr_queue_.Push(log_data_ptr);
        Instance().LogServerBufferResetCheckP();
    }

    static Void ServerOutputInfoLog(
        const zengine::log::ZLog* _log_ptr,
        const zengine::log::ZLog::OutputString_& _output_str
    ) noexcept {
        const zengine::log::ZInfoLog* info_log_ptr = static_cast<const zengine::log::ZInfoLog*>(_log_ptr);
        TCPLogOutputReplyLogData* log_data_ptr = new TCPLogOutputReplyLogData();
        log_data_ptr->log_type_ = zengine::log::kLogType_Info;
        log_data_ptr->log_time_ = info_log_ptr->LogTime();
        log_data_ptr->log_info_.info_log_info_.info_type_ = info_log_ptr->InfoType();
        log_data_ptr->log_string_ = _output_str;
        Instance().log_buffer_ptr_queue_.Push(log_data_ptr);
        Instance().LogServerBufferResetCheckP();
    }

    NODISCARD ReturnType StartServer(
        const ZTCPEndpoint& _tcp_endpoint
    ) noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;

        Z_CHECK(
            log_server_thread_state_ != LogServerState_Idle,
            error_code::kPSocketErrorCode_StateError,
            L"Server state error! state: %d expect state: %d",
            log_server_thread_state_.Value(), LogServerState_Idle
        );

        //set end point
        link_code = log_server_.BindEndpoint(_tcp_endpoint);
        if (link_code != kOK) {
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZTCPServer::SetEndpoint() link error!");
            return ret_val;
        }

        //start log server
        log_server_thread_state_ = LogServerState_Initialzing;
        server_thread_ = ZThread(LogServerThreadFunc);

        //register output func
        link_code = zengine::log::RegisterLogServerOutputFunction(
            zengine::log::kErrorLogPortID,
            ZTCPLogServer::ServerOutputErrorLog
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kMLogErrorCode_LinkError, link_code,
                L"ZLogServer::RegisterOutputFunction() link error!"
            );
        }
        link_code = zengine::log::RegisterLogServerOutputFunction(
            zengine::log::kTraceLogPortID,
            ZTCPLogServer::ServerOutputTraceLog
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kMLogErrorCode_LinkError, link_code,
                L"ZLogServer::RegisterOutputFunction() link error!"
            );
        }
        link_code = zengine::log::RegisterLogServerOutputFunction(
            zengine::log::kInfoLogPortID,
            ZTCPLogServer::ServerOutputInfoLog
        );
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kMLogErrorCode_LinkError, link_code,
                L"ZLogServer::RegisterOutputFunction() link error!"
            );
        }

        return ret_val;
    }

    NODISCARD ReturnType StopServer() noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;

        if (log_server_thread_state_ == LogServerState_Idle || log_server_thread_state_ == LogServerState_Closing) {
            return ret_val;
        }

        log_server_thread_state_ = LogServerState_Closing;

        //unregister output func
        zengine::log::UnregisterLogServerOutputFunction(ZTCPLogServer::ServerOutputErrorLog);
        zengine::log::UnregisterLogServerOutputFunction(ZTCPLogServer::ServerOutputTraceLog);
        zengine::log::UnregisterLogServerOutputFunction(ZTCPLogServer::ServerOutputInfoLog);

        //stop log server
        link_code = log_server_.Close();
        if (link_code != kOK) {
            ret_val = error_code::kPSocketErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZTCPServer::Close() link error!");
            return ret_val;
        }

        return ret_val;
    }

protected:
    using SuperType_ = ZObject;

private:
    enum LogServerThreadState_ : Int32 {
        LogServerState_Idle,
        LogServerState_Initialzing,
        LogServerState_WaitingForConnect,
        LogServerState_Logging,
        LogServerState_Closing
    };

    static Void LogServerThreadFunc() noexcept {
        ReturnType link_code = kOK;
        
        //start server
        link_code = Instance().log_server_.Listen();
        if (link_code != kOK) {
            Instance().log_server_thread_state_ = LogServerState_Idle;
            Z_LOG_ERROR(
                error_code::kFTCPLogOutputErrorCode_LinkError, link_code, 
                L"ZTCPSingleSessionServer::SetEndpoint() link error!"
            );
            link_code = Instance().log_server_.Close();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kFTCPLogOutputErrorCode_LinkError, link_code, 
                    L"ZTCPSingleSessionServer::Close() link error!"
                );
            }
        }

        while (Instance().log_server_.State() == ZTCPSingleSessionServer::ZTCPSingleSessionServerState_Listen) {

            Instance().log_server_thread_state_ = LogServerState_WaitingForConnect;

            //wait for clinet connect
            link_code = Instance().log_server_.Accept();
            if (link_code != kOK) {
                Instance().log_server_thread_state_ = LogServerState_Idle;
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_LinkError, link_code, 
                    L"ZTCPSingleSessionServer::Accept() link error!"
                );
                link_code = Instance().log_server_.Close();
                if (link_code != kOK) {
                    Z_LOG_ERROR(
                        error_code::kFTCPLogOutputErrorCode_LinkError, link_code,
                        L"ZTCPSingleSessionServer::Close() link error!"
                    );
                    break;
                }
                break;
            }

            Instance().log_server_thread_state_ = LogServerState_Logging;
     
            TFixedMemory<sizeof(TCPLogOutputCommand)> socket_command_buffer;
            TFixedMemory<sizeof(TCPLogOutputReply)> socket_reply_buffer;
            TCPLogOutputCommand* command_ptr = socket_command_buffer.DataPtr<TCPLogOutputCommand*>();
            TCPLogOutputReply* reply_ptr = socket_reply_buffer.DataPtr<TCPLogOutputReply*>();
            SizeType reply_size;
            TCPLogOutputReplyLogData* log_data_ptr;
            //send log until disconnent
            while (true) {
                //wait for command
                link_code = Instance().log_server_.Read(ZBuffer(
                    socket_command_buffer.DataPtr<Void*>(),
                    sizeof(TCPLogOutputCommand)
                ));
                if (link_code != kOK) {
                    break;
                }

                //generate reply data
                TCPLogOutputCommandIDEnum command_id = command_ptr->command_id_;
                reply_size = sizeof(TCPLogOutputReplyIDEnum);
                switch (command_id) {
                //GetNextLog
                case TCPLogOutputCommandID_GetNextLog:
                    reply_size += sizeof(TCPLogOutputReplyLogData);
                    reply_ptr->reply_id_ = TCPLogOutputReplyID_Log;

                    //wait for log
                    while (
                        Instance().log_buffer_ptr_queue_.Size() == 0 && 
                        Instance().log_server_thread_state_ == LogServerState_Logging
                    ) {
                        Sleep(1);
                    }

                    //copy log data
                    log_data_ptr = Instance().log_buffer_ptr_queue_.Front();
                    Instance().log_buffer_ptr_queue_.Pop();
                    Copy(&reply_ptr->reply_data_.log_data_, log_data_ptr);
                    delete log_data_ptr;

                    break;
                default:
                    Z_LOG_ERROR(
                        error_code::kFTCPLogOutputErrorCode_CommandIDNotExist, 0,
                        L"Command ID not exist! ID: %d", command_id
                    );
                    reply_size += sizeof(TCPLogOutputReplyCommandNotExistData);
                    reply_ptr->reply_id_ = TCPLogOutputReplyID_CommandIDNotExist;
                    reply_ptr->reply_data_.command_not_exist_data_.command_id_ = command_id;
                    break;
                }

                //reply
                link_code = Instance().log_server_.Write(ZConstBuffer(
                    socket_reply_buffer.DataPtr<Void*>(), reply_size
                ));
                if (link_code != kOK) {
                    break;
                }
            }
        }

        //shutdown
        Instance().log_server_thread_state_ = LogServerState_Idle;
    }

    ZTCPLogServer() noexcept
        : SuperType_()
        , log_buffer_ptr_queue_()
        , socket_context_()
        , log_server_(&socket_context_)
        , server_thread_()
        , log_server_thread_state_(LogServerState_Idle)
    {}
    
    ~ZTCPLogServer() noexcept {     
        if (server_thread_.Joinable()) {
            server_thread_.Join();
        }
    }

    Void LogServerBufferResetCheckP() noexcept {
        if (log_server_thread_state_ == LogServerState_Logging) {
            return;
        }
        else {
            TimeType time_filter = TimeSec() - kLogServerDisconnectLogWaitMaxTime;
            while (log_buffer_ptr_queue_.Size() > kLogServerDisconnectLogWaitMaxNum) {
                auto log_data_ptr = log_buffer_ptr_queue_.Front();
                if (log_data_ptr->log_time_ >= time_filter) {
                    break;
                }
                delete log_data_ptr;
                log_buffer_ptr_queue_.Pop();
            }
        }
    }

private:
    TQueueSafe<TCPLogOutputReplyLogData*> log_buffer_ptr_queue_;
    ZIOContext socket_context_;
    ZTCPSingleSessionServer log_server_;
    ZThread server_thread_;
    TAtom<LogServerThreadState_> log_server_thread_state_;
};

class ZTCPLogClient : ZObject {
public:
    static ZTCPLogClient& Instance() noexcept {
        static ZTCPLogClient log_client;
        return log_client;
    }

    NODISCARD ReturnType StartClient(
        const TFunction<Void(const TCPLogOutputReplyLogData*)>& _data_handle_func,
        const TFunction<Void()>& _client_connect_handle_func,
        const TFunction<Void()>& _client_finish_handle_func,
        const ZTCPEndpoint& _tcp_endpoint,
        Int32 _repeat_times
    ) noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;

        Z_CHECK(
            log_client_thread_state_ != LogClientState_Idle,
            error_code::kPSocketErrorCode_StateError,
            L"Client state error! state: %d expect state: %d",
            log_client_thread_state_.Value(), LogClientState_Idle
        );

        //set handle func
        data_handle_func_ = _data_handle_func;
        client_connect_handle_func_ = _client_connect_handle_func;		
        client_finish_handle_func_ = _client_finish_handle_func;
        //start log client
        log_client_thread_state_ = LogClientState_Initialzing;
        client_thread_ = ZThread(LogClientThreadFunc, ZTCPEndpoint(_tcp_endpoint), _repeat_times);

        return ret_val;
    }

    NODISCARD ReturnType StopClient() noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;

        log_client_.StopConnect();

        if (log_client_thread_state_ == LogClientState_Idle || log_client_thread_state_ == LogClientState_Closing) {
            return ret_val;
        }

        log_client_thread_state_ = LogClientState_Closing;

        if (log_client_.GetSocket().State() != ZTCPSocket::ZTCPSocketState_Idle) {
            //stop log client
            link_code = log_client_.Close();
            if (link_code != kOK) {
                ret_val = error_code::kPSocketErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, L"ZTCPSingleSessionClient::Close() link error!");
                return ret_val;
            }
        }

        //wait for thread finish
        if (client_thread_.Joinable()) {
            client_thread_.Join();
        }

        return ret_val;
    }

protected:
    using SuperType_ = ZObject;

private:
    enum LogClientThreadState_ : Int32 {
        LogClientState_Idle,
        LogClientState_Initialzing,
        LogClientState_WaitingToConnect,
        LogClientState_GettingLogs,
        LogClientState_Closing
    };

    static Void LogClientThreadFunc(
        ZTCPEndpoint&& _tcp_endpoint,
        Int32 _repeat_times
    ) noexcept {
        ReturnType link_code = kOK;

        Instance().log_client_thread_state_ = LogClientState_WaitingToConnect;

        //wait for clinet connect
        link_code = Instance().log_client_.Connect(_tcp_endpoint, _repeat_times);
        if (link_code != kOK) {
            if (link_code == error_code::kPSocketErrorCode_ConnectFailed) {
            }
            else {
                Z_LOG_ERROR(
                    error_code::kPSocketErrorCode_LinkError, link_code,
                    L"ZTCPSingleSessionClient::Connect() link error!"
                );
            }
            Z_LOG_FAILURE(L"Log server connect failed!");
            link_code = Instance().log_client_.Close();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kFTCPLogOutputErrorCode_LinkError, link_code,
                    L"ZTCPSingleSessionClient::Close() link error!"
                );
            }
            //call handle func
            Instance().client_finish_handle_func_();
            //shutdown
            Instance().log_client_thread_state_ = LogClientState_Idle;
            return;
        }

        //call handle func
        Instance().client_connect_handle_func_();

        Instance().log_client_thread_state_ = LogClientState_GettingLogs;        

        TFixedMemory<sizeof(TCPLogOutputCommand)> socket_command_buffer;
        TFixedMemory<sizeof(TCPLogOutputReply)> socket_reply_buffer;
        TCPLogOutputCommand* command_ptr = socket_command_buffer.DataPtr<TCPLogOutputCommand*>();
        TCPLogOutputReply* reply_ptr = socket_reply_buffer.DataPtr<TCPLogOutputReply*>();
        SizeType command_size;
        //get log until disconnent
        while (true) {
            //command data
            command_size = sizeof(TCPLogOutputCommandIDEnum);
            command_ptr->command_id_ = TCPLogOutputCommandID_GetNextLog;

            //send command
            link_code = Instance().log_client_.Write(ZConstBuffer(
                socket_command_buffer.DataPtr<Void*>(),
                command_size
            ));
            if (link_code != kOK) {
                break;
            }

            //wait for reply
            link_code = Instance().log_client_.Read(ZBuffer(
                socket_reply_buffer.DataPtr<Void*>(),
                sizeof(TCPLogOutputReply)
            ));
            if (link_code != kOK) {
                break;
            }

            //handle reply data
            Int32 reply_id = reply_ptr->reply_id_;
            switch (reply_id) {
            //log
            case TCPLogOutputReplyID_Log:
                //handle log
                Instance().data_handle_func_(&reply_ptr->reply_data_.log_data_);
                break;
            //CommandIDNotExist
            case TCPLogOutputReplyID_CommandIDNotExist:
                Z_LOG_ERROR(
                    error_code::kFTCPLogOutputErrorCode_CommandIDNotExist, 0,
                    L"Command ID not exist! ID: %d", reply_ptr->reply_data_.command_not_exist_data_.command_id_
                );
                break;
            default:
                Z_LOG_ERROR(
                    error_code::kFTCPLogOutputErrorCode_ReplyIDNotExist, 0, 
                    L"Reply ID not exist! ID: %d", reply_id
                );
                break;
            }
        }

        //call handle func
        Instance().client_finish_handle_func_();

        //shutdown
        Instance().log_client_thread_state_ = LogClientState_Idle;
    }

    ZTCPLogClient() noexcept
        : SuperType_()
        , data_handle_func_()
        , client_connect_handle_func_()
        , client_finish_handle_func_()
        , socket_context_()
        , log_client_(&socket_context_)
        , client_thread_()
        , log_client_thread_state_(LogClientState_Idle)
    {
    }

    ~ZTCPLogClient() noexcept {
        if (client_thread_.Joinable()) {
            client_thread_.Join();
        }
    }

private:
    TFunction<Void(const TCPLogOutputReplyLogData*)> data_handle_func_;
    TFunction<Void()> client_connect_handle_func_;
    TFunction<Void()> client_finish_handle_func_;
    ZIOContext socket_context_;
    ZTCPSingleSessionClient log_client_;
    ZThread client_thread_;
    TAtom<LogClientThreadState_> log_client_thread_state_;
};

SOCKET_DLLAPI ReturnType StartLogOutputServer(
    const ZTCPEndpoint& _tcp_endpoint
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = ZTCPLogServer::Instance().StartServer(_tcp_endpoint);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kFTCPLogOutputErrorCode_LinkError, link_code,
            L"ZTCPLogServer::StartServer() link error!"
        );
    }
    return kOK;
}

SOCKET_DLLAPI ReturnType StopLogOutputServer() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = ZTCPLogServer::Instance().StopServer();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kFTCPLogOutputErrorCode_LinkError, link_code,
            L"ZTCPLogServer::StopServer() link error!"
        );
    }
    return kOK;
}

SOCKET_DLLAPI ReturnType StartLogOutputClient(
    const TFunction<Void(const TCPLogOutputReplyLogData*)>& _data_handle_func,
    const TFunction<Void()>& _client_connect_handle_func,
    const TFunction<Void()>& _client_finish_handle_func,
    const ZTCPEndpoint& _tcp_endpoint,
    Int32 _repeat_times
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = ZTCPLogClient::Instance().StartClient(
        _data_handle_func,
        _client_connect_handle_func,
        _client_finish_handle_func,
        _tcp_endpoint,
        _repeat_times
    );
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kFTCPLogOutputErrorCode_LinkError, link_code,
            L"ZTCPLogServer::StartServer() link error!"
        );
    }
    return kOK;
}

SOCKET_DLLAPI ReturnType StopLogOutputClient() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = ZTCPLogClient::Instance().StopClient();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kFTCPLogOutputErrorCode_LinkError, link_code,
            L"ZTCPLogClient::StopClient() link error!"
        );
    }
    return kOK;
}

}//socket
}//zengine