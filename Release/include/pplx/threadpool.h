/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved. 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
*
* Simple Linux implementation of a static thread pool.
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
***/
#pragma once

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Winfinite-recursion"
#endif
#include "boost/asio.hpp"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#if defined(__ANDROID__)
#include <atomic>
#include <jni.h>
#include "pplx/pplx.h"
#endif

#include "cpprest/details/cpprest_compat.h"

namespace crossplat {

#if defined(__ANDROID__)
// IDEA: Break this section into a separate android/jni header
extern std::atomic<JavaVM*> JVM;
JNIEnv* get_jvm_env();

struct java_local_ref_deleter
{
    void operator()(jobject lref) const
    {
        crossplat::get_jvm_env()->DeleteLocalRef(lref);
    }
};

template<class T>
using java_local_ref = std::unique_ptr<typename std::remove_pointer<T>::type, java_local_ref_deleter>;
#endif

class threadpool
{
public:
    static threadpool& shared_instance();
    _ASYNCRTIMP static std::unique_ptr<threadpool> __cdecl construct(size_t num_threads);

    virtual ~threadpool() = default;

    template<typename T>
    CASABLANCA_DEPRECATED("Use `.service().post(task)` directly.")
    void schedule(T task)
    {
        service().post(task);
    }

    boost::asio::io_service& service() { return m_service; }

protected:
    threadpool(size_t num_threads) : m_service(num_threads) {}

    boost::asio::io_service m_service;
};

}
