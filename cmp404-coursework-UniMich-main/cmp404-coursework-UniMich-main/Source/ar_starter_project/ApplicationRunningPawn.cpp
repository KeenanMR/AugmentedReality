// Fill out your copyright notice in the Description page of Project Settings.

#include "ApplicationRunningPawn.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ARBlueprintLibrary.h"

// Sets default values
AApplicationRunningPawn::AApplicationRunningPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Constructor helpers to get config
	static ConstructorHelpers::FObjectFinder<UARSessionConfig>CofigAsset(TEXT("ARSessionConfig'/Game/Images/ARSessionConfig.ARSessionConfig'"));
	Config = CofigAsset.Object;
	Config->bUseSceneDepthForOcclusion = true;
	Config->SetSessionTrackingFeatureToEnable(EARSessionTrackingFeature::SceneDepth);

	//Scene component setup
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//Constructor helpers to get assets
	static ConstructorHelpers::FObjectFinder<USoundCue> backgroundCue(TEXT("/Game/Audio/BackgroundMusic.BackgroundMusic"));
	static ConstructorHelpers::FObjectFinder<USoundCue> spawnCue(TEXT("SoundCue'/Game/Audio/Spawn.Spawn'"));
	static ConstructorHelpers::FClassFinder<UMenuClass> menuBPClass(TEXT("WidgetBlueprint'/Game/Blueprints/MainMenu.MainMenu_C'"));
	static ConstructorHelpers::FClassFinder<UUIWidget> UIBPClass(TEXT("WidgetBlueprint'/Game/Blueprints/UI.UI_C'"));

	//Sets up background audio for the pawn
	backgroundAudioCue = backgroundCue.Object;
	backgroundAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundAudioCue"));
	backgroundAudioComponent->AutoAttachParent = SceneComponent;

	//Sets up spawn audio for the pawn
	spawnAudioCue = spawnCue.Object;
	spawnAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnAudioCue"));
	spawnAudioComponent->AutoAttachParent = SceneComponent;

	//Sets up camera components
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Populate the plane colours array
	PlaneColors.Add(FColor::Blue);
	PlaneColors.Add(FColor::Red);
	PlaneColors.Add(FColor::Green);
	PlaneColors.Add(FColor::Cyan);
	PlaneColors.Add(FColor::Magenta);
	PlaneColors.Add(FColor::Emerald);
	PlaneColors.Add(FColor::Orange);
	PlaneColors.Add(FColor::Purple);
	PlaneColors.Add(FColor::Turquoise);
	PlaneColors.Add(FColor::White);
	PlaneColors.Add(FColor::Yellow);

	//Checks if widgets are valid adn if they are set the classes to them
	if (menuBPClass.Class != nullptr)
	{
		menuClass = menuBPClass.Class;
	}
	if (UIBPClass.Class != nullptr)
	{
		UIClass = UIBPClass.Class;
	}
}

void AApplicationRunningPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	//Sets the sound to play from this component
	if (backgroundAudioCue->IsValidLowLevelFast())
	{
		backgroundAudioComponent->SetSound(backgroundAudioCue);
	}
	//Sets the sound to play from this component
	if (spawnAudioCue->IsValidLowLevelFast())
	{
		spawnAudioComponent->SetSound(spawnAudioCue);
	}
}

// Called when the game starts or when spawned
void AApplicationRunningPawn::BeginPlay()
{
	Super::BeginPlay();

	//Basic setup for fade in for the backgorund music 
	float startTime = 9.f;
	float volume = 0.5f;
	float fadeTime = 5.0f;
	backgroundAudioComponent->FadeIn(fadeTime, volume, startTime);

	//Sets up widgets for the main menu and UI
	if (menuClass != nullptr)
	{
		APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

		menu = CreateWidget<UMenuClass>(playerController,menuClass);

		if (menu != nullptr)
		{
			menu->AddToViewport(0);
			menu->UpdateDisplay(UMenuClass::WidgetDisplayMode::MainMenu);	
		}
	}
	if (UIClass != nullptr)
	{
		APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
		UI = CreateWidget<UUIWidget>(playerController, UIClass);

		if (UI != nullptr)
		{
			UI->UpdateDisplay();
			UI->UpdateDisplay(UUIWidget::GameState::Anchor);
			UI->SetGold(playerGold);
		}
	}
}

