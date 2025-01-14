// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8StackTraceImpl_h
#define V8StackTraceImpl_h

#include "platform/inspector_protocol/Platform.h"
#include "platform/v8_inspector/public/V8StackTrace.h"

#include <vector>

namespace blink {

class TracedValue;
class V8DebuggerImpl;

// Note: async stack trace may have empty top stack with non-empty tail to indicate
// that current native-only state had some async story.
// On the other hand, any non-top async stack is guaranteed to be non-empty.
class V8StackTraceImpl final : public V8StackTrace {
    PROTOCOL_DISALLOW_COPY(V8StackTraceImpl);
public:
    class Frame  {
    public:
        Frame();
        Frame(const String16& functionName, const String16& scriptId, const String16& scriptName, int lineNumber, int column = 0);
        ~Frame();

        const String16& functionName() const { return m_functionName; }
        const String16& scriptId() const { return m_scriptId; }
        const String16& sourceURL() const { return m_scriptName; }
        int lineNumber() const { return m_lineNumber; }
        int columnNumber() const { return m_columnNumber; }
        Frame isolatedCopy() const;

    private:
        friend class V8StackTraceImpl;
        std::unique_ptr<protocol::Runtime::CallFrame> buildInspectorObject() const;
        void toTracedValue(TracedValue*) const;

        String16 m_functionName;
        String16 m_scriptId;
        String16 m_scriptName;
        int m_lineNumber;
        int m_columnNumber;
    };

    static std::unique_ptr<V8StackTraceImpl> create(V8DebuggerImpl*, int contextGroupId, v8::Local<v8::StackTrace>, size_t maxStackSize, const String16& description = String16());
    static std::unique_ptr<V8StackTraceImpl> capture(V8DebuggerImpl*, int contextGroupId, size_t maxStackSize, const String16& description = String16());

    std::unique_ptr<V8StackTrace> clone() override;
    std::unique_ptr<V8StackTraceImpl> cloneImpl();
    std::unique_ptr<V8StackTrace> isolatedCopy() override;
    std::unique_ptr<V8StackTraceImpl> isolatedCopyImpl();
    std::unique_ptr<protocol::Runtime::StackTrace> buildInspectorObjectForTail(V8DebuggerImpl*) const;
    ~V8StackTraceImpl() override;

    // V8StackTrace implementation.
    bool isEmpty() const override { return !m_frames.size(); };
    String16 topSourceURL() const override;
    int topLineNumber() const override;
    int topColumnNumber() const override;
    String16 topScriptId() const override;
    String16 topFunctionName() const override;
    std::unique_ptr<protocol::Runtime::StackTrace> buildInspectorObject() const override;
    String16 toString() const override;

private:
    V8StackTraceImpl(int contextGroupId, const String16& description, std::vector<Frame>& frames, std::unique_ptr<V8StackTraceImpl> parent);

    int m_contextGroupId;
    String16 m_description;
    std::vector<Frame> m_frames;
    std::unique_ptr<V8StackTraceImpl> m_parent;
};

} // namespace blink

#endif // V8StackTraceImpl_h
