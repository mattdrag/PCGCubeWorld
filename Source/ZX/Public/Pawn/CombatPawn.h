// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "CombatPawn.generated.h"

class UNameplateComponent;
class UUNameplateComponent;
struct FGameplayAbilitySpecHandle;
class UZXGameplayAbility;
class USkillSetData;
struct FGameplayTag;
class UZXAttributeSet;
class UZXAbilitySystemComponent;
/*
 * A pawn with an ability system component.
 * - allows it to take damage / potentially deal damage / interact with the ASC in general (if it wants to)
 */
UCLASS()
class ZX_API ACombatPawn : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACombatPawn();

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// given a tag, checks if we have active ability with tag. if so, use it on target.
	void UseAbilityOnTarget(FGameplayTag InAbilityTag, ACombatPawn* InTarget);

	// Grants our ASC an ability and maps the tag to a handle.
	void RegisterAbility(const TSubclassOf<UZXGameplayAbility>& GAClass);
	void UnregisterAbility(const FGameplayTag& InAbilityTag);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Abilities that have been given to our ASC
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> MyAbilities;
	
	/** The component used to handle ability system interactions */
	UPROPERTY(VisibleAnywhere)
	UZXAbilitySystemComponent* ASC;

	// Skills that everyone has:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	USkillSetData* DefaultSkillSet;

	// Creating an AttributeSet in an OwnerActor's constructor automatically registers it with its ASC
	UPROPERTY(VisibleAnywhere)
	UZXAttributeSet* AttributeSet;

	// our nameplate:
	UPROPERTY(VisibleAnywhere)
	UNameplateComponent* NameplateComponent;

private:
	// cache our actor info so that we dont need to make the struct every time we cast an ability.
	UPROPERTY()
	FGameplayAbilityActorInfo MyActorInfo;
};
