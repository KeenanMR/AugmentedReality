// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/CanvasPanel.h"
#include "Anchor.h"
#include "Portal.h"
#include "Castle.h"
#include "Blueprint/UserWidget.h"
#include "UIWidget.generated.h"

/**
 * 
 */
 //Widget class for the UI
UCLASS()
class AR_STARTER_PROJECT_API UUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UUIWidget(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//Enum to control what is displayed
	enum GameState
	{
		Anchor,
		Portal,
		Castle,
		Playing
	};

	//Public functions accessed externally
	void UpdateDisplay(GameState mode);
	void UpdateDisplay();
	void Reset();
	void ConfirmRest();

	//Setters and getters
	void SetHealth(int health);
	void SetGold(int gold);
	GameState GetState();

	//Bool variables
	bool anchored = false, pendingReset = false;

protected:

	//Internal functions
	void ErrorMessage(FString string);

	//Setup for and link the BP widget features to the C++ class
	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UButton* resetButton;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UButton* IterateButton;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UTextBlock* text;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UTextBlock* HealthText;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UTextBlock* GoldText;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UCanvasPanel* Panel;


	//Sets the basic gamestate
	GameState gameState = Anchor;

	//Functions for the buttons
	UFUNCTION()
		void iterateOnClick();

	UFUNCTION()
		void resetOnClick();

	

	
	



};