// Called every frame
void AApplicationRunningPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//MAkes function calls based on if the AR session was sucessully run
	switch (UARBlueprintLibrary::GetARSessionStatus().Status)
	{
	case EARSessionStatus::Running:
		if (menu->internalMode == UMenuClass::WidgetDisplayMode::Play)
		{
			UI->AddToViewport(1);
			UI->UpdateDisplay(UUIWidget::GameState::Anchor);
			if(!anchorSet)
			UpdatePlaneActors();
		}
		break;
	case EARSessionStatus::FatalError:
		UARBlueprintLibrary::StartARSession(Config);
		break;
	}

	//If the game state is playing spawn enemies on a timer that gets lower over time
	if (UI->GetState() == UUIWidget::GameState::Playing)
	{
		spawnTimer += DeltaTime;
		{
			if (spawnTimer > spawnCD)
			{
				SpawnEnemies();
				spawnCD -= 0.1;
				spawnTimer = 0;
			}
		}
	}

	//Auto spawns the lands when the anchor is set
	if (UI->GetState() == UUIWidget::GameState::Portal && !landSpawned)
	{
		UKismetSystemLibrary::PrintString(this, "land", true, true, FLinearColor(0, 1, 0, 1), 2);
		SpawnLand();
	}

	//Checks if the user wants to reset the application
	if (UI->pendingReset)
	{
		UI->ConfirmRest();
		ResetARCoreSession();
	}

	//Adds gold for each destoryed enemy
	for (auto it : enemies)
	{
		if (it->GetState())
		{
			it->SetState(false);
			it->Destroy();
			playerGold += 5;
			UI->SetGold(playerGold);

		}
	}

	//Sets the display health t the castle health
	if (castleSpawned)
	{
		UI->SetHealth(castle->GetHealth());
		if (castle->GetHealth() <= 0)
			ResetARCoreSession();
	}
	
}

// Called to bind functionality to input
void AApplicationRunningPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind various player inputs to functions
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AApplicationRunningPawn::OnScreenTouch);
	PlayerInputComponent->BindTouch(IE_Repeat, this, &AApplicationRunningPawn::OnScreenHeld);
	PlayerInputComponent->BindTouch(IE_Released, this, &AApplicationRunningPawn::OnScreenReleased);
	
}

//Updates the geometry actors in the world
void AApplicationRunningPawn::UpdatePlaneActors()
{
	//Get all world geometries and store in an array
	auto geometries = UARBlueprintLibrary::GetAllGeometriesByClass<UARPlaneGeometry>();
	bool found = false;
	
	//Loop through all geometries
	for (auto& it : geometries)
	{
		//Check if current plane exists 
		if (PlaneActors.Contains(it))
		{
			AARPlaneActor* CurrentPActor = *PlaneActors.Find(it);

			//Check if plane is subsumed
			if (it->GetSubsumedBy()->IsValidLowLevel())
			{
				CurrentPActor->Destroy();
				PlaneActors.Remove(it);
				break;
			}
			else
			{
				//Get tracking state switch
				switch (it->GetTrackingState())
				{
					//If tracking update
				case EARTrackingState::Tracking:
					CurrentPActor->UpdatePlanePolygonMesh();
					break;
					//If not tracking destroy the actor and remove from map of actors
				case EARTrackingState::StoppedTracking:
					CurrentPActor->Destroy();
					PlaneActors.Remove(it);
					break;
				}
			}
		}
		else
		{
			//Get tracking state switch
			switch (it->GetTrackingState())
			{
		
			case EARTrackingState::Tracking:
				if (!it->GetSubsumedBy()->IsValidLowLevel())
				{
					planeActor = SpawnPlaneActor();
					planeActor->SetColor(GetPlaneColor(planeIndex));
					planeActor->ARCorePlaneObject = it;

					PlaneActors.Add(it, planeActor);
					planeActor->UpdatePlanePolygonMesh();
					planeIndex++;
				}
				break;
			}
		}
	
	}
}

//Calls different spawns based on the current game state
void AApplicationRunningPawn::OnScreenTouch(const ETouchIndex::Type fingerIndex, const FVector screenPo)
{
	UUIWidget::GameState state = UI->GetState();

	switch (state)
	{
	case UUIWidget::GameState::Anchor:
		if(!anchorSet)
		SpawnAnchor(screenPo);
		break;
	case UUIWidget::GameState::Portal:
		if(!portalSet)
		SpawnPortal(screenPo);
		break;
	case UUIWidget::GameState::Castle:
		if(!castleSet)
		SpawnCastle(screenPo);
		break;
	case UUIWidget::GameState::Playing:
		SpawnDefence(screenPo);
		break;
	default:
		break;
	}
}

