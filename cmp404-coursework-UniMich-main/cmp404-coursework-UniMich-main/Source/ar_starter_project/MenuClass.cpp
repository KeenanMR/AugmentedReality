// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuClass.h"
#include "ARBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


UMenuClass::UMenuClass(const FObjectInitializer& objInit) :Super(objInit)
{
	//Constructor helpers to get assets
	static ConstructorHelpers::FObjectFinder<UARSessionConfig>CofigAsset(TEXT("ARSessionConfig'/Game/Images/ARSessionConfig.ARSessionConfig'"));
	Config = CofigAsset.Object;
	Config->bUseSceneDepthForOcclusion = true;
	Config->SetSessionTrackingFeatureToEnable(EARSessionTrackingFeature::SceneDepth);
}

//Update the display settings for the widget
void UMenuClass::UpdateDisplay(WidgetDisplayMode mode)
{
	switch (mode)
	{
	case MainMenu:
		internalMode = MainMenu;
		MenuPanel->SetVisibility(ESlateVisibility::Visible);
		SettingsPanel->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case Play:
		internalMode = Play;
		MenuPanel->SetVisibility(ESlateVisibility::Collapsed);
		SettingsPanel->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}

}

//Resets the widget back to the base settings
void UMenuClass::Reset()
{
	internalMode = MainMenu;
	UpdateDisplay(internalMode);
	
}



void UMenuClass::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UMenuClass::NativeConstruct()
{

	Super::NativeConstruct();

	//Setup for buttons, links to functions
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UMenuClass::startOnClick);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMenuClass::quitOnClick);
	}

	if (SettingsQuit)
	{
		SettingsQuit->OnClicked.AddDynamic(this, &UMenuClass::settingsQuitOnClick);
	}


}

//Whn the start button is clicked
void UMenuClass::startOnClick()
{
	internalMode = Play;
	UpdateDisplay(internalMode);

	if(UARBlueprintLibrary::GetARSessionStatus().Status != EARSessionStatus::Running  )
	UARBlueprintLibrary::StartARSession(Config);
}

//When the quit button ios clicked
void UMenuClass::quitOnClick()
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

//When the settings button is clicked
void UMenuClass::settingsQuitOnClick()
{
	UpdateDisplay(WidgetDisplayMode::MainMenu);
}
