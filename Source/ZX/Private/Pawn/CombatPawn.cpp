// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/CombatPawn.h"

#include "Combat/ZXAbilitySystemComponent.h"
#include "Combat/ZXAttributeSet.h"
#include "Combat/ZXGameplayAbility.h"
#include "Data/SkillSetData.h"
#include "UI/NameplateComponent.h"


// Sets default values
ACombatPawn::ACombatPawn()
{
	// setup attachment:
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	NameplateComponent = CreateDefaultSubobject<UNameplateComponent>(TEXT("NameplateComponent"));
	NameplateComponent->SetupAttachment(RootComponent);

	// non scene comps:
	AttributeSet = CreateDefaultSubobject<UZXAttributeSet>(TEXT("ZXAttributeSet"));
	ASC = CreateDefaultSubobject<UZXAbilitySystemComponent>(TEXT("ZXAbilitySystemComponent"));
	ASC->SetIsReplicated(true);
}

UAbilitySystemComponent* ACombatPawn::GetAbilitySystemComponent() const
{
	return ASC;
}

// Called when the game starts or when spawned
void ACombatPawn::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(ASC) || !IsValid(AttributeSet) || !IsValid(DefaultSkillSet))
	{
		LOGZXEF("invalid GAS config..");
		return;
	}

	// make actor info out of ourselves
	MyActorInfo.InitFromActor(this, this, ASC);

	// Grant our default abilities:
	for (const TSubclassOf<UZXGameplayAbility> GameplayAbilityClass : DefaultSkillSet->Abilities)
	{
		RegisterAbility(GameplayAbilityClass);
	}
}

void ACombatPawn::RegisterAbility(const TSubclassOf<UZXGameplayAbility>& GAClass)
{
	// Give ability to our ASC:
	const FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(FGameplayAbilitySpec(GAClass));
	
	// Map tags -> handle
	if (UZXGameplayAbility* AbilityObj = GAClass->GetDefaultObject<UZXGameplayAbility>())
	{
		// Note: realistically an ability should only have one tag? maybe theres a case for multiple.. donno
		for (const FGameplayTag& GameplayTag : AbilityObj->GetAssetTags())
		{
			MyAbilities.Add(GameplayTag, AbilityHandle);
		}
	}
}

void ACombatPawn::UnregisterAbility(const FGameplayTag& InAbilityTag)
{
	FGameplayAbilitySpecHandle* FoundAbilityHandle = MyAbilities.Find(InAbilityTag);
	if (FoundAbilityHandle == nullptr)
	{
		LOGZXEF("tried removing ability but handle not found: %s..", *InAbilityTag.ToString());
		return;
	}

	// clear abil, remove from map:
	ASC->ClearAbility(*FoundAbilityHandle);
	MyAbilities.Remove(InAbilityTag);
}

void ACombatPawn::UseAbilityOnTarget(FGameplayTag InAbilityTag, ACombatPawn* InTarget)
{
	if (!IsValid(ASC) || !IsValid(InTarget) || !IsValid(InTarget->ASC))
	{
		LOGZXEF("invalid use ability on target call..");
		return;
	}

	// Find abil in our map:
	FGameplayAbilitySpecHandle* FoundAbilityHandle = MyAbilities.Find(InAbilityTag);
	if (FoundAbilityHandle == nullptr)
	{
		LOGZXEF("ability not found: %s..", *InAbilityTag.ToString());
		return;
	}

	// Make target data out of single pawn:
	FGameplayAbilityTargetDataHandle TargetData;
	FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
	TWeakObjectPtr<AActor> WeakTarget = InTarget;
	NewData->TargetActorArray.Add(WeakTarget);
	TargetData.Add(NewData);
	
	// activate abil by tag:
	FGameplayEventData EventData; // event data is how i plan to do targeting for now.
	EventData.EventTag = InAbilityTag;
	EventData.Target = InTarget;
	EventData.TargetData = TargetData;
	// MD-TODO: theres probably more we want to add to event data, like magnitude or something..
	if (!ASC->TriggerAbilityFromGameplayEvent(*FoundAbilityHandle, &MyActorInfo, InAbilityTag, &EventData, *ASC))
	{
		LOGZXEF("failed to activate ability with tag: %s", *InAbilityTag.ToString());
		return;
	}
}
