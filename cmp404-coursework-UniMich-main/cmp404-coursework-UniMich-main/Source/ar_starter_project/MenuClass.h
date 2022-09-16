// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Slider.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ARSessionConfig.h"
#include "Blueprint/UserWidget.h"
#include "MenuClass.generated.h"

/**
 * 
 */
//Widget class for the menu
UCLASS()
class AR_STARTER_PROJECT_API UMenuClass : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UMenuClass(const FObjectInitializer& ObjectInitializer);

	//Enum to control what is displayed
	enum WidgetDisplayMode
	{
		MainMenu,
		Settings,
		Play

	};


	//Public functions accessed externally
	void UpdateDisplay(WidgetDisplayMode mode);
	void Reset();

	WidgetDisplayMode internalMode = MainMenu;

protected: 


	// Called every frame
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//AR session config settings
	UARSessionConfig* Config;

	//Setup for and link the BP widget features to the C++ class
	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UTextBlock* test;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UButton* StartButton;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UButton* QuitButton;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UButton* SettingsQuit;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UCanvasPanel* MenuPanel;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class UCanvasPanel* SettingsPanel;

	UPROPERTY(BlueprintReadWrite, meta = (bindWidget))
		class USlider* MasterVol;


	//Functions for the buttons
	UFUNCTION()
		void startOnClick();

	UFUNCTION()
		void quitOnClick();

	UFUNCTION()
		void settingsQuitOnClick();

	

};
