// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDefence.h"
#include "ARBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// Sets default values
ABaseDefence::ABaseDefence()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Scene component setup
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//Mesh component setup
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Constructor helpers to get assets

	//Tried to use unique mesh but no free ones suited or they didnt work in the application or weren't centred
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/Defence.Defence'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>BaseColourMat(TEXT("Material'/Game/Mats/WoodMat.WoodMat'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> attackCue(TEXT("SoundCue'/Game/Audio/TowerAttack.TowerAttack'"));

	//Sets the material variables
	colour = BaseColourMat.Object;

	//Sets the mesh and material 
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetMaterial(0, colour);
	SetRootComponent(StaticMeshComponent);


	//Sets up audio for the actor
	attackAudioCue = attackCue.Object;
	attackAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AttackAudioCue"));
	attackAudioComponent->AutoAttachParent = SceneComponent;

}

void ABaseDefence::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Sets the sound to play from this component
	if (attackAudioCue->IsValidLowLevelFast())
	{
		attackAudioComponent->SetSound(attackAudioCue);
	}
}


//Spawns a bullet that tracks an enemy uses the same spawn method as all actors
void ABaseDefence::Attack(ABaseEnemy& target)
{

	//Basic spawn values
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);
	FVector myLoc(0, 0, 0);

	//Sets transforms and spawns
	FTransform defenceTF = PinComponent->GetLocalToWorldTransform();
	ABullet* newBullet = GetWorld()->SpawnActor<ABullet>(myLoc, myRot, SpawnInfo);
	FTransform pinTF = defenceTF;

	//Scales actor down and sets a constant rotation
	pinTF.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
	newBullet->SetActorTransform(pinTF);
	pinTF.SetRotation(FQuat(0, 0, 0, 1));;

	//Spawns an ARPin for the actor and sets it as the spawned actors pin component
	UARPin* actorPin = UARBlueprintLibrary::PinComponent(SceneComponent, pinTF);
	newBullet->SetActorTransform(pinTF);
	newBullet->PinComponent = actorPin;

	//Basic setup for bullet actor
	newBullet->SetTarget(target);
	newBullet->SetDamage(damage);

	return;



}

// Called when the game starts or when spawned
void ABaseDefence::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseDefence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Function to update the anchor each frame - stops the actors from drifting/disappearing in AR
	if (PinComponent->IsValidLowLevelFast())
	{
		if (PinComponent->GetTrackingState() == EARTrackingState::Tracking)
		{
			StaticMeshComponent->SetVisibility(true);
			auto tf = PinComponent->GetLocalToWorldTransform();
			tf.SetScale3D(FVector(0.1f, 0.1f, 0.15f));
			tf.AddToTranslation(FVector(0, 0, 7));
			SetActorTransform(tf);
		}

	}

	


	//Check for attacking enemy
	fireCD += DeltaTime;
	if (fireCD > 1)
	{

		//Gets all current enemies
		TArray<ABaseEnemy*> enemies;
		for (TActorIterator<ABaseEnemy> IT(GetWorld()); IT; ++IT)
		{
			enemies.Add(*IT);
		}

		//If there is a valid enemy
		if (enemies.Num() > 0)
		{
			//Removes any enemies that are destoryed or being destroyed 
			for (auto it : enemies)
			{
				if (it == nullptr || it->IsPendingKill())
				{
					enemies.Remove(it);
				}
					
			}

			//Sets the base target
			ABaseEnemy* target = enemies[0];
			float minDistance = 1000;

			//Sets the target based on the closest enemy 
			for (auto it : enemies)
			{
				FVector enemyLoc = it->GetActorLocation();
				enemyLoc = enemyLoc - GetActorLocation();
				float distance = enemyLoc.Size();
				if (distance < range)
				{
					if (distance < minDistance)
					{
						minDistance = distance;
						target = it;
					}
				}

			}

			//If the target is valid spawn a bullet
			if (target->IsValidLowLevelFast())
			{
				attackAudioComponent->Play();
				Attack(*target);

			}
			
			fireCD = 0;
		}
	}




}

// Called to bind functionality to input
void ABaseDefence::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

