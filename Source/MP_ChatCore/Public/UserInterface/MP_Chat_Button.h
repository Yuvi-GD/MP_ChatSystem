// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MP_Chat_Button.generated.h"

class UTextBlock;
class UButtonSlot;

// Button Click Event Dispatcher
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMPButtonClicked, int32, ButtonIndex);


/**
 * 
 */
UCLASS()
class MP_CHATCORE_API UMP_Chat_Button : public UButton
{
    GENERATED_BODY()
	
public:
    UMP_Chat_Button(const FObjectInitializer& ObjectInitializer);

protected:
    // Called when the button is constructed
    //virtual void NativeConstruct();

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_Chat_Button")
    int32 ButtonIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_Chat_Button")
    FText ButtonText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_Chat_Button|Text")
    FSlateFontInfo Font;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_Chat_Button|Text")
    FSlateColor TextColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP_Chat_Button|Text")
    FMargin TextPadding;

    UFUNCTION(BlueprintCallable, Category = "MP_Chat_Button")
    void SetButtonText(const FText& NewText);

    UFUNCTION(BlueprintCallable, Category = "MP_Chat_Button")
    void SetTextColor(const FSlateColor& NewColor);

    UFUNCTION(BlueprintCallable, Category = "MP_Chat_Button")
    void SetTextFont(const FSlateFontInfo& NewColor);

    UFUNCTION(BlueprintCallable, Category = "MP_Chat_Button")
    void SetTextPadding(const FMargin& NewTextPadding);

    UFUNCTION(BlueprintCallable, Category = "MP_Chat_Button")
    void HandleButtonClick();
    
    UFUNCTION(BlueprintCallable, Category = "MP_Chat_Button")
    void SetImage();

    UFUNCTION(BlueprintCallable, Category = "MP_Chat_Button")
    void SetButtonDrawType(ESlateBrushDrawType::Type InDrawType);

    // Event Dispatcher to bind to button clicks
    UPROPERTY(BlueprintAssignable, Category = "MP_Chat_Button|Events")
    FOnMPButtonClicked OnMPButtonClicked;

    UPROPERTY()
    TObjectPtr<UTextBlock> TextBlock;

    void BuidButton();

};
