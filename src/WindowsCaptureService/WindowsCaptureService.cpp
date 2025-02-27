// Copyright (c) 2021 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WindowsCaptureService/WindowsCaptureService.h"

#include <grpcpp/grpcpp.h>
#include <stdint.h>

#include "CaptureServiceBase/GrpcStartStopCaptureRequestWaiter.h"
#include "GrpcProtos/capture.pb.h"
#include "OrbitBase/ThreadUtils.h"
#include "ProducerEventProcessor/GrpcClientCaptureEventCollector.h"
#include "TracingHandler.h"

namespace orbit_windows_capture_service {

using orbit_grpc_protos::CaptureOptions;
using orbit_grpc_protos::CaptureRequest;
using orbit_grpc_protos::CaptureResponse;

grpc::Status WindowsCaptureService::Capture(
    grpc::ServerContext*,
    grpc::ServerReaderWriter<CaptureResponse, CaptureRequest>* reader_writer) {
  orbit_base::SetCurrentThreadName("WinCS::Capture");

  orbit_producer_event_processor::GrpcClientCaptureEventCollector
      grpc_client_capture_event_collector{reader_writer};
  CaptureServiceBase::CaptureInitializationResult initialization_result =
      InitializeCapture(&grpc_client_capture_event_collector);
  switch (initialization_result) {
    case CaptureInitializationResult::kSuccess:
      break;
    case CaptureInitializationResult::kAlreadyInProgress:
      return {grpc::StatusCode::ALREADY_EXISTS,
              "Cannot start capture because another capture is already in progress"};
  }

  orbit_capture_service_base::GrpcStartStopCaptureRequestWaiter
      grpc_start_stop_capture_request_waiter{reader_writer};
  const CaptureOptions& capture_options =
      grpc_start_stop_capture_request_waiter.WaitForStartCaptureRequest();

  StartEventProcessing(capture_options);
  TracingHandler tracing_handler{producer_event_processor_.get()};
  tracing_handler.Start(capture_options);
  StopCaptureReason stop_capture_reason =
      grpc_start_stop_capture_request_waiter.WaitForStopCaptureRequest();
  tracing_handler.Stop();
  FinalizeEventProcessing(stop_capture_reason);

  TerminateCapture();

  return grpc::Status::OK;
}

}  // namespace orbit_windows_capture_service
