#!/usr/bin/env python
# Copyright (c) 2018-2019, Ivor Wanders
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the author nor the names of contributors may be used to
#   endorse or promote products derived from this software without specific
#   prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


import sys
print("Running tests with Python: {}".format(sys.version))

import scalopus
import time
import os
import unittest
import threading


class TracingTester(unittest.TestCase):
    def __init__(self, *args):
        unittest.TestCase.__init__(self, *args)
        self.factory = scalopus.transport.TransportLoopbackFactory()

        # set up the producer side
        self.server = self.factory.serve()
        self.server.addEndpoint(scalopus.general.EndpointIntrospect())
        processinfo = scalopus.general.EndpointProcessInfo()
        processinfo.setProcessName("MyPythonProcess")
        self.server.addEndpoint(processinfo)
        self.server.addEndpoint(scalopus.tracing.EndpointTraceMapping())
        self.server.addEndpoint(scalopus.tracing.EndpointNativeTraceSender())

        # set up the consumer side.
        self.poller = scalopus.general.EndpointManagerPoll(self.factory)
        self.native_provider = scalopus.tracing.native.NativeTraceProvider(self.poller)
        self.poller.addEndpointFactory(scalopus.tracing.EndpointNativeTraceSender.name, self.native_provider.factory)
        self.poller.manage()  # do one round of discovery
        self.native_source = self.native_provider.makeSource()
        self.native_source.startInterval() # start the recording interval on the native source.

    def test_tracing(self):
        trace_point = scalopus.tracing.TraceContext("MyTraceContext", trace_id=1337)
        scalopus.lib.general.setThreadName("MyTestThread")
        for i in range(3):
            with trace_point:
                time.sleep(0.1)
            time.sleep(0.1)

        # add an extra manual mapping.
        scalopus.tracing.setTraceName(10, "Ten")

        # now try to retrieve as much as possible, first check if we can connect to the loopback server
        # and make a client connection.
        clients = self.factory.discover()
        self.assertEqual(len(clients), 1)
        client = self.factory.connect(clients[0])

        # check if the mappings were stored.
        mapping_client = scalopus.lib.tracing.EndpointTraceMapping()
        client.addEndpoint(mapping_client)
        mappings = mapping_client.mapping()
        pid = os.getpid()
        self.assertIn(pid, mappings)
        self.assertDictEqual({1337: "MyTraceContext", 10:"Ten"}, mappings[pid])

        # check if the process name was stored.
        processinfo_client = scalopus.lib.general.EndpointProcessInfo()
        client.addEndpoint(processinfo_client)
        info = processinfo_client.processInfo()
        self.assertEqual(info.name, "MyPythonProcess")
        self.assertDictEqual({threading.get_ident(): "MyTestThread"}, info.threads)

        # stop the interval and collect the data.
        self.native_source.stopInterval()
        data = self.native_source.finishInterval()
        print(data)

if __name__ == '__main__':
    unittest.main()