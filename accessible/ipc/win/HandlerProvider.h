/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_a11y_HandlerProvider_h
#define mozilla_a11y_HandlerProvider_h

#include "mozilla/a11y/AccessibleHandler.h"
#include "mozilla/AlreadyAddRefed.h"
#include "mozilla/Atomics.h"
#include "mozilla/mscom/IHandlerProvider.h"
#include "mozilla/mscom/Ptr.h"
#include "mozilla/mscom/StructStream.h"
#include "mozilla/Mutex.h"
#include "mozilla/UniquePtr.h"

struct NEWEST_IA2_INTERFACE;

namespace mozilla {

namespace mscom {

class StructToStream;

} // namespace mscom

namespace a11y {

class HandlerProvider final : public IGeckoBackChannel
                            , public mscom::IHandlerProvider
{
public:
  HandlerProvider(REFIID aIid, mscom::InterceptorTargetPtr<IUnknown> aTarget);

  // IUnknown
  STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
  STDMETHODIMP_(ULONG) AddRef() override;
  STDMETHODIMP_(ULONG) Release() override;

  // IHandlerProvider
  STDMETHODIMP GetHandler(NotNull<CLSID*> aHandlerClsid) override;
  STDMETHODIMP GetHandlerPayloadSize(NotNull<mscom::IInterceptor*> aInterceptor,
                                     NotNull<DWORD*> aOutPayloadSize) override;
  STDMETHODIMP WriteHandlerPayload(NotNull<mscom::IInterceptor*> aInterceptor,
                                   NotNull<IStream*> aStream) override;
  STDMETHODIMP_(REFIID) MarshalAs(REFIID aIid) override;
  STDMETHODIMP_(REFIID) GetEffectiveOutParamIid(REFIID aCallIid,
                                                ULONG aCallMethod) override;
  STDMETHODIMP NewInstance(REFIID aIid,
                           mscom::InterceptorTargetPtr<IUnknown> aTarget,
                           NotNull<mscom::IHandlerProvider**> aOutNewPayload) override;

  // IGeckoBackChannel
  STDMETHODIMP put_HandlerControl(long aPid, IHandlerControl* aCtrl) override;
  STDMETHODIMP Refresh(DynamicIA2Data* aOutData) override;

private:
  ~HandlerProvider() = default;

  void SetHandlerControlOnMainThread(DWORD aPid,
                                     mscom::ProxyUniquePtr<IHandlerControl> aCtrl);
  void GetAndSerializePayload(const MutexAutoLock&,
                              NotNull<mscom::IInterceptor*> aInterceptor);
  void BuildStaticIA2Data(NotNull<mscom::IInterceptor*> aInterceptor,
                          StaticIA2Data* aOutData);
  void BuildDynamicIA2Data(DynamicIA2Data* aOutIA2Data);
  void BuildInitialIA2Data(NotNull<mscom::IInterceptor*> aInterceptor,
                           StaticIA2Data* aOutStaticData,
                           DynamicIA2Data* aOutDynamicData);
  static void CleanupStaticIA2Data(StaticIA2Data& aData);
  static void CleanupDynamicIA2Data(DynamicIA2Data& aData);
  bool IsTargetInterfaceCacheable();

  Atomic<uint32_t>                  mRefCnt;
  Mutex                             mMutex; // Protects mSerializer
  const IID                         mTargetUnkIid;
  mscom::InterceptorTargetPtr<IUnknown> mTargetUnk; // Constant, main thread only
  UniquePtr<mscom::StructToStream>  mSerializer;
  RefPtr<IUnknown>                  mFastMarshalUnk;
};

} // namespace a11y
} // namespace mozilla

#endif // mozilla_a11y_HandlerProvider_h
