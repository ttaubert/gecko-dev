/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/U2FAndroidTokenManager.h"
#include "mozilla/ipc/BackgroundParent.h"
#include "GeneratedJNIWrappers.h"

namespace mozilla {
namespace dom {

U2FAndroidTokenManager::U2FAndroidTokenManager()
{
  mozilla::ipc::AssertIsOnBackgroundThread();
  MOZ_ASSERT(XRE_IsParentProcess());
}

RefPtr<U2FRegisterPromise>
U2FAndroidTokenManager::Register(const WebAuthnMakeCredentialInfo& aInfo,
                                 bool aForceNoneAttestation)
{
  mozilla::ipc::AssertIsOnBackgroundThread();

  // Can't be a U2F API request.
  if (aInfo.Extra().type() == WebAuthnMaybeMakeCredentialExtraInfo::Tnull_t) {
    return U2FRegisterPromise::CreateAndReject(NS_ERROR_DOM_NOT_SUPPORTED_ERR, __func__);
  }

  ClearPromises();

  const auto& extra = aInfo.Extra().get_WebAuthnMakeCredentialExtraInfo();
  const auto& rp = extra.Rp();
  const auto& user = extra.User();

  GetMainThreadEventTarget()->Dispatch(NS_NewRunnableFunction(
      "java::GeckoAppShell::WebAuthnMakeCredential",
      [aInfo, rp, user] () {
        MOZ_ASSERT(NS_IsMainThread());

        const nsTArray<uint8_t>& challBuf = aInfo.Challenge();
        jni::ByteBuffer::LocalRef challenge =
          jni::ByteBuffer::New(const_cast<void*>(static_cast<const void*>(challBuf.Elements())), challBuf.Length());

        const nsTArray<uint8_t>& uidBuf = user.Id();
        NS_ConvertUTF8toUTF16 uid(reinterpret_cast<const char*>(uidBuf.Elements()), uidBuf.Length());

        java::GeckoAppShell::WebAuthnMakeCredential(
          aInfo.RpId(), rp.Name(), rp.Icon(), uid, user.Name(), user.Icon(),
          user.DisplayName(), challenge, aInfo.Origin());
      }));

  return mRegisterPromise.Ensure(__func__);
}

RefPtr<U2FSignPromise>
U2FAndroidTokenManager::Sign(const WebAuthnGetAssertionInfo& aInfo)
{
  mozilla::ipc::AssertIsOnBackgroundThread();

  ClearPromises();

  return U2FSignPromise::CreateAndReject(NS_ERROR_DOM_NOT_SUPPORTED_ERR, __func__);
}

void
U2FAndroidTokenManager::Cancel()
{
  mozilla::ipc::AssertIsOnBackgroundThread();

  ClearPromises();
}

}
}
