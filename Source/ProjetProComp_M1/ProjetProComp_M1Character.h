// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ProjetProComp_M1Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPowerComponent;
class UPowerConfig;
class UPowerBase;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProjetProComp_M1Character : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* AbilityAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* Slot1Action;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* Slot2Action;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* Slot3Action;

public:
    AProjetProComp_M1Character();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Powers")
    UPowerComponent* PowerComp;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Powers")
    TSoftObjectPtr<UPowerConfig> FireConfig;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Powers")
    TSoftObjectPtr<UPowerConfig> IceConfig;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Powers")
    TSoftObjectPtr<UPowerConfig> LightningConfig;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Powers")
    TSubclassOf<UPowerBase> FirePowerClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Powers")
    TSubclassOf<UPowerBase> IcePowerClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Powers")
    TSubclassOf<UPowerBase> LightningPowerClass;

protected:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void OnAbility(const FInputActionValue& Value);
    void OnSlot1(const FInputActionValue& Value);   // Fire
    void OnSlot2(const FInputActionValue& Value);   // Ice
    void OnSlot3(const FInputActionValue& Value);   // Lightning

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void BeginPlay() override;

public:
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
