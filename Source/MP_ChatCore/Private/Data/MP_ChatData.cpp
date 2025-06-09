// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 2025, Yuvi_GD
// All rights reserved.

#include "Data/MP_ChatData.h"


// FMP_ChatMessage::NetSerialize implementation
bool FMP_ChatMessage::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
    bOutSuccess = true;

    // Serialize strings (could use GUID for MessageID/SenderID for extra efficiency)
    Ar << SenderID;
    Ar << SenderName;
    Ar << MessageText;
    Ar << Timestamp;
    Ar << MessageFlags;

    return true;
}