//Not used - Moves the selected actor along the plane
void AApplicationRunningPawn::OnScreenHeld(const ETouchIndex::Type fingerIndex, const FVector screenPo)
{

	if (actorSelected)
	{
		
		UUIWidget::GameState state = UI->GetState();

		switch (state)
		{
		case UUIWidget::GameState::Portal:
			break;
		case UUIWidget::GameState::Castle:
			break;
		case UUIWidget::GameState::Playing:
			break;
		default:
			break;
		}
	
		/*

		APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

		FVector worldPos;
		FVector worldDir;

		UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(screenPo), worldPos, worldDir);
		FVector movementVec = worldPos * 100;

		UARPin* actorPin;

		actorPin = selectedActor->FindComponentByClass<UARPin>();

		if (actorPin->IsValidLowLevelFast())
		{
			auto tf = actorPin->GetLocalToWorldTransform();

			tf.AddToTranslation(FVector(movementVec.X, movementVec.Y, 0));

			selectedActor->SetActorTransform(tf);

			actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, tf);
			
		}
		else
		{
			selectedActor->SetActorLocation(FVector(movementVec.X, movementVec.Y, 0));
		}
		*/
	}
}

//Not used
void AApplicationRunningPawn::OnScreenReleased(const ETouchIndex::Type fingerIndex, const FVector screenPo)
{
}

//Checks if the player touch hits an actor
bool AApplicationRunningPawn::WorldHitTest(const FVector2D& screenPos, FHitResult& hitResult)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

	FVector worldPos;
	FVector worldDir;

	bool deprojectionSuccess = UGameplayStatics::DeprojectScreenToWorld(playerController, screenPos, worldPos, worldDir);

	FVector trace = worldDir * 1000;
	trace = worldPos + trace;

	bool traceSuccess = GetWorld()->LineTraceSingleByChannel(hitResult, worldPos, trace, ECollisionChannel::ECC_WorldStatic);

	return traceSuccess;
}

//Spawns the world anchor
void AApplicationRunningPawn::SpawnAnchor(const FVector screenPo)
{
	//Basic variables for functionality 
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector worldPos;
	FVector worldDir;

	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(screenPo), worldPos, worldDir);
	auto tracked = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(screenPo), false, false, false, true);

	//Checks if the location is valid
	if (tracked.IsValidIndex(0))
	{
		auto trackedTF = tracked[0].GetLocalToWorldTransform();

		if (FVector::DotProduct(trackedTF.GetRotation().GetUpVector(), worldDir) < 0)
		{
				
			//Spawn the actor pin and get the transform
			UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, tracked[0].GetLocalToWorldTransform(), tracked[0].GetTrackedGeometry());
			auto pinTF = actorPin->GetLocalToWorldTransform();

			//If the pin is valid 
			if (actorPin->IsValidLowLevelFast())
			{
				//Either spawn a new anchor at the location or moved the current actor
				if (!anchorSpawned)
				{

					FActorSpawnParameters SpawnInfo;
					FRotator myRot(0, 0, 0);
					FVector myLoc(0, 0, 0);


					anchor = GetWorld()->SpawnActor<AAnchor>(myLoc, myRot, SpawnInfo);

					anchor->SetActorTransform(pinTF);

					anchor->PinComponent = actorPin;

					anchorSpawned = true;

					return;
				}
				else
				{
					anchor->SetActorTransform(pinTF);

					anchor->PinComponent = actorPin;

					return;
				}
			}
		}
	}
}

//Spawns a grid of lands around the world anchor
void AApplicationRunningPawn::SpawnLand()
{
	//Basic variables for functionality 
	FHitResult hitResultTest;
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(worldRotation);
	FVector myLoc(0,0,0);

	//Gets the anchor pin transform
	FTransform anchorTF = anchor->PinComponent->GetLocalToWorldTransform();

	//Spawns a grid of land around the anchor based on the extent of the lands
	for (int i = -3; i < 4; i++)
	{
		for (int j = -3; j < 4; j++)
		{

			ABaseLand* nextLand = GetWorld()->SpawnActor<ABaseLand>(myLoc, myRot, SpawnInfo);

			FTransform newPinTF = anchorTF;
			newPinTF.SetScale3D(FVector(0.2f, 0.2f, 0.2f));

			nextLand->SetActorTransform(newPinTF);

			FVector origin;
			FVector extent;

			nextLand->GetActorBounds(true, origin, extent);

			FVector offset = FVector((1.5 * extent.X) * i, (1.5 * extent.Y) * j, 10);

			newPinTF.SetRotation(FQuat(0, 0, 0, 1));
			newPinTF.AddToTranslation(offset);

			UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, newPinTF);

			nextLand->SetActorTransform(newPinTF);

			nextLand->PinComponent = actorPin;

			lands.Add(nextLand);
							
			landSpawned = true;

		}
	}
	//Destroys the tracked planes so they are no longered rendered
	for (auto it : PlaneActors)
	{
		it.Value->Destroy();
	}
	//Hides the anchor
	anchor->Hide();
		
	
}

