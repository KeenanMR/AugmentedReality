// Fill out your copyright notice in the Description page of Project Settings.


#include "Castle.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

// Sets default values
ACastle::ACastle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Scene component setup
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//Mesh component setup with collision
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	StaticMeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Constructor helpers to get assets
		//Tried to use unique mesh but no free ones suited or they didnt work in the application or weren't centred and I cant model
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/Castle.Castle'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>BaseColourMat(TEXT("Material'/Game/Mats/WoodMat.WoodMat'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> lossCue(TEXT("SoundCue'/Game/Audio/Lose.Lose'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> damageCue(TEXT("SoundCue'/Game/Audio/EnemyAttack.EnemyAttack'"));

	//Sets the material variables
	colour = BaseColourMat.Object;

	//Sets the mesh and material 
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetMaterial(0, colour);
	SetRootComponent(StaticMeshComponent);


	//Sets up audio for the actor
	damageAudioCue = damageCue.Object;
	damageAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DamageAudioCue"));
	damageAudioComponent->AutoAttachParent = SceneComponent;

	lossAudioCue = lossCue.Object;
	lossAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LossAudioCue"));
	lossAudioComponent->AutoAttachParent = SceneComponent;
	
	
}

void ACastle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Sets the sound to play from this component
	if (damageAudioCue->IsValidLowLevelFast())
	{
		damageAudioComponent->SetSound(damageAudioCue);
	}

	//Sets the sound to play from this component
	if (lossAudioCue->IsValidLowLevelFast())
	{
		lossAudioComponent->SetSound(lossAudioCue);
	}
}


//Damages the castle
void ACastle::Damage(int takenDamage)
{
	health -= takenDamage;
	damageAudioComponent->Play();

	if (health <= 0)
	{
		lossAudioComponent->Play();
	}
}

//Getter for health
int ACastle::GetHealth()
{
	return health;
}

// Called when the game starts or when spawned
void ACastle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACastle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Function to update the anchor each frame - stops the actors from drifting/disappearing in AR
	if (PinComponent->IsValidLowLevelFast())
	{
		if (PinComponent->GetTrackingState() == EARTrackingState::Tracking)
		{
			StaticMeshComponent->SetVisibility(true);
			auto tf = PinComponent->GetLocalToWorldTransform();
			tf.SetScale3D(FVector(0.15f, 0.15f, 0.15f));
			tf.AddToTranslation(FVector(0,0,7));
			SetActorTransform(tf);
		}
	}
}

