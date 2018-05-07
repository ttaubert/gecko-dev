/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_U2FAndroidTokenManager_h
#define mozilla_dom_U2FAndroidTokenManager_h

#include "mozilla/dom/U2FTokenTransport.h"

/*
 * U2FAndroidTokenManager is a token implementation communicating with
 * Android Fido2 APIs.
 */

namespace mozilla {
namespace dom {

class U2FAndroidTokenManager final : public U2FTokenTransport
{
public:
  explicit U2FAndroidTokenManager();

  RefPtr<U2FRegisterPromise>
  Register(const WebAuthnMakeCredentialInfo& aInfo,
           bool aForceNoneAttestation) override;

  RefPtr<U2FSignPromise>
  Sign(const WebAuthnGetAssertionInfo& aInfo) override;

  void Cancel() override;

private:
  ~U2FAndroidTokenManager() { }

  void ClearPromises() {
    mRegisterPromise.RejectIfExists(NS_ERROR_DOM_UNKNOWN_ERR, __func__);
    mSignPromise.RejectIfExists(NS_ERROR_DOM_UNKNOWN_ERR, __func__);
  }

  MozPromiseHolder<U2FRegisterPromise> mRegisterPromise;
  MozPromiseHolder<U2FSignPromise> mSignPromise;
};

} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_U2FAndroidTokenManager_h