//Spawns a defence on the selected land
void AApplicationRunningPawn::SpawnDefence(const FVector screenPo)
{
	//If the player ahs enough gold to spawn a land
	if (playerGold >= 10)
	{
		//Basic variables for functionality 
		APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
		FActorSpawnParameters SpawnInfo;
		FRotator myRot(0, 0, 0);
		FVector myLoc(0, 0, 0);

		//Get the result of a world hit test
		FHitResult hitTestResult;
		WorldHitTest(FVector2D(screenPo), hitTestResult);

		//Check all lands
		for (auto it : lands)
		{
			//If the result is a land
			if (hitTestResult.Actor == it)
			{
				//Gets the land transform
				FTransform LandTf = it->PinComponent->GetLocalToWorldTransform();
				LandTf.SetScale3D(FVector(0.15f, 0.15f, 0.15f));

				//Spawns a new defence and sets its transform
				ABaseDefence* defenceToSpawn = GetWorld()->SpawnActor<ABaseDefence>(myLoc, myRot, SpawnInfo);
				defenceToSpawn->SetActorTransform(LandTf);

				//Spawns an ARPin for the defence and sets the actor transform to this transform
				FTransform newPinTF = LandTf;
				UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, newPinTF);
				newPinTF.AddToTranslation(FVector(0, 0, 7));
				defenceToSpawn->SetActorTransform(newPinTF);

				//Sets the defence pin component
				defenceToSpawn->PinComponent = actorPin;

				defences.Add(defenceToSpawn);

			}

		}
		//Remove the playes gold and update the UI
		playerGold -= 10;
		UI->SetGold(playerGold);
	}
	
}

//Spawns a castle that acts as the enemies objective
void AApplicationRunningPawn::SpawnCastle(const FVector screenPo)
{
	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector worldPos;
	FVector worldDir;
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);
	FVector myLoc(0, 0, 0);

	//Get the result of a world hit test
	FHitResult hitTestResult;
	WorldHitTest(FVector2D(screenPo), hitTestResult);

	//Check all lands
	for (auto it : lands)
	{
		//If the result is a land
		if (hitTestResult.Actor == it)
		{
			//Get the land transform
			FTransform LandTf = it->PinComponent->GetLocalToWorldTransform();

			//Checks if there is already a castle
			if (!castleSpawned)
			{
				//Spawn a new castle
				castle = GetWorld()->SpawnActor<ACastle>(myLoc, myRot, SpawnInfo);

				//Sets and updates the new TF
				FTransform pinTF = LandTf;
				pinTF.SetScale3D(FVector(0.3f, 0.3f, 0.3f));
				castle->SetActorTransform(pinTF);

				//Spawns, sets and updates the pin TF
				UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, pinTF);
				pinTF.AddToTranslation(FVector(0, 0, 7));
				castle->SetActorTransform(pinTF);

				//Sets the pin component
				castle->PinComponent = actorPin;

				castleSpawned = true;

				//Update hthe health in the UI
				UI->SetHealth(castle->GetHealth());

				return;
			}
			else
			{
				
				//Resets the transform and moves the castle if it is already spawned
				FTransform pinTF = LandTf;
				pinTF.SetScale3D(FVector(0.3f, 0.3f, 0.3f));
				castle->SetActorTransform(pinTF);

				UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, pinTF);
				pinTF.AddToTranslation(FVector(0, 0, 7));
				castle->SetActorTransform(pinTF);
				castle->PinComponent = actorPin;

				return;

			}
		}
	}

	
}

