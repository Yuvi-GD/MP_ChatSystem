// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MP_Chat_Button.h"
#include "Components/TextBlock.h"
#include "Components/ButtonSlot.h"
#include "Components/SlateWrapperTypes.h"

UMP_Chat_Button::UMP_Chat_Button(const FObjectInitializer& ObjectInitializer)
{
    ButtonIndex = -1;
    ButtonText = FText::FromString(TEXT("Text"));
    TextPadding = 5.0f;
    TextColor = FSlateColor(FLinearColor::Black);
    OnClicked.AddDynamic(this, &UMP_Chat_Button::HandleButtonClick);  // Bind the OnClicked event to HandleButtonClick

}

TSharedRef<SWidget> UMP_Chat_Button::RebuildWidget()
{
    BuidButton();
    return Super::RebuildWidget();
}

void UMP_Chat_Button::BuidButton()
{
    // Create TextBlock
    if (!TextBlock)
    {
        TextBlock = NewObject<UTextBlock>(this);
    }
    if (TextBlock)
    {
        TextBlock->SetText(ButtonText);
        TextBlock->SetColorAndOpacity(TextColor);
        this->AddChild(TextBlock);
        UButtonSlot* ButonSlot = Cast<UButtonSlot>(TextBlock->Slot);
        if (ButonSlot)
        {
            ButonSlot->SetPadding(TextPadding);
        }
    }
}

//void UMP_Chat_Button::NativeConstruct()
//{
//    BuidButton();
//}

#if WITH_EDITOR
void UMP_Chat_Button::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Refresh the panel when any property changes in the editor
    BuidButton();
}
#endif

void UMP_Chat_Button::SetButtonText(const FText& NewText)
{
    ButtonText = NewText;
    if (TextBlock)
    {
        TextBlock->SetText(NewText);
    }
}

void UMP_Chat_Button::SetTextColor(const FSlateColor& NewColor)
{
    TextColor = NewColor;
    if (TextBlock)
    {
        TextBlock->SetColorAndOpacity(TextColor);
    }
}

void UMP_Chat_Button::SetTextFont(const FSlateFontInfo& NewColor)
{
    // This function is not implemented in the original code, but you can set the font like this:
     Font = NewColor;
     if (TextBlock)
     {
         TextBlock->SetFont(Font);
     }
}

void UMP_Chat_Button::SetTextPadding(const FMargin& NewTextPadding)
{
    TextPadding = NewTextPadding;
    if (TextBlock)
    {
        UButtonSlot* ButonSlot = Cast<UButtonSlot>(TextBlock->Slot);
        if (ButonSlot)
        {
            ButonSlot->SetPadding(TextPadding);
        }
    }
}

void UMP_Chat_Button::HandleButtonClick()
{
    UE_LOG(LogTemp, Warning, TEXT("Button with Index %d clicked!"), ButtonIndex);

    // Broadcast the button click event with the button index
    OnMPButtonClicked.Broadcast(ButtonIndex);
}

void UMP_Chat_Button::SetImage()
{
    //FSlateBrush style = this->GetStyle;
}

void UMP_Chat_Button::SetButtonDrawType(ESlateBrushDrawType::Type InDrawType)
{
    // Get the current style
    FButtonStyle CurrentStyle = GetStyle();

    // Modify the brush draw types
    CurrentStyle.Normal.DrawAs = InDrawType;
    CurrentStyle.Hovered.DrawAs = InDrawType;
    CurrentStyle.Pressed.DrawAs = InDrawType;
    CurrentStyle.Disabled.DrawAs = InDrawType;

    // Set the updated style
    SetStyle(CurrentStyle);

    // Apply the updated style
    SynchronizeProperties();
}
