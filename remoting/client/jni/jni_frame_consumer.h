// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_CLIENT_JNI_JNI_FRAME_CONSUMER_H_
#define REMOTING_CLIENT_JNI_JNI_FRAME_CONSUMER_H_

#include <list>
#include <memory>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "remoting/client/jni/jni_video_renderer.h"
#include "remoting/protocol/frame_consumer.h"
#include "third_party/webrtc/modules/desktop_capture/desktop_geometry.h"

namespace remoting {

class ChromotingJniRuntime;
class JniClient;
class JniDisplayHandler;
class SoftwareVideoRenderer;

// FrameConsumer and VideoRenderer implementation that draws onto a JNI direct
// byte buffer.
class JniFrameConsumer : public protocol::FrameConsumer,
                         public JniVideoRenderer {
 public:
  JniFrameConsumer(
      ChromotingJniRuntime* jni_runtime,
      base::WeakPtr<JniDisplayHandler> display);

  ~JniFrameConsumer() override;

  // FrameConsumer implementation.
  std::unique_ptr<webrtc::DesktopFrame> AllocateFrame(
      const webrtc::DesktopSize& size) override;
  void DrawFrame(std::unique_ptr<webrtc::DesktopFrame> frame,
                 const base::Closure& done) override;
  PixelFormat GetPixelFormat() override;

  // JniVideoRenderer implementation.
  void OnSessionConfig(const protocol::SessionConfig& config) override;
  protocol::VideoStub* GetVideoStub() override;
  protocol::FrameConsumer* GetFrameConsumer() override;
  void Initialize(
        scoped_refptr<base::SingleThreadTaskRunner> decode_task_runner,
        protocol::PerformanceTracker* perf_tracker) override;

 private:
  class Renderer;

  void OnFrameRendered(const base::Closure& done);

  // Used to obtain task runner references and make calls to Java methods.
  ChromotingJniRuntime* jni_runtime_;

  std::unique_ptr<SoftwareVideoRenderer> video_renderer_;

  // Renderer object used to render the frames on the display thread.
  std::unique_ptr<Renderer> renderer_;

  base::WeakPtrFactory<JniFrameConsumer> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(JniFrameConsumer);
};

}  // namespace remoting

#endif