void AApplicationRunningPawn::SpawnPortal(const FVector screenPo)
{
	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector worldPos;
	FVector worldDir;
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);
	FVector myLoc(0, 0, 0);


	//Get the result of a world hit test
	FHitResult hitTestResult;
	WorldHitTest(FVector2D(screenPo), hitTestResult);

	//Check all lands
	for (auto it : lands)
	{
		//If the result is a land
		if (hitTestResult.Actor == it)
		{
			//Get the land transform
			FTransform LandTf = it->PinComponent->GetLocalToWorldTransform();

			//Checks if there is already a portal
			if (!portalSpawned)
			{
				//Spawn a new cstle
				portal = GetWorld()->SpawnActor<APortal>(myLoc, myRot, SpawnInfo);

				//Sets and updates the new TF
				FTransform pinTF = LandTf;
				pinTF.SetScale3D(FVector(0.3f, 0.3f, 0.3f));
				portal->SetActorTransform(pinTF);

				//Sets the pin component
				UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, pinTF);
				pinTF.AddToTranslation(FVector(0, 0, 7));
				portal->SetActorTransform(pinTF);
				portal->PinComponent = actorPin;

				portalSpawned = true;

			

				return;
			}
			else
			{
				//Resets the transform and moves the portal if it is already spawned
				FTransform pinTF = LandTf;
				pinTF.SetScale3D(FVector(0.3f, 0.3f, 0.3f));
				portal->SetActorTransform(pinTF);

				UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, pinTF);
				pinTF.AddToTranslation(FVector(0, 0, 7));
				portal->SetActorTransform(pinTF);
				portal->PinComponent = actorPin;

				return;
			}
		}
	}
}

void AApplicationRunningPawn::SpawnEnemies()
{
	//Basic variables for functionality
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);
	FVector myLoc(0, 0, 0);

	//Gets the portal TF
	FTransform portalTF = portal->PinComponent->GetLocalToWorldTransform();

	//Spawns a new enemy
	ABaseEnemy* newEnemy = GetWorld()->SpawnActor<ABaseEnemy>(myLoc, myRot, SpawnInfo);

	//Sets and updates the new TF
	FTransform pinTF = portalTF;
	pinTF.SetScale3D(FVector(0.025f, 0.025f, 0.025f));
	newEnemy->SetActorTransform(pinTF);
	pinTF.SetRotation(FQuat(0, 0, 0, 1));;

	//Sets the pin component
	UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, pinTF);
	newEnemy->SetActorTransform(pinTF);
	newEnemy->PinComponent = actorPin;

	//Sets the enemies target
	newEnemy->SetCastle(*castle);
	enemies.Add(newEnemy);

	//Plays the spawn sounds
	spawnAudioComponent->Play();

	return;
}

//Simple spawn function for the tracked AR planes
AARPlaneActor* AApplicationRunningPawn::SpawnPlaneActor()
{
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);
	FVector myLoc(0, 0, 0);

	AARPlaneActor* customPlane = GetWorld()->SpawnActor<AARPlaneActor>(myLoc, myRot, SpawnInfo);

	return customPlane;
}

//Gets the colour to set the plane to when its spawned
FColor AApplicationRunningPawn::GetPlaneColor(int index)
{
	return PlaneColors[index % PlaneColors.Num()];
}

//Resets the application
void AApplicationRunningPawn::ResetARCoreSession()
{

	//Get all actors in the level and destory them as well as emptying the respective arrays
	TArray<AActor*> planes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPlaneActor::StaticClass(), planes);

	for (auto it : planes)
	{
		it->Destroy();
	}
	planes.Empty();
	PlaneActors.Empty();

	if (enemies.Num() > 0)
	{
		for (auto it : enemies)
		{
			it->Destroy();
		}
		enemies.Empty();
	}

	if (defences.Num() > 0)
	{
		for (auto it : defences)
		{
			it->Destroy();
		}
		defences.Empty();
	}

	if (lands.Num() > 0)
	{
		for (auto it : lands)
		{
			it->Destroy();
		}
		lands.Empty();
	}

	if(castle)
	castle->Destroy();
	if(portal)
	portal->Destroy();
	if(anchor)
	anchor->Destroy();

	//Resets variables
	planeIndex = 0;
	spawnCD = 3;
	playerGold = 20;

	anchorSet = false, portalSet = false, castleSet = false, anchorSpawned = false, portalSpawned = false, castleSpawned = false, actorSelected = false;
	landSpawned = false;
	//Reset widgets
	UI->Reset();
	UI->RemoveFromViewport();
	menu->Reset();

	
}
