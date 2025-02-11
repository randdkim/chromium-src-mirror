// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/public/provider/chrome/browser/signin/chrome_identity_interaction_manager.h"

#import "base/ios/weak_nsobject.h"
#include "base/logging.h"

@interface ChromeIdentityInteractionManager () {
  base::WeakNSProtocol<id<ChromeIdentityInteractionManagerDelegate>> _delegate;
}
@end

@implementation ChromeIdentityInteractionManager

- (id<ChromeIdentityInteractionManagerDelegate>)delegate {
  return _delegate;
}

- (void)setDelegate:(id<ChromeIdentityInteractionManagerDelegate>)delegate {
  _delegate.reset(delegate);
}

- (BOOL)isCanceling {
  return NO;
}

- (void)addAccountWithCompletion:(SigninCompletionCallback)completion {
  NOTREACHED() << "Subclasses must override this";
  completion(nil, nil);
}

- (void)reauthenticateUserWithEmail:(NSString*)email
                         completion:(SigninCompletionCallback)completion {
  NOTREACHED() << "Subclasses must override this";
  completion(nil, nil);
}

- (void)cancelAndDismissAnimated:(BOOL)animated {
  NOTREACHED() << "Subclasses must override this";
}

@end
