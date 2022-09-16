// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "ARBlueprintLibrary.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Scene component setup
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//Mesh component setup with collision
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	StaticMeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Constructor helpers to get assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/DragonMesh.DragonMesh'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>BaseColourMat(TEXT("Material'/Game/Mats/AlbinoMat.AlbinoMat'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> deathCue(TEXT("SoundCue'/Game/Audio/EnemyDeath.EnemyDeath'"));

	//Sets the material variables
	colour = BaseColourMat.Object;

	//Sets the mesh and material 
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetMaterial(0, colour);
	SetRootComponent(StaticMeshComponent);

	//Sets up audio for the actor
	deathAudioCue = deathCue.Object;
	deathAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathAudioCue"));
	deathAudioComponent->AutoAttachParent = SceneComponent;
	


}

void ABaseEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Sets the sound to play from this component
	if (deathAudioCue->IsValidLowLevelFast())
	{
		deathAudioComponent->SetSound(deathAudioCue);
	}
}


//Damages the enemy
void ABaseEnemy::Damage(int takenDamage)
{
	health -= takenDamage;
	deathAudioComponent->Play();
	
	if (health <= 0)
	{
		//Sets the check for the controller pawn to destroy this
		dying = true;
	}
}


//Sets the castle taregt
void ABaseEnemy::SetCastle(ACastle& newCastle)
{
	castle = &newCastle;
}

//Gets the current dying state
bool ABaseEnemy::GetState()
{
	return dying;
}

//Sets the dying state
void ABaseEnemy::SetState(bool state)
{
	dying = state;
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	alpha += (DeltaTime /10);




	//Function to update the anchor each frame - stops the actors from drifting/disappearing in AR
	if (PinComponent->IsValidLowLevelFast())
	{
		if (PinComponent->GetTrackingState() == EARTrackingState::Tracking)
		{

			StaticMeshComponent->SetVisibility(true);
			auto tf = PinComponent->GetLocalToWorldTransform();
			tf.SetScale3D(FVector(0.025f, 0.025f, 0.025f));

			//Lerps the transform from the current pos to the target
			auto castleTF = castle->PinComponent->GetLocalToWorldTransform();
			auto newTF = UKismetMathLibrary::TLerp(tf, castleTF, alpha);
			newTF.SetScale3D(FVector(0.025f, 0.025f, 0.025f));
			SetActorTransform(newTF);
		}

	}

	//If the actor reaches the castle
	if (IsOverlappingActor(castle))
	{
		castle->Damage(damage);
		this->Destroy();
	}
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

