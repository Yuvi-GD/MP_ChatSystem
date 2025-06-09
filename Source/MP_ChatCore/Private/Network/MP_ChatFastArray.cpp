// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 2025, Yuvi_GD
// All rights reserved.

#include "Network/MP_ChatFastArray.h"
//#include "Runtime/MP_ChatManager.h"


// Add a message, trim if needed, and mark dirty.
void FMP_ChatMsgArray::AddMessage(const FMP_ChatMessage& NewMsg, int32 MaxMessages)
{
    Messages.Add(NewMsg);
    MarkItemDirty(Messages.Last());

    TrimToRecent(MaxMessages);
    MarkArrayDirty();
}

// Remove oldest messages if array exceeds MaxMessages (>0 means limited)
void FMP_ChatMsgArray::TrimToRecent(int32 MaxMessages)
{
    if (MaxMessages > 0 && Messages.Num() > MaxMessages)
    {
        int32 NumToRemove = Messages.Num() - MaxMessages;
        Messages.RemoveAt(0, NumToRemove, false); // Remove from front
    }
}

bool FMP_ChatMsgArray::EditMessage(int32 MsgIndex, const FString& NewText)
{
    if (Messages.IsValidIndex(MsgIndex))
    {
        FMP_ChatMessage& Msg = Messages[MsgIndex];
        Msg.MessageText = NewText;
        Msg.MessageFlags = 1;
        Msg.Timestamp = FPlatformTime::Seconds();
        MarkItemDirty(Msg);
        MarkArrayDirty();
        return true;
    }
    return false;
}

bool FMP_ChatMsgArray::DeleteMessage(int32 MsgIndex)
{
    if (Messages.IsValidIndex(MsgIndex))
    {
        Messages.RemoveAt(MsgIndex);
        MarkArrayDirty();
        return true;
    }
    return false;
}