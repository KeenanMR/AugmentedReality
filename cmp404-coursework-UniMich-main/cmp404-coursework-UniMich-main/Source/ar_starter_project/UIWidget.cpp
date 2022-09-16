// Fill out your copyright notice in the Description page of Project Settings.


#include "UIWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

UUIWidget::UUIWidget(const FObjectInitializer& objInit) :Super(objInit)
{
}


void UUIWidget::NativeConstruct()
{
	//Setup for buttons, links to functions
	if (IterateButton)
	{
		IterateButton->OnClicked.AddDynamic(this, &UUIWidget::iterateOnClick);
	}
	if(resetButton)
	{
		resetButton->OnClicked.AddDynamic(this, &UUIWidget::resetOnClick);
	}
}

void UUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
}

//Updates the UI display on each stage
void UUIWidget::UpdateDisplay(GameState mode)
{
	switch (gameState)
	{
	case Anchor:
		Panel->SetVisibility(ESlateVisibility::Visible);
		text->SetText(FText(FText::FromString("Allow for plane detection then click to place the world anchor")));
		HealthText->SetVisibility(ESlateVisibility::Collapsed);
		GoldText->SetVisibility(ESlateVisibility::Collapsed);
		IterateButton->SetVisibility(ESlateVisibility::Visible);

		break;
	case Portal:
		text->SetText(FText(FText::FromString("Click any land to place the portal, this is the enemy spawn")));
		HealthText->SetVisibility(ESlateVisibility::Collapsed);
		GoldText->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case Castle:
		text->SetText(FText(FText::FromString("Click any land to place the catle, this is the enemy target")));
		HealthText->SetVisibility(ESlateVisibility::Collapsed);
		GoldText->SetVisibility(ESlateVisibility::Collapsed);
		IterateButton->SetVisibility(ESlateVisibility::Visible);
		break;
	case Playing:
		text->SetText(FText(FText::FromString("")));
		HealthText->SetVisibility(ESlateVisibility::Visible);
		GoldText->SetVisibility(ESlateVisibility::Visible);
		IterateButton->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:
		break;
	}
}

//Sets the visibility 
void UUIWidget::UpdateDisplay()
{
	Panel->SetVisibility(ESlateVisibility::Collapsed);
}

//Error message if current object isn't placed
void UUIWidget::ErrorMessage(FString string)
{
	text->SetText(FText(FText::FromString("You have not placed a " + string)));
}

//Resets the UI
void UUIWidget::Reset()
{
	anchored = false;
	gameState = Anchor;
	UpdateDisplay();
}

//Called when the running pawn confrims the game is to be reset
void UUIWidget::ConfirmRest()
{
	pendingReset = false;
}

//Setter for the displaying of HP
void UUIWidget::SetHealth(int health)
{
	FString healthT;
	healthT.AppendInt(health);

	HealthText->SetText(FText(FText::FromString("Health: " + healthT)));
}

//Setter for the displaying of gold
void UUIWidget::SetGold(int gold)
{
	FString goldT; 
	goldT.AppendInt(gold);

	GoldText->SetText(FText(FText::FromString("Gold: " + goldT)));
}

//Getter for state
UUIWidget::GameState UUIWidget::GetState()
{
	return gameState;
}

//Iterates the game state
void UUIWidget::iterateOnClick()
{
	TArray<AActor*> actors;

	//Gets the current state and sets the next state if the conditions are met, if they aren't met then display an error message
		switch (gameState)
		{
		case Anchor:
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnchor::StaticClass(), actors);
			if (actors.Num() > 0)
			{
				anchored = true;
				if (anchored)
				{
					UKismetSystemLibrary::PrintString(this, "land", true, true, FLinearColor(0, 1, 0, 1), 2);
				}
				gameState = Portal;
				UpdateDisplay(gameState);
			}
			else
			{
				ErrorMessage("Anchor");
			}

			break;
		case Portal:

			anchored = false;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APortal::StaticClass(), actors);
			if (actors.Num() > 0)
			{
				gameState = Castle;
				UpdateDisplay(gameState);
			}
			else
			{
				ErrorMessage("Portal");
			}
			break;
		case Castle:
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACastle::StaticClass(), actors);
			if (actors.Num() > 0)
			{
				gameState = Playing;
				UpdateDisplay(gameState);
			}
			else
			{
				ErrorMessage("Castle");
			}
			break;
		default:
			break;
		}
	

}

//Sets the reset check value for the running pawn
void UUIWidget::resetOnClick()
{

	pendingReset = true;

}
