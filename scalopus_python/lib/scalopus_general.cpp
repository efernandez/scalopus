/*
  Copyright (c) 2018-2019, Ivor Wanders
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of the author nor the names of contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "scalopus_general.h"
#include <pybind11/stl.h>
#include <scalopus_general/general.h>

namespace scalopus
{
namespace py = pybind11;
void add_scalopus_general(py::module& m)
{
  py::module general = m.def_submodule("general", "The general components.");
  py::class_<EndpointIntrospect, EndpointIntrospect::Ptr, Endpoint> endpoint_introspect(general, "EndpointIntrospect");
  endpoint_introspect.def(py::init<>());
  endpoint_introspect.def("supported", &EndpointIntrospect::supported);

  py::class_<EndpointProcessInfo::ProcessInfo> endpoint_process_info_info(general, "ProcessInfo");
  endpoint_process_info_info.def_readwrite("name", &EndpointProcessInfo::ProcessInfo::name);
  endpoint_process_info_info.def_readwrite("threads", &EndpointProcessInfo::ProcessInfo::threads);

  py::class_<EndpointProcessInfo, EndpointProcessInfo::Ptr, Endpoint> endpoint_process_info(general,
                                                                                            "EndpointProcessInfo");
  endpoint_process_info.def(py::init<>());
  endpoint_process_info.def("setProcessName", &EndpointProcessInfo::setProcessName);
  endpoint_process_info.def("processInfo", &EndpointProcessInfo::processInfo);

  general.def("setThreadName", [](const std::string& name) { ThreadNameTracker::getInstance().setCurrentName(name); });
}
}  // namespace scalopus
