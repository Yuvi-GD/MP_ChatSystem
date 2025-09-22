// Copyright Epic Games, Inc. All Rights Reserved.

#include "MP_ChatCore.h"
#include "Misc/ConfigCacheIni.h"

#define LOCTEXT_NAMESPACE "FMP_ChatCoreModule"

void FMP_ChatCoreModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Warning, TEXT("MP_ChatCore::StartupModule: Loading config..."));

	GConfig->LoadFile(TEXT("DefaultMP_ChatSystem.ini"));
	//GConfig->CombineFromBuffer(TEXT("DefaultMP_ChatSystem.ini"));
}

void FMP_ChatCoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMP_ChatCoreModule, MP_ChatCore)